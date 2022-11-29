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

#include <UI/mywindow.cpp>

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/face.hpp>

#include <gtkmm.h>
#include <glib.h>
#include <gtkmm/application.h>

#include <string>
#include <sstream>
#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

using namespace ndn::time_literals;
using namespace Glib;
using namespace Gtk;


int main(int argc, char *argv[])
{

  ndn::Name consumerPrefix = "/ndn/org/md2k/A";
  ndn::Name syncPrefix = "/ndn/org/md2k";
  ndn::Name controllerPrefix = "/ndn/org/md2k/mguard/controller";
  std::string consumerCertPath = "certs/A.cert";
  std::string certPath = "certs/A.cert";
  std::string aaCertPath = "certs/aa.cert";

  RefPtr<Application> app = Application::create(argc, argv);
  mywindow window(consumerPrefix, syncPrefix, controllerPrefix, certPath, aaCertPath);
  return app->run(window);
}
