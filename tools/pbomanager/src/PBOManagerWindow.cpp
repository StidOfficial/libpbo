#include "PBOManagerWindow.hpp"

#include <cstring>

#include <iostream>

PBOManagerWindow::PBOManagerWindow()
  : m_buttonOpen(Gtk::Stock::OPEN), m_buttonNew(Gtk::Stock::NEW),
    m_buttonProperties(Gtk::Stock::PROPERTIES), m_buttonSave(Gtk::Stock::SAVE)
{
  set_title("PBO Manager");
  set_default_size(800, 600);

  //set_default_icon_name("lol");

  m_buttonOpen.signal_clicked().connect(sigc::mem_fun(*this, &PBOManagerWindow::on_open_clicked));
  m_headerBar.pack_start(m_buttonOpen);

  m_buttonNew.signal_clicked().connect(sigc::mem_fun(*this, &PBOManagerWindow::on_new_clicked));
  m_headerBar.pack_start(m_buttonNew);

  m_buttonSave.signal_clicked().connect(sigc::mem_fun(*this, &PBOManagerWindow::on_save_clicked));
  m_buttonSave.set_sensitive(false);
  m_headerBar.pack_end(m_buttonSave);

  m_buttonProperties.signal_clicked().connect(sigc::mem_fun(*this, &PBOManagerWindow::on_properties_clicked));
  m_headerBar.pack_end(m_buttonProperties);

  m_headerBar.set_title("PBO Manager");
  m_headerBar.set_show_close_button(true);

  set_titlebar(m_headerBar);

  // Tree View
  m_refTreeModel = Gtk::TreeStore::create(m_columns);
  m_treeView.set_model(m_refTreeModel);

  //m_treeView.set_reorderable();

  m_treeView.append_column("Path", m_columns.m_col_path);
  m_treeView.append_column("Packing method", m_columns.m_col_packing_method);
  m_treeView.append_column("Original size", m_columns.m_col_original_size);
  m_treeView.append_column("Reserved", m_columns.m_col_reserved);
  m_treeView.append_column("Timestamp", m_columns.m_col_timestamp);
  m_treeView.append_column("Data size", m_columns.m_col_data_size);
  m_treeView.append_column("Data offset", m_columns.m_col_data_offset);

  Gtk::TreeView::Column *pathColumn = m_treeView.get_column(0);
  if(pathColumn)
  {
    pathColumn->set_resizable();
    pathColumn->set_sort_column(m_columns.m_col_path);
  }

  Gtk::TreeView::Column *packingMethodColumn = m_treeView.get_column(1);
  if(packingMethodColumn)
  {
    packingMethodColumn->set_resizable();
    packingMethodColumn->set_sort_column(m_columns.m_col_packing_method);
  }

  Gtk::TreeView::Column *originalSizeColumn = m_treeView.get_column(2);
  if(originalSizeColumn)
  {
    originalSizeColumn->set_resizable();
    originalSizeColumn->set_sort_column(m_columns.m_col_original_size);
  }

  Gtk::TreeView::Column *reservedColumn = m_treeView.get_column(3);
  if(reservedColumn)
  {
    reservedColumn->set_resizable();
    reservedColumn->set_sort_column(m_columns.m_col_reserved);
  }

  Gtk::TreeView::Column *timestampColumn = m_treeView.get_column(4);
  if(timestampColumn)
  {
    timestampColumn->set_resizable();
    timestampColumn->set_sort_column(m_columns.m_col_timestamp);
  }

  Gtk::TreeView::Column *dataSizeColumn = m_treeView.get_column(5);
  if(dataSizeColumn)
  {
    dataSizeColumn->set_resizable();
    dataSizeColumn->set_sort_column(m_columns.m_col_data_size);
  }

  Gtk::TreeView::Column *dataOffsetColumn = m_treeView.get_column(6);
  if(dataOffsetColumn)
  {
    dataOffsetColumn->set_resizable();
    dataOffsetColumn->set_sort_column(m_columns.m_col_data_offset);
  }

  m_treeView.signal_button_release_event().connect(sigc::mem_fun(*this, &PBOManagerWindow::on_treeview_button_release_event));

  m_scrolledWindow.add(m_treeView);
  m_scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  add(m_scrolledWindow);

  // Poup menu
  auto item = Gtk::make_managed<Gtk::MenuItem>("Extract", true);
  item->signal_activate().connect(sigc::mem_fun(*this, &PBOManagerWindow::on_extract_entry));
  m_menuPopup.append(*item);

  m_menuPopup.accelerate(*this);
  m_menuPopup.show_all();

  m_properties.set_transient_for(*this);

  show_all_children();
}

