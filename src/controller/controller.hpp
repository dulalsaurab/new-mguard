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
 
#ifndef MGUARD_CONTROLLER_HPP
#define MGUARD_CONTROLLER_HPP

#include "parser.hpp"
#include "common.hpp"

#include <nac-abe/attribute-authority.hpp>
#include <nac-abe/cache-producer.hpp>

#include <chrono>
#include <unistd.h>
#include <iostream>

namespace mguard {
namespace controller {

/*
  todo: refactor to mGuardController
  Contoller class does two taks a) runs the parser to process mGuard policy
  b) and runs attribute authority which serves public params and keys for producer and consumer
*/
class Controller
{
public:
  Controller(const ndn::Name& controllerPrefix, const ndn::Name& aaPrefix, 
             const std::string& aaCertPath,
             const std::map<ndn::Name, std::string>& requesterCertMap,
             const std::string& availableStreamsFilePath);

  ndn::nacabe::KpAttributeAuthority&
  getAttrAuthority()
  {
    return m_attrAuthority;
  }

  void
  run();

  const std::string
  getRequesterCertPath(const ndn::Name& requester)
  {
    auto it = m_requestersCertPath.find(requester);
    if (it != m_requestersCertPath.end())
      return it->second;
    
    return {}; // return empty string, can be better
  }

  /**
   * @brief read and parse the mGuard policy
   *
   * This function will read/parse the mGuard policy to generate NAC-ABE policy,
   * policy id, and accessible streams
   * @param policyPath path to mGuard policy
  */
  void
  processPolicy(const std::string& policyPath);

  void
  setInterestFilter(const ndn::Name& name, const bool loopback = false);

  void
  processInterest(const ndn::Name& name, const ndn::Interest& interest);

  void
  onRegistrationSuccess(const ndn::Name& name);

  void
  onRegistrationFailed(const ndn::Name& name);

  void
  sendData(const ndn::Name& name);

  void
  sendApplicationNack(const ndn::Name& name);

  template<ndn::encoding::Tag TAG>
  size_t
  wireEncode(ndn::EncodingImpl<TAG>& block);

  const ndn::Block&
  wireEncode();

private:
  
  /**
   * @brief store policy detail
  */
  struct policyDetails
  {
    std::string policyId;
    std::list <ndn::Name> streams;
    std::string abePolicy;
  };

  // ndn::Name --> requesterID or name or prefix, probably this will be cert 
  std::map<ndn::Name, policyDetails> m_policyMap;
  policyDetails m_temp_policyDetail; // we use this for encoding requested key/streams

  ndn::Face m_face;
  ndn::security::KeyChain m_keyChain;
  mutable ndn::Block m_wire;
  
  const ndn::Name& m_controllerPrefix;
  const ndn::Name& m_aaPrefix;
  const std::map<ndn::Name, std::string> m_requestersCertPath;
  parser::PolicyParser m_policyParser;
  ndn::nacabe::KpAttributeAuthority m_attrAuthority;

};

} // namespace controller
} // namespace mguard

#endif // MGUARD_CONTROLLER_HPP