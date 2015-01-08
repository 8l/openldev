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
 
#include "openldev-file.h"

static void openldev_on_delete_tab (GtkWidget*, gpointer);
static void openldev_file_connect_signals (OpenLDev*);
static void openldev_add_recent_file (OpenLDev*, gchar*);
static void openldev_upgrade_project (string&);

// Create a new file. This doesn't add the file to the project. The user must
// do that manually through the makefile editor.
void 
openldev_menu_file_new_file (OpenLDev *openldev)
{
  ProjectSettings *project = openldev->project;
  SideBar *sidebar = SIDEBAR (openldev->sidebar);
  NewFileDialog *dialog;
  gchar *file;
  
  dialog = openldev_new_file_dialog_new (project->location);
  openldev_new_file_dialog_run (dialog, file);
    
  if (g_ascii_strcasecmp (file, "NULL") != 0 && strlen (file) > 0)
  {
    file_browser_refresh (FILE_BROWSER (sidebar_get_file_browser (sidebar)));
    g_signal_emit_by_name (G_OBJECT (openldev), "file-new", 0);
    openldev_open_file (openldev, file); 
  }
    
  gtk_widget_destroy (dialog->dialog);
  g_object_unref (dialog->xml);
  g_slice_free1 (sizeof (NewFileDialog), dialog);
}

/* This function is where you would apply the changes and destroy the assistant. */
static void 
npd_assistant_close (GtkWidget *assistant, 
                     NewProjectDialog *npd)
{
  OpenLDev *openldev = (OpenLDev*) g_object_get_data (G_OBJECT (assistant), "openldev");
  OutputManager *output = OUTPUT (openldev->output);
  EnvironmentSettings *env = openldev->env;
  gchar *directory, *fn, *type, *pf, *comm;
  GList *entries = NULL, *real_entries = NULL, *ptr;
  GString *script;
  gint exitval;
  GDir *dir;
  
  if (npd != NULL)
  {
    type = gtk_combo_box_get_active_text (GTK_COMBO_BOX (npd->combobox));
    directory = openldev_add_prefix ("/share/openldev/templates/");
    dir = g_dir_open (directory, 0, NULL);
    while ((fn = (gchar*) g_dir_read_name (dir)) != NULL)
      entries = g_list_prepend (entries, (gpointer) g_strdup (fn));
  
    for (ptr = entries; ptr != NULL; ptr = ptr->next)
      if (g_strrstr ((gchar*) ptr->data, ".pl") != NULL)
        real_entries = g_list_prepend (real_entries, ptr->data);

    for (ptr = real_entries; ptr != NULL; ptr = ptr->next) 
    {
      gchar buff[8], *command;
      GString *text;
      FILE *in;
      gint i;
    
      command = g_strdup_printf ("perl %s%s name", directory, (gchar*) ptr->data);
      text = g_string_new (NULL);
      in = popen (command, "r");
      while (fgets (buff, sizeof (buff), in))
      {
        i = 0;
        while (buff[i] != '\0' && i < 8)
        {
          text = g_string_append_c (text, buff[i]);
          i++;
        }
      }
      pclose (in);

      if (g_ascii_strcasecmp (text->str, type) == 0)
      {
        script = g_string_new (directory);
        script = g_string_append (script, (gchar*) ptr->data);
        break;
      }
      
      g_string_free (text, TRUE);
    }

    pf =   g_strdup_printf ("%s/%s/%s.dev", 
                            gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (npd->location)),
                            gtk_entry_get_text (GTK_ENTRY (npd->name)),
                            gtk_entry_get_text (GTK_ENTRY (npd->name)));
    comm = g_strdup_printf ("perl %s \"%s/%s/\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"", script->str,
                            gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (npd->location)),
                            gtk_entry_get_text (GTK_ENTRY (npd->name)),
                            gtk_entry_get_text (GTK_ENTRY (npd->name)),
                            gtk_entry_get_text (GTK_ENTRY (npd->author)),
                            gtk_entry_get_text (GTK_ENTRY (npd->homepage)),
                            gtk_entry_get_text (GTK_ENTRY (npd->version)),
                            gtk_entry_get_text (GTK_ENTRY (npd->email)));
    gtk_widget_destroy (assistant);
  
    gtk_notebook_set_current_page (GTK_NOTEBOOK (output), 0);
    g_signal_emit_by_name (G_OBJECT (openldev), "project-new", 0);
    gboolean result = output_build_run_command (OUTPUT_BUILD (output->build), 
                                                comm, "PLACEHOLDER");

    if (result)
      openldev_open_project (openldev, pf);
    
    g_list_foreach (entries, (GFunc) g_free, NULL);
    g_list_free (entries);
    g_list_free (real_entries);
    g_string_free (script, TRUE);
  }
  else
    gtk_widget_destroy (assistant);
}

