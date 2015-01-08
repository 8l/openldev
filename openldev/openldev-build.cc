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
 
#include "openldev-build.h"

static void
mark_errors_and_warnings (OpenLDev *openldev)
{
  OutputBuild *build = OUTPUT_BUILD (OUTPUT (openldev->output)->build);
  FileManager *files = FILE_MANAGER (openldev->files);
  GtkSourceBuffer *buffer;
  GtkTextIter start, end;
  BuildMessage *msg;
  GSList *temp;
  GList *tmp;
  gint tab;
  
  for (tab = 0; tab < files->tab_count; tab++)
  {
    GtkSourceMarker *marker1, *marker2;
    
    buffer = FILE_TAB (files->tabs[tab])->buffer;
    marker1 = gtk_source_buffer_get_first_marker (buffer);
    if (gtk_source_buffer_get_first_marker (buffer) == NULL)
      continue;
  
    while (marker1 != NULL)
    {
      gchar *type = gtk_source_marker_get_marker_type (GTK_SOURCE_MARKER (marker1));
      
      marker2 = marker1;
      marker1 = gtk_source_marker_next (marker2);
      
      if (g_ascii_strcasecmp (type, "error") == 0)
        gtk_source_buffer_delete_marker (buffer, marker2);
      else if (g_ascii_strcasecmp (type, "warning") == 0)
        gtk_source_buffer_delete_marker (buffer, marker2);
    }
  }
  
  for (tmp = build->messages; tmp != NULL; tmp = tmp->next)
  {
    msg = (BuildMessage*) tmp->data;
    tab = file_manager_get_file_is_open (files, msg->location);
    
    if (tab != -1)
    {
      buffer = FILE_TAB (files->tabs[tab])->buffer;
      gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER (buffer), &start, msg->line - 1);
      
      if (msg->type == OUTPUT_ERROR)
        gtk_source_buffer_create_marker (buffer, NULL, "error", &start);
      else
        gtk_source_buffer_create_marker (buffer, NULL, "warning", &start);
        
    }
  }
}

/* Run the autogen.sh command if a project is currently open. */
void 
openldev_menu_build_auto_generate (OpenLDev *openldev)
{
  GtkWidget *output = openldev->output;
  ProjectSettings *project = openldev->project;
  StatusBar *statusbar = openldev->statusbar;
  EnvironmentSettings *env = openldev->env;
  gchar *message;
  gboolean result;
  
  if (!openldev_project_is_open (project))
    return;
  
  gtk_notebook_set_current_page (GTK_NOTEBOOK (output), 0);
  chdir (project->location);
  
  message = g_strconcat ("Running: ", project->autogen, NULL);
  openldev_status_bar_lock_message (statusbar, STATUSBAR_LEFT, message);
  g_signal_emit_by_name (G_OBJECT (openldev), "auto-generate", 0);
  result = output_build_run_command (OUTPUT_BUILD (OUTPUT (output)->build),
                                     project->autogen, project->location);
  
  if (result || !result)
    openldev_status_bar_unlock_message (statusbar, STATUSBAR_LEFT);
}

static gchar*
openldev_get_build_location (vector<string> subdirs,
                             gchar *plocation)
{
  GtkWidget *dialog, *combobox;
  gchar *location;
  string temp;
  gint i, j;

  dialog = gtk_dialog_new_with_buttons ("Build Directory", NULL, GTK_DIALOG_NO_SEPARATOR,
                                        GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_ACCEPT);
  combobox = gtk_combo_box_new_text ();

  for (i = 0; i < subdirs.size() - 2; i++)
    for (j = 0; j < subdirs.size() - 1; j++)
      if (subdirs[i] < subdirs[j])
      {
        temp = subdirs[i];
        subdirs[i] = subdirs[j];
        subdirs[j] = temp;
      }

  subdirs.insert (subdirs.begin(), "Build All");
  subdirs.erase (subdirs.end() - 1);
  
  for (i = 0; i < subdirs.size(); i++)
    gtk_combo_box_append_text (GTK_COMBO_BOX (combobox), subdirs[i].c_str());
  gtk_combo_box_set_active (GTK_COMBO_BOX (combobox), 0);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), combobox, FALSE, TRUE, 5);
  gtk_container_set_border_width (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), 5);

  gtk_widget_show_all (dialog);
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
  {
    location = g_strconcat (g_strndup (plocation, strlen (plocation) - 1),
                            gtk_combo_box_get_active_text (GTK_COMBO_BOX (combobox)),
                            NULL);
    
    if (gtk_combo_box_get_active (GTK_COMBO_BOX (combobox)) == 0)
    {
      gtk_widget_destroy (dialog);
      g_free (location);
      return g_strconcat (g_strndup (plocation, strlen (plocation) - 1), "/", NULL);
    }
    
    gtk_widget_destroy (dialog);
    return location;
  }
  else
  {
    gtk_widget_destroy (dialog);
    return NULL;
  }
}

