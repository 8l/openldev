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

#include "openldev-file-browser.h"

vector<string> read_directory (const char*, gboolean);
void populate_file_browser (FileBrowser*, vector<string>);
static void file_browser_class_init (FileBrowserClass *klass);
static void file_browser_init (FileBrowser *browser);

GType
file_browser_get_type ()
{
  static GType browser_type = 0;

  if (!browser_type)
  {
    static const GTypeInfo browser_info =
    {
	    sizeof (FileBrowserClass),
	    NULL, NULL,
	    (GClassInitFunc) file_browser_class_init,
      NULL, NULL,
      sizeof (FileBrowser), 0,
	    (GInstanceInitFunc) file_browser_init,
    };

    browser_type = g_type_register_static (GTK_TYPE_TREE_VIEW, "FileBrowser", 
                                           &browser_info, (GTypeFlags) 0);
  }

  return browser_type;
}

static void
file_browser_class_init (FileBrowserClass *klass)
{
}

static void
file_browser_init (FileBrowser *browser)
{
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (browser), FALSE);
  browser->swin = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (browser->swin), 
                                         GTK_WIDGET (browser));
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (browser->swin), 
                                  GTK_POLICY_AUTOMATIC, 
                                  GTK_POLICY_ALWAYS);
}

GtkWidget*
file_browser_new ()
{
  return GTK_WIDGET (g_object_new (file_browser_get_type (), NULL));
}

// Get the name of the file that is currently selected along with the path
const char* 
file_browser_get_selected_file (FileBrowser* browser)
{
  GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (browser));
  GtkTreeModel *model;
  GtkTreeIter iter;

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
    gchar *fn;
    gtk_tree_model_get (model, &iter, 0, &fn, -1);
   
    return g_strconcat (browser->base, browser->current, fn, NULL);
  }
  
  return "NONE";
}

// Populate the file browser with the list of files for the directory
void 
file_browser_populate (FileBrowser *browser, 
                       string base_directory)
{
  if (base_directory == "NULL" || base_directory == "")
    return;
  
  if (base_directory[base_directory.length() - 1] != '/')
    base_directory += "/";
  
  browser->base = g_strdup (base_directory.c_str());
  browser->current = g_strdup ("");
  
  vector<string> filenames = read_directory (base_directory.c_str(), FALSE);
  populate_file_browser (browser, filenames);
  filenames.clear();
}

// Move up one level in the file browser
void 
file_browser_move_up (FileBrowser *browser)
{
  // This will allow the stopping of moving the browser beyond its base
  string current = browser->current;
  
  if (current.find ("/", 0) != string::npos)
  {
    current.erase (current.length() - 1, 1);
    while (current.length() > 0 && current[current.length() - 1] != '/')
      current.erase (current.length() - 1, 1);
  }
  
  browser->current = g_strdup (current.c_str());
  
  // Repopulate with the new sources
  string dir = g_strconcat (browser->base, browser->current, NULL);
  vector<string> filenames;
  if (strlen (browser->current) > 0)
    filenames = read_directory (dir.c_str(), TRUE);
  else
    filenames = read_directory (dir.c_str(), FALSE);
  
  populate_file_browser (browser, filenames);
  filenames.clear();
}

// Move to a specific new directory
void 
file_browser_move (FileBrowser *browser,
                   const char* dir_name)
{
  browser->current = g_strconcat (browser->current, dir_name, "/", NULL);
  string dir = g_strconcat (browser->base, browser->current);
  vector<string> filenames = read_directory (dir.c_str(), TRUE);
  populate_file_browser (browser, filenames);
  filenames.clear();
}

// Refresh the current list of sources without changing the directory.
void 
file_browser_refresh (FileBrowser *browser)
{
  string dir = g_strconcat (browser->base, browser->current, NULL);
  vector<string> filenames = read_directory (dir.c_str(), !(dir == browser->base));   
  populate_file_browser (browser, filenames);
  filenames.clear();
}

