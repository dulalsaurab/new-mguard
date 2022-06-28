/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017-2020, Regents of the University of California.
 *
 * This file is part of ndncert, a certificate management system based on NDN.
 *
 * ndncert is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * ndncert is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received copies of the GNU General Public License along with
 * ndncert, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndncert authors and contributors.
 */

#include <chrono>
#include <thread>

#include <ndn-cxx/util/logger.hpp>

#include "async-repo-inserter.hpp"

NDN_LOG_INIT(mguard.util.repoClient);

namespace mguard {
namespace util {
namespace bp = boost::asio::ip;

AsyncRepoInserter::AsyncRepoInserter(boost::asio::io_service& io, std::string repoHost, std::string repoPort)
  : m_io(io)
  , m_resolv(m_io)
  , m_socket(std::make_shared<bp::tcp::socket>(m_io))
{
  bp::tcp::resolver::query query(repoHost, repoPort);
  m_resolv.async_resolve(query, [this](auto& err, auto& it) {
    NDN_LOG_TRACE("Resolvation status: " << err.message());
    if (!err) {
      m_socket->async_connect(*it, [this] (auto& err) {
        NDN_LOG_DEBUG("Connnection status: " << err.message());
        if (!err) {
          m_isConnected = true;
        }
      });    
    }
  });
}

void
AsyncRepoInserter::AsyncWriteDataToRepo(const ndn::Data &data, const AsyncWriteHandler& writeHandler)
{
  m_socket->async_send(boost::asio::buffer(data.wireEncode()), 
    [this, data, writeHandler](auto& err, auto&&) {
      writeHandler(data, err);
    }
  );
}

} // util
} // mguard