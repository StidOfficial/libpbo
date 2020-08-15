#include "PBOManagerProperties.hpp"

PBOManagerProperties::PBOManagerProperties()
{
  set_title("Properties");
  set_default_size(400, 400);

  set_position(Gtk::WindowPosition::WIN_POS_CENTER_ON_PARENT);

  m_refTreeModel = Gtk::ListStore::create(m_columns);
  m_treeView.set_model(m_refTreeModel);

  //m_treeView.set_reorderable();

  m_treeView.append_column("Key", m_columns.m_col_key);
  m_treeView.append_column("Value", m_columns.m_col_value);

  Gtk::TreeView::Column *keyColumn = m_treeView.get_column(0);
  if(keyColumn)
  {
    keyColumn->set_resizable();
    keyColumn->set_sort_column(m_columns.m_col_key);
  }

  Gtk::TreeView::Column *valueColumn = m_treeView.get_column(1);
  if(valueColumn)
  {
    valueColumn->set_resizable();
    valueColumn->set_sort_column(m_columns.m_col_value);
  }

  m_scrolledWindow.add(m_treeView);
  m_scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  add(m_scrolledWindow);

  show_all_children();
}

void PBOManagerProperties::show_properties(PBO::PBO *pbo)
{
  m_refTreeModel->clear();

  for(auto &entry : *pbo)
    if(entry->get_packing_method() == PBO::PackingMethod::Version)
    {
      for(auto &property : entry->get_properties())
      {
        Gtk::TreeModel::Row row = *(m_refTreeModel->append());
        row[m_columns.m_col_key] = property.first;
        row[m_columns.m_col_value] = property.second;
      }
      break;
    }

  show();
}

PBOManagerProperties::~PBOManagerProperties()
{
}