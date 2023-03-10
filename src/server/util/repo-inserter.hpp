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


#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/logger.hpp>

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <chrono>
#include <deque>

/**
 * A repo inserter using TCP bulk insertion protocol
 */

namespace mguard {
namespace util {

class RepoInserter 
{
public:

  RepoInserter(std::string repoHost = "localhost", std::string repoPort = "7376");

  bool
  writeDataToRepo(const ndn::Data &data);
  
private:
  std::string m_repoHost;
  std::string m_repoPort;
  boost::asio::io_service m_ioService;
};

} // util
} // mguard
