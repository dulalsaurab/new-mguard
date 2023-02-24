
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

using namespace ndn::time_literals;

NDN_LOG_INIT(mguard.ui);
// # Todo show the streams that we ca nsubscribe to?

mywindow::mywindow(ndn::Name &consumerPrefix, ndn::Name &syncPrefix, ndn::Name &controllerPrefix,
                   std::string &consumerCertPath, std::string &aaCertPath)
    : m_subscriber(consumerPrefix, syncPrefix, controllerPrefix,
                   consumerCertPath, aaCertPath, 1000_ms,
                   std::bind(&mywindow::processDataCallback, this, _1),
                   std::bind(&mywindow::processSubscriptionCallback, this, _1))
{
    // mywindow::show_ui();
    mywindow::show_ui();

    mywindow::handler();
}

mywindow::~mywindow()
{
    m_ui_thread.join();
}

void mywindow::on_quit_click()
{

    Gtk::MessageDialog quit_dialog("Are you sure?", false,
                                   Gtk::MESSAGE_ERROR,
                                   Gtk::BUTTONS_YES_NO,
                                   true);
    quit_dialog.set_title("Quit");
    int result = quit_dialog.run();
    switch (result)
    {
    case (yes_response):
        hide();
        break;
    case (no_response):
        quit_dialog.hide();
        break;
    }
}



// convert file content to string
std::string mywindow::file_to_string(std::string filename)
{
    //     std::cout<<"set detail text function called" << std::endl;
    //         std::cout<< filename << std::endl;

    // //
    //     std::ifstream input(filename);
    //     std::stringstream sstr;

    //     while(input >> sstr.rdbuf());

    //     // std::cout << sstr.str() << std::endl;
    //     std::string wholefile = sstr.str();
    return "true";
}

void mywindow::on_subscribed_stream_window_selection_changed(Glib::RefPtr<Gtk::TreeSelection> c)
{

    std::cout << "on subscribed stream window changed function" << std::endl;
    Gtk::TreeModel::iterator iter = c->get_selected();
    if (iter) // If anything is selected
    {
        Gtk::TreeModel::Row row = *iter;
        std::string id = row.get_value(m_Columns.m_id);
        std::string source = row.get_value(m_Columns.m_source);
        std::cout<< id << source << std::endl;

        // std::stringstream filename;
        // filename << "content" << id << ".txt";
        // std::string whole_file = mywindow::file_to_string(filename.str());

        // ss_detail->get_buffer()->set_text(whole_file);
        mywindow::reloadStreamDataView();

    }
}

void mywindow::on_row(std::string b)
{

    // if not in list add else remove from list
    if (std::find(sub_st.begin(), sub_st.end(), b) == sub_st.end())
    {
        sub_st.push_back(b);
    }
    else
    {
        sub_st.erase(std::remove(sub_st.begin(), sub_st.end(), b), sub_st.end());
    }
    mywindow::change_btn_display();
    mywindow::callSubscriber(sub_st);
    mywindow::changesubscribedStreamWindow();

    std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@2" << '\n';
}

void mywindow::callSubscriber(std::vector<std::string> sub_st){
    // convert the uris to name and send the subscriber call
    std::vector<ndn::Name> subscriptionList;
        if  (sub_st.size() > 0) {
            for (const auto &s : sub_st){
                subscriptionList.push_back(ndn::Name(s));
            }
        m_subscriber.setSubscriptionList(subscriptionList);
        }
}
void mywindow::changesubscribedStreamWindow(){

            for (const auto &s : sub_st){

            auto row = *(list_store->append());
            row[m_Columns.m_id] = "1";
            row[m_Columns.m_timestamp] = "02/11/1998";
            row[m_Columns.m_source] = s;
            // row[m_Columns.m_info] = "1654907929.430571 DEBUG: [mguard.subscriber] ...";

            }
// #Fill the list_store model
        // Add the TreeView's view columns:
        ss_tv->append_column("id", m_Columns.m_id);
        ss_tv->append_column("timestamp", m_Columns.m_timestamp);
        ss_tv->append_column("source", m_Columns.m_source);
        // ss_tv->append_column("info", m_Columns.m_info);

        m_refTreeSelection = ss_tv->get_selection();
        // m_refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &mywindow::on_subscribed_stream_window_selection_changed));
        m_refTreeSelection->signal_changed().connect(sigc::bind(sigc::mem_fun(*this, &mywindow::on_subscribed_stream_window_selection_changed), m_refTreeSelection));

// update the list 
   }
