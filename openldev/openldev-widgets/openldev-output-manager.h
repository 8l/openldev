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

#ifndef OUTPUT_MANAGER_H
#define OUTPUT_MANAGER_H

/*! \file openldev-output-manager.h
    \brief OutputManager
*/

#include <gtk/gtk.h>

#include "openldev-output-build.h"
#include "openldev-output-search.h"
#include "openldev-output-terminal.h"

G_BEGIN_DECLS

#define OUTPUT_TYPE            (output_manager_get_type ())
#define OUTPUT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), OUTPUT_TYPE, OutputManager))
#define OUTPUT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), OUTPUT_TYPE, OutputManagerClass))
#define IS_OUTPUT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OUTPUT_TYPE))
#define IS_OUTPUT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), OUTPUT_TYPE))

/*! \brief GtkNotebook that holds the build and search output. */
struct OutputManager
{
  GtkNotebook parent;

  /*! Search output widget. */
  GtkWidget *search;
  /*! Build output widget. */
  GtkWidget *build;
  /*! VTE terminal widget. */
  GtkWidget *terminal;

  /*! GtkHBox for the compiler tab label. */
  GtkWidget *compiler;
  /*! GtkHBox for the search tab label. */
  GtkWidget *findtab;
  /*! GtkHBox for the terminal tab label. */
  GtkWidget *terminaltab;
  /*! GtkLabel for the compiler tab. */
  GtkWidget *compiler_label;
  /*! GtkLabel for the search tab. */
  GtkWidget *findtab_label;
  /*! GtkLabel for the terminal tab. */
  GtkWidget *terminaltab_label;
  /*! GtkImage for the compiler tab. */
  GtkWidget *compiler_image;
  /*! GtkImage for the search tab. */
  GtkWidget *findtab_image;
  /*! GtkImage for the terminal tab. */
  GtkWidget *terminaltab_image;
};

struct OutputManagerClass
{
  GtkNotebookClass parent_class;
};

// Internal function: Do not document!
GType output_manager_get_type ();
GtkWidget* output_manager_new (EnvironmentSettings *env, MenuHandle *menus);

/*! Add a new tab to the GtkNotebook. You can add any widget that you want as the tab_label and child.
  \param manager An OutputManager widget.
  \param child GtkWidget that will be displayed as the content when the tab is selected.
  \param tab_label GtkWidget used as the label in the tab.
*/
void output_manager_add_tab (OutputManager *manager, GtkWidget *child, GtkWidget *tab_label);

/*! Retrieve the OutputBuild object of the specified OutputManager.
  \param manager An OutputManager object.
  \return The OutputManager's OutputBuild object.
*/
GtkWidget* output_manager_get_build (OutputManager *manager);

/*! Retrieve the OutputSearch object of the specified OutputManager.
  \param manager An OutputManager object.
  \return The OutputManager's OutputSearch object.
*/
GtkWidget* output_manager_get_search (OutputManager *manager);

/*! Retrieve the OutputTerminal object of the specified OutputManager.
  \param manager An OutputManager object.
  \return The OutputManager's OutputTerminal object.
*/
GtkWidget* output_manager_get_terminal (OutputManager *manager);

G_END_DECLS

#endif
