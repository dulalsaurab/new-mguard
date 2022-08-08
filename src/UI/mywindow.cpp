
#include <user/subscriber.hpp>
#include <common.hpp>

#include <iostream>
#include "mywindow.hpp"
#include <fstream> // std::fstream
#include <string>
#include <sstream>
#include <algorithm>

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/face.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <chrono>
#include <thread>

using namespace ndn::time_literals;

NDN_LOG_INIT(mguard.ui);

mywindow::mywindow(ndn::Name &consumerPrefix, ndn::Name &syncPrefix, ndn::Name &controllerPrefix,
                   std::string &consumerCertPath, std::string &aaCertPath)
    : 
      m_subscriber(consumerPrefix, syncPrefix, controllerPrefix,
                   consumerCertPath, aaCertPath, 1000_ms,
                   std::bind(&mywindow::processDataCallback, this, _1),
                   std::bind(&mywindow::processSubscriptionCallback, this, _1)),
      m_WorkerThread(nullptr),
      m_Dispatcher()

{
  m_subscriber.run();
}

mywindow::~mywindow()
{
}

// convert file content to string
std::string mywindow::file_to_string(std::string filename)
{
  return "true";
}

void mywindow::on_changed(Glib::RefPtr<Gtk::TreeSelection> c)
{

 mywindow::update_view();
//   std::cout << "on changed function" << std::endl;
//   Gtk::TreeModel::iterator iter = c->get_selected();
//   if (iter) // If anything is selected
//   {
//     Gtk::TreeModel::Row row = *iter;
//     int id = row.get_value(m_Columns.m_id);

//     std::stringstream filename;
//     filename << "content" << id << ".txt";
//     std::string whole_file = mywindow::file_to_string(filename.str());
// // std::string whole_file =
//     ss_detail->get_buffer()->set_text(whole_file);
//   }
}

void mywindow::on_row(ndn::Name b)
{

  // if not in list add else remove from list
  if (std::find(subscriptionList.begin(), subscriptionList.end(), b) == subscriptionList.end())
  {
    subscriptionList.push_back(b);
  }
  else
  {
    subscriptionList.erase(std::remove(subscriptionList.begin(), subscriptionList.end(), b), subscriptionList.end());
  }
  mywindow::update_available_streams_view();
  mywindow::update_subscribed_streams_view();
}

void mywindow::processDataCallback(const std::vector<std::string> &updates)
{
   content.insert(content.end(),updates.begin(),updates.end());
  // print the data being shown here
  for (auto &a : updates)
    std::cout << a << std::endl;

}

void mywindow::show_ui()
{
  Glib::RefPtr<Gtk::Builder> ui = Gtk::Builder::create_from_file("src/UI/tabsbox.glade");
  if (ui)
  {
    ui->get_widget<Gtk::Box>("box", box);
    ss_tv = Glib::RefPtr<Gtk::TreeView>::cast_dynamic(
        ui->get_object("ss_tv"));
    policy = Glib::RefPtr<Gtk::Label>::cast_dynamic(
        ui->get_object("policy"));
    list_store = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(
        ui->get_object("list_store"));
    ss_detail = Glib::RefPtr<Gtk::TextView>::cast_dynamic(
        ui->get_object("ss_detail"));
    as_grid = Glib::RefPtr<Gtk::Grid>::cast_dynamic(
        ui->get_object("as_grid"));

    mywindow::update_available_streams_view();
    mywindow::update_subscribed_streams_view();

    policy->set_text("Your Policy : /ndn/org/md2k/mguard/dd40c/phone/accelerometer");

    std::cout << "end of UI function" << std::endl;
    m_Dispatcher.connect(sigc::mem_fun(*this, &mywindow::on_notification_from_worker_thread));


    int timeout_value = 5500; //in ms (1.5 sec)
    sigc::slot<bool>my_slot = sigc::mem_fun(*this, &mywindow::update_view);
    //connect slot to signal
    Glib::signal_timeout().connect(my_slot, timeout_value);
    add(*box);
  }
  set_title("MGuard");
  set_default_size(800, 400);
  show_all();
  std::cout << "Shows everything" << std::endl;
}

