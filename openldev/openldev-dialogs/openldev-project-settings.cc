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

#include "openldev-project-settings.h"
#include <cstring>

void openldev_project_dialog_on_files_change (GtkComboBox*, gpointer);

gboolean openldev_project_dialog_on_version_change (GtkWidget*, GdkEventFocus*, gpointer);
gboolean openldev_project_dialog_on_configure_change (GtkWidget*, GdkEventFocus*, gpointer);
gboolean openldev_project_dialog_on_clean_change (GtkWidget*, GdkEventFocus*, gpointer);
gboolean openldev_project_dialog_on_compile_change (GtkWidget*, GdkEventFocus*, gpointer);
gboolean openldev_project_dialog_on_build_change (GtkWidget*, GdkEventFocus*, gpointer);
gboolean openldev_project_dialog_on_execute_change (GtkWidget*, GdkEventFocus*, gpointer);
gboolean openldev_project_dialog_on_filecontent_change (GtkWidget*, GdkEventFocus*, gpointer);

// Load the project settings dialog, connect the signals and set the data.
ProjectDialog* 
openldev_project_dialog_new (ProjectSettings *settings)
{
  ProjectDialog *project = new ProjectDialog ();
  
  // Create Makefile.am location string
  string gladefile = openldev_add_prefix ("/share/openldev/glade/projectproperties.glade");
  project->xml = glade_xml_new (gladefile.c_str(), NULL, NULL);
  project->dialog =  glade_xml_get_widget (project->xml, "dialog");
  project->version = glade_xml_get_widget (project->xml, "version");
  project->files = glade_xml_get_widget (project->xml, "files");
  project->filecontent = glade_xml_get_widget (project->xml, "filecontent");
  project->configure = glade_xml_get_widget (project->xml, "configure");
  project->compile = glade_xml_get_widget (project->xml, "compile");
  project->clean = glade_xml_get_widget (project->xml, "clean");
  project->build = glade_xml_get_widget (project->xml, "build");
  project->execute = glade_xml_get_widget (project->xml, "execute");
  project->settings = settings;
  
  g_signal_connect (G_OBJECT (project->execute), "focus-out-event",
                    G_CALLBACK (openldev_project_dialog_on_execute_change),
                    (gpointer) project->settings);
  g_signal_connect (G_OBJECT (project->version), "focus-out-event",
                    G_CALLBACK (openldev_project_dialog_on_version_change),
                    (gpointer) project->settings);
  g_signal_connect (G_OBJECT (project->configure), "focus-out-event",
                    G_CALLBACK (openldev_project_dialog_on_configure_change),
                    (gpointer) project->settings);
  g_signal_connect (G_OBJECT (project->compile), "focus-out-event",
                    G_CALLBACK (openldev_project_dialog_on_compile_change),
                    (gpointer) project->settings);
  g_signal_connect (G_OBJECT (project->build), "focus-out-event",
                    G_CALLBACK (openldev_project_dialog_on_build_change),
                    (gpointer) project->settings);
  g_signal_connect (G_OBJECT (project->clean), "focus-out-event",
                    G_CALLBACK (openldev_project_dialog_on_clean_change),
                    (gpointer) project->settings);
  g_signal_connect (G_OBJECT (project->filecontent), "focus-out-event",
                    G_CALLBACK (openldev_project_dialog_on_filecontent_change),
                    (gpointer) project);
  g_signal_connect (G_OBJECT (project->files), "changed",
                    G_CALLBACK (openldev_project_dialog_on_files_change),
                    (gpointer) project);

  // Create entries for project properties
  gtk_entry_set_text (GTK_ENTRY (project->version), settings->version);
  gtk_entry_set_text (GTK_ENTRY (project->configure), settings->configure);
  gtk_entry_set_text (GTK_ENTRY (project->clean), settings->clean);
  gtk_entry_set_text (GTK_ENTRY (project->build), settings->build);
  gtk_entry_set_text (GTK_ENTRY (project->compile), settings->compile);
  gtk_entry_set_text (GTK_ENTRY (project->execute), settings->execute);
  gtk_widget_modify_font (project->filecontent, 
                          pango_font_description_from_string ("Monospace 9"));
  gtk_combo_box_set_active (GTK_COMBO_BOX (project->files), 0);
  
  return project;
}