/* If a project is open, build it. Run autogen.sh and then build again if
 * Makefiles are not found. All of the files are first saved. */
void 
openldev_menu_build_project (OpenLDev *openldev)
{
  GtkWidget *output = openldev->output;
  ProjectSettings *project = openldev->project;
  StatusBar *statusbar = openldev->statusbar;
  EnvironmentSettings *env = openldev->env;
  EditorProperties *editor = openldev->editor;
  gchar *message, *location;
  gboolean result;
  vector<string> subdirs;
  
  if (!openldev_project_is_open (project))
    return;
  
  subdirs = openldev_project_settings_get_makefile_list (project);
  for (int i = 0; i < subdirs.size(); i++)
  {
    subdirs[i].erase (0, strlen (project->location) - 1);
    subdirs[i].erase (subdirs[i].length() - 11, 11);
  }
  
  location = openldev_get_build_location (subdirs, project->location);
  if (location == NULL)
    return;

  gtk_notebook_set_current_page (GTK_NOTEBOOK (output), 0);
  openldev_menu_file_save_all (openldev);
  chdir (location);
  
  if (editor->build_dir != NULL)
  {
    g_free (editor->build_dir);
    editor->build_dir = NULL;
  }
  
  editor->build_dir = g_strdup (location);
  editor->build_dir[strlen(editor->build_dir)-1] = 0;

  message = g_strconcat ("Running: ", project->build, NULL);
  openldev_status_bar_lock_message (statusbar, STATUSBAR_LEFT, message);

  g_signal_emit_by_name (G_OBJECT (openldev), "build", 0);
  result = output_build_run_command (OUTPUT_BUILD (OUTPUT (output)->build),
                                     project->build, location);
  
  if (result || !result)
  {
    openldev_status_bar_unlock_message (statusbar, STATUSBAR_LEFT);
    mark_errors_and_warnings (openldev);
  }
  
  g_free (message);
}

/* Clean all of the sources, build files, object files and those files
 * created by configure. */
void 
openldev_menu_build_clean (OpenLDev *openldev)
{
  GtkWidget *output = openldev->output;
  ProjectSettings *project = openldev->project;
  StatusBar *statusbar = openldev->statusbar;
  EnvironmentSettings *env = openldev->env;
  gchar *message;
  gboolean result;
  
  if (!openldev_project_is_open (project))
    return;

  gtk_notebook_set_current_page (GTK_NOTEBOOK (output), 0);
  chdir (project->location);
    
  message = g_strconcat ("Running: ", project->clean, NULL);
  openldev_status_bar_lock_message (statusbar, STATUSBAR_LEFT, message);
  g_signal_emit_by_name (G_OBJECT (openldev), "make-clean", 0);
  result = output_build_run_command (OUTPUT_BUILD (OUTPUT (output)->build), 
                                     project->clean, project->location);
    
  if (result || !result)
    openldev_status_bar_unlock_message (statusbar, STATUSBAR_LEFT);
}

/* Compile current file if a file and a project is open. The function then
 * creates the compile string from the project language & runs the command. */
