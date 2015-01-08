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
 
#ifndef OPENLDEV_MAIN_H
#define OPENLDEV_MAIN_H

/*!
  \file openldev.h
  This header file is the base for many aspects of the OpenLDev application. You can use it to include most header files that are useful to plug-ins. It contains a few utility functions along with declarations for menu callback functions.
  \brief OpenLDev
*/

#include "openldev-utils.h"
#include "openldev-menu-callbacks.h"
#include "openldev-widgets/openldev-goto-line-toolbar.h"
#include "openldev-widgets/openldev-find-toolbar.h"
#include "openldev-widgets/openldev-menu.h"
#include "openldev-widgets/openldev-file-manager.h"
#include "openldev-widgets/openldev-output-manager.h"
#include "openldev-widgets/openldev-sidebar.h"
#include "openldev-widgets/openldev-status-bar.h"
#include "openldev-settings/openldev-editor.h"
#include "openldev-settings/openldev-settings.h"
#include "openldev-settings/openldev-project.h"
#include "openldev-plugin.h"

#include <glib-object.h>
#include <glib.h>
#include <config.h>

G_BEGIN_DECLS

#define OPENLDEV_TYPE            (openldev_get_type ())
#define OPENLDEV(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), OPENLDEV_TYPE, OpenLDev))
#define OPENLDEV_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), OPENLDEV_TYPE, OpenLDevClass))
#define IS_OPENLDEV(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OPENLDEV_TYPE))
#define IS_OPENLDEV_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), OPENLDEV_TYPE))

#define OPENLDEV_VERSION "1.0"

typedef struct _OpenLDevClass    OpenLDevClass;
typedef struct _GotoLineToolbar  GotoLineToolbar;

/*! 
  \struct OpenLDev
  This is the most important class when writing a plugin for the application. Everything is contained by it. When using it, there are a few things you need to know.\n\n
  First, it is important to note that the OpenLDev structure is derived from the GtkWindow object. This means that it can be cast with GTK_WINDOW (OpenLDev*). It can also be cast as any other class GtkWindow is derived from. It also inherits all of the signals from those classes.\n\n
  There are multiple signals available that you can connect to your OpenLDev object:\n
  <ul>
    <li><i>file-new</i>: A new file was successfully created
    <li><i>file-open</i>: An existing file was successfully opened
    <li><i>project-new</i>: A new project was successfully created
    <li><i>project-open</i>: A project has successfully been loaded
    <li><i>project-import</i>: The user imported a directory as an OpenLDev project
    <li><i>file-save</i>: A file was successfully saved
    <li><i>file-save-as</i>: A file was successfully saved with a new name. This will also emit <i>file-save</i>.
    <li><i>file-save-all</i>: All files were saved. This will also emit <i>file-save</i> for every file.
    <li><i>file-reload</i>: The user reverted to the version of the file on disk.
    <li><i>project-close</i>: The current project was closed. No project is open at this point.
    <li><i>compile</i>: The user compiled the currently open file.
    <li><i>build</i>: The user is building from the chosen base directory.
    <li><i>execute</i>: The user executed the projects main executable.
    <li><i>auto-generate</i>: The user called the auto generate script.
    <li><i>configure</i>: The user called the configure script.
    <li><i>make-clean</i>: The project source tree is being cleaned.
  </ul>\n\n
  There are also multiple properties available that give you access to the widgets and settings of the IDE:\n
  <ul>
    <li><i>sidebar</i>: SideBar widget
    <li><i>file-browser</i>: FileManager widget
    <li><i>output-pane</i>: OutputManager widget
    <li><i>hpaned</i>: GtkHPaned widget
    <li><i>vbox</i>: GtkVBox widget
    <li><i>vpaned</i>: GtkVPaned widget
    <li><i>find-toolbar</i>: FindToolbar object
    <li><i>goto-toolbar</i>: GotoLineToolbar object
    <li><i>menus</i>: MenuHandle object
    <li><i>status-bar</i>: StatusBar object
    <li><i>editor-prop</i>: EditorProperties object
    <li><i>env-settings</i>: EnvironmentSettings object
    <li><i>project</i>: ProjectSettings object
    <li><i>plugins</i>: OpenLDevPluginList object
  </ul>
*/
struct _OpenLDev
{
  GtkWindow window;
  
