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

#include <user/subscriber.hpp>
#include <common.hpp>

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/face.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <numeric>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

using namespace ndn::time_literals;

NDN_LOG_INIT(mguard.examples.consumerApp);

static void
usage(const boost::program_options::options_description& options)
{
  NDN_LOG_INFO("Usage: ndnsd-consumer [options] e.g. printer \n" << options);
   exit(2);
}

class mGuardConsumer
{
public:

 mGuardConsumer(ndn::Name& consumerPrefix, ndn::Name& syncPrefix, ndn::Name& controllerPrefix,
                std::string& consumerCertPath, std::string& aaCertPath)
 : m_subscriber(consumerPrefix, syncPrefix, controllerPrefix,
                 consumerCertPath, aaCertPath,
                 std::bind(&mGuardConsumer::processDataCallback, this, _1),
                 std::bind(&mGuardConsumer::processSubscriptionCallback, this, _1),
                 1000_ms)
  {
  }

  void
  processDataCallback(const std::map<std::string, std::string>& updates)
  {
    std::cout <<"\n" << std::endl;
    for (auto const& x : updates)
    {
      std::string stream_name_detail = x.first;
      std::string stream_data = x.second;
      NDN_LOG_INFO("Received data: " << x.second << " for name: " << x.first);
      NDN_LOG_INFO("Stream: " << stream_name_detail << ", data: " << stream_data);
    }
  }

  void
  processSubscriptionCallback(const std::unordered_set<ndn::Name>& streams)
  {
  
    std::cout<< "\n\nStreams available for subscription" <<std::endl;
    NDN_LOG_INFO("\n\nStreams available for subscription");
    std::vector<ndn::Name> availableStreams, subscriptionList;
  
    int counter=0;
    if (streams.size() <= 0) {
      NDN_LOG_INFO("No eligible stream found for your policy");
    }
    
    NDN_LOG_INFO("Stream/s available for subscription");

    for (auto &a : streams) {
      NDN_LOG_INFO(++counter << ": " << a);
      availableStreams.push_back(a);
    }

    std::vector<int> input; //
    NDN_LOG_INFO("enter selection, enter any char to stop");

    while(!std::cin.fail()) {
      int value;
      std::cin >> value;
      if(!std::cin.fail())
        input.push_back(value);
    }

    NDN_LOG_INFO("Subscribed to the stream/s");
    for (auto k : input)
    {
      auto ind = k-1;
      if (availableStreams[ind] != "/") // todo: fix this
        m_subscriber.subscribe(availableStreams[ind]);
      NDN_LOG_DEBUG("Subscribed to the stream/s" << availableStreams[ind]);
    }
  }

  void
  handler() { 
    m_subscriber.run(); 
  }

  private:
    ndn::Face m_face;
    mguard::subscriber::Subscriber m_subscriber;
};

int
main(int argc, char* argv[])
{
  std::string applicationPrefix;
  std::string certPath;

  namespace po = boost::program_options;
  po::options_description visibleOptDesc("Options");

  visibleOptDesc.add_options()
    ("help,h",      "print this message and exit")
    ("applicationPrefix,p", po::value<std::string>(&applicationPrefix)->required(),
      "application prefix, this name needs to match the one controller has")
    ("certificatePath,c", po::value<std::string>(&certPath), " location of consumer certificate")
  ;
  
  try
  {
    po::variables_map optVm;
    po::store(po::parse_command_line(argc, argv, visibleOptDesc), optVm);
    po::notify(optVm);

    if (optVm.count("applicationPrefix")) {
      if (applicationPrefix.empty())
      {
        std::cerr << "ERROR: applicationPrefix cannot be empty" << std::endl;
        usage(visibleOptDesc);
      }
    }
    if (optVm.count("certificatePath")) {
      if (certPath.empty())
      {
        std::cerr << "ERROR: certificatePath cannot be empty" << std::endl;
        usage(visibleOptDesc);
      }
    }
  }
  catch (const po::error& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    usage(visibleOptDesc);
  }

  ndn::Name consumerPrefix(applicationPrefix);
  ndn::Name syncPrefix = "/ndn/org/md2k";
  ndn::Name controllerPrefix = "/ndn/org/md2k/mguard/controller";
  std::string aaCertPath = "certs/aa.cert";
  mGuardConsumer consumer (consumerPrefix, syncPrefix, controllerPrefix, certPath, aaCertPath);
  consumer.handler();
}
