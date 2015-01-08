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
 
#ifndef OPENLDEV_PROJECT_SETTINGS_H
#define OPENLDEV_PROJECT_SETTINGS_H

/*! \file openldev-settings/openldev-project.h
    \brief ProjectSettings
*/

#include "../openldev-utils.h"
#include "openldev-makefile-parser.h"
#include "openldev-xml-parser.h"

#include <vector>
#include <string>
#include <fstream>
#include <glib.h>
#include <gtk/gtk.h>
using namespace std;

/*! \brief Information about a currently open OpenLDev project. */
struct ProjectSettings
{
  /*! The loaded XML file that holds all of the data about the OpenLDev project. */
  OpenLDevXmlDocument *xml;
  /*! List of files that should be loaded when the project is reopened. */
  GSList *lastfiles;

  /*! The short name of the template that was used to create the project. */
  gchar *template_name;
  /*! The auto generate command. */
  gchar *autogen;
  /*! The build command. */
  gchar *build;
  /*! The compile command. And instance of {FILE} will be replaced by the full path of the current file. */
  gchar *compile;
  /*! The configure command. You can edit this to include parameters to pass every time it is run. */
  gchar *configure;
  /*! The clean command. */
  gchar *clean;
  /*! The execute command. All instances of {BASE} wil be replaced by the full location of the project and all instances of {NAME} will be replaced by the project name. */
  gchar *execute;
  /*! The full file name of the project file from which these settings were loaded. */
  gchar *fullname;
  /*! The programming language(s) used in the project. */
  gchar *language;
  /*! The full location of the project. This is obtained from the project file name, so if the project is moved, this will be updated. */
  gchar *location;
  /*! The name of the project as it appears in the full project file name. */
  gchar *name;
  /*! The version of the project as it appears in your configure.in file. */
  gchar *version;
  
  gboolean open;
};

/*! Create a new ProjectSettings object. If file is empty, a new object will be created, but all of the elements will be empty.
  \param file The location of a project file.
  \return A new ProjectSettings object or an empty object if file is empty.
*/
ProjectSettings* openldev_project_settings_new (gchar *file);

/*! Save the current project and reset all of the variables in the ProjectSettings object.
  \param project A ProjectSettings object.
*/
void openldev_project_settings_close_project (ProjectSettings *project);

/*! Load a project file in to an already created ProjectSettings object.
  \param project A ProjectSettings object.
  \param file The project file to be loaded.
  \return TRUE if the project was successfully loaded.
*/
gboolean openldev_project_settings_load_project_file (ProjectSettings *project, gchar *file);

/*! Check whether a file is found within the project.
  \param project A ProjectSettings object.
  \param file The file that is being checked.
  \return TRUE if the file is found in the project.
*/
gboolean openldev_project_settings_get_file_in_project (ProjectSettings *project, gchar *file);

/*! Get a list of Makefile.am files in the project. This is done by parsing all of the SUBDIRS variables.
  \param project A ProjectSettings object.
  \return List of makefiles in the project.
*/
vector<string> openldev_project_settings_get_makefile_list (ProjectSettings *project);

/*!
  \param project A ProjectSettings object.
  \param makefile The full path file location of the Makefile.am.
  \param full TRUE if all of the SUBDIRS in makefile should be parsed recursively for sources.
  \return The list of sources in the folder or the whole project.
*/
vector<string> openldev_project_settings_get_sources (ProjectSettings *project, gchar *makefile, gboolean full);

/*! Get a list of all of the sources in a project's file structure. These are obtained from the targets in your Makefile.am(s).
  \param project A ProjectSettings object.
  \return The list of sources in the project.
*/
vector<string> openldev_project_settings_get_sources_all (ProjectSettings *project);

/*! Add a file from the list of open files. This should be called when a file is opened and OpenLDev has not invoked this action.
  \param project A ProjectSettings object.
  \param file The full path of the file that was just opened.
*/
void openldev_project_settings_add_lastfile (ProjectSettings *project, gchar *file);

/*! Remove a file from the list of open files. This should be called when a file is closed and OpenLDev has not invoked this action.
  \param project A ProjectSettings object.
  \param file The full path of the file that was just closed.
*/
void openldev_project_settings_remove_lastfile (ProjectSettings *project, gchar *file);

/*! Change the default programming language of the project. Changing this won't affect the editor; it is just for reference.
  \param project A ProjectSettings object.
  \param value The string to replace the current value.
*/
void openldev_project_settings_set_language (ProjectSettings *project, gchar *value);