  GtkWidget *sidebar;
  GtkWidget *files;
  GtkWidget *output;
  FindToolbar *findtoolbar;
  MenuHandle *menus;
  StatusBar *statusbar;
  GtkWidget *hpaned;
  GtkWidget *vbox;
  GtkWidget *vpaned;
  EditorProperties *editor;
  EnvironmentSettings *env;
  ProjectSettings *project;
  OpenLDevPluginList *pluginlist;
  GotoLineToolbar *gototoolbar;
};

struct _OpenLDevClass
{
  GtkWindowClass parent_class;
  
  void (*project_open) (OpenLDev *openldev);
  void (*file_new) (OpenLDev *openldev);
  void (*file_open) (OpenLDev *openldev);
  void (*project_new) (OpenLDev *openldev);
  void (*project_import) (OpenLDev *openldev);
  void (*file_save) (OpenLDev *openldev);
  void (*file_save_as) (OpenLDev *openldev);
  void (*file_save_all) (OpenLDev *openldev);
  void (*file_reload) (OpenLDev *openldev);
  void (*project_close) (OpenLDev *openldev);
  void (*compile) (OpenLDev *openldev);
  void (*build) (OpenLDev *openldev);
  void (*execute) (OpenLDev *openldev);
  void (*auto_generate) (OpenLDev *openldev);
  void (*configure) (OpenLDev *openldev);
  void (*make_clean) (OpenLDev *openldev);
};

GType openldev_get_type ();
GtkWidget* openldev_new ();
void openldev_initialize_gconf (OpenLDev *openldev);
OpenLDevPluginList* openldev_load_plugins_all (OpenLDev *openldev);
GotoLineToolbar* openldev_goto_toolbar_new (OpenLDev*);

/*! A file with the specified name is opened in a new tab of the FileManager. You must specify the full path of the file name.
  \param openldev The OpenLDev application object.
  \param file The full path of a file that should be opened in a new tab.
  \return TRUE if the file was successfully opened.
*/
gboolean openldev_open_file (OpenLDev *openldev, gchar *file);

/*! A project at the specified location is closed. If a project is already opened, it will first be closed.
  \param openldev The OpenLDev application object.
  \param file The full path location of a project.
  \return TRUE if the project was successfully loaded.
*/
gboolean openldev_open_project (OpenLDev *openldev, gchar *file);

/*! This is a convenience function that calls "openldev_browser_project_populate" on the current BrowserProject object.
  \param openldev The OpenLDev application object.
*/
void openldev_populate_treeview (OpenLDev *openldev);

/*! The Find and Replace dialog is shown to the user.
  \param openldev The OpenLDev application object.
*/
void openldev_find_and_replace (OpenLDev *openldev);

