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

#include <chrono>
#include <thread>

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>

#include "async-repo-inserter.hpp"

NDN_LOG_INIT(mguard.util.repoClient);

namespace mguard {
namespace util {
namespace bp = boost::asio::ip;

AsyncRepoInserter::AsyncRepoInserter(boost::asio::io_service& io)
  : m_io(io)
  , m_resolv(m_io)
  , m_socket(std::make_shared<bp::tcp::socket>(m_io))
{
}

void
AsyncRepoInserter::AsyncConnectToRepo(std::string repoHost, std::string repoPort, const AsyncConnectHandler& connectHandler)
{
  bp::tcp::resolver::query query(repoHost, repoPort);
  m_resolv.async_resolve(query, [this, connectHandler](auto& err, auto& it) {
    NDN_LOG_TRACE("Resolvation status: " << err.message());
    if (!err) {
      m_socket->async_connect(*it, [this, connectHandler] (auto& err) {
        NDN_LOG_DEBUG("Connnection status: " << err.message());
        connectHandler(err);
      });
    }
    else {
      NDN_THROW(std::runtime_error("Repo endpoint cannot be resolved: " << err.message()));
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