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

#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H

/*! 
  \file openldev-file-browser.h
  \brief FileBrowser
*/

#include "../openldev-settings/openldev-project.h"

#include <gtk/gtk.h>
#include <vector>
#include <iostream>
#include <dirent.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomeui/gnome-icon-lookup.h>
#include <glib-object.h>
#include <glib.h>

G_BEGIN_DECLS

#define FILE_BROWSER_TYPE            (file_browser_get_type ())
#define FILE_BROWSER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), FILE_BROWSER_TYPE, FileBrowser))
#define FILE_BROWSER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), FILE_BROWSER_TYPE, FileBrowserClass))
#define IS_FILE_BROWSER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FILE_BROWSER_TYPE))
#define IS_FILE_BROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), FILE_BROWSER_TYPE))

/*!
  FileBrowser is a custom widget derived from GtkTreeView. Therefore, it inherits all of the functions, signals and properties of GtkTreeView and the classes it is derived from. It is possible to use this widget in any other part of the application.
  \brief Project file browser located in the SideBar GtkNotebook. 
*/
struct FileBrowser
{
  GtkTreeView parent;
  
  /*! File browser scrolled window that contains the GtkTreeView widget */
  GtkWidget *swin;
  
  /*! The path to the highest directory the file browser will move to. */
  gchar *base;
  /*! The path of the currently shown directory with respect to base. */
  gchar *current;
};

struct FileBrowserClass
{
  GtkTreeViewClass parent_class;
};

GType file_browser_get_type ();

/*! Create a new FileBrowser widget. This will allow you to include the file browser in other places of the OpenLDev application. It will not initially display the contents of any folder.
  \return A new FileBrowser widget.
*/
GtkWidget* file_browser_new ();

/*! Retrive the full file path and name of the currently selected file. If there is no file currently selected, the string "NONE" will be returned.
  \param browser A FileBrowser widget object.
  \return The full file path and name of the selected file or "NONE" if no file is selected.
*/
const char* file_browser_get_selected_file (FileBrowser *browser);

/*! The FileBrowser widget will be reset, and base_directory will define the highest directory it will browse up to.
  \param browser A FileBrowser widget object.
  \param base_directory The full path of the directory to open.
*/
void file_browser_populate (FileBrowser *browser, string base_directory);

/*! Move up one directory level in the file browser. If it is currently at the base_directory, the file browser will just be refreshed.
  \param browser A FileBrowser widget object.
*/
void file_browser_move_up (FileBrowser *browser);

/*! Move in to a directory under base_directory. This will be appended to the back of the current directory. You do not need to include any backslash characters in the directory name.
  \param browser A FileBrowser widget object.
  \param directory The name of the directory to move to.
*/
void file_browser_move (FileBrowser *browser, const char *directory);

/*! Refresh the current directory of the file browser. This does not happen when files are added or removed, so you must do this manually.
  \param browser A FileBrowser widget object.
*/
void file_browser_refresh (FileBrowser *browser);

G_END_DECLS

#endif
