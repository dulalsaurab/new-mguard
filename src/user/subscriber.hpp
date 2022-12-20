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

#ifndef MGUARD_SUBSCRIBER_HPP
#define MGUARD_SUBSCRIBER_HPP

#include <PSync/consumer.hpp>
#include <nac-abe/attribute-authority.hpp>
#include <nac-abe/consumer.hpp>

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/face.hpp>

#include <functional>
#include <string>
#include <chrono>
#include <thread>

using namespace ndn::time_literals;

namespace mguard {

struct ApplicationData
{
  ndn::Name dataName;
  std::string dataRows;
};

typedef std::function<void(const std::vector<std::string>& updates)> DataCallback;

typedef std::function<void(const std::unordered_set<ndn::Name>& streams)> SubscriptionCallback;


namespace subscriber {
namespace tlv {
}

class Error : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

class Subscriber
{
public:
  Subscriber(const ndn::Name& consumerPrefix, const ndn::Name& syncPrefix,
             const ndn::Name& controllerPrefix, const std::string& consumerCertPath,
             const std::string& aaCertPath, ndn::time::milliseconds syncInterestLifetime,
             const DataCallback& callback, const SubscriptionCallback& subCallback);

  void
  run();

  void
  stop();

  ~Subscriber()
  {
    m_face_thread.join();
  }

  std::vector<ndn::Name>&
  getSubscriptionList()
  {
    return m_subscriptionList;
  }

  uint64_t
  getLowSeqOfPrefix(const ndn::Name& prefix)
  {
    auto it = m_prefixToLowSeq.find(prefix);
    return (it == m_prefixToLowSeq.end()) ? NOT_AVAILABLE : it->second;
  }

  void
  setLowSeqOfPrefix(const ndn::Name& prefix, uint64_t seqNum)
  {
    auto it = m_prefixToLowSeq.find(prefix);
    if (it == m_prefixToLowSeq.end()) // prefix doesn't exist in the map
      m_prefixToLowSeq.emplace(prefix, seqNum);
    else
      it->second = seqNum;
  }

  void
  setSubscriptionList(const std::vector<ndn::Name>& subList)
  {
    m_subscriptionList = subList;
  }

  bool
  checkConvergence();

  void
  expressInterest(const ndn::Name& name, bool canBePrefix = false, bool mustBeFresh = false);

  void
  onData(const ndn::Interest& interest, const ndn::Data& data);

  void
  onTimeout(const ndn::Interest& interest);

  void
  subscribe(ndn::Name streamName);
  
  void
  unsubscribe(ndn::Name streamName);

  void
  receivedHelloData(const std::map<ndn::Name, uint64_t>& availStreams);

  void
  receivedSyncUpdates(const std::vector<psync::MissingDataInfo>& updates);

  void
  sendInterest();
  
  void
  wireDecode(const ndn::Block& wire);

  // abe callbacks
  void
  abeOnData(const ndn::Buffer& buffer, const ndn::Name& name);
  
  void
  abeOnError(const std::string& errorMessage, const ndn::Name& name);

private:
  ndn::Face m_face;
  ndn::security::KeyChain m_keyChain;
  ndn::Scheduler m_scheduler;
  std::thread m_face_thread;

  ndn::Name m_consumerPrefix;
  ndn::Name m_syncPrefix;
  ndn::Name m_controllerPrefix;
  std::unordered_map<ndn::Name, uint64_t> m_prefixToLowSeq;
  std::vector<ndn::Name> m_subscriptionList;
  bool m_subscribe;

  // available streams are the ones received from psync
  // and eligible streams are determined from the policy
  std::unordered_map<ndn::Name, uint64_t> m_availableStreams; // name, sequence number
  std::unordered_set<ndn::Name> m_eligibleStreams;
  std::map<ndn::Name, int> m_retransmissionCount;
  ndn::nacabe::algo::PrivateKey decryptionKey;
  ndn::nacabe::Consumer m_abe_consumer;

  psync::Consumer m_psync_consumer;
  DataCallback m_ApplicationDataCallback;
  SubscriptionCallback m_subCallback;
};

} //namespace subscriber
} //namespace mguard

#endif // MGUARD_SUBSCRIBER_HPP