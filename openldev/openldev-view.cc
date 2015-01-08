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

#include "openldev-view.h"

// Show or hide the toolbar
void openldev_menu_view_toolbar (OpenLDev *openldev)
{
  EditorProperties *editor;
  MenuHandle *menus;
  
  g_object_get (openldev, "editor-prop", &editor, "menus", &menus, NULL);
  if (editor->toolbar)
    gtk_widget_hide (menus->toolbar);
  else 
    gtk_widget_show (menus->toolbar);
  
  openldev_editor_set_toolbar (editor, !editor->toolbar);
}

// Show or hide the status bar
void openldev_menu_view_statusbar (OpenLDev *openldev)
{
  EditorProperties *editor;
  StatusBar *statusbar;
  
  g_object_get (openldev, "editor-prop", &editor, "status-bar", &statusbar, NULL);
  if (editor->statusbar) 
    gtk_widget_hide (statusbar->bar);
  else 
    gtk_widget_show (statusbar->bar);
  
  openldev_editor_set_statusbar (editor, !editor->statusbar);
}
// Show or hide the Output Manager
void openldev_menu_view_output (OpenLDev *openldev)
{
  EditorProperties *editor;
  GtkWidget *output;
  
  g_object_get (openldev, "editor-prop", &editor, "output-pane", &output, NULL);
  if (editor->outputmanager)
    gtk_widget_hide (output);
  else 
    gtk_widget_show (output);
  
  openldev_editor_set_outputmanager (editor, !editor->outputmanager);
}

// Show or hide the Browser side pane
void openldev_menu_view_browser (OpenLDev *openldev) 
{
  EditorProperties *editor;
  GtkWidget *sidebar;
  
  g_object_get (openldev, "editor-prop", &editor, "sidebar", &sidebar, NULL);
  if (editor->browser)
    gtk_widget_hide (sidebar);
  else
    gtk_widget_show (sidebar);
  
  openldev_editor_set_browser (editor, !editor->browser);
}

// Show or hide the line numbering
void openldev_menu_view_line_numbering (OpenLDev *openldev)
{
  EnvironmentSettings *env;
  FileManager *files;
  gboolean ln;
  
  g_object_get (openldev, "env-settings", &env, "file-browser", &files, NULL);
  g_object_get (env, "line-numbers", &ln, NULL);
  
  if (GTK_WIDGET_VISIBLE (GTK_WIDGET (openldev)))
  {
    g_object_set (env, "line-numbers", !ln, NULL);
    file_manager_set_properties (files, TRUE, env);
  }
}

// Show or hide the Browser side pane
void 
openldev_view_fullscreen (GtkMenuItem *item,
                          OpenLDev *openldev) 
{
  if (gdk_window_get_state (GTK_WIDGET (openldev)->window) == GDK_WINDOW_STATE_FULLSCREEN)
    gtk_window_unfullscreen (GTK_WINDOW (openldev));
  else
    gtk_window_fullscreen (GTK_WINDOW (openldev));
}