void PBOManagerWindow::on_open_clicked()
{
  Gtk::FileChooserDialog dialog("Please choose a file",
          Gtk::FILE_CHOOSER_ACTION_OPEN);

  dialog.set_transient_for(*this);

  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("Select", Gtk::RESPONSE_OK);

  // Filter for PBO
  auto filter_pbo = Gtk::FileFilter::create();
  filter_pbo->set_name("PBO files");
  filter_pbo->add_pattern("*.pbo");
  dialog.add_filter(filter_pbo);

  int result = dialog.run();
  switch(result)
  {
    case Gtk::RESPONSE_OK:
    {
      open(dialog.get_filename());
      break;
    }
    case Gtk::RESPONSE_CANCEL:
      break;
    default:
    {
      std::cout << "Unexpected button clicked." << std::endl;
      break;
    }
  }
}

void PBOManagerWindow::on_new_clicked()
{

}

void PBOManagerWindow::on_properties_clicked()
{
  m_properties.show_properties(&m_pbo);
}

void PBOManagerWindow::on_save_clicked()
{

}

PBO::Entry *PBOManagerWindow::get_selected_entry()
{
  auto refSelection = m_treeView.get_selection();
  if(refSelection)
  {
    Gtk::TreeModel::iterator iter = refSelection->get_selected();
    if(iter)
      return (*iter)[m_columns.m_col_entry];
  }

  return nullptr;
}

bool PBOManagerWindow::on_treeview_button_release_event(GdkEventButton *button_event)
{
  if(button_event->button == 3 && get_selected_entry())
    m_menuPopup.popup_at_pointer((GdkEvent*)button_event);

  return true;
}

void PBOManagerWindow::on_extract_entry()
{
  PBO::Entry *entry = get_selected_entry();
  std::cout << entry << std::endl;

  Gtk::FileChooserDialog dialog("Save as",
          Gtk::FILE_CHOOSER_ACTION_SAVE);

  dialog.set_transient_for(*this);

  std::string entry_path = entry->get_path().string();
  std::size_t pos = entry_path.rfind('\\');
  if(pos != std::string::npos)
    dialog.set_current_name(entry_path.substr(pos + 1));
  else
    dialog.set_current_name(entry_path);

  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Save", Gtk::RESPONSE_OK);

  int result = dialog.run();
  switch(result)
  {
    case Gtk::RESPONSE_OK:
    {
      switch(entry->get_packing_method())
      {
        case PBO::PackingMethod::Uncompressed:
        {
          std::vector<char> buffer(entry->get_data_size());
          std::ifstream input(m_pbo.get_path(), std::ios_base::binary);

          std::ofstream output(dialog.get_current_name(), std::ios_base::binary);
          input.seekg(entry->get_data_offset());
          input.read(buffer.data(), buffer.size());
          output.write(buffer.data(), buffer.size());
          output.close();

          input.close();

          break;
        }
        case PBO::PackingMethod::Compressed:
          break;
      }
      break;
    }
    case Gtk::RESPONSE_CANCEL:
      break;
    default:
    {
      std::cout << "Unexpected button clicked." << std::endl;
      break;
    }
  }
}

void PBOManagerWindow::append_entry(std::shared_ptr<PBO::Entry> entry)
{
  Gtk::TreeModel::Children children = m_refTreeModel->children();

  std::string string_path = entry->get_path().string();
  std::size_t prev_pos = 0, pos = 0;
  bool found;
  do
  {
    prev_pos = pos;
    pos = string_path.find_first_of("\\", pos);
    if(pos == std::string::npos)
      break;

    std::string directory = string_path.substr(prev_pos, pos - prev_pos);

    found = false;
    for(auto &child : children)
      if(child[m_columns.m_col_path] == directory)
      {
        children = child.children();
        found = true;
        break;
      }

    if(!found)
    {
      Gtk::TreeModel::Row row = *(m_refTreeModel->append(children));
      row[m_columns.m_col_path] = directory;

      children = row->children();
    }

    pos++;
  } while(true);

  Gtk::TreeModel::Row row = *(m_refTreeModel->append(children));
  row[m_columns.m_col_entry] = entry.get();
  row[m_columns.m_col_path] = string_path.substr(prev_pos);
  PBO::PackingMethod packing_method = static_cast<PBO::PackingMethod>(entry->get_packing_method());
  row[m_columns.m_col_packing_method] = PBO::to_string(packing_method);
  row[m_columns.m_col_original_size] = entry->get_original_size();
  row[m_columns.m_col_reserved] = entry->get_reserved();
  time_t timestamp = entry->get_timestamp();
  row[m_columns.m_col_timestamp] = std::strtok(std::ctime(&timestamp), "\n");
  row[m_columns.m_col_data_size] = entry->get_data_size();
  row[m_columns.m_col_data_offset] = entry->get_data_offset();
}

void PBOManagerWindow::open(std::filesystem::path file_path)
{
  std::cout << "Open : " << file_path << std::endl;

  m_pbo.open(file_path);

  set_title("PBO Manager : " + file_path.filename().string());

  m_pbo.unpack();

  m_refTreeModel->clear();

  for(auto &entry : m_pbo)
  {
    if(!entry->is_file_entry())
      continue;

    append_entry(entry);
  }
}

PBOManagerWindow::~PBOManagerWindow()
{
}