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

#ifndef MGUARD_STREAM_HPP
#define MGUARD_STREAM_HPP

#include <ndn-cxx/name.hpp>
#include <string>
#include <algorithm>
#include <regex>

namespace mguard {
namespace util {

static inline
ndn::Name
getNdnNameFromSemanticLocationName(std::string md2kname)
{
  ndn::Name semLocAttr("/ndn/org/md2k/attribute/location");
  semLocAttr.append(md2kname);
  return semLocAttr;
}

class Stream
{
public:
  Stream(const std::string& md2kName, std::vector<std::string> attributeSet = {});

  /*
   This function will update the manifest list and send status whether it is reedy to be published or not
   e.g. if 20 new data names are added or X ms/s time has passed since addition of the last data poing,
   manifest will be read to be published
   for time expiration, we need to use signal ??
  */
  bool
  updateManifestList(const ndn::Name& dataNameWithDigest);

  std::vector<ndn::Name>&
  getManifestList()
  {
    return m_manifestList;
  }

  /*
    once the manifest is published (inserted list to repo, need to reset it for fresh names)
  */
  void
  resetManifestList()
  {
    m_manifestList.clear();
  }

  ndn::Name&
  getName()
  {
    return m_name;
  }

	std::string&
	getMD2KName()
  {
    return m_md2kName;
  }

  void
  setUpdateManifestName(ndn::Name& name)
  {
    m_manifestName = name;
  }

	ndn::Name&
	getManifestName()
  {
    return m_manifestName;
  }

  // stream attribute
	void
	setAttributes(std::vector<std::string>& attributeSet)
  {
    m_attributeSet = attributeSet;
  }

  // stream attribute
	void
	updateAttributes(std::vector<std::string>& attributeSet)
  {
    std::vector<std::string> _comb = m_attributeSet;
    _comb.insert(_comb.end(), attributeSet.begin(), attributeSet.end());
    m_attributeSet = _comb;
  }

  // TODO: data points from a same stream can be encrypted with different set 
  // of attribtues, need to consider it in the future
  std::vector<std::string>&
  getAttributes()
  {
    return m_attributeSet;
  }

private:
  std::string m_md2kName;
  ndn::Name m_name;
  ndn::Name m_manifestName; // will contain the manifest name with latest sequence number
  std::vector<std::string> m_attributeSet;
  std::vector<ndn::Name> m_manifestList;
  int m_manifestCounter;

};
} // util
} // mguard


#endif //MGUARD_STREAM_HPP