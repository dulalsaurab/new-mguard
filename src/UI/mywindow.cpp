
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
#include <iterator>

using namespace ndn::time_literals;

NDN_LOG_INIT(mguard.ui);

mywindow::mywindow(ndn::Name &consumerPrefix, ndn::Name &syncPrefix, ndn::Name &controllerPrefix,
                   std::string &consumerCertPath, std::string &aaCertPath)
    : m_subscriber(consumerPrefix, syncPrefix, controllerPrefix,
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

void mywindow::get_stream_detail_data(Glib::RefPtr<Gtk::TreeSelection> c)
{
  // get data for that stream
    m_export_csv->set_sensitive(true); // true to enable the button 

  mywindow::update_stream_detail_view();
}

void mywindow::onSubscribeBtnClick(ndn::Name b)
{

  // if not in list add else remove from list
  if (std::find(m_subscriptionList.begin(), m_subscriptionList.end(), b) == m_subscriptionList.end())
  {
    m_subscriptionList.push_back(b);
  }
  else
  {
    m_subscriptionList.erase(std::remove(m_subscriptionList.begin(), m_subscriptionList.end(), b), m_subscriptionList.end());
  }

  mywindow::updateAvailableStreamsView();
  mywindow::updateSubscribedStreamsView();

  m_subscriber.setSubscriptionList(m_subscriptionList);
  mywindow::startThread();
}

void mywindow::processDataCallback(const std::vector<std::string> &updates)
{
  m_content.insert(m_content.end(), updates.begin(), updates.end());
  // print the data being shown here
  for (auto &a : updates)
    std::cout << a << std::endl;
}

void mywindow::showUI()
{
  Glib::RefPtr<Gtk::Builder> m_ui = Gtk::Builder::create_from_file("src/UI/tabsbox.glade");
  if (m_ui)
  {
    m_ui->get_widget<Gtk::Box>("m_box", m_box);
    m_ss_tv = Glib::RefPtr<Gtk::TreeView>::cast_dynamic(
        m_ui->get_object("ss_tv"));
    m_policy = Glib::RefPtr<Gtk::Label>::cast_dynamic(
        m_ui->get_object("m_policy"));
    m_list_store = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(
        m_ui->get_object("list_store"));
    m_ss_detail = Glib::RefPtr<Gtk::TextView>::cast_dynamic(
        m_ui->get_object("ss_detail"));
    m_as_grid = Glib::RefPtr<Gtk::Grid>::cast_dynamic(
        m_ui->get_object("as_grid"));
   
    m_export_csv = Glib::RefPtr<Gtk::Button>::cast_dynamic(
        m_ui->get_object("export_csv"));
    mywindow::updateAvailableStreamsView();
    // mywindow::updateSubscribedStreamsView();

    m_export_csv->signal_clicked().connect(sigc::mem_fun(*this, &mywindow::export_csv));

    m_policy->set_text("Your Policy : /ndn/org/md2k/mguard/dd40c/phone/accelerometer");

    std::cout << "end of UI function" << std::endl;
    m_Dispatcher.connect(sigc::mem_fun(*this, &mywindow::on_notification_from_worker_thread));

    int timeout_value = 5500; // in ms (1.5 sec)
    sigc::slot<bool> my_slot = sigc::mem_fun(*this, &mywindow::update_stream_detail_view);
    // connect slot to signal
    Glib::signal_timeout().connect(my_slot, timeout_value);
    add(*m_box);
  }
  set_title("MGuard");
  set_default_size(800, 400);
  show_all();
  // sleep(20);
  std::cout << "Shows everything" << std::endl;
}

void mywindow::updateSubscribedStreamsView()
{
  m_ss_tv->remove_all_columns();

  m_list_store = Gtk::ListStore::create(m_Columns);
  m_ss_tv->set_model(m_list_store);
  m_ss_tv->set_reorderable();
  m_ss_tv->set_headers_clickable(true);
  m_ss_tv->set_headers_visible(true);

  int counter = 1;
  for (auto &a : m_subscriptionList)
  {

    // TODO: what else to show in sub name details
    auto row = *(m_list_store->append());
    row[m_Columns.m_id] = counter;
    row[m_Columns.m_source] = a.toUri();
    counter++;
    NDN_LOG_DEBUG("Subscribed to the stream/s" << a); // << std::endl;
  }
  // m_subscriber.setm_SubscriptionList(m_subscriptionList);

  // Add the TreeView's view columns:
  m_ss_tv->append_column("id", m_Columns.m_id);
  m_ss_tv->append_column("source", m_Columns.m_source);

  m_refTreeSelection = m_ss_tv->get_selection();
  m_refTreeSelection->signal_changed().connect(sigc::bind(sigc::mem_fun(*this, &mywindow::get_stream_detail_data), m_refTreeSelection));
  show_all_children();
}
void mywindow::updateAvailableStreamsView()
{

  for (auto &b : m_subscriptionList)
  {
    NDN_LOG_INFO(": " << b);
  }

  std::vector<Gtk::Widget *> children = m_as_grid->get_children();
  for (long unsigned int c = 0; c < children.size(); c++)
  {
    // std::cout << "removed child " << std::endl;
    m_as_grid->remove(*children[c]);
  }

  int counter = 0;

  for (auto &a : m_availableStreams)
  {
    std::string fullName = a.toUri();
    Gtk::Label *stream_name = Gtk::manage(new Gtk::Label());

    stream_name->set_markup(fullName);
    m_as_grid->attach(*stream_name, 1, counter, 1, 1);

    Gtk::Button *button = Gtk::manage(new Gtk::Button());
    button->set_size_request(150, 30);

    Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();

    if (std::find(m_subscriptionList.begin(), m_subscriptionList.end(), m_availableStreams[counter]) == m_subscriptionList.end())
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

    m_as_grid->attach(*button, 2, counter, 1, 1);
    counter++;
    button->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &mywindow::onSubscribeBtnClick), a));
    std::cout << "updated view " << std::endl;
  }

  m_as_grid->show_all();

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
    m_availableStreams.push_back(a);
  }

  // m_subscriptionList.push_back(m_availableStreams[0]); // battery
  // m_subscriptionList.push_back(m_availableStreams[1]); // semloc
  // m_subscriptionList.push_back(m_availableStreams[3]); // gps

  // these codes are only for testing purposes
  // automatically subscriber to the respective streams

  // A. battery only
  // m_subscriptionList.push_back(m_availableStreams[0]); // battery

  // all stream

  // not gps
  // m_subscriptionList.push_back(m_availableStreams[0]); // battery
  // m_subscriptionList.push_back(m_availableStreams[2]); // sem_loc

  // accelerometer
  // m_subscriptionList.push_back(m_availableStreams[0]); // battery

  // only work
  // m_subscriptionList.push_back(m_availableStreams[0]); // gps, only the one with attribute work should be accessible
  // for (auto &s : m_subscriptionList)
  // {
  //   // m_subscriber.subscribe(s);
  //   NDN_LOG_DEBUG("Subscribed to the stream/s" << s); // << std::endl;
  //                                                     // std::cout << "subscribed" << std::endl;
  // }
  //

  // call this function on subscribed click or maybe create a new button for this??

  //ASK: This function sets the subscription list but now that we have the option to change this frequently 

  // m_subscriber.setSubscriptionList(m_subscriptionList);
  // run the processevent again, this time with sync as well

  // rerunning the process event closes the UI
  // m_subscriber.run(true);
  // mywindow::showUI();

  mywindow::showUI();

  // std::this_thread::sleep_for (std::chrono::seconds(5));

  // int i = 0;
  // while (i < 100000) {++i;}

  // m_subscriber.run(true);
    //ASK: This function gets the stream detail maybe we can get it seperately and do async thing.


  // m_subscriber.run();
}

