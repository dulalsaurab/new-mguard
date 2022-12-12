#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <gtkmm.h>

class mywindow : public Gtk::Window
{
public:
  mywindow(ndn::Name &consumerPrefix, ndn::Name &syncPrefix, ndn::Name &controllerPrefix,
           std::string &consumerCertPath, std::string &aaCertPath); // constructor);
  virtual ~mywindow();

  void get_stream_detail_data(Glib::RefPtr<Gtk::TreeSelection>);
  void show_accessible_stream_from_file();
  void onSubscribeBtnClick(ndn::Name);
  void handler();
  void showUI();
  void updateAvailableStreamsView();
  void updateSubscribedStreamsView();

  void processDataCallback(const std::vector<std::string> &updates);
  void processSubscriptionCallback(const std::unordered_set<ndn::Name> &streams);

  void callSubscriber(mywindow *caller);
  void startThread();
  void notify();
  void on_notification_from_worker_thread();
  void on_fetch_button_click();
  bool update_stream_detail_view();
  void export_csv();

private:
  ndn::Face m_face;
  mguard::subscriber::Subscriber m_subscriber;

  std::vector<ndn::Name> m_availableStreams;
  std::vector<ndn::Name> m_subscriptionList;
  std::vector<std::string> m_content;

  Glib::RefPtr<Gtk::Builder> m_ui;
  Gtk::Box *m_box;

  Glib::RefPtr<Gtk::Label> m_policy;

  Glib::RefPtr<Gtk::TreeView> m_ss_tv;
  Glib::RefPtr<Gtk::TextView> m_ss_detail;

  Glib::RefPtr<Gtk::Grid> m_as_grid;
    Glib::RefPtr<Gtk::Button> m_export_csv;

  Glib::RefPtr<Gtk::ListStore> m_list_store;
  Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;

  class ModelColumns : public Gtk::TreeModelColumnRecord
  {
  public:
    ModelColumns()
    {
      add(m_id);
      add(m_source);
    }

    Gtk::TreeModelColumn<int> m_id;
    Gtk::TreeModelColumn<std::string> m_source;
  };

  ModelColumns m_Columns;

  Glib::Threads::Thread *m_WorkerThread;

  bool m_has_stopped;
  mutable Glib::Threads::Mutex m_Mutex;
  Glib::Dispatcher m_Dispatcher;
};

#endif
