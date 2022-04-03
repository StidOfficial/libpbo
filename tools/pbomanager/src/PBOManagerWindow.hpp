#pragma once

#include "PBOManagerProperties.hpp"

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
#include <gtkmm.h>
#ifdef __GNUC__
#pragma GCC diagnostic warning "-Wold-style-cast"
#endif
#include <libpbo/pbo.hpp>
#include <memory>

class PBOManagerWindow : public Gtk::Window
{
public:
  PBOManagerWindow();
  virtual ~PBOManagerWindow();
protected:
  Gtk::HeaderBar m_headerBar;
  Gtk::Button m_buttonOpen;
  Gtk::Button m_buttonNew;
  Gtk::Button m_buttonProperties;
  Gtk::Button m_buttonSave;

  PBO::PBO m_pbo;

  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    ModelColumns()
    {
      add(m_col_entry);
      add(m_col_path);
      add(m_col_packing_method);
      add(m_col_original_size);
      add(m_col_reserved);
      add(m_col_timestamp);
      add(m_col_data_size);
      add(m_col_data_offset);
    }

    Gtk::TreeModelColumn<PBO::Entry*> m_col_entry;
    Gtk::TreeModelColumn<Glib::ustring> m_col_path;
    Gtk::TreeModelColumn<Glib::ustring> m_col_packing_method;
    Gtk::TreeModelColumn<uint32_t> m_col_original_size;
    Gtk::TreeModelColumn<uint32_t> m_col_reserved;
    Gtk::TreeModelColumn<Glib::ustring> m_col_timestamp;
    Gtk::TreeModelColumn<uint32_t> m_col_data_size;
    Gtk::TreeModelColumn<uint32_t> m_col_data_offset;
  };

  ModelColumns m_columns;

  Gtk::ScrolledWindow m_scrolledWindow;
  Gtk::TreeView m_treeView;
  Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;
  Gtk::Menu m_menuPopup;

  PBOManagerProperties m_properties;
private:
  void on_open_clicked();
  void on_new_clicked();
  void on_properties_clicked();
  void on_save_clicked();
  PBO::Entry *get_selected_entry();
  bool on_treeview_button_release_event(GdkEventButton *button_event);
  void on_extract_entry();
  void append_entry(std::shared_ptr<PBO::Entry> entry);
  void open(std::filesystem::path file_path);
};