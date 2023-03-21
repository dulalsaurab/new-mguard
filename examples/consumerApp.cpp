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
                 consumerCertPath, aaCertPath, 1000_ms,
                 std::bind(&mGuardConsumer::processDataCallback, this, _1),
                 std::bind(&mGuardConsumer::processSubscriptionCallback, this, _1))
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
      // get name of stream
      int firstDelPos = stream_name_detail.find("/data_analysis/") +14 ;
      // Find the position of second delimiter
      int secondDelPos = stream_name_detail.find("/DATA/");
      // Get the substring between two delimiters
      std::string strbetweenTwoDels = stream_name_detail.substr(firstDelPos+1, secondDelPos-firstDelPos-1); 
      // std::cout << strbetweenTwoDels << std::endl;

      std::string stream_name = strbetweenTwoDels;
      // std::string formatted;
      stream_name_data[stream_name].append(x.second);
      stream_name_data[stream_name].append("\n");

      if (stream_name == "gps_episodes_and_semantic_location"){
        std::cout << std::left << " " << "semantic_location" << " | ";
        printTabularSemantic(stream_data);
      }
      if (stream_name == "mguard/dd40c/phone/gps"){
        std::cout << std::left << " " << "gps" << " | ";
            printTabularGps(stream_data);

      }
      if (stream_name == "mguard/dd40c/phone/battery"){
        std::cout << std::left << " " << "Battery" << " | ";
            printTabularBat(stream_data);

      }
      // std::cout << std::left << "___________________________________________________________________________________________________________________________________"<<std::endl;


      
      
    }
    writeToFile();
    // printTabular();
  }

std::vector<std::string> semanticData(std::string s){
    std::stringstream test(s);
    std::string segment;
    std::vector<std::string> segList;
    std::vector<std::string> finalList;
    std::string finalString;
    while(std::getline(test, segment, '"'))
    {
    segList.push_back(segment);
    }
    std::string sub;
    std::stringstream sub_text(segList[0]);
    while(std::getline(sub_text, sub, ','))
    {
    finalList.push_back(sub);
    }
    finalList.push_back(segList[1]);
    std::string sub2;
    std::stringstream sub_text2(segList[2]);
    while(std::getline(sub_text2, sub2, ','))
    {
    finalList.push_back(sub2);
    }

    // for (auto const& s : finalList) finalString += "#" +s;
    return finalList;
}

std::string batteryGpsData(std::string s ){
    // std::string s = "4,2022-05-02 10:00:47,2022-05-02 05:00:47,99.52999999999976,3700,70,dd40c,1";
    std::stringstream test(s);
    std::string segment;
    std::vector<std::string> segList;
    std::vector<std::string> finalList;
    std::string finalString;

    while(std::getline(test, segment, ' '))
    {
    segList.push_back(segment);
    }
    // # this happens 3 times for gps data 
    for ( auto a : segList){
        std::string sub;
        std::stringstream sub_text(a);
        while(std::getline(sub_text, sub, ','))
        {
        finalList.push_back(sub);
        }
    }

    for (auto const& s : finalList) { finalString += "#" +s; }
    // std::cout << finalString <<std::endl;
    return finalString;

}


void printTabularGps(std::string stream_data){
    
      const char separator    = '  |  ';
      const int nameWidth     = 7;
      std::string substr;
      std::vector<std::string> result;
      std::stringstream test(stream_data);

       while(std::getline(test, substr, ','))
        {
        result.push_back(substr);
        }

      for (int i; i< result.size(); i++){
          if(i == 3 || i ==4 ){
            std::cout << std::left << std::setw(25) << std::setfill(separator) << result[i] <<"  |  ";
          }
          else if(i == 6 || i ==7 || i == 8){
          std::cout << std::left << std::setw(10) << std::setfill(separator) << result[i] <<"  |  ";

          }
          else{
          std::cout << std::left << std::setw(5) << std::setfill(separator) << result[i] <<"  |  ";

          }
      }
      int length = stream_data.length() + 5*5;
      // std::cout << std::left << " \n ";
      // for (int i = 0; i <length; i++){
      //         std::cout << "-";
      // }
    }

void printTabularBat(std::string stream_data){
    
      const char separator    = '  |  ';
      const int nameWidth     = 7;
      std::string substr;
      std::vector<std::string> result;
      std::stringstream test(stream_data);

       while(std::getline(test, substr, ','))
        {
        result.push_back(substr);
        }

      for (int i; i< result.size(); i++){
          if(i == 3 ){
            std::cout << std::left << std::setw(25) << std::setfill(separator) << result[i] <<"  |  ";
          }
          else{
          std::cout << std::left << std::setw(5) << std::setfill(separator) << result[i] <<"  |  ";

          }
      }
      int length = stream_data.length() + 5*5;
      // std::cout << std::left << " \n ";
      // for (int i = 0; i <length; i++){
      //         std::cout << "-";
      // }
    }

