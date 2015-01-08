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
 
#ifndef OPENLDEV_UTILS_H
#define OPENLDEV_UTILS_H

/*!
  \file openldev-utils.h
  This header file includes a few utility functions that are used throughout OpenLDev. They are organized here in one header file so they can easily be included in plug-ins.
  \brief Utility Functions
*/

#include <gtksourceview/gtksourceview.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <iostream>
#include <fstream>
using namespace std;

/*! Add INSTALL_PREFIX to the beginning of the string. This is so the user can define custom install prefixes.
  \param file File name that INSTALL_PREFIX should be added to.
  \return The full file name the prefix was attached to.
*/
gchar* openldev_add_prefix (gchar *file);

/*! Give the user a message. There is no need to add newline characters because the message will wrap automatically. The message also supports Pango formatting.
  \param title The title text that should be shown in the dialog.
  \param message The secondary text that should be shown in the dialog.
  \param type The type of message that is displayed. This will change the image displayed in the dialog.
*/
void openldev_message (gchar *title, gchar *message, GtkMessageType type);

/*! Ask the user a question, specified by your message. The user's choice is returned from the 
  \param message The question that is to be shown in the dialog.
  \return TRUE if the Yes button was pressed.
*/
gboolean openldev_question (gchar *message);

/*! Write text to a file. This uses the standard C++ ofstream functions.
  \param file Full location where the file is located.
  \param text Text that should be written to the file.
*/
void openldev_write_file (string file, string text);

/*! Read the contents of a file. This function uses GLib functions to verify that the file exists.
  \param file Full location where the file is located.
  \return Text content of the file.
*/
gchar *openldev_read_file (gchar *file);

/*! You can use this function to check whether the line of make output is an error or a warning.
  \param output The build output string. This should only be one line long.
  \return TRUE if the build output line is an error or a warning.
*/
gboolean openldev_is_error_or_warning (string output);

/*! Get the line number the error or warning is on and what file it is from.
  \param output The build output string, which is converted just to the short file name.
  \return The line number where the error or warning occurred.
*/
int openldev_get_file_name_and_line (string &output);

/*! Add a new stock menu item. These can be used in menus, toolbars or popup menus.
  \param icon_factory The stock item is added to this GtkIconFactory.
  \param name Text displayed beside the stock image.
  \param id Access id that will be used when referencing the stock item.
  \param file Full location of the image.
*/
void openldev_add_new_stock_item (GtkIconFactory *icon_factory, gchar *name, 
                                  gchar *id, gchar *file);

/*! Create a new process using fork() and execl(). This can be done with any arbitrary command and the forked application will not affect the operation of OpenLDev.
  \param command The command that is to be run.
  \return The process id or -1 if the fork was unsuccessful.
*/
gint openldev_fork_new_process (string command);

/*! Retrive all of the text in the specified buffer. This has to be a GtkSourceBuffer to use.
  \param buffer A GtkSourceBuffer that the text is in.
  \return The text in the buffer.
*/
string openldev_file_get_text_in_buffer (GtkSourceBuffer *buffer);

#endif
