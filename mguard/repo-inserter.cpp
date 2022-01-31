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

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <chrono>
#include <deque>

/**
 * A repo inserter using TCP bulk insertion protocol
 */
class RepoInserter {
public:

  RepoInserter(std::string repoHost = "localhost", std::string repoPort = "7376")
  : m_repoHost(repoHost),
    m_repoPort(repoPort)
  {}

  bool
  writeDataToRepo(const Data &data) {
    boost::asio::ip::tcp::iostream requestStream;
#if BOOST_VERSION >= 106600
    requestStream.expires_after(std::chrono::seconds(3));
#else
    requestStream.expires_from_now(boost::posix_time::seconds(3));
#endif //BOOST_VERSION >= 106600
    requestStream.connect(m_repoHost, m_repoPort);
    if (!requestStream) {
      std::cerr << "ERROR: Cannot publish the data to repo-ng"
                << " (" << requestStream.error().message() << ")" << std::endl;
      return false;
    }
    requestStream.write(reinterpret_cast<const char *>(data.wireEncode().wire()),
                        data.wireEncode().size());
    return true;
  }
private:
  std::string m_repoHost;
  std::string m_repoPort;
  boost::asio::io_service m_ioService;
};