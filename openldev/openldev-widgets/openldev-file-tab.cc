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

#include "openldev-file-tab.h"

static void file_tab_class_init (FileTabClass *klass);
static void file_tab_init (FileTab *tab);

GType
file_tab_get_type ()
{
  static GType tab_type = 0;

  if (!tab_type)
  {
    static const GTypeInfo tab_info =
    {
	    sizeof (FileTabClass),
	    NULL, NULL,
	    (GClassInitFunc) file_tab_class_init,
      NULL, NULL,
      sizeof (FileTab), 0,
	    (GInstanceInitFunc) file_tab_init,
    };

    tab_type = g_type_register_static (GTK_TYPE_SOURCE_VIEW, "FileTab", 
                                       &tab_info, (GTypeFlags) 0);
  }

  return tab_type;
}

static void
file_tab_class_init (FileTabClass *klass)
{
}

static void
file_tab_init (FileTab *tab)
{
  gchar *error, *warning, *info;

  tab->modified = FALSE;
  tab->swin = gtk_scrolled_window_new (NULL, NULL);
  tab->image = gtk_image_new_from_stock ("gtk-close", GTK_ICON_SIZE_MENU);
  tab->hbox = gtk_hbox_new (FALSE, 0);
  tab->button = gtk_button_new ();
  
  error = openldev_add_prefix ("/share/pixmaps/openldev/error.png");
  warning = openldev_add_prefix ("/share/pixmaps/openldev/warning.png");
  info = openldev_add_prefix ("/share/pixmaps/openldev/info.png");
  
  gtk_source_view_set_marker_pixbuf (GTK_SOURCE_VIEW (tab), "error", 
                                     gdk_pixbuf_new_from_file (error, NULL));
  gtk_source_view_set_marker_pixbuf (GTK_SOURCE_VIEW (tab), "warning", 
                                     gdk_pixbuf_new_from_file (warning, NULL));
  gtk_source_view_set_marker_pixbuf (GTK_SOURCE_VIEW (tab), "info", 
                                     gdk_pixbuf_new_from_file (info, NULL));
  gtk_source_view_set_show_line_markers (GTK_SOURCE_VIEW (tab), TRUE);
  
  g_free (error);
  g_free (warning);
  g_free (info);
}

GtkWidget*
file_tab_new (const char *file)
{
  FileTab *tab = FILE_TAB (g_object_new (file_tab_get_type (), NULL));
  string fn = file;
  int location = fn.find ('/', 0);
  
  while (location != (signed int) string::npos) 
  {
    fn.erase (0,location+1);
    location = fn.find ('/', 0);
  }
  
  string text = openldev_read_file ((gchar*) file);
  tab->buffer = gtk_source_buffer_new (NULL);
  tab->filename = g_strdup (file);
  tab->label = gtk_label_new (fn.c_str());
  
  // Make sure the initial population of the view is not added to undo
  gtk_source_buffer_begin_not_undoable_action (tab->buffer);
  gtk_text_buffer_set_text (GTK_TEXT_BUFFER (tab->buffer), text.c_str(), -1);
  gtk_source_buffer_end_not_undoable_action (tab->buffer);
  
  gtk_text_view_set_buffer (GTK_TEXT_VIEW (tab), GTK_TEXT_BUFFER (tab->buffer));
  
  // Create the textview & add it to the scrolled window
  gtk_text_view_set_editable (GTK_TEXT_VIEW (tab), TRUE);
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (tab), 2);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (tab->swin), GTK_SHADOW_IN);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (tab->swin), GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (tab->swin), GTK_WIDGET (tab));

  // Create the button & add the close image to it
  gtk_widget_set_size_request (tab->button, 18, 18);
  gtk_button_set_relief (GTK_BUTTON (tab->button), GTK_RELIEF_NONE);
  gtk_container_add (GTK_CONTAINER (tab->button), tab->image);
	
  // Add the label and button the the tab hbox
  gtk_box_pack_start (GTK_BOX (tab->hbox), tab->label, FALSE, FALSE, 6);
  gtk_box_pack_start (GTK_BOX (tab->hbox), tab->button, FALSE, FALSE, 0);
  
  // Show all of the objects
  gtk_widget_show (GTK_WIDGET (tab));
  gtk_widget_show (tab->swin);
  gtk_widget_show (tab->label);
  gtk_widget_show (tab->image);
  gtk_widget_show (tab->hbox);
  gtk_widget_show (tab->button);

  return GTK_WIDGET (tab);
}

string file_tab_get_filename_short (FileTab* tab)
{ return gtk_label_get_text (GTK_LABEL(tab->label));
}

void file_tab_set_label (FileTab* tab,
                         const char* name)
{ gtk_label_set_text (GTK_LABEL(tab->label), name);
}
