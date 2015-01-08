/***************************************************************************
 *   Copyright (C) 2004-2005 by Andrew Krause                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "openldev-project-info.h"

string openldev_project_info_dialog_get_number_of_files (vector<string>&);
string openldev_project_info_dialog_get_lines_of_code   (vector<string>&, string);

// Create the project info dialog and populate all of the information
ProjectInfoDialog* openldev_project_info_dialog_new (ProjectSettings* project)
{
  ProjectInfoDialog *info = new ProjectInfoDialog ();
  
  string gladefile = openldev_add_prefix ("/share/openldev/glade/projectinfo.glade");
  info->xml = glade_xml_new (gladefile.c_str(), NULL, NULL);
  info->dialog = glade_xml_get_widget (info->xml, "dialog");
  info->treeview = glade_xml_get_widget (info->xml, "treeview");
  info->close = glade_xml_get_widget (info->xml, "button_close");
  
  GtkCellRenderer *renderer;
  GtkListStore *model;
  GtkTreeIter iter;
  GtkTreeViewColumn *column;

  model = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_STRING);
  
  // Append the data
  gtk_list_store_append (model, &iter);
  gtk_list_store_set (model, &iter, COL_STATISTIC, "Project Name", COL_VALUE, 
                      project->name, -1);
                     
  gtk_list_store_append (model, &iter);
  gtk_list_store_set (model, &iter, COL_STATISTIC, "Current Version", COL_VALUE, 
                      project->version, -1);
                     
  gtk_list_store_append (model, &iter);
  gtk_list_store_set (model, &iter, COL_STATISTIC, "Language", COL_VALUE, 
                      project->language, -1);
                  
  vector<string> sources = openldev_project_settings_get_sources_all (project);
  gtk_list_store_append (model, &iter);
  gtk_list_store_set (model, &iter, COL_STATISTIC, "Source Files", COL_VALUE, 
                      openldev_project_info_dialog_get_number_of_files(sources).c_str(), -1);
                     
  gtk_list_store_append (model, &iter);
  gtk_list_store_set (model, &iter, COL_STATISTIC, "Lines of Code", COL_VALUE, 
                      openldev_project_info_dialog_get_lines_of_code (sources,
                      project->location).c_str(), -1);
  
  gtk_tree_view_set_model (GTK_TREE_VIEW (info->treeview), GTK_TREE_MODEL(model));
  g_object_unref (model);
  
  renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Statistic", renderer,
                                                     "text", COL_STATISTIC, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(info->treeview), column);
  
  renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Value", renderer, 
                                                     "text", COL_VALUE, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(info->treeview), column);

  gtk_window_set_default (GTK_WINDOW(info->dialog), info->close);
  
  return info;
}

void openldev_project_info_dialog_run (ProjectInfoDialog* info)
{
  int result = gtk_dialog_run (GTK_DIALOG(info->dialog));
  
  switch (result)
  {
    default:
      gtk_widget_destroy (info->dialog);
      g_object_unref (info->xml);
  }
}

// Get the number of files in the project. This is done by counting the
// number of backslashes in the files list
string openldev_project_info_dialog_get_number_of_files (vector<string> &files)
{
  // Convert integer properties to character arrays
  gchar buff[5];
  snprintf (buff, sizeof (buff), "%d", files.size());
  return string (buff);
}

// Get the number of lines of code in the project. The function receives the
// list of filenames and the project source directory location. It then opens
// each of the files & counts how many lines are in the file.
string openldev_project_info_dialog_get_lines_of_code (vector<string> &filenames, 
                                                       string location)
{
  // Loop through to get all of the files
  long int count = 0;
  string line;
  for (unsigned int i=0; i < filenames.size(); i++)
  {
    ifstream fin (filenames[i].c_str());
    while (getline (fin, line))
      count++;
    count++;
    fin.close();
  }
  
  // Convert integer properties to character arrays
  char buff[10];
  sprintf (buff, "%ld", count);
  return string (buff);
}
