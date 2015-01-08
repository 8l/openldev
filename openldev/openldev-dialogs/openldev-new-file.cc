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

#include "openldev-new-file.h"

static void openldev_new_file_dialog_on_apply (GtkWidget*, NewFileDialog*);
static void openldev_new_file_dialog_on_cancel (GtkWidget*, NewFileDialog*);

// Load the new file dialog, set the current folder to the project directory
// and connect the button signals.
NewFileDialog* 
openldev_new_file_dialog_new (gchar* location)
{
  NewFileDialog *newfile = g_slice_new (NewFileDialog);
  
  newfile->file_location = location;
  gchar *gladefile = openldev_add_prefix ("/share/openldev/glade/newfile.glade");
  newfile->xml = glade_xml_new (gladefile, NULL, NULL);
  
  newfile->dialog = glade_xml_get_widget (newfile->xml, "dialog");
  newfile->cancel = glade_xml_get_widget (newfile->xml, "button_cancel");
  newfile->apply =  glade_xml_get_widget (newfile->xml, "button_ok");
  newfile->filechooser = glade_xml_get_widget (newfile->xml, "filechooser");
  newfile->name =   glade_xml_get_widget (newfile->xml, "filename");
  
  if (location == NULL)
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (newfile->filechooser),
                                         g_get_home_dir ());
  else
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (newfile->filechooser),
                                         newfile->file_location);
  
  g_signal_connect (G_OBJECT (newfile->apply), "clicked", 
                    G_CALLBACK (openldev_new_file_dialog_on_apply), (gpointer) newfile);
  g_signal_connect (G_OBJECT (newfile->cancel), "clicked", 
                    G_CALLBACK (openldev_new_file_dialog_on_cancel), (gpointer) newfile);
  
  return newfile;
}

// Run the dialog and set the filename if the user specified one and accept
// was pressed. Otherwise set it to the string NULL.
void 
openldev_new_file_dialog_run (NewFileDialog *newfile, 
                              gchar *&file)
{
  gint result = 0;
  
  while (result == 0)
    result = gtk_dialog_run (GTK_DIALOG (newfile->dialog));
  
  switch (result)
  {
    case (GTK_RESPONSE_ACCEPT):
    {
      file = newfile->file_location;
      break;
    }
    default:
      file = "NULL";
  }
}

// Emit a cancel signal
static void 
openldev_new_file_dialog_on_cancel (GtkWidget *button, 
                                    NewFileDialog *newfile)
{
  gtk_dialog_response (GTK_DIALOG (newfile->dialog), GTK_RESPONSE_CANCEL);
}

// Apply was pressed and now we must handle it
static void 
openldev_new_file_dialog_on_apply (GtkWidget *button, 
                                   NewFileDialog *newfile)
{
  gchar *location = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (newfile->filechooser)),
        *projectname = (gchar*) gtk_entry_get_text (GTK_ENTRY (newfile->name));
  
  // Create new file if no errors are found
  if (!(g_strrstr (projectname, " ") != NULL) && strlen (projectname) > 0)
  {
    newfile->file_location = g_strconcat (location, "/", projectname, NULL);

    // Write new file
    openldev_write_file (newfile->file_location, "");
    gtk_dialog_response (GTK_DIALOG (newfile->dialog), GTK_RESPONSE_ACCEPT);
  }
  // Throw an error
  else 
    openldev_message ("OpenLDev Error", "You must specify a file name!", GTK_MESSAGE_ERROR);
}