// Create a new project by calling the new project dialog & then open the
// project that was created.
void 
openldev_menu_file_new_project (OpenLDev *openldev)
{
  NewProjectDialog *npd = openldev_new_project_dialog_new ((gpointer) openldev);
  
  g_signal_connect (G_OBJECT (npd->assistant), "cancel",
                    G_CALLBACK (npd_assistant_close), NULL);
  g_signal_connect (G_OBJECT (npd->assistant), "close",
                    G_CALLBACK (npd_assistant_close), (gpointer) npd);
}

// Get the file that needs to be opened. This function doesn't actually open the
// file. Therefore, it is tricky and lies, but oh well.
void 
openldev_menu_file_open (OpenLDev *openldev)
{
  FileManager *files = FILE_MANAGER (openldev->files);
  GtkWidget *dialog;
  gint l, result, i;
  GSList *filenames;
  string temp, file = "";
  
  dialog = gtk_file_chooser_dialog_new ("Choose a File", GTK_WINDOW (openldev),
                                        GTK_FILE_CHOOSER_ACTION_OPEN, 
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, 
                                        GTK_STOCK_OPEN, GTK_RESPONSE_OK, NULL);
  gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog), TRUE);
  
  // Set default folder
  if (file_manager_get_file_open (files))
  {
    temp = file_manager_get_current_filename (files);
    
    l = temp.find ("/", 0);
    while (l != string::npos)
    {
      file += temp.substr (0, l + 1);
      temp.erase (0, l + 1);
      l = temp.find ("/", 0);
    }
    
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), file.c_str());
  } 
  
  result = gtk_dialog_run (GTK_DIALOG (dialog));
  switch (result)
  {
    // If OK is pressed
    case (GTK_RESPONSE_OK):
    {
      // Check if file is a .dev file
      filenames = gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER (dialog));
      for (i = 0; i < g_slist_length (filenames); i++)
        openldev_open_file (openldev, (gchar*) g_slist_nth_data (filenames, i));
    }
    default:
      gtk_widget_destroy (dialog);
      break;
  }
}

void 
openldev_menu_file_open_project (OpenLDev *openldev)
{
  GtkFileFilter *filter;
  GtkWidget *dialog;
  string filename;
  gint result;
  
  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, "OpenLDev Project Files");
  gtk_file_filter_add_pattern (filter, "*.dev");
  gtk_file_filter_add_pattern (filter, "*.openldev");
  
  dialog = gtk_file_chooser_dialog_new ("Choose a Project", GTK_WINDOW (openldev), 
                                        GTK_FILE_CHOOSER_ACTION_OPEN, 
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                        GTK_STOCK_OPEN, GTK_RESPONSE_OK, NULL);
  gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (dialog), filter);
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), g_get_home_dir ());
  
  result = gtk_dialog_run (GTK_DIALOG (dialog));
  switch (result)
  {
    case (GTK_RESPONSE_OK):
    {
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      gtk_widget_destroy (dialog);
      gboolean result_val;
      
      if (filename.find (".dev", 0) != string::npos ||
          filename.find (".openldev", 0) != string::npos)
        result_val = openldev_open_project (openldev, (gchar*) filename.c_str());
      break;
    }
    default:
      gtk_widget_destroy (dialog);
      break;
  }
}

gboolean 
openldev_open_file (OpenLDev *openldev,
                    gchar *file)
{
  FileManager *files = FILE_MANAGER (openldev->files);
  ProjectSettings *project = openldev->project;
  EnvironmentSettings *env = openldev->env;
  string filename = file;
  gchar *font_face, *font;
  GtkSourceBuffer *buffer;
  PangoFontDescription *fd;
  PangoTabArray *tabs;
  gint font_size;
  FileTab *tab;
  
  if (!g_file_test (filename.c_str(), G_FILE_TEST_EXISTS))
  {
    openldev_message ("OpenLDev Error", g_strdup_printf("The following file cannot be opened because it does not exist: %s", filename.c_str()), GTK_MESSAGE_ERROR);
    if (project->name == NULL)
      openldev_project_settings_remove_lastfile (project, (gchar*) filename.c_str());
    return FALSE;
  }
  else
  {
    if (file_manager_add_tab (files, filename.c_str(), env))
    {
      if (project->name != NULL)
        openldev_project_settings_add_lastfile (project, (gchar*) filename.c_str());
  
      buffer = file_manager_get_current_buffer (files);
      file_manager_set_properties (files, FALSE, env);
      openldev_file_connect_signals (openldev);
      
      g_object_get (env, "font-face", &font_face, "font-size", &font_size, NULL);
      tab = FILE_TAB (files->tabs[files->tab_count - 1]);
      font = g_strdup_printf ("%s %i", font_face, font_size);
      fd = pango_font_description_from_string (font);
      tabs = file_manager_get_tab_array (fd, env, GTK_WIDGET (tab));
      
      gtk_text_view_set_tabs (GTK_TEXT_VIEW (tab), tabs);
      file_manager_modify_fonts (files, fd);
      openldev_add_recent_file (openldev, (gchar*) filename.c_str());
      g_signal_emit_by_name (G_OBJECT (openldev), "file-open", 0);
      pango_font_description_free (fd);
      
      menu_handle_manage_file_items (openldev->menus, TRUE);
    }
  }
  
  return TRUE;
}

