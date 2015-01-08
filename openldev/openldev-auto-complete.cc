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

#include "openldev-auto-complete.h"
#include <cstring>

static void autocomplete_hide (OpenLDev*, GtkTextBuffer*);
static void autocomplete_show (OpenLDev*, GtkTextBuffer*, GdkEventKey*);
static void autocomplete_populate (OpenLDev*, GtkTextBuffer*, gchar*, gchar*);
static gboolean openldev_handle_autocomplete (OpenLDev*, GtkTextBuffer*, GdkEventKey*);
static gboolean autocomplete_insert_word (OpenLDev*, GtkTextBuffer*);
static gboolean autocomplete_check_key (GdkEventKey*);

/* Handle keypresses in the GtkTextView. This function makes sure the key press
 * is acceptable to go forward. */
gboolean 
openldev_on_textview_keypress (GtkWidget *widget,
                               GdkEventKey *event,
                               OpenLDev *openldev)
{
  GtkTextBuffer *buffer;
  
  buffer = GTK_TEXT_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget)));
  
  if ((event->state & GDK_CONTROL_MASK) &&  !(event->state & GDK_MOD1_MASK))
    autocomplete_hide (openldev, buffer);
  else if (event->keyval >= 0xFFE1 && event->keyval <= 0xFFEE)
    return FALSE;
  else if (event->type == GDK_KEY_PRESS)
  {
    if (openldev_handle_autocomplete (openldev, buffer, event))
      return TRUE;
  }
  else
    autocomplete_hide (openldev, buffer);
  
  return FALSE;
}

/* Autocomplete has been called when there is a key press event. Now we must find
 * out if we should try and find a match or not. */
static gboolean 
openldev_handle_autocomplete (OpenLDev *openldev,
                              GtkTextBuffer *buffer,
                              GdkEventKey *event)
{
  GtkTextIter insert, select;
  GtkWidget *files;
  gint current;
  
  g_object_get (openldev, "file-browser", &files, NULL);
  current = gtk_notebook_get_current_page (GTK_NOTEBOOK (files));

  if (event->length > 0 || event->keyval == GDK_Delete || 
      event->keyval == GDK_KP_Delete || event->keyval == GDK_BackSpace)
    file_manager_mark_current_tab_modified (FILE_MANAGER (files), TRUE, current);

  if (autocomplete_check_key (event))
  {
    gtk_text_buffer_get_selection_bounds (buffer, &insert, &select);
    gtk_text_buffer_delete (buffer, &insert, &select);
    gtk_text_buffer_get_iter_at_mark (buffer, &insert, gtk_text_buffer_get_insert (buffer));
    
    if (gtk_text_iter_get_offset (&insert) >= 3)
      autocomplete_show (openldev, buffer, event);
    else
      return FALSE;

    return TRUE;
  }
  else if (event->keyval == GDK_KP_Enter || event->keyval == GDK_Return ||
           event->keyval == GDK_End || event->keyval == GDK_KP_End ||
           event->keyval == GDK_Right || event->keyval == GDK_KP_Right)
    return autocomplete_insert_word (openldev, buffer);
  else
    autocomplete_hide (openldev, buffer);
  
  return FALSE;
}

/* Make sure the character is acceptable for a word. */
gboolean 
openldev_auto_complete_check_char (gchar ch)
{
  if (isalnum(ch))
    return TRUE;
  else if (ch != '_')
    return FALSE;

  return TRUE;
}

