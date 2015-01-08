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

#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

/*!
  \file openldev-file-manager.h
  \brief FileManager
*/

#include "openldev-file-tab.h"
#include "../openldev-utils.h"
#include "../openldev-settings/openldev-settings.h"

#include <glib.h>
#include <glib-object.h>
#include <libgnomevfs/gnome-vfs.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguagesmanager.h>
#include <vector>
using namespace std;

G_BEGIN_DECLS

#define FILE_MANAGER_TYPE            (file_manager_get_type ())
#define FILE_MANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), FILE_MANAGER_TYPE, FileManager))
#define FILE_MANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), FILE_MANAGER_TYPE, FileManagerClass))
#define IS_FILE_MANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FILE_MANAGER_TYPE))
#define IS_FILE_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), FILE_MANAGER_TYPE))

/*! \brief GtkNotebook that holds a FileTab for each open file. */
struct FileManager
{
  GtkNotebook parent;

  /*! List of FileTab objects, one for each open file. */
 	vector<GtkWidget*> tabs;
  /*! Number of open files. This can be obtained from the tabs list, but this is for convenience. */
	int tab_count;
};

struct FileManagerClass
{
  GtkNotebookClass parent_class;
};

// Internal functions: Do not document!
GType file_manager_get_type ();
GtkWidget* file_manager_new ();

gint file_manager_get_file_is_open (FileManager *manager, gchar *file);

/*! Get the currently shown GtkTextView if there is a file open. Otherwise a blank GtkTextView widget will be returned.
  \param manager A FileManager widget object.
  \return The current GtkSourceView widget.
*/
GtkSourceView* file_manager_get_current_view (FileManager *manager);

/*! Get the currently shown GtkSourceBuffer if there is a file open. Otherwise a blank GtkSourceBuffer widget will be returned.
  \param manager A FileManager widget object.
  \return The GtkSourceBuffer from the current GtkTextView.
*/
GtkSourceBuffer* file_manager_get_current_buffer (FileManager *manager);

/*! Get the full file name of the currently selected file if there is a file open.
  \param manager A FileManager widget object.
  \return The full file name of the currently selected file.
*/
gchar* file_manager_get_current_filename (FileManager *manager);

/*! Get the short file name of the currently selected file if there is a file open.
  \param manager A FileManager widget object.
  \return The short file name of the currently selected file.
*/
string file_manager_get_current_filename_short (FileManager *manager);

/*! Get the position of the currently selected tab.
  \param manager A FileManager widget object.
  \return Position of the currently selected tab.
*/
int file_manager_get_current_tab (FileManager *manager);

/*! Get the offset in characters of the "insert" GtkTextIter in the current file.
  \param manager A FileManager widget object.
  \return Offset of the "insert" GtkTextIter.
*/
int file_manager_get_insert_offset (FileManager *manager);

/*! Get the offset in characters of the "selection_bound" GtkTextIter in the current file.
  \param manager A FileManager widget object.
  \return Offset of the "selection_bound" GtkTextIter.
*/
int file_manager_get_selection_offset (FileManager *manager);

/*! Check whether there is currently a file open in the FileManager. This should be called before trying to retrieve information about a current file. The FileManager will assume this has already been checked whe you ask for information!
  \param manager A FileManager widget object.
  \return TRUE if one or more files are currently open.
*/
gboolean file_manager_get_file_open (FileManager *manager);

/*! Set a new file name for the current file. For example, this would be used if you are renaming a certain file.
  \param manager A FileManager widget object.
  \param file New full file name to rename the current file to.
*/
void file_manager_set_filename (FileManager *manager, const char *file);

/*! Reset the editor properties of one or more open tabs. This includes syntax highlighting, tab size and other settings from EnvironmentSettings.
  \param manager A FileManager widget object.
  \param all_files FALSE if only the current tab should be updated or TRUE to update all open files.
  \param env EnvironmentSettings object to retrieve the settings from.
*/
void file_manager_set_properties (FileManager *manager, gboolean all_files, EnvironmentSettings *env);

/*! Open a new file tab for the specified file. Editor settings from the EnvironmentSettings object provided will be used. This function does not actually open the file, but just created a new tab with the proper settings. Use openldev_open_file to open a new file.
  \param manager A FileManager widget object.
  \param file The full path of the file that was opened.
  \param env EnvironmentSettings object to retrieve the settings from.
*/
gboolean file_manager_add_tab (FileManager *manager, const char *file, EnvironmentSettings *env);

/*! Close all of the tabs that are currently open. This does not check whether files are modified, so you must do that manually. It is just meant to immediately wipe out all open files.
  \param manager A FileManager widget object.
*/
void file_manager_erase_all_tabs (FileManager *manager);

/*! Mark a specified tab as modified or unmodified. This is done automatically by OpenLDev, so you should have no need to call this function.
  \param manager A FileManager widget object.
  \param modified TRUE if the tab should be marked as modified.
  \param tab Position of the tab to be marked.
*/
void file_manager_mark_current_tab_modified (FileManager *manager, gboolean modified, int tab);

/*! Scroll the the current position of the cursor. This is done to the file that is currently in focus.
  \param manager A FileManager widget object.
*/
void file_manager_scroll_to_mark (FileManager *manager);

/*! Change the font of every open tab. This is used when you do not want to update all of the available settings.
  \param manager A FileManager widget object.
  \param fd PangoFontDescription that will be set for each file.
*/
void file_manager_modify_fonts (FileManager *manager, PangoFontDescription *fd);

/*! Revert to the saved version of the file that is currently in focus. This function assumes that you have already asked the user if they want to do this. You should use openldev_menu_file_revert if you want to ask the user for permission before proceding.
  \param manager A FileManager widget object.
*/
void file_manager_revert_to_saved (FileManager *manager);

/*! Create a new PangoTabArray for the widget. This is used internally and really is not necessary for plugins.
  \param fd PangoFontDescription to use for the calculations.
  \param env EnvironmentSettings structure that provides tab settings.
  \param widget GtkTextView widget that the tab array should be taken from.
  \return PangoTabArray calculated from the parameters.
*/
PangoTabArray* file_manager_get_tab_array (PangoFontDescription *fd, EnvironmentSettings *env, GtkWidget *widget);

// Internal function: Do not document!
gboolean file_manager_on_delete_tab (GtkWidget *button, gpointer data);

G_END_DECLS

#endif