static void 
openldev_add_recent_file (OpenLDev *openldev,
                          gchar *filename)
{
  EnvironmentSettings *env = openldev->env;
  MenuHandle *menus = openldev->menus;
  GSList *recent_files;
  gint i;

  g_object_get (env, "recent-files", &recent_files, NULL);
  environment_add_recent_file (env, (gchar*) filename);
  
  for (i = 0; i < 5; i++)
  {
    string recent, name, tooltip;
  
    recent = g_strdup_printf ("/MenuBar/File/FileRecent%i", i + 1);
    
    if (g_slist_length (recent_files) <= i)
      gtk_widget_hide (gtk_ui_manager_get_widget (menus->uimanager, recent.c_str()));
    else
    {
      name = g_strdup_printf ("%i. %s", i + 1, environment_get_recent_short (env, i));
      tooltip = g_strconcat ("Open \"", (gchar*) g_slist_nth_data (recent_files, i), "\"", NULL);
      gtk_label_set_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN (gtk_ui_manager_get_widget 
                          (menus->uimanager, recent.c_str())))), name.c_str());
      gtk_widget_show (gtk_ui_manager_get_widget (menus->uimanager, recent.c_str()));
      gtk_tooltips_set_tip (menus->tooltips, gtk_ui_manager_get_widget 
                            (menus->uimanager, recent.c_str()), tooltip.c_str(), tooltip.c_str());
    }
  }
}

// Connect the signals for the newly opened file
static void 
openldev_file_connect_signals (OpenLDev *openldev)
{
  FileManager *files = FILE_MANAGER (openldev->files);
  
  g_signal_connect (G_OBJECT (file_manager_get_current_view (files)), 
                    "key-press-event", G_CALLBACK (openldev_on_textview_keypress), 
                    (gpointer) openldev);
  g_signal_connect (G_OBJECT (file_manager_get_current_view (files)), 
                    "event", G_CALLBACK (openldev_on_textview_event), (gpointer) openldev);
  g_signal_connect (G_OBJECT (FILE_TAB (files->tabs[files->tab_count-1])->button), "clicked",
                    G_CALLBACK (openldev_on_delete_tab), (gpointer) openldev);
}

gboolean 
openldev_open_project (OpenLDev *openldev,
                       gchar *file)
{
  FileManager *files = FILE_MANAGER (openldev->files);
  ProjectSettings *project = openldev->project;
  EnvironmentSettings *env = openldev->env;
  string filename = file;
  GSList *list = NULL;
  gboolean success;
  
  if (filename.find (".openldev") != string::npos)
  {
    openldev_message ("OpenLDev Message", "This project file is no longer compatible with OpenLDev! You have two options.\n\n1. You can create a new project and import your sources into that project.\n\n2. You can use OpenLDev Switch available at http://mannequin.invigorated.org to upgrade your project.", GTK_MESSAGE_INFO);
    return FALSE;
  }

  file_manager_erase_all_tabs (files);
  success = openldev_project_settings_load_project_file (project, (gchar*) filename.c_str());
 
  if (success)
  {
    for (list = project->lastfiles; list != NULL; list = list->next)
      openldev_open_file (openldev, (gchar*) list->data);

    openldev_populate_treeview (openldev);
    g_object_set (env, "last-project", (gchar*) filename.c_str(), NULL);
    g_signal_emit_by_name (G_OBJECT (openldev), "project-open", 0);
    menu_handle_manage_project_items (openldev->menus, TRUE);
    
    return TRUE;
  }
  else
  {
    g_object_set (env, "last-project", "EMPTY", NULL);
    openldev_message ("OpenLDev Error", g_strdup_printf ("The following project could not be found: %s", (gchar*) filename.c_str()), GTK_MESSAGE_ERROR);
  }
  
  return FALSE;
}

