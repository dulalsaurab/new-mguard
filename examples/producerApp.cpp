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
 
#include "server/data-adapter.hpp"
#include "server/util/stream.hpp"
#include "common.hpp"

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include <chrono>
#include <unistd.h>
#include <thread>

struct streamInfo
{
  std::string streamName;
  std::vector<std::string> attributes;
  std::string dataPath;
};

class mGuardProducer
{
public:

  mGuardProducer(ndn::Name& producerPrefix, const std::string& producerCertPath,
                 ndn::Name& aaPrefix, const std::string& aaCertPath,
                 const std::string& dbname, const std::string& attributeMappingFilePath)
  : m_scheduler(m_face.getIoService())
  , m_dataAdaptor(m_face, producerPrefix, producerCertPath, aaPrefix, aaCertPath,
                  dbname, attributeMappingFilePath)
  {
  }
  
 void
 handler()
 {
  m_dataAdaptor.run();
 }

private:
  ndn::Face m_face;
  ndn::Scheduler m_scheduler;
  mguard::DataAdapter m_dataAdaptor;
};


int main ()
{
  /**
   * @producerPrefix data producers' identity or name prefix
   * @aaPrefix attribute authority'd name
   * @dbname database name to store lookup tables (e.g. gps_semlocation)
   * @aaCertPath path to attribute authority certificate
   * @producerCertPath path to data producer's certificate
   * @attributeMappingFilePath path to a file that maps attribute to its corresponding table
   *  e.g. attribute home,gym are mapped to semantic_location table
  */
  ndn::Name producerPrefix = "/ndn/org/md2k";
  ndn::Name aaPrefix = "/ndn/org/md2k/mguard/aa";
  std::string dbname = "lookup.db";
  std::string aaCertPath = "certs/aa.cert";
  std::string producerCertPath = "certs/producer.cert";
  std::string attributeMappingFilePath = "attribute_mapping_table.info";

  mGuardProducer producerApp (producerPrefix, producerCertPath, aaPrefix, aaCertPath, dbname, attributeMappingFilePath);
  producerApp.handler();
}