void mywindow::change_btn_display()
{

    //  Gtk::Widget * const parent = gtk_widget_get_parent(GTK_WIDGET(as_grid));
    std::vector<Gtk::Widget *> children = as_grid->get_children();

    // as_grid->remove(children);
    for (long unsigned int a = 0; a < children.size(); a++)
    {
        std::cout << "cjild" << std::endl;
        as_grid->remove(*children[a]);
    }

    for (long unsigned int s = 0; s < acc_st.size(); s++)
    {

        Gtk::Label *stream_name = Gtk::manage(new Gtk::Label());
        stream_name->set_markup(acc_st[s]);
        as_grid->attach(*stream_name, 1, s, 1, 1);

        Gtk::Button *button = Gtk::manage(new Gtk::Button());
        Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
        if (std::find(sub_st.begin(), sub_st.end(), acc_st[s]) == sub_st.end())
        {

            // Element in subscribed vector.
            button->set_label("subscribe");
            css_provider->load_from_data(
                "button {background-image: image(#bcffb5); margin-top: 10px;  margin-left: 10px;}");
            button->get_style_context()->add_provider(
                css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
        }
        else
        {

            button->set_label("unsubscribe");
            css_provider->load_from_data(
                "button {background-image: image(#ffc2b5);  margin-top: 10px;  margin-left: 10px;}");
            button->get_style_context()->add_provider(
                css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
        }
        as_grid->attach(*button, 2, s, 1, 1);
        button->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &mywindow::on_row), acc_st[s]));
    }

    show_all_children();
}

// void mywindow::on_row( Glib::RefPtr<Gtk::Button> b) {
//     display_btn = Glib::RefPtr<Gtk::Button>::cast_dynamic(Gtk::Builder->get_object(b));
//     // Gtk::Button* button = Glib::wrap(b);
//     //   b->set_label("subscribe");
//     std::cout <<display_btn.get_label() << '\n';
// }

void mywindow::processDataCallback(const std::map<std::string, std::string>& updates)
{
    // print the data being shown here
    for (auto  const& x : updates){
         std::string stream_name_detail = x.first;
        std::string stream_data = x.second;


        NDN_LOG_INFO("Received data: " << stream_data << " for name: " << stream_name_detail);


    //  get name of stream
        int firstDelPos = stream_name_detail.find("/data_analysis/") +14 ;
        // Find the position of second delimiter
        int secondDelPos = stream_name_detail.find("/DATA/");
        // Get the substring between two delimiters
        std::string strbetweenTwoDels = stream_name_detail.substr(firstDelPos+1, secondDelPos-firstDelPos-1); 
        std::cout << strbetweenTwoDels << std::endl;
        std::string stream_name = strbetweenTwoDels;
        gps_episodes_and_semantic_location.append(x.second);
        gps_episodes_and_semantic_location.append("\n");
    }

    // # reload the UI

    // gps_episodes_and_semantic_location.append(x.second);

  
    // append the value to the stream vector

        // std::cout << x << std::endl;
}

