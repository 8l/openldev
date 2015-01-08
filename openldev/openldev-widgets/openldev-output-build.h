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

#ifndef OUTPUT_BUILD_H
#define OUTPUT_BUILD_H

/*!
  \file openldev-output-build.h
  \brief OutputBuild
*/

#include "../openldev-settings/openldev-settings.h"
#include "openldev-menu.h"

#include <gtk/gtk.h>
#include <iostream>
#include <glib.h>
#include <glib-object.h>
#include <csignal>
using namespace std;

G_BEGIN_DECLS

#define OUTPUT_BUILD_TYPE            (output_build_get_type ())
#define OUTPUT_BUILD(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), OUTPUT_BUILD_TYPE, OutputBuild))
#define OUTPUT_BUILD_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), OUTPUT_BUILD_TYPE, OutputBuildClass))
#define IS_OUTPUT_BUILD(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OUTPUT_BUILD_TYPE))
#define IS_OUTPUT_BUILD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), OUTPUT_BUILD_TYPE))

enum
{
  OUTPUT_ERROR,
  OUTPUT_WARNING
};

typedef struct
{
  gint type, line;
  gchar *location;
} BuildMessage;

/*! \brief Widgets for the build output GtkTreeView. */
struct OutputBuild
{
  GtkTreeView parent;
  
  /*! GtkScrolledWindow that contains the main GtkTreeView widget. */
  GtkWidget *swin;
  
  EnvironmentSettings *env;
  MenuHandle *menus;
  
  gboolean successful;
  gboolean terminated;
  
  GIOChannel *channel;
  GIOChannel *channelerr;
  
  GPid pid;
  GTimer *timer;
  GMainLoop *loop;
  
  GList *location;
  GList *messages;
};

struct OutputBuildClass
{
  GtkTreeViewClass parent_class;
};

GType output_build_get_type ();
GtkWidget* output_build_new (EnvironmentSettings *settings, MenuHandle *menus);

/*! Get the GtkScrolledWindow widget in the OutputBuild object.
  \param build An OutputBuild object.
  \return The OutputBuild's GtkScrolledWindow.
*/
GtkWidget* output_build_get_scrolled_window (OutputBuild *build);

/*! Run a command using fopen and place the output, one line per row, in the GtkTreeView widget. The EnvironmentSettings object is used to retrieve colors to use for build output errors or warnings.
  \param build An OutputBuild object.
  \param command The command to be executed.
  \param base_dir The directory where the command was run.
  \return TRUE if the command was successfully completed without errors.
*/
gboolean output_build_run_command (OutputBuild *build, gchar *command, gchar *base_dir);

/*! Stop any command that is currently running. This will happen on the next output cycle.
  \param build An OutputBuild object.
*/
void output_build_stop_build (OutputBuild *build);

/*! Clear the content of the GtkTreeView.
  \param build An OutputBuild object.
*/
void output_build_clear (OutputBuild *build);

G_END_DECLS

#endif
