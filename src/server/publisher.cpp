/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2021-2022,  The University of Memphis
 *
 * This file is part of mGuard.
 * See AUTHORS.md for complete list of mGuard authors and contributors.
 *
 * mGuard is free software: you can redistribute it and/or modify it under the terms
 * of the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * mGuard is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with
 * mGuard, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "publisher.hpp"
#include "common.hpp"

#include <boost/bind.hpp>

NDN_LOG_INIT(mguard.Publisher);

namespace mguard {

Publisher::Publisher(ndn::Face& face, ndn::security::KeyChain& keyChain,
                    const ndn::Name& producerPrefix,
                    const ndn::security::Certificate& producerCert,
                    const ndn::security::Certificate& attrAuthorityCertificate)
: m_face(face)
, m_keyChain(keyChain)
, m_scheduler(m_face.getIoService())
// 40 = expected number of entries also will be used as IBF size
// syncPrefix = /org.md2k/sync, userPrefix = /org.md2k/uprefix <--- this will be changed
, m_partialProducer(m_face, m_keyChain, 40, "/ndn/org/md2k",
                                "/ndn/org/md2k/mguard/dd40c/data_analysis/gps_episodes_and_semantic_location/manifest")
, m_asyncRepoInserter(m_face.getIoService())
, m_producerPrefix(producerPrefix)
, m_producerCert(producerCert)
, m_authorityCert(attrAuthorityCertificate)
, m_abe_producer(m_face, m_keyChain, m_producerCert, m_authorityCert)
{

  m_asyncRepoInserter.AsyncConnectToRepo("0.0.0.0", "7376", std::bind(&Publisher::connectHandler, this, _1));
  std::this_thread::sleep_for (std::chrono::seconds(1));
  NDN_LOG_DEBUG("Connecting to repo...");

  // --------------- in duscussion, only for testing----------
  // Discussion: we will add all the possible stream to the psync at the begnning
  // this is for the current testing and experiment only, the design needs to be better
  // reason: consumer kicks in, finds that the stream is still not available (sequential processing)
  // of the stream during the experiment, and thus keeps sending hello interest (causes a lot of overhead)
  // second, if we send hello interest less frequently, the overall data retrival delay increases.
  
  // m_partialProducer.addUserNode("/ndn/org/md2k/mguard/dd40c/data_analysis/gps_episodes_and_semantic_location/manifest");
  m_partialProducer.addUserNode("/ndn/org/md2k/mguard/dd40c/phone/battery/manifest");
  m_partialProducer.addUserNode("/ndn/org/md2k/mguard/dd40c/phone/gps/manifest");
  // -------------------------

  // sleep to init kp-abe producer
  std::this_thread::sleep_for (std::chrono::seconds(1));
}

void
Publisher::connectHandler(const mguard::util::AsyncRepoError& err)
{
  if (!err)
    NDN_LOG_DEBUG("Connection successful");
  else
    NDN_LOG_DEBUG("Connection failled");
}

void
Publisher::writeHandler(const ndn::Data& data, const mguard::util::AsyncRepoError& err)
{
  if (!err)
    NDN_LOG_DEBUG(data.getName() << " inserted into the repo");
  else
    NDN_LOG_DEBUG("failed to insert: " << data.getName());
}

void
Publisher::doUpdate(ndn::Name manifestName)
{
  m_partialProducer.publishName(manifestName);
  uint64_t seqNo =  m_partialProducer.getSeqNo(manifestName).value();
  std::cout << "sequence number: " << seqNo << std::endl;
  NDN_LOG_DEBUG("Publish sync update for the name/manifest: " << manifestName << " sequence Number: " << seqNo);
}

void
Publisher::scheduledManifestForPublication(util::Stream& stream)
{
  auto name = stream.getName();
  auto itr = m_scheduledIds.find(name);
  NDN_LOG_DEBUG("Scheduling manifest: " << name << " for publication");
  
  if (itr != m_scheduledIds.end()) {
    NDN_LOG_DEBUG("Manifest: " << name << " was already scheduled, updating the schedule");
    itr->second.cancel();
    auto scheduleId = m_scheduler.schedule(MAX_UPDATE_WAIT_TIME, [&] {
                      publishManifest(stream);
                      doUpdate(stream.getManifestName());
                      NDN_LOG_DEBUG("Updated manifest: " << stream.getName() << " via scheduling");
                    });
    itr->second = scheduleId;
  }
  else {
    auto scheduleId = m_scheduler.schedule(MAX_UPDATE_WAIT_TIME, [&] {
                      publishManifest(stream);
                      doUpdate(stream.getManifestName());
                      NDN_LOG_DEBUG("Updated manifest: " << stream.getName() << " via scheduling");
                    });
    m_scheduledIds.emplace(name, scheduleId);
  }
}

void
Publisher::publish(ndn::Name& dataName, std::string data, util::Stream& stream,
                   std::vector<std::string> semLocAttrList)
{ 
  // create a manifest, and append each <data-name>/<implicit-digetst> to the manifest
  // Manifest name: <stream name>/manifest/<seq-num>
  NDN_LOG_DEBUG("Publishing data: " << data << " and size: " << data.size());

  std::shared_ptr<ndn::Data> enc_data, ckData;
  try {
      NDN_LOG_DEBUG("Encrypting data: " << dataName);
      auto dataSufix = dataName.getSubName(3); // gives suffix except /ndn/org/md2k/..... 
      NDN_LOG_TRACE("--------- data suffix: " << dataSufix);

      auto attrList = stream.getAttributes();

      // debugging
      for (auto& a: attrList)
        NDN_LOG_DEBUG("attribute: " << a);

      if (semLocAttrList.size() > 0)
        attrList.insert(attrList.end(), semLocAttrList.begin(), semLocAttrList.end());

      std::tie(enc_data, ckData) = m_abe_producer.produce(dataSufix, attrList,
                                                          {reinterpret_cast<const uint8_t *>(data.c_str()), data.size()});

  }
  catch(const std::exception& e) {
    NDN_LOG_ERROR("Encryption failled");
    std::cerr << e.what() << '\n';
    // return false;
  }
  //  encrypted data is created, store it in the buffer and publish it
  NDN_LOG_INFO("full name of the data: " << enc_data->getFullName() << " and size: " << enc_data->getContent().size());
  NDN_LOG_INFO("full name of the ckData: " << ckData->getFullName() << " and size: " << ckData->getContent().size());

  

  try {
    NDN_LOG_INFO("start repo insertion for name: " << enc_data->getName());
    
    // insert data and CK data into repo
    m_asyncRepoInserter.AsyncWriteDataToRepo(*ckData, std::bind(&Publisher::writeHandler, this, _1, _2));
    m_asyncRepoInserter.AsyncWriteDataToRepo(*enc_data, std::bind(&Publisher::writeHandler, this, _1, _2));
  }
  catch(const std::exception& e) {
      NDN_LOG_ERROR("data and cKdata insertion failed");
      std::cerr << e.what() << '\n';
  }


  bool doPublishManifest = stream.updateManifestList(enc_data->getFullName());

  // manifest are publihsed to sync after receiving X (e.g. 10) number of application data or if
  // "t" time has passed after receiving the last application data.
  if(doPublishManifest) {
    cancleIfManifestScheduledForPublication(stream.getName());
    // create manifest data packet, and insert it into the repo
    publishManifest(stream);
    doUpdate(stream.getManifestName());
  }
  else
    scheduledManifestForPublication(stream);
}

void
Publisher::publishManifest(util::Stream& stream)
{
  auto dataName = stream.getManifestName();
  m_partialProducer.addUserNode(dataName);

  auto prevSeqNum = m_partialProducer.getSeqNo(stream.getManifestName()).value();
  dataName.appendNumber(prevSeqNum + 1);
  auto manifestData = std::make_shared<ndn::Data>(dataName);
  
  m_temp = stream.getManifestList();
  manifestData->setContent(wireEncode());
  NDN_LOG_DEBUG ("Manifest name: " << dataName << " manifest data size: " << manifestData->getContent().size() << " and seqNumber: " << prevSeqNum + 1);
  
  m_keyChain.sign(*manifestData);

  try {
      NDN_LOG_INFO("start repo insertion for name: " << manifestData->getName());
      m_asyncRepoInserter.AsyncWriteDataToRepo(*manifestData, std::bind(&Publisher::writeHandler, this, _1, _2));

      m_temp.clear(); // clear temp variable
      stream.resetManifestList(); // clear manifest list
      m_wire.reset(); // reset the wire for new content
  }
  catch(const std::exception& e) {
    NDN_LOG_ERROR("Failed to insert mainfest into the repo");
    std::cerr << e.what() << '\n';
  }
}

const ndn::Block&
Publisher::wireEncode() const
{
  if (m_wire.hasWire()) {
    m_wire.reset();
  }

  ndn::EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  ndn::EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}

template <ndn::encoding::Tag TAG>
size_t
Publisher::wireEncode(ndn::EncodingImpl<TAG> &encoder) const
{
  size_t totalLength = 0;
  
  for (auto it = m_temp.rbegin(); it != m_temp.rend(); ++it) {
    NDN_LOG_DEBUG ("Encoding data name: " << *it);
    totalLength += it->wireEncode(encoder);
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(mguard::tlv::mGuardPublisher);
  
  return totalLength;
}
} //mguard