void 
openldev_menu_file_save (OpenLDev *openldev)
{
  FileManager *files = FILE_MANAGER (openldev->files);
  GtkSourceBuffer *buffer;
  string current_fn;
  gint current;

  if (file_manager_get_file_open (files))
  {
    current = gtk_notebook_get_current_page (GTK_NOTEBOOK (files));
    
    if (IS_FILE_TAB (FILE_TAB (files->tabs[current])))
    {
      current_fn = file_manager_get_current_filename (files);
      if (current_fn.empty() || current_fn == "EMPTY")
        openldev_menu_file_save_as (openldev);
  
      // Save the file & set as unmodified
      buffer = file_manager_get_current_buffer (files);
      openldev_write_file (file_manager_get_current_filename (files),
                           openldev_file_get_text_in_buffer (buffer));

      file_manager_mark_current_tab_modified (files, FALSE, current);
      g_signal_emit_by_name (G_OBJECT (openldev), "file-save", 0);
    }
  }
}

void 
openldev_menu_file_save_as (OpenLDev *openldev)
{
  FileManager *files = FILE_MANAGER (openldev->files);
  ProjectSettings *project = openldev->project;
  gint current, l, result;
  GtkWidget *dialog;
  string temp, file;

  if (file_manager_get_file_open (files))
  {
    current = gtk_notebook_get_current_page (GTK_NOTEBOOK (files));
    
    if (IS_FILE_TAB (FILE_TAB (files->tabs[current])))
    {
      dialog = gtk_file_chooser_dialog_new ("Save file as", GTK_WINDOW (openldev),
                                            GTK_FILE_CHOOSER_ACTION_SAVE,
                                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                            GTK_STOCK_SAVE, GTK_RESPONSE_OK,
                                            NULL);
      
      temp = file_manager_get_current_filename (files);
      l = temp.find ("/", 0);
      while (l != string::npos)
      {
        file += temp.substr (0, l + 1);
        temp.erase (0, l + 1);
        l = temp.find ("/", 0);
      }

      // Add the filename to the text entry in Save As dialog
      gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), file.c_str());
      gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog),
          file_manager_get_current_filename_short (files).c_str());
  
      // Open fileselection dialog to get file location
      result = gtk_dialog_run (GTK_DIALOG (dialog));
      switch (result)
      {
        // If OK is pressed
        case (GTK_RESPONSE_OK):
        {
          if (project->name != NULL)
          {
            openldev_project_settings_remove_lastfile (project, 
                     file_manager_get_current_filename (files));
            openldev_project_settings_add_lastfile (project, 
                     gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog)));
          }
        
          file_manager_set_filename (files,
               gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog)));
          g_signal_emit_by_name (G_OBJECT (openldev), "file-save-as", 0);
          openldev_menu_file_save (openldev);
          break;
        }
        default:
          break;
      }
    
      gtk_widget_destroy (dialog);
    }
  }
}

// Call the notebookmanager function to save all of the files that are
// currently open
void 
openldev_menu_file_save_all (OpenLDev *openldev)
{
  FileManager *files = FILE_MANAGER (openldev->files);
  GtkTextBuffer *buffer;
  gint i;

  for (i = 0; i < gtk_notebook_get_n_pages (GTK_NOTEBOOK (files)); i++)
  {
    // Save the file & set as unmodified
    if (IS_FILE_TAB (FILE_TAB (files->tabs[i])))
    {
      if (FILE_TAB (files->tabs[i])->modified)
      {
        buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (files->tabs[i]));
        openldev_write_file (FILE_TAB (files->tabs[i])->filename,
                             openldev_file_get_text_in_buffer (GTK_SOURCE_BUFFER (buffer)));
        file_manager_mark_current_tab_modified (files, FALSE, i);
      }
    }
  }
  
  g_signal_emit_by_name (G_OBJECT (openldev), "file-save-all", 0);
}

// If the user chooses to allow it, revert to the saved version of a file. Then
// the syntax highlighting, action manager and line numbering are refreshed
void 
openldev_menu_file_revert (OpenLDev *openldev)
{
  FileManager *files = FILE_MANAGER (openldev->files);
  gint current;
  
  current = gtk_notebook_get_current_page (GTK_NOTEBOOK (files));
  if (IS_FILE_TAB (FILE_TAB (files->tabs[current])))
    if (file_manager_get_file_open (files))
      if (openldev_question ("Are you sure you want to reload this file? Any unsaved changes will be lost."))
      {
        file_manager_revert_to_saved (files);
        g_signal_emit_by_name (G_OBJECT (openldev), "file-reload", 0);
      }
}

static void 
openldev_on_delete_tab (GtkWidget *widget,
                        gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  FileManager *files = FILE_MANAGER (openldev->files);
  ProjectSettings *project = openldev->project;
  gint i;
  
  if (project != NULL)
    if (project->name != NULL && g_ascii_strcasecmp (project->name, "") != 0)
      for (i=0; i < files->tab_count; i++)
        if (FILE_TAB (files->tabs[i])->button == widget)
          openldev_project_settings_remove_lastfile (project, 
                                    FILE_TAB (files->tabs[i])->filename);

  if (file_manager_on_delete_tab (widget, (gpointer) files))
    menu_handle_manage_file_items (openldev->menus, FALSE);
}
