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
 
#include "openldev-printing.h"

GnomePrintJob* openldev_print_create_job (OpenLDev*, const char*);

// Print the current file if there is a file open with the user's settings.
void openldev_menu_file_print (OpenLDev *openldev)
{
  FileManager *files;
  EnvironmentSettings *env;
  gchar *font_face;
  gint font_size;
  
  g_object_get (openldev, "env-settings", &env, "file-browser", &files, NULL);
  g_object_get (env, "font-face", &font_face, "font-size", &font_size, NULL);
  
  if (file_manager_get_file_open (files))
  {  
    string file = "File: " + file_manager_get_current_filename_short (files),
           font = g_strdup_printf ("%s Regular %i", font_face, font_size);
    GnomePrintJob *job = openldev_print_create_job (openldev, font.c_str());
    GtkWidget *dialog = gnome_print_dialog_new (job, (guchar*) "Print Document",
                        GNOME_PRINT_DIALOG_RANGE | GNOME_PRINT_DIALOG_COPIES);
  
    gint result = gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
  
    if (result == GNOME_PRINT_DIALOG_RESPONSE_PREVIEW)
      openldev_menu_file_print_preview (openldev);
    else if (result == GNOME_PRINT_DIALOG_RESPONSE_PRINT)
      gnome_print_job_print (job);
  
    g_object_unref (G_OBJECT (job));
  }
}

// Preview the current file as it would be printed if there is a file open
// with the user's settings.
void openldev_menu_file_print_preview (OpenLDev *openldev)
{
  FileManager *files;
  EnvironmentSettings *env;
  gchar *font_face;
  gint font_size;
  
  g_object_get (openldev, "env-settings", &env, "file-browser", &files, NULL);
  g_object_get (env, "font-face", &font_face, "font-size", &font_size, NULL);
  
  if (file_manager_get_file_open (files))
  {  
    string file = "File: " + file_manager_get_current_filename_short (files),
           font = g_strdup_printf ("%s Regular %i", font_face, font_size);
    GnomePrintJob *job = openldev_print_create_job (openldev, font.c_str());
    GtkWidget *preview;

    preview = gnome_print_job_preview_new (job, (guchar*) "Print Preview");
    gtk_widget_show (preview);
  
    g_object_unref (job);
  }
}

GnomePrintJob* openldev_print_create_job (OpenLDev *openldev,
                                          const char* font)
{
  GnomePrintConfig *cfg = gnome_print_config_default ();
  GtkSourcePrintJob *sjob = gtk_source_print_job_new (cfg); 
  FileManager *files;
  EnvironmentSettings *env;
  gint tab_size;
  
  g_object_get (openldev, "env-settings", &env, "file-browser", &files, NULL);
  g_object_get (env, "tab-size", &tab_size, NULL);
  
  gtk_source_print_job_set_buffer (sjob, GTK_SOURCE_BUFFER (file_manager_get_current_buffer (files)));
  gtk_source_print_job_set_tabs_width (sjob, tab_size);
  gtk_source_print_job_set_wrap_mode (sjob, GTK_WRAP_NONE);
  gtk_source_print_job_set_highlight (sjob, FALSE);
  gtk_source_print_job_set_print_numbers (sjob, 5);
  gtk_source_print_job_set_text_margins (sjob, 0.5, 0.5, 0.5, 0.5);
  gtk_source_print_job_set_font (sjob, font);
  gtk_source_print_job_set_numbers_font (sjob, font);
  
  GnomePrintJob *job = gtk_source_print_job_print (sjob);
  g_object_unref (G_OBJECT(sjob));
  
  return job;
}
