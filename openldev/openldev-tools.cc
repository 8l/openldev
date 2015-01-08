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
 
#include "openldev-tools.h"

void openldev_search_all_files (vector<string>&, vector<string>&, string, string);
void openldev_mark_found_text (OpenLDev*, GtkTextIter*, GtkTextIter*);

// Toggle the visibility of the goto line toolbar. Also, this makes sure that if
// the goto line toolbar is visible, the find toolbar is hidden
void 
openldev_menu_edit_goto_line (OpenLDev *openldev)
{
  GotoLineToolbar *gototoolbar;
  FileManager *fileman;
  GtkTextBuffer *buffer;
  GtkTextIter iter, insert;
  gint x, y, dx, dy, tab, lines, line;

  g_object_get (openldev, "goto-toolbar", &gototoolbar, "file-browser", &fileman, NULL);
  
  tab = file_manager_get_current_tab (fileman);
  if (tab == -1)
    return;
  
  gtk_window_get_position (GTK_WINDOW (openldev), &x, &y);
  gdk_window_get_position (fileman->tabs[tab]->window, &dx, &dy);
  gtk_window_move (GTK_WINDOW (gototoolbar->window), x + dx + 10, y + dy + 30);
  
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (fileman->tabs[tab]));
  gtk_text_buffer_get_end_iter (buffer, &iter);
  gtk_text_buffer_get_iter_at_mark (buffer, &insert, gtk_text_buffer_get_insert (buffer));
  lines = gtk_text_iter_get_line (&iter) + 1;
  line = gtk_text_iter_get_line (&insert) + 1;  
  gtk_spin_button_set_range (GTK_SPIN_BUTTON (gototoolbar->spin), 1.0, (gdouble) lines);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (gototoolbar->spin), (gdouble) line);
  
  gtk_widget_show_all (gototoolbar->window);
  gtk_window_set_focus (GTK_WINDOW (gototoolbar->window), gototoolbar->spin);
}

// Toggle the visibility of the find toolbar. Also, this makes sure that if
// the Find toolbar is visible, the goto line toolbar is hidden
void openldev_menu_edit_find (OpenLDev *openldev)
{
  FindToolbar *findtoolbar;

  g_object_get (openldev, "find-toolbar", &findtoolbar, NULL);
  if (GTK_WIDGET_VISIBLE (findtoolbar->hbox))
    gtk_widget_hide (findtoolbar->hbox);
  else
  {
    gtk_widget_show (findtoolbar->hbox);
    gtk_widget_grab_focus (findtoolbar->find_text);
  }
}

// Find the next iteration of the text the user specified in the buffer.
// If there is no selected text, search from the current cursor position.
// Otherwise, search from the selection bound. If the text is not found, wrap
// around the document once and then throw an error if it is still not found.
void openldev_menu_edit_find_next (OpenLDev *openldev)
{
  FindToolbar *findtoolbar;
  FileManager *files;
  OutputManager *output;
  ProjectSettings *project;

  g_object_get (openldev, "find-toolbar", &findtoolbar, "file-browser", &files,
                "output-pane", &output, "project", &project, NULL);
  
  if (!openldev_find_toolbar_search_files_is_active (findtoolbar) &&
      file_manager_get_current_tab (files) == -1)
    return;
  
  string entry_text = gtk_combo_box_get_active_text (GTK_COMBO_BOX (findtoolbar->find_text));
  if (!entry_text.empty())
  {
    openldev_find_toolbar_add_choice (findtoolbar, entry_text);
    
    if (openldev_find_toolbar_search_files_is_active (findtoolbar))
    {
      gtk_notebook_set_current_page (GTK_NOTEBOOK (output), 1);
      
      vector<string> search_results;
      vector<string> sources = openldev_project_settings_get_sources_all (project);
      openldev_search_all_files (search_results, sources, project->location, entry_text);
      
      if (search_results.size() > 0)
      {
        output_list_clear (LIST (output->search));
        GtkListStore *model = gtk_list_store_new (1, G_TYPE_STRING);
	      GtkTreeIter iter;
	
	      for (unsigned int i=0; i < search_results.size(); i++) 
        {
		      gtk_list_store_append (model, &iter);
		      gtk_list_store_set (model, &iter, 0, search_results[i].c_str(), -1);
	      }
    
        gtk_tree_view_set_model (GTK_TREE_VIEW (output->search), GTK_TREE_MODEL (model));
	      g_object_unref (model);
        
        openldev_find_toolbar_show_error (findtoolbar, FALSE);
      }
      else
        openldev_find_toolbar_show_error (findtoolbar, TRUE);
      
      search_results.clear ();
      sources.clear ();
    }
    else
    {
      GtkSourceBuffer *buffer = file_manager_get_current_buffer (files);
      GtkTextIter iter, end, enditer;
      gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (buffer), &enditer);
        
      gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER (buffer), &iter, 
        gtk_text_buffer_get_selection_bound (GTK_TEXT_BUFFER (buffer)));
      gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER (buffer), &end, 
        gtk_text_buffer_get_insert (GTK_TEXT_BUFFER (buffer)));
      if (gtk_text_iter_compare (&end, &iter) > 0)
        iter = end;
  
      if (gtk_text_iter_forward_search (&iter, gtk_combo_box_get_active_text 
                                        (GTK_COMBO_BOX (findtoolbar->find_text)), 
                                        GTK_TEXT_SEARCH_VISIBLE_ONLY, 
                                        &iter, &end, &enditer))
            openldev_mark_found_text (openldev, &iter, &end);
      else 
      {
        gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (buffer), &iter);
        if (gtk_text_iter_forward_search (&iter, entry_text.c_str(), 
            GTK_TEXT_SEARCH_VISIBLE_ONLY, &iter, &end, &enditer))
              openldev_mark_found_text (openldev, &iter, &end);
        else
          openldev_find_toolbar_show_error (findtoolbar, TRUE);
      }
    }
  }
}

