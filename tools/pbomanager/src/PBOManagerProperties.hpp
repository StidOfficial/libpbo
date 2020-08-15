#pragma once

#include <gtkmm.h>
#include <libpbo/pbo.hpp>

class PBOManagerProperties : public Gtk::Window
{
public:
  PBOManagerProperties();
  virtual ~PBOManagerProperties();

  void show_properties(PBO::PBO *pbo);
protected:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    ModelColumns()
    {
      add(m_col_key);
      add(m_col_value);
    }

    Gtk::TreeModelColumn<Glib::ustring> m_col_key;
    Gtk::TreeModelColumn<Glib::ustring> m_col_value;
  };

  ModelColumns m_columns;

  Gtk::ScrolledWindow m_scrolledWindow;
  Gtk::TreeView m_treeView;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
};