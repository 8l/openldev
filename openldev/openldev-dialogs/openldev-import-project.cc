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

#include "openldev-import-project.h"

gchar*
openldev_import_project ()
{
  gchar *file = openldev_add_prefix ("/share/openldev/glade/importproject.glade"), *find, *fn;
  GladeXML *xml = glade_xml_new (file, NULL, NULL);
  GtkWidget *dialog = glade_xml_get_widget (xml, "dialog");
  GtkWidget *location = glade_xml_get_widget (xml, "location");
  
  gint result = gtk_dialog_run (GTK_DIALOG (dialog)), len;
  if (result == GTK_RESPONSE_APPLY)
  {
    fn = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (location));
    GString *name = g_string_new (g_strdup (fn));
    
    if (!g_file_test (g_strconcat (fn, "/", "Makefile.am", NULL), G_FILE_TEST_EXISTS))
    {
      openldev_message ("OpenLDev Error", "A base Makefile.am file could not be found, so the project could not be imported! Instead, you should create a new OpenLDev project and add your source files.", GTK_MESSAGE_ERROR);
      gtk_widget_destroy (dialog);
      g_object_unref (xml);
      return "NONE";
    }
    else if (!g_file_test (g_strconcat (fn, "/", "configure.in", NULL), G_FILE_TEST_EXISTS))
    {
      openldev_message ("OpenLDev Error", "The configure.in file could not be found, so the project could not be imported! Instead, you should create a new OpenLDev project and add your source files.", GTK_MESSAGE_ERROR);
      gtk_widget_destroy (dialog);
      g_object_unref (xml);
      return "NONE";
    }
    
    while (name->str[0] == '/')
    {
      while (name->str[0] == '/')
        g_string_erase (name, 0, 1);
      find = g_strstr_len (name->str, name->len, "/");
    
      if (find != NULL)
      {
        len = (gsize) (strlen (name->str) - strlen (find));
        g_string_erase (name, 0, len);
      }
    }
    
    fn = g_strconcat (fn, "/", name->str, ".dev", NULL);
  }
  else
    fn = "NONE";
  
  gtk_widget_destroy (dialog);
  g_object_unref (xml);
  return fn;
}