void 
openldev_search_all_files (vector<string> &search_results, 
                           vector<string> &sources,
                           string location,
                           string find_text)
{
  string text;
  gchar buff[5];

  while (sources.size() > 0)
  {
    int line = 1;
  
    ifstream fin (sources[0].c_str());
    sources[0].erase (0, location.length());
    while (getline (fin, text))
    {
      if (text.find (find_text, 0) != string::npos)
      {
        snprintf (buff, sizeof(buff), "%d", line);
        search_results.push_back (string(sources[0] + ":" + buff + ": " + text));
      }
      line++;
    }
    
    fin.close ();
    sources.erase (sources.begin());
  }
}

void openldev_mark_found_text (OpenLDev *openldev,
                               GtkTextIter *iter, 
                               GtkTextIter *end)
{
  FindToolbar *findtoolbar;
  FileManager *files;

  g_object_get (openldev, "find-toolbar", &findtoolbar, "file-browser", &files, NULL);
  GtkTextBuffer *buffer = GTK_TEXT_BUFFER (file_manager_get_current_buffer (files));
  gtk_text_buffer_select_range (buffer, iter, end);
  file_manager_scroll_to_mark (files);
  gtk_widget_grab_focus (GTK_WIDGET (file_manager_get_current_view (files)));
      
  openldev_find_toolbar_show_error (findtoolbar, FALSE);
}

void openldev_find_and_replace (OpenLDev *openldev)
{
  FileManager *files = FILE_MANAGER (openldev->files);
  FindToolbar *findtoolbar = openldev->findtoolbar;
  gint count = 0;

  if (file_manager_get_current_tab (files) == -1)
    return;
  openldev_menu_edit_find_next (openldev);
  
  string gladefile = string (INSTALL_PREFIX) + "/share/openldev/glade/replace.glade";
  GladeXML *xml = glade_xml_new (gladefile.c_str(), NULL, NULL);
  GtkWidget *dialog = glade_xml_get_widget (xml, "dialog");
  GtkWidget *search = glade_xml_get_widget (xml, "search");
  GtkWidget *replace = glade_xml_get_widget (xml, "replace");
  
  gtk_entry_set_text (GTK_ENTRY (search), gtk_combo_box_get_active_text 
                                             (GTK_COMBO_BOX (findtoolbar->find_text)));
  
  int result = gtk_dialog_run (GTK_DIALOG (dialog));
  switch (result)
  {
    case (GTK_RESPONSE_APPLY):
    {
      GtkTextBuffer *buffer = GTK_TEXT_BUFFER (file_manager_get_current_buffer (files));
      string text = openldev_file_get_text_in_buffer (GTK_SOURCE_BUFFER (buffer)),
             replace_text = gtk_entry_get_text (GTK_ENTRY (replace)),
             find_text = gtk_entry_get_text (GTK_ENTRY (search));
      gtk_entry_set_text (GTK_ENTRY (gtk_bin_get_child (GTK_BIN (findtoolbar->find_text))), 
                          find_text.c_str());
      
      if (replace_text.empty())
        break;

      int location = text.find (find_text, 0);
      while (location != string::npos)
      {
        text.erase (location, find_text.length());
        text.insert (location, replace_text);
        location = text.find (find_text, location + replace_text.length());
      }
      
      gtk_text_buffer_set_text (buffer, text.c_str(), text.length());
      
      break;
    }
    default:
      break;
  }
  
  gtk_widget_destroy (dialog);
}
