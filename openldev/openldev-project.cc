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
 
#include "openldev-project.h"

void openldev_close_project (OpenLDev*);

// If a project is open, load the project information dialog.
void openldev_menu_project_info (OpenLDev *openldev)
{
  ProjectSettings *project;

  g_object_get (openldev, "project", &project, NULL);
  if (project->name == NULL) 
    openldev_message ("OpenLDev Error", "Please open or create a project first!", GTK_MESSAGE_ERROR);
  else 
  {
    ProjectInfoDialog *dialog = openldev_project_info_dialog_new (project);
    openldev_project_info_dialog_run (dialog);
  }
}

void openldev_menu_file_import_project (OpenLDev *openldev)
{  
  string project = openldev_import_project ();
  
  if (project == "NONE")
    return;
  
  openldev_write_file (project, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<OpenLDevProject version=\"0.6+\">\n  <ProjectTemplate>C</ProjectTemplate>\n  <Language>C</Language>\n  <Compile>make {FILE}</Compile>\n  <Build>make</Build>\n  <AutoGenerate>./autogen.sh</AutoGenerate>\n  <Configure>./configure</Configure>\n  <Clean>make clean</Clean>\n  <Execute>{BASE}src/{NAME}</Execute>\n</OpenLDevProject>\n");
  g_signal_emit_by_name (G_OBJECT (openldev), "project-import", 0);
  openldev_open_project (openldev, (gchar*) project.c_str());
  openldev_message ("OpenLDev Message", "The project was successfully imported!", GTK_MESSAGE_INFO);
}

// If a project is open, load the project properties dialog and set the
// changes specified by the user. Also, rewrite the project compile flags to
// the src/Makefile.am
void openldev_menu_project_preferences (OpenLDev *openldev)
{  
  ProjectSettings *project;

  g_object_get (openldev, "project", &project, NULL);
  if (project->name == NULL)
    openldev_message ("OpenLDev Error", "Please open or create a project first!", GTK_MESSAGE_ERROR);
  else 
  {
    ProjectDialog *dialog = openldev_project_dialog_new (project);
    int result = openldev_project_dialog_run (dialog);
    
    if (result < 0)
    {
      project->version = g_strdup ((gchar*) gtk_entry_get_text (GTK_ENTRY (dialog->version)));
      project->configure = g_strdup ((gchar*) gtk_entry_get_text (GTK_ENTRY (dialog->configure)));
      project->clean = g_strdup ((gchar*) gtk_entry_get_text (GTK_ENTRY (dialog->clean)));
      project->build = g_strdup ((gchar*) gtk_entry_get_text (GTK_ENTRY (dialog->build)));
      project->compile = g_strdup ((gchar*) gtk_entry_get_text (GTK_ENTRY (dialog->compile)));
    }
    
    gtk_widget_destroy (dialog->dialog);
    g_object_unref (dialog->xml);
    delete dialog;
  }
}

// Close the project, clear all of the files and then disable all of the project
// menu and toolbar items
void openldev_close_project (OpenLDev *openldev)
{
  menu_handle_manage_project_items (openldev->menus, FALSE);
  openldev_project_settings_close_project (openldev->project);
  openldev_populate_treeview (openldev);
  openldev_menu_file_save_all (openldev);
  file_manager_erase_all_tabs (FILE_MANAGER (openldev->files));
  g_object_set (openldev->env, "last-project", "EMPTY", NULL);

  g_signal_emit_by_name (G_OBJECT (openldev), "project-close", 0);
}

// Close project file. First call the close project funtion and then erase
// the project files treeview, save all of the files and erase all of the tabs.
void openldev_menu_file_close_project (OpenLDev *openldev)
{
  gboolean is_modified = FALSE;
  FileManager *files;

  g_object_get (openldev, "file-browser", &files, NULL);
  
  for (int i=0; i < files->tab_count; i++)
  {
    if (FILE_TAB (files->tabs[i])->modified)
      is_modified = TRUE;
  }
  
  if (!is_modified)
    openldev_close_project (openldev);
  else if (openldev_question ("You have unsaved files! Are you sure you want to close?"))
    openldev_close_project (openldev);
}