void mywindow::update_subscribed_streams_view()
{
  ss_tv->remove_all_columns();

  list_store = Gtk::ListStore::create(m_Columns);
  ss_tv->set_model(list_store);
  ss_tv->set_reorderable();
  ss_tv->set_headers_clickable(true);
  ss_tv->set_headers_visible(true);

  int counter = 1;
  for (auto &a : subscriptionList)
  {

    // TODO: what else to show in sub name details
    auto row = *(list_store->append());
    row[m_Columns.m_id] = counter;
    row[m_Columns.m_timestamp] = "12/12/2021";
    row[m_Columns.m_source] = a.toUri();
    row[m_Columns.m_info] = "1654907929.430571 DEBUG: [mguard.subscriber] ...";
    counter++;
    NDN_LOG_DEBUG("Subscribed to the stream/s" << a); // << std::endl;
  }
  // m_subscriber.setSubscriptionList(subscriptionList);

  // Add the TreeView's view columns:
  ss_tv->append_column("id", m_Columns.m_id);
  ss_tv->append_column("timestamp", m_Columns.m_timestamp);
  ss_tv->append_column("source", m_Columns.m_source);
  ss_tv->append_column("info", m_Columns.m_info);

  m_refTreeSelection = ss_tv->get_selection();
  m_refTreeSelection->signal_changed().connect(sigc::bind(sigc::mem_fun(*this, &mywindow::on_changed), m_refTreeSelection));
  show_all_children();
}
void mywindow::update_available_streams_view()
{

  for (auto &b : subscriptionList)
  {
    NDN_LOG_INFO(": " << b);
  }

  std::vector<Gtk::Widget *> children = as_grid->get_children();
  for (long unsigned int c = 0; c < children.size(); c++)
  {
    // std::cout << "removed child " << std::endl;
    as_grid->remove(*children[c]);
  }

  int counter = 0;

  for (auto &a : availableStreams)
  {
    std::string fullName = a.toUri();
    Gtk::Label *stream_name = Gtk::manage(new Gtk::Label());

    stream_name->set_markup(fullName);
    as_grid->attach(*stream_name, 1, counter, 1, 1);

    Gtk::Button *button = Gtk::manage(new Gtk::Button());
    Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();

    if (std::find(subscriptionList.begin(), subscriptionList.end(), availableStreams[counter]) == subscriptionList.end())
    {
      // Element in subscribed vector.
      css_provider->load_from_data(
          "button {background-image: image(#bcffb5); margin-top: 10px;  margin-left: 10px;}");
      button->get_style_context()->add_provider(
          css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
      button->set_label("subscribe");
      // std::cout << "updated sub view " << std::endl;
    }
    else
    {
      css_provider->load_from_data(
          "button {background-image: image(#ffc2b5); margin-top: 10px;  margin-left: 10px;}");
      button->get_style_context()->add_provider(
          css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
      button->set_label("unsubscribe");
      // std::cout << "updated un susub view " << std::endl;
    }

    as_grid->attach(*button, 2, counter, 1, 1);
    counter++;
    button->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &mywindow::on_row), a));
    std::cout << "updated view " << std::endl;
  }

  as_grid->show_all();

  // show_all_children();
}

void mywindow::processSubscriptionCallback(const std::unordered_set<ndn::Name> &streams)
{
  // check for convergence.
  m_subscriber.checkConvergence();

  // stop the process event
  m_subscriber.stop();

  NDN_LOG_INFO("\n\nStreams available for subscription");

  if (streams.size() <= 0)
  {
    NDN_LOG_INFO("No eligible stream found for your policy");
  }

  int counter = 0;
  // print avaliable streams here
  for (auto &a : streams)
  {
    NDN_LOG_INFO(++counter << ": " << a);
    availableStreams.push_back(a);
  }

  subscriptionList.push_back(availableStreams[0]); // battery
  subscriptionList.push_back(availableStreams[1]); // semloc
  subscriptionList.push_back(availableStreams[3]); // gps

  // these codes are only for testing purposes
  // automatically subscriber to the respective streams

  // A. battery only
  // subscriptionList.push_back(availableStreams[0]); // battery

  // all stream

  // not gps
  // subscriptionList.push_back(availableStreams[0]); // battery
  // subscriptionList.push_back(availableStreams[2]); // sem_loc

  // accelerometer
  // subscriptionList.push_back(availableStreams[0]); // battery

  // only work
  // subscriptionList.push_back(availableStreams[0]); // gps, only the one with attribute work should be accessible
  for (auto &s : subscriptionList)
  {
    // m_subscriber.subscribe(s);
    NDN_LOG_DEBUG("Subscribed to the stream/s" << s); // << std::endl;
                                                      // std::cout << "subscribed" << std::endl;
  }
  //

  // call this function on subscribed click or maybe create a new button for this??
  m_subscriber.setSubscriptionList(subscriptionList);
  // run the processevent again, this time with sync as well

  // rerunning the process event closes the UI
  // m_subscriber.run(true);
  // mywindow::show_ui();

  mywindow::show_ui();
  mywindow::startThread();

  // std::this_thread::sleep_for (std::chrono::seconds(5));

  // int i = 0;
  // while (i < 100000) {++i;}

  // m_subscriber.run(true);

  // m_subscriber.run();
}

void mywindow::callSubscriber(mywindow *caller)
{

  {
    Glib::Threads::Mutex::Lock lock(m_Mutex);
    m_has_stopped = false;

  } // The mutex is unlocked here by lock's destructor.
std::cout<<"started thread work"<< std::endl;
  Glib::Threads::Mutex::Lock lock(m_Mutex);
  m_subscriber.run(true);
  m_has_stopped = true;
  std::cout<<"ended"<< std::endl;

  lock.release();
  caller->notify();
}

void mywindow::startThread()
{
  std::cout << "start btn clicked" << std::endl;
  if (m_WorkerThread)
  {
    std::cout << "Can't start a worker thread while another one is running." << std::endl;
  }
  else
  {
    std::cout << "starting a thread " << std::endl;

    // Start a new worker thread.
    m_WorkerThread = Glib::Threads::Thread::create(
        sigc::bind(sigc::mem_fun(*this, &mywindow::callSubscriber), this));
  }
  //   update_start_stop_buttons();
}

void mywindow::notify()
{
  m_Dispatcher.emit();
}

void mywindow::on_notification_from_worker_thread()
{
  if (m_WorkerThread && m_has_stopped)
  {
    // Work is done.
    m_WorkerThread->join();
    m_WorkerThread = nullptr;
    std::cout << "DOne" << std::endl;
  }
}



bool mywindow::update_view()
{

if (!content.empty()){
// std::cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<std::endl;
    std::string joined = boost::algorithm::join(content, "\n ");

    Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer1 = Gtk::TextBuffer::create();
    std::string whole_file = joined;
    m_refTextBuffer1->set_text(whole_file);
    ss_detail->set_buffer(m_refTextBuffer1);
    show_all_children();
    return true;
}
return false;


  
}
