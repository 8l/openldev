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

#ifndef SIDEBAR_H
#define SIDEBAR_H

/*!
  \file openldev-sidebar.h
  \brief SideBar
*/

#include "openldev-file-browser.h"
#include <gtk/gtk.h>
#include <glib-object.h>
#include <glib.h>

G_BEGIN_DECLS

#define SIDEBAR_TYPE            (sidebar_get_type ())
#define SIDEBAR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SIDEBAR_TYPE, SideBar))
#define SIDEBAR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), SIDEBAR_TYPE, SideBarClass))
#define IS_SIDEBAR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SIDEBAR_TYPE))
#define IS_SIDEBAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SIDEBAR_TYPE))

/*! 
  SideBar is a custom widget derived from GtkNotebook. Therefore, it inherits all of the functions, signals and properties of GtkNotebook and the classes it is derived from.
  \brief GtkNotebook widget that holds the FileBrowser widget. 
*/
struct SideBar
{
  GtkNotebook parent;

  /*! Default file browser widget used by OpenLDev */
  GtkWidget *filebrowser;
};

struct SideBarClass
{
  GtkNotebookClass parent_class;
};

GType sidebar_get_type ();
GtkWidget* sidebar_new ();

/*! Add a new tab to the GtkNotebook. You can add any widget that you want as the tab_label and child.
  \param sidebar A SideBar widget.
  \param child GtkWidget that will be displayed as the content when the tab is selected.
  \param tab_label GtkWidget used as the label in the tab.
*/
void sidebar_add_tab (SideBar *sidebar, GtkWidget *child, GtkWidget *tab_label);

/*! Retrieve the FileBrowser widget that was created by sidebar_new().
  \param sidebar A SideBar widget.
  \return A FileBrowser widget.
*/
FileBrowser* sidebar_get_file_browser (SideBar *sidebar);

G_END_DECLS

#endif