void mywindow::callSubscriber(mywindow *caller)
{

  {
    Glib::Threads::Mutex::Lock lock(m_Mutex);
    m_has_stopped = false;

  } // The mutex is unlocked here by lock's destructor.
  std::cout << "started thread work" << std::endl;
  Glib::Threads::Mutex::Lock lock(m_Mutex);
  m_subscriber.run(true);
  m_has_stopped = true;

  std::cout << "ended" << std::endl;

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

bool mywindow::update_stream_detail_view()
{

  if (!m_content.empty())
  {
    // std::cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<std::endl;
    std::string joined = boost::algorithm::join(m_content, "\n ");

    Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer1 = Gtk::TextBuffer::create();
    std::string whole_file = joined;
    m_refTextBuffer1->set_text(whole_file);
    m_ss_detail->set_buffer(m_refTextBuffer1);
    show_all_children();
    return true;
  }
  return false;
}

// // TODO:
// show the content for each stream seperately
// save each stream content to different variable and show that on click
// show content in table form?

// void mywindow::on_fetch_button_click()
// {
//   //deactivate the button
//     m_subscriber.setSubscriptionList(m_subscriptionList);

//   // stop the previous thread;
//   mywindow::startThread();
  
// }

void mywindow::export_csv(){
    m_export_csv->set_sensitive(false); // true to enable the button 
     //get currently selected stream name and export their content
    std::ofstream output_file("./example.csv");
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    std::copy(m_content.begin(), m_content.end(), output_iterator);
}