/* Insert the next character in the word or find a match. */
static void
autocomplete_show (OpenLDev *openldev,
                   GtkTextBuffer *buffer,
                   GdkEventKey *event)
{
  GtkTextIter begin, end, match_start, match_end, start;
  gint length = 0, offset, len;
  EditorProperties *editor;
  gchar *slice, *full_word;
  
  g_object_get (openldev, "editor-prop", &editor, NULL);
  gtk_text_buffer_get_iter_at_mark (buffer, &begin, gtk_text_buffer_get_insert (buffer));
  
  if (editor->current_autocomplete != NULL)
  {
    if (strlen (editor->current_autocomplete) > 0 && editor->autocomplete_active)
    {
      if (editor->current_autocomplete[0] == event->string[0])
      {
        gtk_text_buffer_get_iter_at_mark (buffer, &begin, gtk_text_buffer_get_insert (buffer));
        gtk_text_buffer_get_iter_at_mark (buffer, &end, gtk_text_buffer_get_selection_bound (buffer));
        gtk_text_buffer_delete (buffer, &begin, &end);
        
        offset = gtk_text_iter_get_offset (&begin), 
        len = strlen (editor->current_autocomplete) - 1;
        gtk_text_buffer_insert_at_cursor (buffer, editor->current_autocomplete,
                                          strlen (editor->current_autocomplete));
        offset++;
        
        gtk_text_buffer_get_iter_at_offset (buffer, &begin, offset);
        gtk_text_buffer_get_iter_at_offset (buffer, &end, offset + len);
        gtk_text_buffer_select_range (buffer, &begin, &end);

        editor->current_autocomplete++;
        return;
      }
    }
  }

  gtk_text_buffer_insert_at_cursor (buffer, event->string, 1);
  gtk_text_buffer_get_iter_at_mark (buffer, &begin, gtk_text_buffer_get_insert (buffer));
  end = begin;
  
  /* Get the text that the user has typed back to the beginning of one word. */
  gtk_text_iter_backward_char (&begin);
  while (openldev_auto_complete_check_char (gtk_text_iter_get_char (&begin)))
  {
    gtk_text_iter_backward_char (&begin);
    length++;
    
    if (gtk_text_iter_is_start (&begin))
    {
      if (!openldev_auto_complete_check_char (gtk_text_iter_get_char (&begin)))
        gtk_text_iter_forward_char (&begin);
      break;
    }
  }
  
  if (!gtk_text_iter_is_start (&begin))
    gtk_text_iter_forward_char (&begin);
  slice = gtk_text_buffer_get_slice (buffer, &begin, &end, FALSE);

  /* Get the match & show it if the 1st letter isn't a digit and the slice
   * is atleast 3 characters. */
  if ((strlen (slice) > 2) && !g_ascii_isdigit (slice[0]))
  {
    gtk_text_buffer_get_start_iter (buffer, &start);
    gtk_text_buffer_get_end_iter (buffer, &end);
    
    while (gtk_text_iter_forward_search (&start, slice, 
           GTK_TEXT_SEARCH_TEXT_ONLY, &match_start, &match_end, NULL))
    {
      if (gtk_text_iter_compare (&match_start, &end) == 0)
      {
        autocomplete_hide (openldev, buffer);
        break;
      }
      else if ((gtk_text_iter_compare (&match_start, &begin) == 0) ||
               (!gtk_text_iter_starts_word (&match_start)))
      {
        start = match_start;
        gtk_text_iter_forward_char (&start);
      }
      else
      {
        while (openldev_auto_complete_check_char (gtk_text_iter_get_char (&match_end)))
          gtk_text_iter_forward_char (&match_end);
        full_word = gtk_text_iter_get_slice (&match_start, &match_end);
        
        if (g_ascii_strcasecmp (full_word, slice) != 0)
          autocomplete_populate (openldev, buffer, full_word, slice);
        
        if (full_word != NULL)
          g_free (full_word);
        g_free (slice);
        break;
      }
    }
  }
  else
    autocomplete_hide (openldev, buffer);
}

