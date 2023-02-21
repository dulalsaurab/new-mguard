#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <gtkmm.h>

class mywindow : public Gtk::Window
{
public:
  mywindow(ndn::Name &consumerPrefix, ndn::Name &syncPrefix, ndn::Name &controllerPrefix,
           std::string &consumerCertPath, std::string &aaCertPath); // constructor);
  virtual ~mywindow() {
        m_ui_thread.join();

  };
  static const int view_subscriber_list = -8;
  static const int view_accessble_stream = -9;
  static const int yes_response = -6;
  static const int no_response = -4;
  static const int ok_response = -5;
  ndn::Face m_face;
  std::thread m_ui_thread;

  mguard::subscriber::Subscriber m_subscriber;

  std::vector<std::string> acc_st = {};
  std::vector<std::string> sub_st = {};

  Glib::RefPtr<Gtk::Builder> ui;
  Gtk::Box *box;
  Glib::RefPtr<Gtk::ScrolledWindow> window;
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

    Gtk::TreeModelColumn<std::string> m_id;
    Gtk::TreeModelColumn<std::string> m_timestamp;
    Gtk::TreeModelColumn<std::string> m_source;
    Gtk::TreeModelColumn<std::string> m_info;
  };

  ModelColumns m_Columns;

  Gtk::TreeModel::Path m_TreePath;

  Gtk::Window *m_pWindow_Example;

  Glib::RefPtr<Gtk::TreeSelection> m_refTreeSelection;

  void on_quit_click();
  void change_btn_display();
  // void on_select_changed(Gtk::Widget);
  void on_changed(Glib::RefPtr<Gtk::TreeSelection>);
  void show_accessible_stream_from_file();
  //    static bool select_function(const Glib::RefPtr<Gtk::TreeModel>& model, const Gtk::TreeModel::Path& path, bool currently_selected);

  //   virtual void on_treeview_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);

  //    void button_press_event(GTk::Treeview, event):
  std::string file_to_string(std::string);

  //    void user_function (Gtk::TreeView   ,
  //                    Gtk::TreePath     ,
  //                    Gtk::TreeViewColumn ,
  //                    gpointer           );
  void on_row(std::string);
  void handler();
  void show_ui();
  void update_available_streams_view(std::unordered_set<ndn::Name>);

  void processDataCallback(const std::map<std::string, std::string>& updates);
  void processSubscriptionCallback(const std::unordered_set<ndn::Name>& streams);
  // void on_row(Glib::RefPtr<Gtk::Button>);
};

#endif
