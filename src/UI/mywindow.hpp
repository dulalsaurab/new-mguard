#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <gtkmm.h>

class mywindow : public Gtk::Window
{
public:
  mywindow(ndn::Name &consumerPrefix, ndn::Name &syncPrefix, ndn::Name &controllerPrefix,
           std::string &consumerCertPath, std::string &aaCertPath); // constructor);
  virtual ~mywindow();

  void on_changed(Glib::RefPtr<Gtk::TreeSelection>);
  void show_accessible_stream_from_file();
  std::string file_to_string(std::string);
  void on_row(ndn::Name);
  void handler();
  void show_ui();
  void update_available_streams_view();
  void update_subscribed_streams_view();

  void processDataCallback(const std::vector<std::string> &updates);
  void processSubscriptionCallback(const std::unordered_set<ndn::Name> &streams);

  void callSubscriber(mywindow *caller);
  void startThread();
  void notify();
  void on_notification_from_worker_thread();
  bool update_view();

private:

  ndn::Face m_face;
  mguard::subscriber::Subscriber m_subscriber;

  std::vector<ndn::Name> availableStreams;
  std::vector<ndn::Name> subscriptionList;
  std::vector<std::string> content;

  Glib::RefPtr<Gtk::Builder> ui;
  Gtk::Box *box;
  Glib::RefPtr<Gtk::ScrolledWindow> window;
  Glib::RefPtr<Gtk::ScrolledWindow> ss_scrl;

  Glib::RefPtr<Gtk::Fixed> fixed;
  Glib::RefPtr<Gtk::Stack> stack;
  Glib::RefPtr<Gtk::Fixed> as_tab;
  Glib::RefPtr<Gtk::Label> as_label;
  Glib::RefPtr<Gtk::Label> policy;
  Glib::RefPtr<Gtk::Fixed> ss_tab;

  Glib::RefPtr<Gtk::Viewport> ss_viewport;
  Glib::RefPtr<Gtk::TreeView> ss_tv;
  Glib::RefPtr<Gtk::TextView> ss_detail;

  Glib::RefPtr<Gtk::TreeSelection> select;

  Glib::RefPtr<Gtk::StackSwitcher> switch_s1;
  Glib::RefPtr<Gtk::Grid> as_grid;

  Glib::RefPtr<Gtk::ListStore> list_store;
  Gtk::TreeModel::Path m_TreePath;
  Gtk::Window *m_pWindow_Example;
  Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;

  class ModelColumns : public Gtk::TreeModelColumnRecord
  {
  public:
    ModelColumns()
    {
      add(m_id);
      add(m_timestamp);
      add(m_source);
      add(m_info);
    }

    Gtk::TreeModelColumn<int> m_id;
    Gtk::TreeModelColumn<std::string> m_timestamp;
    Gtk::TreeModelColumn<std::string> m_source;
    Gtk::TreeModelColumn<std::string> m_info;
  };

  ModelColumns m_Columns;

  Glib::Threads::Thread *m_WorkerThread;

  bool m_shall_stop;
  bool m_has_stopped;
  double m_fraction_done;
  Glib::ustring m_message;
  mutable Glib::Threads::Mutex m_Mutex;
  Glib::Dispatcher m_Dispatcher;
};

#endif
