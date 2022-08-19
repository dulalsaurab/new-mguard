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
 
#include <controller/controller.hpp>
#include <iostream>

int main()
{
  // init controller
  std::string availableStreamsFilePath = "policies/available_streams";
  // std::string policyFilePath = "../policiespolicy1";
  std::string aaPrefix = "/ndn/org/md2k/mguard/aa";
  ndn::Name controllerPrefix = "/ndn/org/md2k/mguard/controller";
  std::string aaCertPath = "certs/aa.cert";

  std::map<ndn::Name, std::string> requesterCertMap;
  requesterCertMap.emplace("/ndn/org/md2k/A", "certs/A.cert");
  requesterCertMap.emplace("/ndn/org/md2k/B", "certs/B.cert");
  requesterCertMap.emplace("/ndn/org/md2k/C", "certs/C.cert");
  requesterCertMap.emplace("/ndn/org/md2k/D", "certs/D.cert");
  requesterCertMap.emplace("/ndn/org/md2k/E", "certs/E.cert");

  mguard::controller::Controller mGuardController(controllerPrefix, aaPrefix,
                                                  aaCertPath, requesterCertMap,
                                                  availableStreamsFilePath);

  mGuardController.run();
  return 0;
}
