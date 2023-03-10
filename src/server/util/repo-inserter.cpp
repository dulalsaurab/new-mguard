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

#include "repo-inserter.hpp"

NDN_LOG_INIT(mguard.util.repoClient);

namespace mguard {
namespace util {

RepoInserter::RepoInserter(std::string repoHost, std::string repoPort)
: m_repoHost(repoHost),
  m_repoPort(repoPort)
{
}

bool
RepoInserter::writeDataToRepo(const ndn::Data &data)
{
  boost::asio::ip::tcp::iostream requestStream;
  #if BOOST_VERSION >= 106600
      requestStream.expires_after(std::chrono::seconds(3));
  #else
      requestStream.expires_from_now(boost::posix_time::seconds(3));
  #endif //BOOST_VERSION >= 106600
  requestStream.connect(m_repoHost, m_repoPort);
  if (!requestStream) {
    NDN_LOG_ERROR("ERROR: Cannot publish the data to repo" << " (" << requestStream.error().message() << ")");
    return false;
  }
  // for (auto& data: dataSet) {
    NDN_LOG_DEBUG("writing data " << data);
    requestStream.write(reinterpret_cast<const char *>(data.wireEncode().wire()),
                        data.wireEncode().size());
    NDN_LOG_INFO("write complete----------");
    // std::this_thread::sleep_for(std::chrono::milliseconds(20)); // sleep for 1 second
  // }

  return true;
}

} // util
} // mguard