void printTabularSemantic(std::string stream_data){
    
      const char separator    = '  |  ';
      const int nameWidth     = 8;

      std::vector<std::string> result;
      result = semanticData(stream_data);
  
      for (int i; i< result.size(); i++){
        // std::cout << " \n i is "<< i <<  " result us  "<< result [i] << "\n";
          if(i == 3  ){
            std::cout << std::left << std::setw(90) << std::setfill(separator) << result[i] <<"  |  ";
          }
          else if(i == 5  ){
            std::cout << std::left << std::setw(14) << std::setfill(separator) << result[i] <<"  |  ";
          }
          else{
          std::cout << std::left << std::setw(5) << std::setfill(separator) << result[i] <<"  |  ";

          }
      }
      int length = stream_data.length() + 5*5;
      // std::cout << std::left << " \n ";
      // for (int i = 0; i <length; i++){
      //         std::cout << "-";
      // }
    }


  void writeToFile(){


    std::ofstream file("output.txt");
    for (auto element :stream_name_data)
    {

    file << element.first;
    // if (element.first == "gps_episodes_and_semantic_location"){
      

    // } 
 
    file << element.second;
    }
  }

  void
  processSubscriptionCallback(const std::unordered_set<ndn::Name>& streams)
  {
     // wait until keys are fetched?
    // check for convergence.
    if (!m_subscriber.checkConvergence()) {
      NDN_LOG_DEBUG("couldnt fetch appropriate keys, exiting");
      exit(-1);
    }
    std::cout<< "\n\nStreams available for subscription" <<std::endl;

    NDN_LOG_INFO("\n\nStreams available for subscription");
    std::vector<ndn::Name> availableStreams, subscriptionList;
  
    int counter=0;
    auto strm_size = streams.size();
    if (streams.size() <= 0) {
      NDN_LOG_INFO("No eligible stream found for your policy");
    }

    for (auto &a : streams) {
      std::cout<< ++counter << ": " << a << std::endl ;
      NDN_LOG_INFO(++counter << ": " << a);
      availableStreams.push_back(a);
    }

    // // these codes are only for testing purposes
    // // automatically subscriber to the respective streams

    // // A. battery only 
    // // subscriptionList.push_back(availableStreams[0]); // battery

    // // all stream

        // for (std::size_t a = 0; a < strm_size ; a++){
        //   subscriptionList.push_back(availableStreams[a]);
        // }
    // subscriptionList.push_back(availableStreams[0]); // battery
    // subscriptionList.push_back(availableStreams[1]); // semloc
    // subscriptionList.push_back(availableStreams[3]); // gps
      

    // // not gps
    // // subscriptionList.push_back(availableStreams[0]); // battery
    // // subscriptionList.push_back(availableStreams[2]); // sem_loc

    // // accelerometer
    // // subscriptionList.push_back(availableStreams[0]); // battery

    // // only work
    // // subscriptionList.push_back(availableStreams[0]); // gps, only the one with attribute work should be accessible

    // for (auto& s: subscriptionList) {
    //   // m_subscriber.subscribe(s);
    //   NDN_LOG_DEBUG("Subscribed to the stream/s" << s); // << std::endl;
    // }


    // uncomment if: taking input from user ----------------------------------------------
    
    std::vector<int> input; //
    std::cout << "enter selection, enter any char to stop" << std::endl;


    while(!std::cin.fail())
    {
        int value;
        std::cin >> value;
        if(!std::cin.fail())
          input.push_back(value);
    }
    
    std::cout << "\n" << std::endl;
    for (auto k : input){
    // std::cout <<" values of k"<< k << std::endl;
    }

    std::cout << "Subscribed to the stream/s" << std::endl;
    for (auto k : input)
    {
      auto ind = k-1;
      // std::cout <<"k is"<< k << ": " << availableStreams[ind] << std::endl;
      if (availableStreams[ind] != "/") // todo: fix this
        subscriptionList.push_back(availableStreams[ind]);
        std::cout<<"Subscribed to the stream/s" << availableStreams[ind] <<std::endl; // << std::endl;

        NDN_LOG_DEBUG("Subscribed to the stream/s" << availableStreams[ind]); // << std::endl;

    }
   
    // taking input from user end----------------------------------------------

    m_subscriber.setSubscriptionList(subscriptionList);

    // unsubscribe thing is here

    // NDN_LOG_DEBUG("---------");
    // for(auto x: m_subscriber.getSubscriptionList())
    //   NDN_LOG_DEBUG("stream: "<< x);

    // m_subscriber.unsubscribe(availableStreams[3]);
    
    // NDN_LOG_DEBUG("---------");
    // for(auto x: m_subscriber.getSubscriptionList())
    //   NDN_LOG_DEBUG("stream: "<< x);
  }

  void
  handler() { m_subscriber.run(); }

private:
  ndn::Face m_face;
  mguard::subscriber::Subscriber m_subscriber;
  std::map<std::string, std::string> stream_name_data;

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
    ("applicationPrefix,p", po::value<std::string>(&applicationPrefix)->required(), "application prefix, this name needs to match the one controller has")
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
  // std::string consumerCertPath = "certs/A.cert";
  std::string aaCertPath = "certs/aa.cert";
  mGuardConsumer consumer (consumerPrefix, syncPrefix, controllerPrefix, certPath, aaCertPath);
  consumer.handler();
}
