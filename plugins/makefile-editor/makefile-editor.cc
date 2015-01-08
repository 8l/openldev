/***************************************************************************
 *   Copyright (C) 2005-2006 by Andrew Krause                              *
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

#include "makefile-editor.h"

void makefile_editor_edit_clicked (GtkMenuItem*, gpointer);
void makefile_editor_button_clicked (GtkToolButton*);
void on_update_makefile_editor (OpenLDev*, MakefileEditor*);

static MakefileEditor *mfeditor;

#define IMAGE_DIRECTORY INSTALL_PREFIX"/share/pixmaps/openldev/mfeditor-directory.png"
#define IMAGE_FILE      INSTALL_PREFIX"/share/pixmaps/openldev/mfeditor-file.png"
#define IMAGE_PREFIX    INSTALL_PREFIX"/share/pixmaps/openldev/mfeditor-prefix.png"
#define IMAGE_SCRIPT    INSTALL_PREFIX"/share/pixmaps/openldev/mfeditor-script.png"
#define IMAGE_TARGET    INSTALL_PREFIX"/share/pixmaps/openldev/mfeditor-target.png"
#define IMAGE_VARIABLE  INSTALL_PREFIX"/share/pixmaps/openldev/mfeditor-variable.png"

extern "C" 
{
// Initialize the plugin
G_MODULE_EXPORT PluginState init (gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  ProjectSettings *project;
  
  g_object_get (openldev, "project", &project, NULL);
  mfeditor = makefile_editor_init (openldev);
  mfeditor->fullname = "";
  mfeditor->project = project;
  
  g_signal_connect (G_OBJECT (openldev), "project-open",
                    G_CALLBACK (on_update_makefile_editor), (gpointer) mfeditor);
  g_signal_connect (G_OBJECT (openldev), "project-close",
                    G_CALLBACK (on_update_makefile_editor), (gpointer) mfeditor);
  g_signal_connect (G_OBJECT (mfeditor->content), "button-press-event",
                    G_CALLBACK (makefile_editor_button_press), NULL);
  g_signal_connect (G_OBJECT (mfeditor->button), "clicked",
                    G_CALLBACK (makefile_editor_button_clicked), NULL);
  g_signal_connect (G_OBJECT (mfeditor->edit), "activate",
                    G_CALLBACK (makefile_editor_edit_clicked), (gpointer) mfeditor);
  g_signal_connect (G_OBJECT (mfeditor->add), "activate",
                    G_CALLBACK (makefile_editor_add), (gpointer) mfeditor);
  g_signal_connect (G_OBJECT (mfeditor->remove), "activate",
                    G_CALLBACK (makefile_editor_remove), (gpointer) mfeditor);
  
  return PLUGIN_OK;
}

// This actually will just hide the plugin for later use
G_MODULE_EXPORT PluginState destroy (gpointer data)
{
  gtk_widget_hide_all (mfeditor->vbox);
  
  return PLUGIN_OK;
}

// Show the plugin
G_MODULE_EXPORT PluginState reload (gpointer data)
{
  gtk_widget_show_all (mfeditor->vbox);
  
  return PLUGIN_OK;
}
};

// Create the Makefile Editor
MakefileEditor* makefile_editor_init (OpenLDev *openldev)
{
  MakefileEditor *editor = new MakefileEditor;
  ProjectSettings *project;
  SideBar *sidebar;
  
  g_object_get (openldev, "project", &project, "sidebar", &sidebar, NULL);

  editor->vbox = gtk_vbox_new (FALSE, 3);
  editor->scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  editor->content = gtk_tree_view_new ();
  editor->project = project;
  
  editor->toolbar = gtk_toolbar_new ();
  editor->button =  gtk_menu_tool_button_new_from_stock ("gtk-refresh");
  editor->menu = gtk_menu_new ();
  editor->add = gtk_menu_item_new_with_label ("Add New Element");
  editor->edit = gtk_menu_item_new_with_label ("Edit Properties");
  editor->remove = gtk_menu_item_new_with_label ("Remove Element");
  
  gtk_menu_attach (GTK_MENU (editor->menu), editor->add, 0, 1, 0, 1);
  gtk_menu_attach (GTK_MENU (editor->menu), editor->edit, 0, 1, 1, 2);
  gtk_menu_attach (GTK_MENU (editor->menu), editor->remove, 0, 1, 2, 3);
  
  gtk_menu_tool_button_set_menu (GTK_MENU_TOOL_BUTTON (editor->button), editor->menu);
  gtk_tool_button_set_label (GTK_TOOL_BUTTON (editor->button), "Makefile Editor");
  gtk_tool_item_set_expand (editor->button, TRUE);
  gtk_tool_item_set_is_important (editor->button, TRUE);
  gtk_toolbar_insert (GTK_TOOLBAR (editor->toolbar), editor->button, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (editor->toolbar), GTK_TOOLBAR_BOTH_HORIZ);
  gtk_toolbar_set_icon_size (GTK_TOOLBAR (editor->toolbar), GTK_ICON_SIZE_MENU);
  gtk_widget_show_all (editor->menu);
  
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (editor->content), FALSE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (editor->scrolledwindow),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (editor->scrolledwindow), editor->content);
  
  gtk_box_pack_start (GTK_BOX (editor->vbox), editor->toolbar, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (editor->vbox), editor->scrolledwindow, TRUE, TRUE, 0);
  
  sidebar_add_tab (sidebar, editor->vbox, gtk_label_new ("Project"));
  gtk_widget_hide_all (editor->vbox);
  
  return editor;
}

// Add all of the information for the current Makefile.am
void makefile_editor_populate (MakefileEditor *editor)
{
  if (editor->fullname.empty())
    return;

  unsigned int i, j;
  GtkTreeIter row, child, childchild;
  GtkTreeModel *model = GTK_TREE_MODEL (gtk_tree_store_new (3, GDK_TYPE_PIXBUF, 
                        G_TYPE_STRING, G_TYPE_INT));
  gtk_tree_view_set_model (GTK_TREE_VIEW (editor->content), model);
  
  while (g_list_length (gtk_tree_view_get_columns (GTK_TREE_VIEW (editor->content))) > 0)
    gtk_tree_view_remove_column (GTK_TREE_VIEW (editor->content),
      gtk_tree_view_get_column (GTK_TREE_VIEW (editor->content), 0));
  
  string makefile = g_strconcat (editor->project->location, (gchar*) editor->current_path.c_str(), 
                                 "Makefile.am", NULL);
  Makefile *mf = openldev_makefile_new (makefile);
  
  // Create the columns and add them
  GtkCellRenderer *irenderer = gtk_cell_renderer_pixbuf_new ();
  GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
	GtkTreeViewColumn *icolumn = gtk_tree_view_column_new_with_attributes ("",
                               irenderer, "pixbuf", 0, NULL);
	GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes ("Content",
                              renderer, "text", 1, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (editor->content), icolumn);
  gtk_tree_view_append_column (GTK_TREE_VIEW (editor->content), column);
  
  // Add the top directory & the '..' directory if needed
  gtk_tree_store_append (GTK_TREE_STORE (model), &row, NULL);
  if (!editor->current_path.empty())
  {
    gtk_tree_store_set (GTK_TREE_STORE (model), &row, 0, gtk_image_get_pixbuf (GTK_IMAGE 
                        (gtk_image_new_from_file (IMAGE_DIRECTORY))), 1, 
                        editor->current_dir.c_str(), 2, MAKEFILE_EDITOR_BASE, -1);
    makefile_editor_add_row (model, child, row, gtk_image_get_pixbuf (GTK_IMAGE
                             (gtk_image_new_from_file (IMAGE_DIRECTORY))), "..",
                             MAKEFILE_EDITOR_MOVE_UP);
  }
  else
    gtk_tree_store_set (GTK_TREE_STORE (model), &row, 0, gtk_image_get_pixbuf (GTK_IMAGE 
                        (gtk_image_new_from_file (IMAGE_DIRECTORY))), 1, "/",
                        2, MAKEFILE_EDITOR_BASE, -1);
  
  // List all of the elements in the project
  for (i=0; i < mf->subdirs.size(); i++)
    makefile_editor_add_row (model, child, row, gtk_image_get_pixbuf (GTK_IMAGE
                             (gtk_image_new_from_file (IMAGE_DIRECTORY))), 
                             mf->subdirs[i].c_str(), MAKEFILE_EDITOR_SUBPROJECT);
  for (i=0; i < mf->prefixes.size(); i++)
    makefile_editor_add_row (model, child, row, gtk_image_get_pixbuf (GTK_IMAGE
                             (gtk_image_new_from_file (IMAGE_PREFIX))), 
                             mf->prefixes[i]->name.c_str(), MAKEFILE_EDITOR_PREFIX);
  for (i=0; i < mf->targets.size(); i++)
    makefile_editor_add_row (model, child, row, gtk_image_get_pixbuf (GTK_IMAGE
                             (gtk_image_new_from_file (IMAGE_TARGET))), 
                             mf->targets[i]->name.c_str(), MAKEFILE_EDITOR_TARGET);
  for (i=0; i < mf->variables.size(); i++)
    makefile_editor_add_row (model, child, row, gtk_image_get_pixbuf (GTK_IMAGE
                             (gtk_image_new_from_file (IMAGE_VARIABLE))), 
                             mf->variables[i]->name.c_str(), MAKEFILE_EDITOR_VARIABLE);
  for (i=0; i < mf->scripts.size(); i++)
    makefile_editor_add_row (model, child, row, gtk_image_get_pixbuf (GTK_IMAGE
                             (gtk_image_new_from_file (IMAGE_SCRIPT))), 
                             mf->scripts[i]->header.c_str(), MAKEFILE_EDITOR_SCRIPT);
  
  gtk_tree_view_expand_row (GTK_TREE_VIEW (editor->content),
                            gtk_tree_path_new_from_string ("0"),
                            FALSE);
}

// Check if the current project for which the Makefile is being shown is still
// the project that is open
void on_update_makefile_editor (OpenLDev *openldev,
                                MakefileEditor *editor)
{
  if (editor->project->fullname == NULL)
  {
    editor->fullname = "";
    editor->current_dir = "";
    editor->current_path = "";
    makefile_editor_populate (editor);
  }
  else if (editor->project->fullname != editor->fullname)
  {
    editor->fullname = editor->project->fullname;
    editor->current_dir = "/";
    editor->current_path = "";
    makefile_editor_populate (editor);
  }
  
  if (openldev->project->name == NULL)
    gtk_widget_hide_all (editor->vbox);
  else
    gtk_widget_show_all (editor->vbox);
}

void makefile_editor_button_clicked (GtkToolButton *toolbutton)
{ makefile_editor_populate (mfeditor);
}

// Handle button press events in the Makefile Editor content treeview
gboolean makefile_editor_button_press (GtkWidget *widget, 
                                       GdkEventButton *event)
{
  if (event->type == GDK_2BUTTON_PRESS)
  {
    GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
      gchar *directory;
      int type;
      gtk_tree_model_get (model, &iter, 1, &directory, 2, &type, -1);
      
      // Move one directory up in the Makefile structure
      if (type == MAKEFILE_EDITOR_MOVE_UP)
      {
        string temp = mfeditor->current_path, temp2;
        temp.erase (temp.length () - 1, 1);
        
        while (temp.find ("/", 0) != string::npos)
        {
          temp2 += temp.substr (0, temp.find ("/", 0) + 1);
          temp.erase (0, temp.find ("/", 0) + 1);
        }
        
        mfeditor->current_path = g_strdup ((gchar*) temp2.c_str());
        temp = temp2.substr (0, temp2.length() - 1);
        while (temp.find ("/", 0) != string::npos)
          temp.erase (0, temp.find ("/", 0) + 1);
        mfeditor->current_dir = g_strdup ((gchar*) temp.c_str());
        makefile_editor_populate (mfeditor);
      }
      // Move into the directory of a subproject
      else if (type == MAKEFILE_EDITOR_SUBPROJECT)
      {
        mfeditor->current_dir = directory;
        mfeditor->current_path = mfeditor->current_path + directory + "/";
        makefile_editor_populate (mfeditor);
      }
      // Otherwise, edit the element. This can be done with the Edit button as well
      else
        makefile_editor_edit (mfeditor);
    }
    
    return TRUE;
  }
  
  return FALSE;
}

// The edit button was pressed, so we need to edit the element
void makefile_editor_edit_clicked (GtkMenuItem *item,
                                   gpointer data)
{ makefile_editor_edit (mfeditor);
}

// Add a row to the Makefile Editor content GtkTreeView
void makefile_editor_add_row (GtkTreeModel *model,
                              GtkTreeIter &child,
                              GtkTreeIter &row,
                              GdkPixbuf *pixbuf,
                              const char *text,
                              int id)
{
  gtk_tree_store_append (GTK_TREE_STORE (model), &child, &row);
  gtk_tree_store_set (GTK_TREE_STORE (model), &child,
                      0, pixbuf, 1, text, 2, id, -1);
}