void mywindow::reloadStreamDataView(){
    std::cout << "reloading the buffer" << std::endl;

    Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer1 = Gtk::TextBuffer::create();
    std::string whole_file = gps_episodes_and_semantic_location;
    m_refTextBuffer1->set_text(whole_file);
    ss_detail->set_buffer(m_refTextBuffer1);
    show_all();


}
  void
  mywindow::handler()
  {
    // mywindow::show_ui();
    // m_ui_thread = std::thread([this] { mywindow::show_ui(); });
    m_subscriber.run();
  }

  void mywindow::show_ui(){
    Glib::RefPtr<Gtk::Builder> ui = Gtk::Builder::create_from_file("src/UI/tabsbox.glade");
    std::cout << "hell" << std::endl;

    // std::cout << consumerPrefix << std::endl;

    if (ui)
    {
        ui->get_widget<Gtk::Box>("box", box);
        std::cout << "box" << std::endl;

        ss_tv = Glib::RefPtr<Gtk::TreeView>::cast_dynamic(
            ui->get_object("ss_tv"));
        std::cout << "glib error" << std::endl;

        policy = Glib::RefPtr<Gtk::Label>::cast_dynamic(
            ui->get_object("policy"));
        list_store = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(
            ui->get_object("list_store"));
        ss_detail = Glib::RefPtr<Gtk::TextView>::cast_dynamic(
            ui->get_object("ss_detail"));

        // acc_st = {"/ndn/org/md2k/mguard/dd40c/phone/accelerometer",
        //           "/ndn/org/md2k/mguard/dd40c/data_analysis/gps_episodes_and_semantic_location ",
        //           "/ndn/org/md2k/mguard/dd40c/phone/gyroscope",
        //           "/ndn/org/md2k/mguard/dd40c/phone/battery",
        //           "/ndn/org/md2k/mguard/dd40c/phone/gps"};
        // sub_st = {"/ndn/org/md2k/mguard/dd40c/phone/accelerometer",
        //           "/ndn/org/md2k/mguard/dd40c/phone/gyroscope",
        //           "/ndn/org/md2k/mguard/dd40c/phone/gps"};

        policy->set_text("Your Policy : /ndn/org/md2k/mguard/dd40c/phone/accelerometer");
        std::cout << "eta samma " << std::endl;

        list_store = Gtk::ListStore::create(m_Columns);
        ss_tv->set_model(list_store);
        ss_tv->set_reorderable();
        ss_tv->set_headers_clickable(true);
        ss_tv->set_headers_visible(true);

        // // #Fill the list_store model
        // auto row = *(list_store->append());
        // row[m_Columns.m_id] = "1";
        // row[m_Columns.m_timestamp] = "02/11/1998";
        // row[m_Columns.m_source] = "/ndn/org/md2k/mguard/dd40c/phone/accelerometer";
        // row[m_Columns.m_info] = "1654907929.430571 DEBUG: [mguard.subscriber] ...";

        // // #Fill the list_store model
        // row = *(list_store->append());
        // row[m_Columns.m_id] = "2";
        // row[m_Columns.m_timestamp] = "12/12/2021";
        // row[m_Columns.m_source] = "/ndn/org/md2k/mguard/dd40c/phone/gyroscope";
        // row[m_Columns.m_info] = "1654907929.430571 DEBUG: [mguard.subscriber] ...";

        // // #Fill the list_store model
        // row = *(list_store->append());
        // row[m_Columns.m_id] = "3";
        // row[m_Columns.m_timestamp] = "03/02/2020";
        // row[m_Columns.m_source] = "/ndn/org/md2k/mguard/dd40c/phone/gps";
        // row[m_Columns.m_info] = "1654907929.430571 DEBUG: [mguard.subscriber] ...";

        // // Add the TreeView's view columns:
        // ss_tv->append_column("id", m_Columns.m_id);
        // ss_tv->append_column("timestamp", m_Columns.m_timestamp);
        // ss_tv->append_column("source", m_Columns.m_source);
        // ss_tv->append_column("info", m_Columns.m_info);



        // m_refTreeSelection = ss_tv->get_selection();
        // // m_refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &mywindow::on_subscribed_stream_window_selection_changed));
        // m_refTreeSelection->signal_changed().connect(sigc::bind(sigc::mem_fun(*this, &mywindow::on_subscribed_stream_window_selection_changed), m_refTreeSelection));

        //   m_button1.signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &HelloWorld::on_button_clicked), "button 1"));
        //    Gtk::TreeModel::iterator iter = m_refTreeSelection->get_selected();
        //     if(iter) //If anything is selected
        //     {
        //     Gtk::TreeModel::Row row = *iter;
        //     std::cout<< iter;
        //     //Do something with the row.
        //     }

        // Text box to show the conteot of each stream
        Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer1 = Gtk::TextBuffer::create();
        // Fill text box with file content
        std::string whole_file = mywindow::file_to_string("ctrue");
        m_refTextBuffer1->set_text(whole_file);
        ss_detail->set_buffer(m_refTextBuffer1);

        as_grid = Glib::RefPtr<Gtk::Grid>::cast_dynamic(
            ui->get_object("as_grid"));
        // mywindow::change_btn_display();

        std::cout << "Hello" << std::endl;

        add(*box);
    }

    set_title("MGuard");
    set_default_size(400, 400);
    show_all();
    std::cout << "Shows everything" << std::endl;


  }


