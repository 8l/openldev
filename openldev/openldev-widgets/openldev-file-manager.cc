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

#include "openldev-file-manager.h"

void file_manager_set_properties_one_tab (FileManager*, int, EnvironmentSettings*);
void file_manager_on_notebook_event (GtkNotebook*, gint, gpointer);

static void file_manager_class_init (FileManagerClass *klass);
static void file_manager_init (FileManager *tab);

GType
file_manager_get_type ()
{
  static GType manager_type = 0;

  if (!manager_type)
  {
    static const GTypeInfo manager_info =
    {
	    sizeof (FileManagerClass),
	    NULL, NULL,
	    (GClassInitFunc) file_manager_class_init,
      NULL, NULL,
      sizeof (FileManager), 0,
	    (GInstanceInitFunc) file_manager_init,
    };

    manager_type = g_type_register_static (GTK_TYPE_NOTEBOOK, "FileManager", 
                                           &manager_info, (GTypeFlags) 0);
  }

  return manager_type;
}

static void
file_manager_class_init (FileManagerClass *klass)
{
}

static void
file_manager_init (FileManager *manager)
{
  gtk_notebook_set_scrollable (GTK_NOTEBOOK (manager), TRUE);
  gtk_widget_show (GTK_WIDGET (manager));
  
  g_signal_connect (G_OBJECT (manager), "change_current_page",
                    G_CALLBACK (file_manager_on_notebook_event), (gpointer) manager);
  manager->tab_count = 0;
}

GtkWidget*
file_manager_new ()
{
  return GTK_WIDGET (g_object_new (file_manager_get_type (), NULL));
}

gint
file_manager_get_file_is_open (FileManager *manager,
                               gchar *file)
{
  for (gint i = 0; i < manager->tab_count; i++)
    if (g_ascii_strcasecmp (file, FILE_TAB (manager->tabs[i])->filename) == 0)
      return i;
  
  return -1;
}

/* Get the current text view. */
GtkSourceView* 
file_manager_get_current_view (FileManager *manager)
{ 
  gint current = gtk_notebook_get_current_page (GTK_NOTEBOOK (manager));

  if (manager->tab_count != 0)
    return GTK_SOURCE_VIEW (manager->tabs[current]);
  else
    return NULL;
}

/* Get the text buffer from the current text view. */
GtkSourceBuffer* 
file_manager_get_current_buffer (FileManager *manager)
{ 
  gint current = gtk_notebook_get_current_page (GTK_NOTEBOOK (manager));
  return FILE_TAB (manager->tabs[current])->buffer;
}

/* Get the full filename of the current file. */
gchar* file_manager_get_current_filename (FileManager *manager)
{ 
  GtkWidget *tab = manager->tabs[gtk_notebook_get_current_page (GTK_NOTEBOOK (manager))];
  return FILE_TAB (tab)->filename;
}

/* Get the shortened filename of the current file. */
string file_manager_get_current_filename_short (FileManager *manager)
{
  gint current = gtk_notebook_get_current_page (GTK_NOTEBOOK (manager));
  
  return gtk_label_get_text (GTK_LABEL (FILE_TAB (manager->tabs[current])->label));
}

// Get the id of the current tab that the user selected
int file_manager_get_current_tab (FileManager *manager)
{ 
  g_assert (manager != NULL);
  return gtk_notebook_get_current_page (GTK_NOTEBOOK (manager));
}

// Get the offset of the cursor in the current file
int file_manager_get_insert_offset (FileManager *manager)
{
  g_assert (manager != NULL);
  GtkTextBuffer *buffer = GTK_TEXT_BUFFER (file_manager_get_current_buffer (manager));
  GtkTextIter iter;
  
  gtk_text_buffer_get_iter_at_mark (buffer, &iter, gtk_text_buffer_get_insert (buffer));
  return gtk_text_iter_get_offset (&iter);
}

// Get the mime type of the specified file
string get_mime_type (string fn)
{ 
  return gnome_vfs_get_mime_type (fn.c_str());
}

// Get the offset of the selection iterator in the current file
int file_manager_get_selection_offset (FileManager *manager)
{ 
  g_assert (manager != NULL);
  GtkTextBuffer *buffer = GTK_TEXT_BUFFER (file_manager_get_current_buffer (manager));
  GtkTextIter iter;
  
  gtk_text_buffer_get_iter_at_mark (buffer, &iter, gtk_text_buffer_get_selection_bound (buffer));
  return gtk_text_iter_get_offset(&iter);
}

// Check whether or not there is a file open
gboolean file_manager_get_file_open (FileManager *manager)
{
  g_assert (manager != NULL);
  if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (manager)) > 0)
    return TRUE;
  
  return FALSE;
}

