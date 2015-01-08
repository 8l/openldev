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

#include "openldev-menu.h"

// Create the new menubar and toolbar handler
MenuHandle* 
menu_handle_new ()
{
  MenuHandle *handle = new MenuHandle();
  
  handle->group = gtk_action_group_new ("ActionGroup");
  handle->uimanager = gtk_ui_manager_new ();
  handle->tooltips = gtk_tooltips_new ();
  
  return handle;
}

// This function creates a new icon factory that adds the stock menu/toolbar
// items for the compile, build, project files and about commands
void 
menu_handle_create_icon_factory ()
{
  GtkIconFactory *icon_factory = gtk_icon_factory_new ();
  
  // Add new stock menu items
  openldev_add_new_stock_item (icon_factory, "Compile", "COMPILE",
      openldev_add_prefix ("/share/pixmaps/openldev/compile.png"));
  openldev_add_new_stock_item (icon_factory, "Compile", "COMPILE_SMALL",
      openldev_add_prefix ("/share/pixmaps/openldev/compile_sm.png"));
  openldev_add_new_stock_item (icon_factory, "Build", "BUILD", 
      openldev_add_prefix ("/share/pixmaps/openldev/build.png"));
  openldev_add_new_stock_item (icon_factory, "Build", "BUILD_SMALL", 
      openldev_add_prefix ("/share/pixmaps/openldev/build_sm.png"));
  openldev_add_new_stock_item (icon_factory, "New Project", "NEWPROJECT", 
      openldev_add_prefix ("/share/pixmaps/openldev/newproject.png"));
  openldev_add_new_stock_item (icon_factory, "Project Info", "PROJECTINFO",
      openldev_add_prefix ("/share/pixmaps/openldev/projectinfo.png"));
  
  // Add items to default
  gtk_icon_factory_add_default (icon_factory);
  icon_factory = NULL;
}

void
menu_handle_uimanager_init (MenuHandle* handle)
{
  GError *uierror;
  
  // Load the main menu, toolbar and popup menus
  if (!gtk_ui_manager_add_ui_from_file (handle->uimanager, 
      openldev_add_prefix ("/share/openldev/menus/mainmenu.ui"), &uierror))
    openldev_message ("OpenLDev Error", "Cannot load file 'mainmenu.ui'!", GTK_MESSAGE_ERROR);
  if (!gtk_ui_manager_add_ui_from_file (handle->uimanager, 
      openldev_add_prefix ("/share/openldev/menus/toolbar.ui"), &uierror))
    openldev_message ("OpenLDev Error", "Cannot load file 'toolbar.ui'!", GTK_MESSAGE_ERROR);
  if (!gtk_ui_manager_add_ui_from_file (handle->uimanager, 
      openldev_add_prefix ("/share/openldev/menus/popup_menus.ui"), &uierror))
    openldev_message ("OpenLDev Error", "Cannot load file 'popup_menus.ui'!", GTK_MESSAGE_ERROR);
  
  // Add the action group so signals can be connected
  gtk_ui_manager_insert_action_group (handle->uimanager, handle->group, 0);
  
  // Get all of the menu & toolbar widgets so they can easily be used
  handle->menubar = gtk_ui_manager_get_widget (handle->uimanager, "/MenuBar");
  handle->toolbar = gtk_ui_manager_get_widget (handle->uimanager, "/ToolBar");
  handle->popup_filebrowser = gtk_ui_manager_get_widget (handle->uimanager, "/FileBrowserMenu");
  handle->popup_searchoutput = gtk_ui_manager_get_widget (handle->uimanager, "/SearchOutputMenu");
  handle->popup_buildoutput = gtk_ui_manager_get_widget (handle->uimanager, "/BuildOutputMenu");
  
  // Set some important styles for the toolbar
  gtk_toolbar_set_style (GTK_TOOLBAR (handle->toolbar), GTK_TOOLBAR_BOTH_HORIZ);
  gtk_toolbar_set_show_arrow (GTK_TOOLBAR (handle->toolbar), FALSE);
  
  // Make sure the text on the New File and Open menu items is shown
  gtk_tool_item_set_is_important (GTK_TOOL_ITEM (gtk_ui_manager_get_widget 
                 (handle->uimanager, "/ToolBar/FileNewFileToolbar")), TRUE);
  gtk_tool_item_set_is_important (GTK_TOOL_ITEM (gtk_ui_manager_get_widget 
                 (handle->uimanager, "/ToolBar/FileOpenToolbar")), TRUE);
  
  gtk_tooltips_enable (handle->tooltips);
}

void 
menu_handle_set_sensitive (GtkUIManager *ui, 
                           string id, 
                           gboolean sensitive)
{
  gtk_widget_set_sensitive (gtk_ui_manager_get_widget (ui, id.c_str()), sensitive);
}

