/***************************************************************************
 *   Copyright (C) 2006 by Andrew Krause                                   *
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

#include "openldev-utils.h"
#include <glade/glade.h>

gchar* 
openldev_add_prefix (gchar *text)
{ 
  return g_strdup_printf ("%s%s", INSTALL_PREFIX, text);
}

// Throw an error dialog with the specified message
void
openldev_message (gchar *title, 
                  gchar *message, 
                  GtkMessageType type)
{
  GtkWidget *dialog;
  
  dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, type,
                                   GTK_BUTTONS_CLOSE, title);
  gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), message);
  
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

// Show a question dialog with the specified message. Return the answer the
// user choses. This function handles creating the dialog, setting the properties
// and then deleting it and returning the answer.
gboolean 
openldev_question (gchar *question)
{
  GtkWidget *dialog;
  
  dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION,
                                   GTK_BUTTONS_YES_NO, question);
  
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES)
  {
    gtk_widget_destroy (dialog);
    return TRUE;
  }
  else
  {
    gtk_widget_destroy (dialog);
    return FALSE;
  }
}

// Write the text to the file. This function receives the filename and the file
// text. The text is then written to the file located at the filename.
void 
openldev_write_file (string filename, 
                     string text)
{
  ofstream fout (filename.c_str());
  fout << text;
  fout.close ();
}

// Read in the text from a file. This function receives a filename and returns
// the text string of the file.
gchar* 
openldev_read_file (gchar *filename)
{
  gchar *contents;
  gsize length;
  GError *error = NULL;
  
  if (!g_file_test (filename, G_FILE_TEST_EXISTS))
  {
    openldev_message ("OpenLDev Error", "File does not exist!", GTK_MESSAGE_ERROR);
    return NULL;
  }
  
  if (g_file_get_contents (filename, &contents, &length, &error));
    return contents;

  openldev_message ("OpenLDev Error", "There was an error opening this file!", GTK_MESSAGE_ERROR);
  return "";

}

// Check if the output is an error or warning. If it only has two colons in
// close proximity of eachother in the beginning, then it is an error
// or a warning.
gboolean 
openldev_is_error_or_warning (string output)
{
  // Find :'s (It is an error or warning if 2 :'s are found close together
  int location  = output.find (":", 0);
  int location2 = output.find (":", location+1);
  
  // Check if :'s were found & return if they were in proper format
  if (location != (signed int)string::npos)
    if (location2 != (signed int)string::npos && (location2-location < 5))
      return TRUE;
  
  return FALSE;
}

// Get file name and line of the error or warning. This function parses the
// error or warning sent by the output string. Output is then converted into
// only the short filename containing the error & the line number of the
// error or warning is returned.
int 
openldev_get_file_name_and_line (string &output)
{
  string line;
  int location = output.find(":", 0);
  
  // Get the line number & delete it out so only filename is left
  line = output.substr (location+1, output.length()-location-1);
  output.erase (location, output.length()-location);
  
  // Erase all except for the line number
  location = line.find (":", 0);
  line.erase (location, line.length()-location);
  
  // Return the line number
  return atol (line.c_str());
}

// Create new stock menu item. This function receives the icon factory, the
// name of the item, the id of the item, and the filename of the icon.
void 
openldev_add_new_stock_item (GtkIconFactory *icon_factory, 
                             gchar *name,
                             gchar *id, 
                             gchar *filename)
{
  GtkIconSource *icon_source = gtk_icon_source_new ();
  GtkIconSet *icon = gtk_icon_set_new ();
  
  // Set the properties of the stock menu/toolbar item
  gtk_icon_source_set_icon_name (icon_source, name);
  gtk_icon_source_set_filename (icon_source, filename);
  gtk_icon_set_add_source (icon, icon_source);
  gtk_icon_factory_add (icon_factory, id, icon);
}

// Get all of the text in the GtkSourceBuffer and return it.
string 
openldev_file_get_text_in_buffer (GtkSourceBuffer *buffer)
{
  GtkTextIter start, end;
  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (buffer), &start);
  gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (buffer), &end);
  return gtk_text_buffer_get_text (GTK_TEXT_BUFFER (buffer), &start, &end, TRUE);
}

// Create a new process
gint 
openldev_fork_new_process (string command)
{
  gint argc, pid, ret;
  gchar **argv;

  g_shell_parse_argv (command.c_str(), &argc, &argv, NULL);
  ret = g_spawn_async_with_pipes (NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL,
                                  &pid, NULL, NULL, NULL, NULL);
  
  g_strfreev (argv);
  if (!ret)
  {
    g_warning ("The fork instruction has failed!");
    return -1;
  }
  
  return pid;
}