// Set a new filename to the tab
void file_manager_set_filename (FileManager *manager,
                                const char* fn)
{
  g_assert (manager != NULL && fn != NULL);
  string file = fn;
  int current = gtk_notebook_get_current_page (GTK_NOTEBOOK (manager));
  FILE_TAB (manager->tabs[current])->filename = g_strdup (fn);
  
  while (file.find('/',0) != string::npos)
    file.erase (0, file.find('/',0) + 1);
  
  gtk_label_set_text (GTK_LABEL (FILE_TAB (manager->tabs[current])->label), file.c_str());
}

// Make sure all of the properties of every tab are up to dat
void file_manager_set_properties (FileManager* manager, 
                                  gboolean all_files, 
                                  EnvironmentSettings *env)
{
  g_assert (manager != NULL && env != NULL);
  
  if (all_files)
    for (int i=0; i < manager->tab_count; i++)
      file_manager_set_properties_one_tab (manager, i, env);
  else
    file_manager_set_properties_one_tab (manager, gtk_notebook_get_current_page 
                                         (GTK_NOTEBOOK (manager)), env);
}

// Make sure all of the properties of the specific tab are correct
void file_manager_set_properties_one_tab (FileManager *manager,
                                          int tab,
                                          EnvironmentSettings *env)
{
  GtkSourceView *view = GTK_SOURCE_VIEW (manager->tabs[tab]);
  GtkSourceBuffer *buffer = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer 
                                               (GTK_TEXT_VIEW (view)));
  gint int_value;
  gboolean bool_value;
  
  g_object_get (env, "line-numbers", &bool_value, NULL);
  gtk_source_view_set_show_line_numbers (view, bool_value);
  g_object_get (env, "tab-size", &int_value, NULL);
  gtk_source_view_set_tabs_width (view, int_value);
  g_object_get (env, "auto-indent", &bool_value, NULL);
  gtk_source_view_set_auto_indent (view, bool_value);
  g_object_get (env, "right-margin", &bool_value, NULL);
  gtk_source_view_set_show_margin (view, bool_value);
  g_object_get (env, "replace-tabs", &bool_value, NULL);
  gtk_source_view_set_insert_spaces_instead_of_tabs (view, bool_value);
  g_object_get (env, "undo-actions", &int_value, NULL);
  gtk_source_buffer_set_max_undo_levels (buffer, int_value);
  g_object_get (env, "brackets", &bool_value, NULL);
  gtk_source_buffer_set_check_brackets (buffer, bool_value);

  // Set language specific values
  string mime_type = get_mime_type (FILE_TAB (manager->tabs[tab])->filename); 
  if (mime_type == "NONE")
    gtk_source_buffer_set_highlight (buffer, FALSE);
  else
  {
    gtk_source_buffer_set_highlight (buffer, TRUE);
    GtkSourceLanguagesManager *manager = gtk_source_languages_manager_new ();
    GtkSourceLanguage* language = gtk_source_languages_manager_get_language_from_mime_type (manager, mime_type.c_str());

    if (language)
      gtk_source_buffer_set_language (buffer, language);
  }
  
  g_object_get (env, "wrap-enable", &bool_value, NULL);
  if (bool_value)
  {
    g_object_get (env, "wrap-split", &bool_value, NULL);
    if (bool_value)
      gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW(view), GTK_WRAP_CHAR);
    else
      gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW(view), GTK_WRAP_WORD);
  }
  else
      gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW(view), GTK_WRAP_NONE);
}

// A new file was opened so we must add the tab for it
gboolean file_manager_add_tab (FileManager *manager,
                               const char* file,
                               EnvironmentSettings *env)
{
  g_assert (manager != NULL && file != NULL && env != NULL);
  
  // Check to make sure file is not already open
  string filename = file;
  for (int i=0; i < manager->tab_count; i++)
    if (filename == FILE_TAB (manager->tabs[i])->filename)
    {
      gtk_notebook_set_current_page (GTK_NOTEBOOK (manager), i);
      return FALSE;
    }

  FileTab *tab = FILE_TAB (file_tab_new (file));
  tab->filename = g_strdup (file);
  
  // Parse the filename to remove directory structure
  while (filename.find ('/',0) != string::npos)
    filename.erase (0,filename.find ('/',0) + 1);

  // Add the tab to the notebook and notebook_tab's deque
	manager->tabs.push_back (GTK_WIDGET (tab));
  gtk_notebook_append_page (GTK_NOTEBOOK (manager),
                            FILE_TAB (manager->tabs[manager->tab_count])->swin,
                            FILE_TAB (manager->tabs[manager->tab_count])->hbox);
  
  // Do final initialization
  gtk_notebook_set_current_page (GTK_NOTEBOOK (manager), manager->tab_count);
	manager->tab_count++;
  return TRUE;
}

// Remove all of the tabs and tab information
void file_manager_erase_all_tabs (FileManager *manager)
{
  g_assert (manager != NULL);
  manager->tabs.erase (manager->tabs.begin(), manager->tabs.end());
  while (gtk_notebook_get_n_pages (GTK_NOTEBOOK (manager)) != 0)
    gtk_notebook_remove_page (GTK_NOTEBOOK (manager), 0);
  
  manager->tab_count = 0;
}

