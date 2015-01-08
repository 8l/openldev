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
 
#include "openldev-edit.h"

static void openldev_edit_update_textview (OpenLDev*);

/* Cut the currently selected text from the buffer. Then add the text to the
 * actionmanager of the current buffer and redo the syntax highlighting. */
void 
openldev_menu_edit_cut (OpenLDev *openldev)
{
  GtkWidget *widget;
  GtkTextBuffer *buffer;
  
  widget = gtk_window_get_focus (GTK_WINDOW (openldev));
  
  if (GTK_IS_TEXT_VIEW (widget))
  {
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
    gtk_text_buffer_cut_clipboard (GTK_TEXT_BUFFER (buffer), gtk_clipboard_get (GDK_NONE), TRUE);
    openldev_edit_update_textview (openldev);
  }
  else if (GTK_IS_EDITABLE (widget))
    gtk_editable_cut_clipboard (GTK_EDITABLE (widget));
}

/* Cut the currently selected text from the buffer. */
void 
openldev_menu_edit_copy (OpenLDev *openldev)
{
  GtkWidget *widget;
  GtkTextBuffer *buffer;
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;
  gchar *str, *text = "";
  gint i;
  
  widget = gtk_window_get_focus (GTK_WINDOW (openldev));
  
  if (GTK_IS_TEXT_VIEW (widget))
  {
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
    gtk_text_buffer_copy_clipboard (GTK_TEXT_BUFFER (buffer), 
                                    gtk_clipboard_get (GDK_NONE));
  }
  else if (GTK_IS_TREE_VIEW (widget))
  {
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));

    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {      
      for (i=0; i < gtk_tree_model_get_n_columns (model); i++)
        if (gtk_tree_model_get_column_type (model, i) == G_TYPE_STRING)
        {
          gtk_tree_model_get (model, &iter, i, &str, -1);
          text = g_strconcat (text, str, " ", NULL);
        }
      
      g_strstrip (text);      
      gtk_clipboard_set_text (gtk_clipboard_get (GDK_NONE), text, strlen (text));
      
      g_free (str);
      g_free (text);
    }
  }
  else if (GTK_IS_EDITABLE (widget))
    gtk_editable_copy_clipboard (GTK_EDITABLE (widget));
}

/* Paste the clipboard text to the current cursor position. Then add the text 
 * to the actionmanager of the current buffer and redo the syntax highlighting. */
void 
openldev_menu_edit_paste (OpenLDev *openldev)
{
  GtkWidget *widget;
  GtkTextBuffer *buffer;
  
  widget = gtk_window_get_focus (GTK_WINDOW (openldev));
  
  if (GTK_IS_TEXT_VIEW (widget))
  {
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
    gtk_text_buffer_paste_clipboard (GTK_TEXT_BUFFER (buffer), 
                                     gtk_clipboard_get (GDK_NONE), NULL, TRUE);
    openldev_edit_update_textview (openldev);
  }
  else if (GTK_IS_EDITABLE (widget))
    gtk_editable_paste_clipboard (GTK_EDITABLE (widget));
}

/* Select all text in the text_view. */
void 
openldev_menu_edit_select_all (OpenLDev *openldev)
{
  GtkTextIter start, end;
  GtkTextBuffer *buffer;
  FileManager *files;
  
  g_object_get (openldev, "file-browser", &files, NULL);
  if (file_manager_get_file_open (files))
  {
    buffer = GTK_TEXT_BUFFER (file_manager_get_current_buffer (files));
    gtk_text_buffer_get_bounds (buffer, &start, &end);
    gtk_text_buffer_select_range (buffer, &start, &end);
  }
}

/* Deselect any text that is selected. */
void 
openldev_menu_edit_deselect (OpenLDev *openldev)
{
  GtkSourceBuffer *buffer;
  GtkTextMark *mark;
  GtkTextIter iter;
  FileManager *files;
  
  g_object_get (openldev, "file-browser", &files, NULL);
  if (file_manager_get_file_open (files))
  {
    buffer = file_manager_get_current_buffer (files);
    mark = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER (buffer));
    
    gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER (buffer), &iter, mark);
    gtk_text_buffer_move_mark (GTK_TEXT_BUFFER (buffer),
                               gtk_text_buffer_get_selection_bound (GTK_TEXT_BUFFER (buffer)),
                               &iter);
  }
}

