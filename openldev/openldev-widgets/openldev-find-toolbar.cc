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

#include "openldev-find-toolbar.h"

// Populate the find toolbar
FindToolbar* 
openldev_find_toolbar_new ()
{
  FindToolbar *toolbar = new FindToolbar ();
  
  // Create Horizontal Find 'Toolbar'
  toolbar->hbox = gtk_hbox_new (FALSE, 0);
  toolbar->label_find = gtk_label_new ("Search for: ");
  toolbar->label_error = gtk_label_new ("Text not found!");
  toolbar->find_text = gtk_combo_box_entry_new_text ();
  toolbar->image_close = gtk_image_new_from_stock ("gtk-stop", GTK_ICON_SIZE_MENU);
  toolbar->image_error = gtk_image_new_from_stock ("gtk-dialog-error", GTK_ICON_SIZE_MENU);
  toolbar->button_find = gtk_button_new_from_stock ("gtk-find");
  toolbar->button_close = gtk_button_new ();
  toolbar->search_files = gtk_check_button_new_with_label ("Search in all files");
  
  // Create the buttons
  gtk_button_set_relief (GTK_BUTTON (toolbar->button_close), GTK_RELIEF_NONE);
  gtk_container_add (GTK_CONTAINER (toolbar->button_close), toolbar->image_close);
  gtk_button_set_relief (GTK_BUTTON (toolbar->button_find), GTK_RELIEF_NONE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (toolbar->search_files), FALSE);
  
  // Add the widgets to the horizontal box
  gtk_box_set_spacing (GTK_BOX (toolbar->hbox), 5);
  gtk_container_set_border_width (GTK_CONTAINER (toolbar->hbox), 3);
  gtk_box_pack_start (GTK_BOX (toolbar->hbox), toolbar->button_close, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (toolbar->hbox), toolbar->label_find, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (toolbar->hbox), toolbar->find_text, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (toolbar->hbox), toolbar->button_find, FALSE, FALSE, 3);
  gtk_box_pack_start (GTK_BOX (toolbar->hbox), toolbar->search_files, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (toolbar->hbox), toolbar->image_error, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (toolbar->hbox), toolbar->label_error, FALSE, FALSE, 0);
  
  return toolbar;
}

void 
openldev_find_toolbar_add_choice (FindToolbar *toolbar, 
                                  string choice)
{
  unsigned int i;
  for (i=0; i < toolbar->choices.size(); i++)
    if (toolbar->choices[i] == choice)
      return;

  toolbar->choices.push_back (choice);
  if (toolbar->choices.size() > 10)
  {
    vector<string>::iterator iter = toolbar->choices.begin();
    toolbar->choices.erase (iter);
  }
  
  gtk_list_store_clear (GTK_LIST_STORE (gtk_combo_box_get_model 
                        (GTK_COMBO_BOX (toolbar->find_text))));
  for (i=0; i < toolbar->choices.size(); i++)
    gtk_combo_box_append_text (GTK_COMBO_BOX (toolbar->find_text), 
                               toolbar->choices[i].c_str());
}

// Show or hide the error message
void 
openldev_find_toolbar_show_error (FindToolbar *toolbar,
                                  gboolean show)
{
  if (show)
  {
    gtk_widget_show (toolbar->label_error);
    gtk_widget_show (toolbar->image_error);
  }
  else
  {
    gtk_widget_hide (toolbar->label_error);
    gtk_widget_hide (toolbar->image_error);
  }
}

gboolean 
openldev_find_toolbar_search_files_is_active (FindToolbar *toolbar)
{ 
  return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(toolbar->search_files));
}
