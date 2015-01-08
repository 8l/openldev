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

#include "openldev-goto-line-toolbar.h"

void 
openldev_goto_line_number (GtkSpinButton *spin,
                           OpenLDev *openldev)
{
  FileManager *files;
  GtkTextBuffer *buffer;
  GtkTextIter iter;
  gint line;

  g_object_get (openldev, "file-browser", &files, NULL);
  
  buffer = GTK_TEXT_BUFFER (file_manager_get_current_buffer (files));
  line = gtk_spin_button_get_value_as_int (spin) - 1;
  gtk_text_buffer_get_iter_at_line (buffer, &iter, line);
      
  gtk_text_buffer_select_range (buffer, &iter, &iter);
  file_manager_scroll_to_mark (files);
}


// Populate the goto line toolbar
GotoLineToolbar* 
openldev_goto_toolbar_new (OpenLDev *openldev)
{
  GotoLineToolbar *toolbar = g_slice_new (GotoLineToolbar);
  
  toolbar->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_accept_focus (GTK_WINDOW (toolbar->window), TRUE);
  gtk_window_set_focus_on_map (GTK_WINDOW (toolbar->window), TRUE);  
  gtk_window_set_transient_for (GTK_WINDOW (toolbar->window), GTK_WINDOW (openldev));
  gtk_window_set_decorated (GTK_WINDOW (toolbar->window), FALSE);
  gtk_window_set_skip_taskbar_hint (GTK_WINDOW (toolbar->window), TRUE);
  gtk_window_set_skip_pager_hint (GTK_WINDOW (toolbar->window), TRUE);
  
  toolbar->spin = gtk_spin_button_new_with_range (1.0, 1.0, 1.0);
  gtk_spin_button_set_digits (GTK_SPIN_BUTTON (toolbar->spin), 0);
  gtk_widget_set_size_request (toolbar->spin, 150, -1);
  gtk_container_add (GTK_CONTAINER (toolbar->window), toolbar->spin);
  
  GTK_WIDGET_SET_FLAGS (toolbar->window, GTK_CAN_FOCUS);
  GTK_WIDGET_SET_FLAGS (toolbar->spin, GTK_CAN_FOCUS);
  
  g_signal_connect (G_OBJECT (toolbar->window), "focus-out-event",
                    G_CALLBACK (gtk_widget_hide), NULL);
  g_signal_connect (G_OBJECT (toolbar->spin), "value-changed",
                    G_CALLBACK (openldev_goto_line_number), (gpointer) openldev);
  
  return toolbar;
}
