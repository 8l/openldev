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

#ifndef MENU_WIDGET_H
#define MENU_WIDGET_H

/*!
  \file openldev-menu.h
  \brief MenuHandle
*/

#include "../openldev-utils.h"
#include <gtk/gtk.h>
#include <iostream>
using namespace std;

/*! \brief The menubar, toolbar and popup menu widgets. */
struct MenuHandle
{
  /*! GtkMenuBar shown at the top of the OpenLDev window. */
  GtkWidget *menubar;
  /*! Popup menu shown when you right-click on the BrowserProject treeview. */
  GtkWidget *popup_filebrowser;
  /*! Popup menu shown when you right-click on the OutputBuild treeview. */
  GtkWidget *popup_buildoutput;
  /*! Popup menu shown when you right-click on the OutputSearch treeview. */
  GtkWidget *popup_searchoutput;
  /*! GtkToolbar shown at the top of the OpenLDev window. */
  GtkWidget *toolbar;
  /*! Global GtkActionGroup used by OpenLDev. */
  GtkActionGroup *group;
  /*! Global GtkUIManager used by OpenLDev. */
  GtkUIManager *uimanager;
  /*! Tooltips for Recent Files menu items and anything else you want. */
  GtkTooltips *tooltips;
};

// Internal function: Do not document!
MenuHandle* menu_handle_new ();

/*! Get OpenLDev's default GtkMenuBar widget.
  \param handle A MenuHandle object.
  \return GtkMenuBar widget.
*/
GtkWidget* menu_handle_get_menubar (MenuHandle *handle);

/*! Get the popup_filebrowser widget. This is the most commonly edited menu.
  \param handle A MenuHandle object.
  \return The popup_filebrowser widget.
*/
GtkWidget* menu_handle_get_popup (MenuHandle *handle);

/*! Get OpenLDev's main GtkToolbar widget.
  \param handle A MenuHandle object.
  \return GtkToolbar widget.
*/
GtkWidget* menu_handle_get_toolbar (MenuHandle *handle);

/*! Get OpenLDev's default GtkActionGroup.
  \param handle A MenuHandle object.
  \return The default GtkActionGroup.
*/
GtkActionGroup* menu_handle_get_action_group (MenuHandle *handle);

/*! Get OpenLDev's default GtkUIManager.
  \param handle A MenuHandle object.
  \return The default GtkUIManager.
*/
GtkUIManager* menu_handle_get_uimanager (MenuHandle *handle);

/*! Set a widget in the specified GtkUIManager as sensitive or not sensitive. This helps you avoid having to load the widget from the GtkUIManager manually.
  \param ui A valid GtkUIManager object.
  \param id The full path string to a widget in the GtkUIManager.
  \param sensitive TRUE if the widget should be set as sensitive.
*/
void menu_handle_set_sensitive (GtkUIManager *ui, string id, gboolean sensitive);

void menu_handle_manage_file_items (MenuHandle *handle, gboolean file_open);
void menu_handle_manage_project_items (MenuHandle *handle, gboolean project_open);
void menu_handle_manage_build_items (MenuHandle *handle, gboolean building);

// Do not document!
void menu_handle_create_icon_factory ();
void menu_handle_uimanager_init (MenuHandle*);

#endif