// MArk the current tab as modified or unmodified
void file_manager_mark_current_tab_modified (FileManager *manager,
                                             gboolean modified,
                                             int tab)
{
  string markup;
  
  // If the file was modified
  if (modified)
  {
    markup = g_markup_printf_escaped ("<span weight=\"bold\">%s</span>", 
             gtk_label_get_text (GTK_LABEL (FILE_TAB (manager->tabs[tab])->label)));
    gtk_label_set_markup (GTK_LABEL (FILE_TAB (manager->tabs[tab])->label), markup.c_str());
    FILE_TAB (manager->tabs[tab])->modified = true;
  }
  // If it was not modified
  else 
  {
    markup = g_markup_printf_escaped ("<span style=\"normal\">%s</span>", 
             gtk_label_get_text (GTK_LABEL (FILE_TAB (manager->tabs[tab])->label)));
    gtk_label_set_markup (GTK_LABEL (FILE_TAB (manager->tabs[tab])->label), markup.c_str());
    FILE_TAB (manager->tabs[tab])->modified = false;
  }
}

// Show the cursor on-screen. This is used for things such as the goto line function.
void file_manager_scroll_to_mark (FileManager *manager)
{
  GtkTextBuffer *buffer = GTK_TEXT_BUFFER (file_manager_get_current_buffer (manager));
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_mark (buffer, &iter, gtk_text_buffer_get_insert (buffer));
  gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW (file_manager_get_current_view (manager)), 
                                &iter, 0.0, FALSE, 0.0, 0.0);
}

// Set a new font for each of the tabs
void file_manager_modify_fonts (FileManager *manager,
                                PangoFontDescription *fd)
{
  for (int i=0; i < manager->tab_count; i++)
    gtk_widget_modify_font (GTK_WIDGET (manager->tabs[i]), fd);
}

// Revert to the saved version of a file
void file_manager_revert_to_saved (FileManager *manager)
{
  int current = gtk_notebook_get_current_page (GTK_NOTEBOOK (manager));
  file_manager_mark_current_tab_modified (manager, FALSE, current);
  string text = openldev_read_file (FILE_TAB (manager->tabs[current])->filename);
  
  GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (manager->tabs[current]));
  gtk_text_buffer_set_text (buffer, text.c_str(), -1);
  gtk_text_view_set_buffer (GTK_TEXT_VIEW (manager->tabs[current]), 
                            GTK_TEXT_BUFFER(buffer));
}

// When an event happens in the notebook, make sure to grab focus.
void file_manager_on_notebook_event (GtkNotebook *widget,
                                     gint page,
                                     gpointer data)
{
  FileManager *ptr = (FileManager*) data;
  gint current_page = gtk_notebook_get_current_page (GTK_NOTEBOOK (widget));
  gtk_widget_grab_focus (ptr->tabs[current_page]);
}

// Delete the tab for which the specific close button corresponds
gboolean
file_manager_on_delete_tab (GtkWidget *widget,
                            gpointer data)
{
  FileManager *manager = (FileManager*) data;
  
  for (int i=0; i < manager->tab_count; i++)
    if (FILE_TAB (manager->tabs[i])->button == widget)
    {
      // Make sure the user wants to close an unsaved file
      if (FILE_TAB (manager->tabs[i])->modified)
        if (!openldev_question ("The file you are closing is not saved. Are you sure you want to close?"))
          return FALSE;

      // Remove the file and set the current page to the previous page
      vector<GtkWidget*>::iterator itr = manager->tabs.begin();
      gtk_notebook_remove_page (GTK_NOTEBOOK (manager), i);

      for (int count=0; count < i; count++)
        itr++;

      manager->tab_count--;
      manager->tabs.erase (itr);

	    if (manager->tab_count != 0)
        gtk_notebook_set_current_page (GTK_NOTEBOOK (manager), i-1);
      break;
    }
  
  return (manager->tab_count == 0);
}

// Set the tab width for when the file contains tabs instead of spaces. This
// function receives the font description, environment settings object and the
// current textview. It returns the tabarray from the textview
PangoTabArray* file_manager_get_tab_array (PangoFontDescription *fd, 
                                           EnvironmentSettings *env,
                                           GtkWidget *text_view)
{
  PangoTabArray *tab_array;
  PangoLayout *layout;
	int width, height, tab_size;
	gchar *tab;

  // Add spaces for the number of spaces per tab
  g_object_get (env, "tab-size", &tab_size, NULL);
	tab = g_strnfill (tab_size, ' ');

  // Create the pango layout & get the pixel width
  layout = gtk_widget_create_pango_layout (text_view, tab);
  pango_layout_set_font_description (layout, fd);
  pango_layout_get_pixel_size (layout, &width, &height);
 
  // Create the tab array from the width
  tab_array = pango_tab_array_new (1, TRUE);
  pango_tab_array_set_tab (tab_array, 0, PANGO_TAB_LEFT, width);

  return tab_array;
}