/* Undo the last action that was performed. If there was no action, then don't
 * do anything. To undo a delete, just insert the text. To undo an insert, the
 * text must be removed from the specified offset & then backwards the length. */
void 
openldev_menu_edit_undo (OpenLDev *openldev)
{
  GtkSourceBuffer *buffer;
  FileManager *files;
  
  g_object_get (openldev, "file-browser", &files, NULL);
  if (file_manager_get_file_open (files))
  {
    buffer = file_manager_get_current_buffer (files);
    if (gtk_source_buffer_can_undo (buffer))
      gtk_source_buffer_undo (buffer);
  }
}

/* Redo the last action that was undone. If there was no action, then don't
 * do anything. To redo an insert, you must remove the text from the specified
 * offset to backwards the length of the text. To redo a delete, just move
 * backward the length of the string & insert it. */
void 
openldev_menu_edit_redo (OpenLDev *openldev)
{
  GtkSourceBuffer *buffer;
  FileManager *files;
  
  g_object_get (openldev, "file-browser", &files, NULL);
  if (file_manager_get_file_open (files))
  {
    buffer = file_manager_get_current_buffer (files);
    if (gtk_source_buffer_can_redo (buffer))
      gtk_source_buffer_redo (buffer);
  }
}

/* Load the environment settings dialog and then store the changes that the user
 * makes and rewrite the environment settings file. Then update the fonts. */
void 
openldev_menu_edit_preferences (OpenLDev *openldev)
{
  OpenLDevPluginList *pluginlist;
  EnvironmentSettings *settings;
  
  g_object_get (openldev, "plugins", &pluginlist, "env-settings", &settings, NULL);
  openldev_env_dialog_new (pluginlist, settings);
}

void
openldev_menu_edit_unindent (OpenLDev *openldev)
{
  FileManager *files = FILE_MANAGER (openldev->files);
  EnvironmentSettings *env = openldev->env;
  GtkSourceBuffer *buffer;
  GtkTextIter start, end;
  gint offset;
  gint tab_size, i;
  
  if (file_manager_get_current_tab (files) == -1)
    return;
  
  g_object_get (env, "tab-size", &tab_size, NULL);
  offset = file_manager_get_insert_offset (files);
  buffer = file_manager_get_current_buffer (files);
  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER (buffer), &start, offset);
  gtk_text_iter_set_line_offset (&start, 0);
  end = start;
  
  if (gtk_text_iter_get_char (&start) == '\t')
    gtk_text_iter_forward_char (&end);
  else
  {
    for (i = 0; i < tab_size; i++)
    {
      if (gtk_text_iter_get_char (&end) == ' ')
        gtk_text_iter_forward_char (&end);
      else
        break;
    }
  }
  
  gtk_text_buffer_delete (GTK_TEXT_BUFFER (buffer), &start, &end);
}

void
openldev_menu_edit_indent (OpenLDev *openldev)
{
  FileManager *files = FILE_MANAGER (openldev->files);
  EnvironmentSettings *env = openldev->env;
  GtkSourceBuffer *buffer;
  GtkTextIter iter;
  gint offset, tab_size, i;
  gboolean replace_tabs;
  
  if (file_manager_get_current_tab (files) == -1)
    return;
  
  g_object_get (env, "replace-tabs", &replace_tabs, "tab-size", &tab_size, NULL);
  offset = file_manager_get_insert_offset (files);
  buffer = file_manager_get_current_buffer (files);
  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER (buffer), &iter, offset);
  gtk_text_iter_set_line_offset (&iter, 0);
  
  if (!replace_tabs)
    gtk_text_buffer_insert (GTK_TEXT_BUFFER (buffer), &iter, "\t", -1);
  else
    for (i = 0; i < tab_size; i++)
      gtk_text_buffer_insert (GTK_TEXT_BUFFER (buffer), &iter, " ", -1);
}

/* Update the TextBuffer that is sent to this function. It marks it modified
 * when text has been entered and shows the cursor on screen. */
static void
openldev_edit_update_textview (OpenLDev *openldev)
{
  gint current;
  FileManager *files;
  
  g_object_get (openldev, "file-browser", &files, NULL);
  current = gtk_notebook_get_current_page (GTK_NOTEBOOK (files));
  file_manager_mark_current_tab_modified (files, TRUE, current);
  file_manager_scroll_to_mark (files);
}