void 
openldev_menu_build_compile (OpenLDev *openldev)
{
  ProjectSettings *project = openldev->project;
  StatusBar *statusbar = openldev->statusbar;
  EnvironmentSettings *env = openldev->env;
  GtkWidget *output = openldev->output;
  GtkWidget *files = openldev->files;
  gchar *message, *location, *fn;
  GString *command;
  gboolean result;
  gint loc;
  
  if (!openldev_project_is_open (project))
    return;

  gtk_notebook_set_current_page (GTK_NOTEBOOK (output), 0);
  command = g_string_new (project->compile);
  fn = file_manager_get_current_filename (FILE_MANAGER (files));
    
  loc = g_strstr_len (command->str, command->len, "{FILE}") - command->str;
  while (loc >= 0)
  {
    command = g_string_erase (command, loc, 6);
    command = g_string_insert (command, loc, fn);
    loc = g_strstr_len (command->str, command->len, "{FILE}") - command->str;
  }
    
  message = g_strconcat ("Running: ", command->str, NULL);
  location = g_strconcat (project->location, "src/", NULL);
  chdir (project->location);

  openldev_status_bar_lock_message (statusbar, STATUSBAR_LEFT, message);
  g_signal_emit_by_name (G_OBJECT (openldev), "compile", 0);
  result = output_build_run_command (OUTPUT_BUILD (OUTPUT (output)->build), 
                                     command->str, project->location);
    
  if (result || !result)
  {
    openldev_status_bar_unlock_message (statusbar, STATUSBAR_LEFT);
    mark_errors_and_warnings (openldev);
  }
}

/* If a project is open, run the ./configure command in the project directory. */
void 
openldev_menu_build_configure (OpenLDev *openldev)
{
  GtkWidget *output = openldev->output;
  ProjectSettings *project = openldev->project;
  StatusBar *statusbar = openldev->statusbar;
  EnvironmentSettings *env = openldev->env;
  gchar *message, *command, *conf;
  GString *configure;
  gboolean result;
  
  if (!openldev_project_is_open (project))
    return;

  configure = g_string_new (project->configure);
  if (g_strstr_len (configure->str, strlen (configure->str), "./configure"))
    configure = g_string_erase (configure, 0, 11);
  g_strchug (configure->str);
  conf = openldev_input_dialog_run ("Parameters:", "Configure Parameters", configure->str);
    
  if (g_ascii_strcasecmp (conf, "EMPTY") != 0)
  {
    gtk_notebook_set_current_page (GTK_NOTEBOOK (output), 0);
    chdir (project->location);
    
    if (strlen (conf) > 0)
      command = g_strconcat ("./configure ", conf, NULL);
    else
      command ="./configure";
      
    project->configure = g_strdup (command);
    message = g_strconcat ("Running: ", command, NULL);
    openldev_status_bar_lock_message (statusbar, STATUSBAR_LEFT, message);
    g_signal_emit_by_name (G_OBJECT (openldev), "configure", 0);
    result = output_build_run_command (OUTPUT_BUILD (OUTPUT (output)->build), 
                                       command, project->location);
   
    if (result || !result)
      openldev_status_bar_unlock_message (statusbar, STATUSBAR_LEFT);
  }
}

/* If a project is open, run the executable in the terminal specified by the user. */
void 
openldev_menu_build_execute (OpenLDev *openldev)
{
  ProjectSettings *project = openldev->project;
  gchar *command, *location;
  GString *exec;
  gint pid, dif;
  
  if (!openldev_project_is_open (project))
    return;

  exec = g_string_new (project->execute);
  
  location = g_strrstr (exec->str, "{BASE}");
  if (location != NULL)
  {
    dif = location - exec->str;
    exec = g_string_erase (exec, dif, 6);
    exec = g_string_insert (exec, dif, project->location);
  }

  location = g_strrstr (exec->str, "{NAME}");
  if (location != NULL)
  {
    dif = location - exec->str;
    exec = g_string_erase (exec, dif, 6);
    exec = g_string_insert (exec, dif, project->name);
  }
  
  command = g_strconcat ("gnome-terminal -e \"openldev_launcher ", exec->str, "\"", NULL);    
  g_signal_emit_by_name (G_OBJECT (openldev), "execute", 0);
  pid = openldev_fork_new_process (command);
  
  g_string_free (exec, TRUE);
  g_free (command);
}

/* Stop the current compilation, clean, configure, autogen or build. */
void 
openldev_menu_build_stop (OpenLDev *openldev)
{ 
  OutputManager *output = OUTPUT (openldev->output);
  output_build_stop_build (OUTPUT_BUILD (output->build)); 
}
