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


#include "stream.hpp"
#include "../../common.hpp"

#include <ndn-cxx/util/logger.hpp>

namespace mguard {
namespace util {

NDN_LOG_INIT(mguard.Stream);

Stream::Stream(const ndn::Name& streamName)
: m_streamName(streamName)
, m_manifestCounter (0)
{
  m_manifestName = m_streamName;
  m_manifestName.append("MANIFEST");

  NDN_LOG_DEBUG("Stream Name: " << m_streamName);
  NDN_LOG_DEBUG("Manifest name: " << m_manifestName);
}

bool
Stream::updateManifestList(const ndn::Name& dataNameWithDigest)
{
  m_manifestList.push_back(dataNameWithDigest);
  if (m_manifestCounter == MANIFEST_BATCH_SIZE - 1)
    {
      m_manifestCounter = 0; // reset the counter
      return true; // time to publish the manifest
    }
  
  ++m_manifestCounter;
  return false;
}

} // util
} // mguard