// Compare file names alphabetically, used by g_list_sort() in read_directory()
gint 
comp_filenames_vfs (gconstpointer a,
                    gconstpointer b)
{
  GnomeVFSFileInfo *info_a, *info_b;
  info_a = (GnomeVFSFileInfo*) a;
  info_b = (GnomeVFSFileInfo*) b;
  return strcmp (info_a->name, info_b->name);
}

// Read the directory from the file system and populate the file browser
// with a list of all of the files in that directory.
vector<string> 
read_directory (const char *dir_path,
                gboolean show_up)
{
  GList *dir_entries, *itr;
  GnomeVFSFileInfo *file_info;
  vector<string> files;
  gnome_vfs_init();

  if (gnome_vfs_directory_list_load (&dir_entries, dir_path,
      GNOME_VFS_FILE_INFO_DEFAULT) == GNOME_VFS_OK)
  {
		// Sort directory and file names alphabetically
    dir_entries = g_list_sort (dir_entries, comp_filenames_vfs);

    // Extract sub directory names
    for (itr = dir_entries; itr != NULL; itr = itr->next)
    {
      file_info = (GnomeVFSFileInfo*) itr->data;
      if (file_info->type == GNOME_VFS_FILE_TYPE_DIRECTORY)
        if (file_info->name[0] != '.')
          files.push_back (file_info->name);
    }

    // Extract file names
    for (itr = dir_entries; itr != NULL; itr = itr->next)
    {
      file_info = (GnomeVFSFileInfo*) itr->data;
      if (file_info->type == GNOME_VFS_FILE_TYPE_REGULAR)
        files.push_back (file_info->name);
    }

    gnome_vfs_file_info_list_free (dir_entries);
  }
  
  // Insert the ability to move up a directory if the function
  // it told that it should do so.
  if (show_up)
    files.insert (files.begin(), "..");
  
  return files;
}

// Actually do the population of the file browser (finally)
void 
populate_file_browser (FileBrowser *browser,
                       vector<string> filenames)
{
  // Populate the view
  gnome_vfs_init ();
  GtkIconTheme *icon_theme = gtk_icon_theme_get_default ();
  GtkTreeIter row;
  GtkTreeModel *model = GTK_TREE_MODEL (gtk_tree_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING));
                                                          
  gtk_tree_view_set_model (GTK_TREE_VIEW (browser), model);
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (browser));
  
  while (g_list_length(gtk_tree_view_get_columns (GTK_TREE_VIEW (browser))) > 0)
    gtk_tree_view_remove_column (GTK_TREE_VIEW (browser),
      gtk_tree_view_get_column (GTK_TREE_VIEW (browser), 0));

  GtkCellRenderer *irenderer = gtk_cell_renderer_pixbuf_new ();
	GtkTreeViewColumn *icolumn = gtk_tree_view_column_new_with_attributes ("",
                               irenderer, "pixbuf", 0, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (browser), icolumn);
  
  GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
	GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes ("Project Files",
                              renderer, "text", 1, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (browser), column);
  
  for (unsigned int i=0; i < filenames.size(); i++)
  {    
    string fn = g_strconcat (browser->base, browser->current, filenames[i].c_str(), NULL);
    gchar *mime = gnome_vfs_get_mime_type (fn.c_str());
    gchar *icon = gnome_icon_lookup (icon_theme, NULL, NULL, NULL, NULL,
                  mime, GNOME_ICON_LOOKUP_FLAGS_NONE, NULL);
    GdkPixbuf *pixbuf = gtk_icon_theme_load_icon (icon_theme, icon, 16,
                        GTK_ICON_LOOKUP_NO_SVG, NULL);
    
    gtk_tree_store_append (GTK_TREE_STORE (model), &row, NULL);
    gtk_tree_store_set (GTK_TREE_STORE (model), &row, 0, pixbuf,
                        1, filenames[i].c_str(), -1);
  }
  
  if (filenames.size() > 0)
  {
    GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (browser));
    gtk_tree_model_get_iter (model, &row, gtk_tree_path_new_from_string("0"));
    gtk_tree_selection_select_iter (selection, &row);
  }
  
  g_object_unref (model);
}