// Run the project settings dialog
int 
openldev_project_dialog_run (ProjectDialog* project)
{
  return gtk_dialog_run (GTK_DIALOG (project->dialog));
}

// When the version number is changed, tell the project settings structure
// to be changed.
gboolean 
openldev_project_dialog_on_version_change (GtkWidget *widget,
                                           GdkEventFocus *event,
                                           gpointer data)
{
 openldev_project_settings_set_version ((ProjectSettings*) data, 
                            (gchar*) gtk_entry_get_text (GTK_ENTRY (widget)));
  
  return FALSE;
}

gboolean 
openldev_project_dialog_on_configure_change (GtkWidget *widget,
                                             GdkEventFocus *event,
                                             gpointer data)
{
  openldev_project_settings_set_configure ((ProjectSettings*) data, 
                            (gchar*) gtk_entry_get_text (GTK_ENTRY (widget)));
  return FALSE;
}

gboolean 
openldev_project_dialog_on_clean_change (GtkWidget *widget,
                                         GdkEventFocus *event,
                                         gpointer data)
{
  openldev_project_settings_set_clean ((ProjectSettings*) data, 
                            (gchar*) gtk_entry_get_text (GTK_ENTRY (widget)));
  return FALSE;
}

gboolean 
openldev_project_dialog_on_build_change (GtkWidget *widget,
                                         GdkEventFocus *event,
                                         gpointer data)
{
  openldev_project_settings_set_build ((ProjectSettings*) data, 
                            (gchar*) gtk_entry_get_text (GTK_ENTRY (widget)));
  return FALSE;
}

gboolean 
openldev_project_dialog_on_execute_change (GtkWidget *widget,
                                           GdkEventFocus *event,
                                           gpointer data)
{
  openldev_project_settings_set_execute ((ProjectSettings*) data, 
                            (gchar*) gtk_entry_get_text (GTK_ENTRY (widget)));
  return FALSE;
}

gboolean 
openldev_project_dialog_on_compile_change (GtkWidget *widget,
                                           GdkEventFocus *event,
                                           gpointer data)
{
  openldev_project_settings_set_compile ((ProjectSettings*) data, 
                            (gchar*) gtk_entry_get_text (GTK_ENTRY (widget)));
  return FALSE;
}

// When the user changes the content of the selected file, write the changes
// to the file. This is called when the GtkTextView is focused out.
gboolean 
openldev_project_dialog_on_filecontent_change (GtkWidget *filecontent,
                                               GdkEventFocus *event,
                                               gpointer data)
{
  ProjectDialog *project = (ProjectDialog*) data;
  
  string file = g_strconcat (project->settings->location, 
                  gtk_combo_box_get_active_text (GTK_COMBO_BOX (project->files)), NULL);
  GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (filecontent));
  GtkTextIter start, end;
  
  gtk_text_buffer_get_start_iter (buffer, &start);
  gtk_text_buffer_get_end_iter (buffer, &end);
  openldev_write_file (file, gtk_text_buffer_get_text (buffer, &start, &end, FALSE));

  return FALSE;
}

// Repopulate the GtkTextView when the file selector is changed.
void 
openldev_project_dialog_on_files_change (GtkComboBox *combobox,
                                         gpointer data)
{
  ProjectDialog *project = (ProjectDialog*) data;
  gchar *file, *location;

  location = g_strconcat (project->settings->location, gtk_combo_box_get_active_text (combobox), NULL);
  file = openldev_read_file (location);
  
  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (project->filecontent)), 
                            file, strlen (file));
}