/*! Change the compile command that is used. Every instance of {FILE} will be replaced by the full path of the current file.
  \param project A ProjectSettings object.
  \param value The string to replace the current value.
*/
void openldev_project_settings_set_compile (ProjectSettings *project, gchar *value);

/*! Change the build command that is used.
  \param project A ProjectSettings object.
  \param value The string to replace the current value.
*/
void openldev_project_settings_set_build (ProjectSettings *project, gchar *value);

/*! Change the automatic generation command that is used. This is useful if you do not want to build every source directory on build.
  \param project A ProjectSettings object.
  \param value The string to replace the current value.
*/
void openldev_project_settings_set_autogen (ProjectSettings *project, gchar *value);

/*! Change the configure command that is used. This can be used when you want to automatically pass parameters to configure.
  \param project A ProjectSettings object.
  \param value The string to replace the current value.
*/
void openldev_project_settings_set_configure (ProjectSettings *project, gchar *value);

/*!  Change the clean command that is used. The default is 'make clean', but you can change it to 'make distclean' if you want not only object files to be deleted, but also configure output files.
  \param project A ProjectSettings object.
  \param value The string to replace the current value.
*/
void openldev_project_settings_set_clean     (ProjectSettings *project, gchar *value);

/*! Change the current version of the project. This will edit your project configure.in file.
  \param project A ProjectSettings object.
  \param value The string to replace the current value.
*/
void openldev_project_settings_set_version   (ProjectSettings *project, gchar *value);

/*! Change the execute command that is used. Every instance of {BASE} will be replaced by the project location. Every instance of {NAME} will be replaced with the project name.
  \param project A ProjectSettings object.
  \param value The string to replace the current value.
*/
void openldev_project_settings_set_execute   (ProjectSettings *project, gchar *value);

/*! Retrieve a list of files that are currently open in the editor. This is used to reopen files after the application has been closed.
  \param project A ProjectSettings object.
  \return A list of files that are currently open in the editor.
*/
GSList* openldev_project_settings_get_lastfiles (ProjectSettings *project);

/*! Retrieve the short template name used to create the project. This can be useful when trying to obtain default settings for build commands or file content.
  \param project A ProjectSettings object.
  \return The short name of the project template used.
*/
gchar* openldev_project_settings_get_template  (ProjectSettings *project);

/*! Retrieve the programming language of the project. This is really just for reference and can be changed without affecting the operation of OpenLDev.
  \param project A ProjectSettings object.
  \return Project programming language such as 'C++' or 'C'.
*/
gchar* openldev_project_settings_get_language (ProjectSettings *project);

/*! Retrieve the command that will be called when the Compile menu item is activated.
  \param project A ProjectSettings object.
  \return The compile command (usually 'make {FILE}').
*/
gchar* openldev_project_settings_get_compile (ProjectSettings *project);

/*! Retrieve the command that will be called when the Build menu item is activated.
  \param project A ProjectSettings object.
  \return The build command (usually 'make').
*/
gchar* openldev_project_settings_get_build (ProjectSettings *project);

/*! Retrieve the command that will be called when the Auto Generate menu item is activated.
  \param project A ProjectSettings object.
  \return The auto generate command (usually './autogen.sh').
*/
gchar* openldev_project_settings_get_autogen (ProjectSettings *project);

/*! Retrieve the command that will be called when the Configure menu item is activated.
  \param project A ProjectSettings object.
  \return The configure command (usually './configure').
*/
gchar* openldev_project_settings_get_configure (ProjectSettings *project);

/*! Retrieve the command that will be called when the Clean menu item is activated.
  \param project A ProjectSettings object.
  \return The clean command (usually 'make clean').
*/
gchar* openldev_project_settings_get_clean (ProjectSettings *project);

/*! Retrieve the full location of the project. This includes the base project directory.
  \param project A ProjectSettings object.
  \return The full location of the project, obtained from the project file name.
*/
gchar* openldev_project_settings_get_location (ProjectSettings *project);

/*! Retrieve the formal name of the project. This is obtained from the project file name.
  \param project A ProjectSettings object.
  \return The specified name of the project, obtained from the project file name.
*/
gchar* openldev_project_settings_get_name (ProjectSettings *project);

/*! Retrieve the current version of the project that is passed in your project configure.in.
  \param project A ProjectSettings object.
  \return The current version of the project, obtained from the configure.in file.
*/
gchar* openldev_project_settings_get_version (ProjectSettings *project);

/*! Retrieve the full file name of the project file. This includes the path, project name and file extension.
  \param project A ProjectSettings object.
  \return The full location of the project file.
*/
gchar* openldev_project_settings_get_fullname (ProjectSettings *project);

gboolean openldev_project_is_open (ProjectSettings *project);

#endif
