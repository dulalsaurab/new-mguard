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


#include "../common.hpp"
#include "data-adapter.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <typeinfo>
#include <optional>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

NDN_LOG_INIT(mguard.DataAdapter);

namespace mguard {

ConnectionHandler::ConnectionHandler(boost::asio::io_service& io_service, 
                                     const Callback& callbackFromController)
: sock(io_service)
, m_onReceiveDataFromClient(callbackFromController)
{
}

void
ConnectionHandler::start()
{
  // NOTE: This might be causing error aiso:2, refer log for more detail.
  async_read(sock, response_,
             boost::bind(&ConnectionHandler::readHandle,
             shared_from_this(),
             boost::asio::placeholders::error,
             boost::asio::placeholders::bytes_transferred));

  sock.async_write_some(
      boost::asio::buffer(message, max_length),
      boost::bind(&ConnectionHandler::writeHandle,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
}

void 
ConnectionHandler::readHandle(const boost::system::error_code& err, size_t bytes_transferred)
{
  NDN_LOG_DEBUG("bytes_transferred: " << bytes_transferred);
  
  if (err) {
    NDN_LOG_DEBUG("Error: " << err);
    // sock.close();
    // return; // SD: is this a better way to handle ?
  }

  if (response_.size() <= 0) {
    NDN_LOG_DEBUG("Missing content");
    sock.close();
    return; // SD: is this a better way to handle ?
  }

  std::string dataReceived = boost::asio::buffer_cast<const char*>(response_.data());
  boost::property_tree::ptree pt;
  std::istringstream ss(dataReceived);
  boost::property_tree::read_json(ss, pt);
  
  auto streamMetaData = pt.get<std::string>("header");
  auto streamName = pt.get<std::string>("header.name");
  auto payload = pt.get<std::string>("payload");

  NDN_LOG_INFO("Data received for the following stream: "<< streamName);
  NDN_LOG_TRACE("Metadata of the stream: "<< streamName << " = " << streamMetaData);

  m_onReceiveDataFromClient(streamName, streamMetaData, payload);
  sock.close();
}

void 
ConnectionHandler::writeHandle(const boost::system::error_code& err, size_t bytes_transferred)
{
  if (err) {
    NDN_LOG_ERROR("error: " << err.message());
    sock.close();
    return;
  }
  NDN_LOG_INFO("Total bytes transfered: " << bytes_transferred);
  NDN_LOG_INFO("Server sent Hello message!");
}

Receiver::Receiver(boost::asio::io_service& io_service, const Callback& callbackFromReceiver)
: acceptor_(io_service, tcp::endpoint(tcp::v4(), 15000))
, m_onReceiveDataFromController(callbackFromReceiver)
{
  startAccept();
}

void 
Receiver::startAccept()
{
  // ConnectionHandler::pointer
  auto connection = ConnectionHandler::create(GET_IO_SERVICE(acceptor_),
                                              std::bind(&Receiver::processCallbackFromController,
                                              this, _1, _2, _3));

  acceptor_.async_accept(connection->socket(),
                         boost::bind(&Receiver::handleAccept, this, connection,
                         boost::asio::placeholders::error));
}


void
Receiver::processCallbackFromController(const std::string& streamName,
                                        const std::string& metaData,
                                        const std::string& response)
{
  // Check if the metaData is empty, and there is no response
  if (!(response.empty()))
    m_onReceiveDataFromController(streamName, metaData, response);

  // Do nothing
  NDN_LOG_DEBUG("Didn't receive any data from the receiver");
}

void 
Receiver::handleAccept(ConnectionHandler::pointer connection, const boost::system::error_code& err)
{
  if (!err) { connection->start();}

  startAccept();
}

DataAdapter::DataAdapter(ndn::Face& face, const ndn::Name& producerPrefix,
                         const std::string& producerCertPath,
                         const ndn::Name& aaPrefix, const std::string& aaCertPath,
                         const std::string& lookupDatabase,
                         const std::string& attributeMappintFilePath)
: m_face(face)
, m_producerPrefix(producerPrefix)
, m_producerCert(*loadCert(producerCertPath))
, m_ABE_authorityCert(*loadCert(aaCertPath))
, m_attrMappingProcessor(attributeMappintFilePath)
, m_publisher(m_face, m_keyChain, m_producerPrefix, m_producerCert,
              m_ABE_authorityCert, m_attrMappingProcessor.getStreamNames())
, m_receiver(m_face.getIoService(), 
             std::bind(&DataAdapter::processCallbackFromReceiver, this, _1, _2, _3))
, m_dataBase(lookupDatabase)
{
  NDN_LOG_DEBUG ("Initialized data adaptor and publisher");
  NDN_LOG_DEBUG ("---------------------------------------------");
  NDN_LOG_DEBUG ("Producer cert: " << m_producerCert);
  NDN_LOG_DEBUG ("---------------------------------------------");
  NDN_LOG_DEBUG ("ABE authority cert: " << m_ABE_authorityCert);
}

ndn::Name
DataAdapter::makeDataName(ndn::Name streamName, std::string timestamp)
{
  NDN_LOG_TRACE("Creating data name from streamName: " << streamName << " and timestamp: " << timestamp);
  return streamName.append("DATA").append(timestamp);
}

void
DataAdapter::processCallbackFromReceiver(const std::string& streamName, const std::string& metaData,
                                         const std::string& streamContent)
{
  NDN_LOG_DEBUG("Received data from the receiver for streamName: " << streamName);
  auto content = m_fileProcessor.getVectorByDelimiter(streamContent, "\n", 1);

  if (streamName == SEMANTIC_LOCATION) {
    // insert the data into the lookup table
    NDN_LOG_DEBUG("Received semantic location data");
    m_dataBase.insertRows(content);
  }

  auto streamNDNName = std::regex_replace(streamName, std::regex("--"), "/"); // convert to ndn name
  publishDataUnit(streamNDNName, metaData, content);
}

void
DataAdapter::run()
{
  try {
    m_face.processEvents();
  }
  catch (const std::exception& ex)
  {
    NDN_LOG_ERROR("Face error: " << ex.what()); 
    NDN_THROW(Error(ex.what()));
  }
}

void
DataAdapter::stop()
{
  NDN_LOG_DEBUG("Shutting down face: ");
  m_face.shutdown();
}

void
DataAdapter::publishDataUnit(ndn::Name streamName, const std::string& metaData,
                             const std::vector<std::string>& dataSet)
{
  NDN_LOG_INFO("Processing stream: " << streamName);

  // first process/publish the metadata
  auto metaDataName = streamName;
  // version number will be extracted from the metaData itself.
  // right now the information is not available there
  // naming /<stream-name>/metadata/<version-number>
  metaDataName.append("metadata/v1");
  m_publisher.publish(metaDataName, metaData, {streamName.toUri()}, streamName);

  // next, process/publish each individual data stream
  for (auto data : dataSet)
  {
    char timestamp [80];
    struct tm tm;

    // Get timestamp from the data row
    std::string delimiter = ",";
    m_tempRow = data;
    auto _tvec = m_fileProcessor.getVectorByDelimiter(m_tempRow, delimiter);
    auto timestamp_unprocessed = _tvec[1];

    NDN_LOG_DEBUG("Unprocessed data timestamp: " << timestamp_unprocessed);

    if (strptime(timestamp_unprocessed.c_str(), "%Y-%m-%d %H:%M:%S", &tm)) {
      std::strftime(timestamp,80,"%Y%m%d%H%M%S",&tm);
      NDN_LOG_DEBUG("Converted timestamp format: " << timestamp);
    }

    auto dataName = makeDataName(streamName, timestamp);
    NDN_LOG_DEBUG ("Publishing data name: " << dataName << " with timestamp: " << timestamp);

    /*
      Here, we need to modify the semantic location table checking process. One possible
      solution is to implement a 'getAttribute' function that can check all possible
      lookups and retrieve all attributes that will be applied
    */
    std::vector<std::string> attrList= {streamName.toUri()};
    try {
      auto semAttr = m_dataBase.getSemanticLocations(std::string(timestamp));
      if (!semAttr.empty()){
        for (auto& attr: semAttr) {
          auto _semLocAttr = mguard::util::getNdnNameFromSemanticLocationName(attr);
          NDN_LOG_TRACE("Semanantic location attribute: " << _semLocAttr);
          attrList.push_back(_semLocAttr.toUri());
        }
      }
      else {
        NDN_LOG_DEBUG("Context attribute doesn't exist for this data");
      }
    }
    catch (const std::exception& ex) {
      NDN_LOG_DEBUG("Couldn't get semantic location attribute for timestamp: " << timestamp);
    }

    // std::optional<ndn::Name> name = streamName;
    m_publisher.publish(dataName, data, attrList, streamName);
  }
}

} //mguard