// Show the match as a selection
static void 
autocomplete_populate (OpenLDev *openldev,
                       GtkTextBuffer *buffer,
                       gchar *word,
                       gchar *slice)
{
  GtkTextIter insert, select;
  EditorProperties *editor;
  gint offset, i;
  
  g_object_get (openldev, "editor-prop", &editor, NULL);
  gtk_text_buffer_get_iter_at_mark (buffer, &insert, gtk_text_buffer_get_insert (buffer));
  offset = gtk_text_iter_get_offset (&insert);
  for (i = 0; i < strlen (slice); i++)
    ++word;
  editor->current_autocomplete = g_strdup (word);
  
  gtk_text_buffer_insert_at_cursor (buffer, word, strlen (word));
  gtk_text_buffer_get_iter_at_offset (buffer, &insert, offset);
  gtk_text_buffer_get_iter_at_offset (buffer, &select, offset + strlen (word));
  
  gtk_text_buffer_select_range (buffer, &insert, &select);
  openldev_editor_set_autocomplete_active (editor, TRUE);
}

// Insert the rest of the word that is currently a selection
static gboolean 
autocomplete_insert_word (OpenLDev *openldev,
                          GtkTextBuffer *buffer)
{
  EditorProperties *editor;
  GtkTextIter insert, select;
  
  gtk_text_buffer_get_selection_bounds (buffer, &insert, &select);
  g_object_get (openldev, "editor-prop", &editor, NULL);

  // Make sure the iterators are inthe correct order
  if (gtk_text_iter_get_offset (&insert) < gtk_text_iter_get_offset (&select))
  {
    gtk_text_buffer_move_mark_by_name (buffer, "insert", &select);
    openldev_editor_set_autocomplete_active (editor, FALSE);
    return TRUE;
  }
  
  return FALSE;
}

// We don't want to see this window anymore
static void 
autocomplete_hide (OpenLDev *openldev,
                   GtkTextBuffer *buffer)
{
  GtkTextIter insert, select;
  EditorProperties *editor;
  
  g_object_get (openldev, "editor-prop", &editor, NULL);
  if (editor->autocomplete_active)
  {
    gtk_text_buffer_get_selection_bounds (buffer, &insert, &select);
    gtk_text_buffer_delete (buffer, &insert, &select);
    openldev_editor_set_autocomplete_active (editor, FALSE);
  }
}

// This is the same thing as the character function. We need to make sure that
// the inserted character will be included in a word.
static gboolean 
autocomplete_check_key (GdkEventKey *event)
{
  guint key = event->keyval;
  
  if (key == GDK_underscore)
    return TRUE;
  else if (key >= GDK_0 && key <= GDK_9)
    return TRUE;
  else if (key >= GDK_A && key <= GDK_Z)
    return TRUE;
  else if (key >= GDK_a && key <= GDK_z)
    return TRUE;
  else
    return FALSE;
}

// Catch other random events from the GtkTextView and delete the autocomplete
// match when they happen. This will also update the status bar.
gboolean 
openldev_on_textview_event (GtkWidget *textview, 
                            GdkEvent *event,
                            OpenLDev *openldev)
{
  StatusBar *statusbar = openldev->statusbar;
  ProjectSettings *project = openldev->project;
  gchar vertical[5], horizontal[5], *message = "";
  GtkTextIter insert;
  GtkTextBuffer *buffer;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  if (event->type == GDK_BUTTON_PRESS || event->type == GDK_FOCUS_CHANGE)
    autocomplete_hide (openldev, buffer);
  
  if (openldev_project_is_open (project))
    message = g_strconcat (message, "Project: ", project->name, "     ",
                           "Version: ", project->version, "       ", NULL);
  
  gtk_text_buffer_get_iter_at_mark (buffer, &insert, gtk_text_buffer_get_insert (buffer));
  g_snprintf (vertical, sizeof (vertical), "%d", gtk_text_iter_get_line (&insert) + 1);
  g_snprintf (horizontal, sizeof (horizontal), "%d", gtk_text_iter_get_line_offset (&insert) + 1);
  message = g_strconcat (message, "Line ", vertical, ", ", "Column ", horizontal, NULL);
  
  openldev_status_bar_set_message (statusbar, STATUSBAR_RIGHT, message);
  
  return FALSE;
}