void mywindow::update_available_streams_view(std::unordered_set<ndn::Name> streams){
// //  Gtk::Widget * const parent = gtk_widget_get_parent(GTK_WIDGET(as_grid));
//     std::vector<Gtk::Widget *> children = as_grid->get_children();
//     // std::vector<ndn::Name> availableStreams;
//     // as_grid->remove(children);
//     for (long unsigned int a = 0; a < children.size(); a++)
//     {
//         std::cout << "cjild" << std::endl;
//         as_grid->remove(*children[a]);
//     }

//     int counter = 0;
//     for (auto &a : streams)
//     {
//         // std::map<std::string, MyClass*> map;

//     // std::map<ndn::Name, std::string> a;
//         // const std::map<ndn::Name, std::string>&
//         std::string fullName ="hello";
//         // std::string fullName = a.toUri();
//         Gtk::Label *stream_name = Gtk::manage(new Gtk::Label());

        
//         stream_name->set_markup("fullName");
//         as_grid->attach(*stream_name, 1, counter, 1, 1);

//         Gtk::Button *button = Gtk::manage(new Gtk::Button());
//         Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
        
//         as_grid->attach(*button, 2, counter, 1, 1);
//         button->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &mywindow::on_row), a));

//       NDN_LOG_INFO(++counter << ": " << a);
//     //   availableStreams.push_back(a);
//     }


    show_all_children();

  }


void mywindow::processSubscriptionCallback(const std::unordered_set<ndn::Name>& streams)
{

       

    // check for convergence.
    if (!m_subscriber.checkConvergence()) {
      NDN_LOG_DEBUG("couldnt fetch appropriate keys, exiting");
      exit(-1);
    }

    // stop the process event
    // m_subscriber.stop();

    NDN_LOG_INFO("\n\nStreams available for subscription");
    // NDN_LOG_INFO("\n\nStreams available for subscription");


    std::vector<ndn::Name> availableStreams, subscriptionList;
    int counter = 0;


    if (streams.size() <= 0)
    {
        //show no stream text
        Gtk::Label *no_stream = Gtk::manage(new Gtk::Label());
        no_stream->set_markup("No eligible stream found for your policy");
        as_grid->attach(*no_stream, 1, 0, 1, 1);

      NDN_LOG_INFO("No eligible stream found for your policy");
    }

    // mywindow::update_available_streams_view(streams);
        // mywindow::show_ui();

    // print avaliable streams here
    for (auto &a : streams)
    {
        

      NDN_LOG_INFO(++counter << ": " << a);
      availableStreams.push_back(a);
      acc_st.push_back(a.toUri());
      stream_name_data[a.toUri()] = "";
    }
        NDN_LOG_INFO("available st name set ");

    for (auto  const& x : acc_st){
                NDN_LOG_INFO("available st name set ");

                NDN_LOG_INFO(x);

    
    }
 


                // NDN_LOG_INFO(acc_st);


    mywindow::change_btn_display();
    // update hte view with available streams 


    // # streams is Name we need it as string 
    // acc_st = streams;

    // these codes are only for testing purposes
    // automatically subscriber to the respective streams

    // A. battery only
    // subscriptionList.push_back(availableStreams[0]); // battery
    // sub_st

    // all stream
    // subscriptionList.push_back(availableStreams[0]); // battery
    // subscriptionList.push_back(availableStreams[1]); // semloc
    // subscriptionList.push_back(availableStreams[3]); // gps

    // not gps
    // subscriptionList.push_back(availableStreams[0]); // battery
    // subscriptionList.push_back(availableStreams[2]); // sem_loc

    // accelerometer
    // subscriptionList.push_back(availableStreams[0]); // battery

    // only work
    // subscriptionList.push_back(availableStreams[0]); // gps, only the one with attribute work should be accessible

    // for (auto &s : subscriptionList)
    // {
    //   // m_subscriber.subscribe(s);
    //   NDN_LOG_DEBUG("Subscribed to the stream/s" << s); // << std::endl;
    // }
    // uncomment if: taking input from user ----------------------------------------------

    // std::vector<int> input; //
    // std::cout << "enter selection, enter any char to stop" << std::endl;
    // while(!std::cin.fail())
    // {
    //     int value;
    //     std::cin >> value;
    //     if(!std::cin.fail())
    //       input.push_back(value);
    // }
    // std::cout << "\n" << std::endl;
    // std::cout << "Subscribed to the stream/s" << std::endl;
    // for (auto k : input)
    // {
    //   auto ind = k-1;
    //   std::cout << k << ": " << availableStreams[ind] << std::endl;
    //   if (availableStreams[ind] != "/") // todo: fix this
    //     subscriptionList.push_back(availableStreams[ind]);
    // }

    // taking input from user end ----------------------------------------------

    // m_subscriber.setSubscriptionList(subscriptionList);


    // run the processevent again, this time with sync as well
    
    // rerunning the process event closes the UI
    // m_subscriber.run(true);

    // m_subscriber.run();
}