void 
menu_handle_manage_file_items (MenuHandle *handle, 
                               gboolean file_open)
{
  GtkUIManager *uimanager = handle->uimanager;
  
  menu_handle_set_sensitive (uimanager, "/MenuBar/File/FileSave", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/File/FileSaveAs", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/File/FileSaveAll", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/File/FileRevert", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/File/FilePrintPreview", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/File/FilePrint", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Edit/EditUndo", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Edit/EditRedo", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Edit/EditCut", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Edit/EditCopy", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Edit/EditPaste", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Edit/EditSelectAll", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Edit/EditDeselect", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Edit/EditIndent", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Edit/EditUnIndent", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Search/EditFindNext", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Search/EditReplace", file_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Search/EditGotoLine", file_open);
  menu_handle_set_sensitive (uimanager, "/ToolBar/FileSave", file_open);
  menu_handle_set_sensitive (uimanager, "/ToolBar/FilePrint", file_open);
  menu_handle_set_sensitive (uimanager, "/ToolBar/FilePrintPreview", file_open);
  menu_handle_set_sensitive (uimanager, "/ToolBar/EditCut", file_open);
  menu_handle_set_sensitive (uimanager, "/ToolBar/EditCopy", file_open);
  menu_handle_set_sensitive (uimanager, "/ToolBar/EditPaste", file_open);
  menu_handle_set_sensitive (uimanager, "/ToolBar/EditUndo", file_open);
  menu_handle_set_sensitive (uimanager, "/ToolBar/EditRedo", file_open);
}

void 
menu_handle_manage_project_items (MenuHandle *handle, 
                                  gboolean project_open)
{
  GtkUIManager *uimanager = handle->uimanager;
  
  menu_handle_set_sensitive (uimanager, "/MenuBar/File/FileClose", project_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Project/ProjectConfigure", project_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Project/ProjectInfo", project_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Build/BuildCompile", project_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Build/BuildBuild", project_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Build/BuildExecute", project_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Build/BuildAutogen", project_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Build/BuildConfigure", project_open);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Build/BuildClean", project_open);
  menu_handle_set_sensitive (uimanager, "/ToolBar/BuildCompileToolbar", project_open);
  menu_handle_set_sensitive (uimanager, "/ToolBar/BuildBuildToolbar", project_open);
  menu_handle_set_sensitive (uimanager, "/ToolBar/BuildExecute", project_open);
  menu_handle_set_sensitive (uimanager, "/ToolBar/BuildClean", project_open);
  menu_handle_set_sensitive (uimanager, "/ToolBar/ProjectConfigure", project_open);
  menu_handle_set_sensitive (uimanager, "/FileBrowserMenu/ProjectConfigure", project_open);
  menu_handle_set_sensitive (uimanager, "/FileBrowserMenu/ProjectInfo", project_open);
}

void 
menu_handle_manage_build_items (MenuHandle *handle, 
                                gboolean building)
{
  GtkUIManager *uimanager = handle->uimanager;
  
  menu_handle_set_sensitive (uimanager, "/MenuBar/Build/BuildCompile", !building);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Build/BuildBuild", !building);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Build/BuildClean", !building);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Build/BuildExecute", !building);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Build/BuildAutogen", !building);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Build/BuildConfigure", !building);
  menu_handle_set_sensitive (uimanager, "/MenuBar/Build/BuildStop", building);
  menu_handle_set_sensitive (uimanager, "/BuildOutputMenu/BuildOutputGo", !building);
  menu_handle_set_sensitive (uimanager, "/BuildOutputMenu/BuildOutputCopy", !building);
  menu_handle_set_sensitive (uimanager, "/BuildOutputMenu/BuildOutputClear", !building);
  menu_handle_set_sensitive (uimanager, "/ToolBar/BuildCompileToolbar", !building);
  menu_handle_set_sensitive (uimanager, "/ToolBar/BuildBuildToolbar", !building);
  menu_handle_set_sensitive (uimanager, "/ToolBar/BuildExecute", !building);
  menu_handle_set_sensitive (uimanager, "/ToolBar/BuildClean", !building);
  menu_handle_set_sensitive (uimanager, "/ToolBar/BuildStop", building);
}

GtkWidget* 
menu_handle_get_menubar (MenuHandle* handle)
{ return handle->menubar;
}

GtkWidget* 
menu_handle_get_popup (MenuHandle* handle)
{ return handle->popup_filebrowser;
}

GtkWidget* 
menu_handle_get_toolbar (MenuHandle* handle)
{ return handle->toolbar;
}

GtkActionGroup* 
menu_handle_get_action_group (MenuHandle* handle)
{ return handle->group;
}

GtkUIManager* 
menu_handle_get_uimanager (MenuHandle* handle)
{ return handle->uimanager;
}