/*! Close the project that is currently open. If there is no project open, nothing will happen.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_file_close_project (OpenLDev *openldev);

/*! Show the dialog that allows the user to create a new file. If a new file is successfully created, it will be automatically opened in a new tab.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_file_new_file (OpenLDev *openldev);

/*! Show the dialog that allows the user to create a new project. If a new project is successfully created, it will be loaded in the editor and Auto Generate will be run.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_file_new_project (OpenLDev *openldev);

/*! Allow the user to choose a file to be opened in the editor. This is done using the GtkFileChooserDialog.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_file_open (OpenLDev *openldev);

/*! Allow the user to choose a project to be opened in the editor. This is done using the GtkFileChooserDialog.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_file_open_project (OpenLDev *openldev);

/*! Create a new OpenLDev project file in the specified source directory. This will use the default build commands.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_file_import_project (OpenLDev *openldev);

/*! Show the print dialog for the current file. This function should not be called if no files are open.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_file_print (OpenLDev *openldev);

/*! Show the print preview dialog for the current file. This function should not be called if no files are open.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_file_print_preview (OpenLDev *openldev);

/*! Revert to the saved version of a file. If the file as been modified, permission will be asked from the user before this is done.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_file_revert (OpenLDev *openldev);

/*! Save the current file under its current file name.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_file_save (OpenLDev *openldev);

/*! Save all of the open files that have been modified since the last save.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_file_save_all (OpenLDev *openldev);

/*! Save the current file under a new file name.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_file_save_as (OpenLDev *openldev);

/*! Copy any selected text in the current file on to the Clipboard.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_edit_copy (OpenLDev *openldev);

/*! Copy any selected text in the current file on to the Clipboard and delete it from the buffer.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_edit_cut (OpenLDev *openldev);

/*! Unselect any selected text in the current file.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_edit_deselect (OpenLDev *openldev);

/*! Open or close the Find toolbar. To actually search for text, you must call the "openldev_menu_edit_find_next" function.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_edit_find (OpenLDev *openldev);

/*! Find the next occurrence of the specified string in the current file. If "Search all files" is selected, all source files will be searched and the output will be shown in the Search output tab.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_edit_find_next (OpenLDev *openldev);

/*! Show or hide the Goto Line toolbar. To actually jump to a line, you must call the "openldev_goto_to_line_number" function.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_edit_goto_line (OpenLDev *openldev);

/*! Insert the text from the Clipboard into the current buffer. If there is selected text, it will be replaced.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_edit_paste (OpenLDev *openldev);

/*! Allow the user to edit general preferences for the application. These includes options for the editor, syntax highlighting and plugins.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_edit_preferences (OpenLDev *openldev);

/*! Redo the last action that was undone by the user. This is performed on the current file.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_edit_redo (OpenLDev *openldev);

/*! Select all of the text in the current file.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_edit_select_all (OpenLDev *openldev);

/*! Undo the last action that was done by the user. This is performed on the current file.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_edit_undo (OpenLDev *openldev);

/*! Indent the buffer to the level of the previous line.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_edit_indent (OpenLDev *openldev);

/*! Remove all white space at the beginning of the line.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_edit_unindent (OpenLDev *openldev);

/*! Show basic information about the current project such as the number of lines and files.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_project_info (OpenLDev *openldev);

/*! Allow the user to edit basic settings in their project such as build commands and version number.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_project_preferences (OpenLDev *openldev);

/*! Run the Automatic Generation command specified by the project.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_build_auto_generate (OpenLDev *openldev);

/*! Run the Build (make) command specified by the project.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_build_project (OpenLDev *openldev);

/*! Run the Clean command specified by the project.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_build_clean (OpenLDev *openldev);

/*! Run the Compile command specified by the project. All instances of {FILE} in the command are replaced by the full path of the current file.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_build_compile (OpenLDev *openldev);

/*! Run the Configure (./configure) command specified by the project.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_build_configure (OpenLDev *openldev);

/*! Execute the binary built by the build command by the project.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_build_execute (OpenLDev *openldev);

/*! Stop any build command currently running. This excludes the Execute command.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_build_stop (OpenLDev *openldev);

/*! Show or hide the project file browser. This will happen to the whole GtkNotebook, so any plugins in another tab will also be shown or hidden.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_view_browser (OpenLDev *openldev);

/*! Show or hide the line numbering in each of the open files.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_view_line_numbering (OpenLDev *openldev);

/*! Show or hide the project output manager. This will happen to the whole GtkNotebook, so any plugins in another tab will also be shown or hidden.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_view_output (OpenLDev *openldev);

/*! Show or hide the status bar and progress bar.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_view_statusbar (OpenLDev *openldev);

/*! Show or hide the whole toolbar.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_view_toolbar (OpenLDev *openldev);

/*! Launch a browser containing the documentation web site. This will be done in the default Gnome web browser.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_help_documentation (OpenLDev *openldev);

/*! Launch a browser containing the bug reporting web site. This will be done in the default Gnome web browser.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_help_report_bug (OpenLDev *openldev);

/*! Show the about dialog. This contains version, copyright and developer information.
  \param openldev The OpenLDev application object.
*/
void openldev_menu_help_about (OpenLDev *openldev);

G_END_DECLS

#endif
