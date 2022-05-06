#include <user/subscriber.hpp>
#include <common.hpp>

#include <ndn-cxx/util/logger.hpp>
#include <ndn-cxx/face.hpp>

#include <gtkmm.h>
#include <string>
#include <sstream>
#include <iostream>

using namespace ndn::time_literals;
using namespace Glib;
using namespace Gtk;

class mGuardConsumer
{
public:

  mGuardConsumer()
  : m_subscriber("/org/md2k/A", "/org/md2k", 1600_ms,
                 std::bind(&mGuardConsumer::processDataCallback, this, _1),
                 std::bind(&mGuardConsumer::processSubscriptionCallback, this, _1))
  {
  }

  void
  processDataCallback(const std::vector<std::string>& updates)
  {
    for (auto &a : updates)
      std::cout << "received data: " << a << std::endl;
  }

  void
  processSubscriptionCallback(const std::unordered_set<ndn::Name>& streams)
  {
    // check for convergence.
    m_subscriber.checkConvergence();
    
    // stop the process event
    m_subscriber.stop();

    std::cout << "\n\nStreams available for subscription" << std::endl;
    std::vector<ndn::Name> availableStreams, subscriptionList;
    int counter=0;
    if (streams.size() <= 0)
    {
      std::cout << "No eligible stream found for your policy" << std::endl;
    }
    for (auto &a : streams)
    {
      std::cout << ++counter << ": " << a << std::endl;
      availableStreams.push_back(a);
    }

    // these codes are only for testing purposes
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
    std::cout << "Subscribed to the stream/s" << std::endl;
    for (auto k : input)
    {
      auto ind = k-1;
      std::cout << k << ": " << availableStreams[ind] << std::endl;
      if (availableStreams[ind] != "/") // todo: fix this
        subscriptionList.push_back(availableStreams[ind]);
    }
    m_subscriber.setSubscriptionList(subscriptionList);

    // run the processevent again, this time with sync as well
    m_subscriber.run(true);

  }

  void
  handler()
  {
    m_subscriber.run();
  }

private:
  ndn::Face m_face;
  mguard::subscriber::Subscriber m_subscriber;
};




class Form : public Window {
public:
  Form() {

    add(scrolledWindow);
    set_size_request(800, 600);
    scrolledWindow.add(fixed);

    
    tabControl1.set_size_request(800, 600);
    fixed.add(tabControl1);

    tabControl1.insert_page(tabPage1, "tabPage1", 0);
    tabControl1.insert_page(tabPage2, "Subscribed Streams", 1);

    labelPage1.set_label("Accessible Stream");
    tabControl1.set_tab_label(tabPage1, labelPage1);

    tabPage1.add(fixedTabPage1);


// content of accessible stream page

    // create a grid to add content inside the scrollwindow
    //set difference between rows and columns in the grid
    grid.set_row_spacing(5);
    grid.set_column_spacing(5);
    grid.set_margin_top(10);
    grid.set_margin_bottom(10);
    grid.set_margin_left(10);
    grid.set_margin_right(10);
    

    fixedTabPage1.add(grid);

    // show streams with attributes
    for(int i = 0; i < 8; i=i+2)
    {
        Gtk::Label *stream_name = Gtk::manage(new Gtk::Label());
        stream_name->set_markup("<b> Streams: </b> org/md2k/health/home");
        grid.attach(*stream_name, 1, i);
        Gtk::Button *stream_btn = Gtk::manage(new Gtk::Button("Subscribe"));
        grid.attach(*stream_btn, 2, i);
        Gtk::Label *attribute_list = Gtk::manage(new Gtk::Label());
        attribute_list->set_markup("<b> Attributes: </b> home, gym, health");
        grid.attach(*attribute_list, 1, i+1);
  }


 //second page

    tabPage2.add(fixedTabPage2);
    // create a grid to add content inside the scrollwindow
    Gtk::Grid *sub_grid = Gtk::manage(new Gtk::Grid);
    //set difference between rows and columns in the grid
    sub_grid->set_row_spacing(5);
    sub_grid->set_column_spacing(5);
    sub_grid->set_margin_top(10);
    sub_grid->set_margin_bottom(10);
    sub_grid->set_margin_left(10);
    sub_grid->set_margin_right(10);

    fixedTabPage2.add(*sub_grid);

 
    // show streams with attributes
    for(int i = 0; i < 50; i=i+5)
    {
        Gtk::Label *stream_name = Gtk::manage(new Gtk::Label());
        stream_name->set_markup("<b> Streams: </b> org/md2k/health/home");
        sub_grid->attach(*stream_name, 1, i);
        Gtk::Label *attribute_list = Gtk::manage(new Gtk::Label());
        attribute_list->set_markup(" timestamp: 2019-01-09 18:27:23");
        sub_grid->attach(*attribute_list, 1, i+1);
        Gtk::Label *details= Gtk::manage(new Gtk::Label());
        details->set_markup(" level: 68.5599999999839");
        sub_grid->attach(*details, 1, i+2);

        Gtk::Label *details2= Gtk::manage(new Gtk::Label());
        details2->set_markup(" voltage: 3700");
        sub_grid->attach(*details2, 1, i+3);
        Gtk::Label *details3= Gtk::manage(new Gtk::Label());
        details3->set_markup(" temperature:70");
        sub_grid->attach(*details3, 1, i+4);
  }


    set_title("MGuard");
    show_all();
  }
  
private:
  Fixed fixed;
  ScrolledWindow scrolledWindow;
  Notebook tabControl1;
  Label labelPage1;
  Frame tabPage1;
  Frame tabPage2;
  RadioButtonGroup radioButtonGroup;
  RadioButton radioTop;
  RadioButton radioLeft;
  RadioButton radioRight;
  RadioButton radioBottom;
  Fixed fixedTabPage1;
  Fixed fixedTabPage2;
  ScrolledWindow streamDetail;
  Fixed streamDetailFixed;
  Grid grid;

};



int 
main (int argc, char* argv[])
{


  // std::vector<std::string> subscriptionList {"/org/md2k/mguard/dd40c/phone/gps"};
  // mGuardConsumer consumer (subscriptionList);
  // consumer.handler();
 

  RefPtr<Application> application = Application::create(argc, argv);
  Form form;
  return application->run(form);


  mGuardConsumer consumer;
  consumer.handler();

}