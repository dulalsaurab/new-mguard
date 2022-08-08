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
