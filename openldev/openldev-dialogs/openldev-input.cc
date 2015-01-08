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

#include "openldev-input.h"

/* Run a dialog that asks the user for some sort of input. This is a generic
 * dialog that can be used by any part of the application. It receives the label
 * for what the user should enter and the title of the dialog. It returns the
 * text the user has entered. */
gchar* 
openldev_input_dialog_run (gchar* label_text, 
                           gchar* title,
                           gchar* content)
{
  InputDialog *input = g_slice_new (InputDialog);
  gchar *text;

  input->dialog = gtk_dialog_new ();
  input->cancel = gtk_button_new_from_stock ("gtk-cancel");
  input->ok = gtk_button_new_from_stock ("gtk-ok");
  input->label = gtk_label_new (label_text);
  input->entry = gtk_entry_new ();
  input->hbox = gtk_hbox_new (FALSE, 0);
  
  gtk_entry_set_text (GTK_ENTRY (input->entry), content);
  GTK_WIDGET_SET_FLAGS (input->ok, GTK_CAN_DEFAULT);
  gtk_container_set_border_width (GTK_CONTAINER (input->hbox), 10);
  gtk_box_set_spacing (GTK_BOX (input->hbox), 10);
  gtk_box_pack_start (GTK_BOX (input->hbox), input->label, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (input->hbox), input->entry, FALSE, TRUE, 0);

  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (input->dialog)->vbox), input->hbox, TRUE, TRUE, 0);
  gtk_window_set_title (GTK_WINDOW (input->dialog), title);
  gtk_window_set_modal (GTK_WINDOW (input->dialog), FALSE);
  gtk_window_set_resizable (GTK_WINDOW (input->dialog), TRUE);
  gtk_dialog_set_has_separator (GTK_DIALOG (input->dialog), TRUE);
  gtk_dialog_add_action_widget (GTK_DIALOG (input->dialog), input->cancel, -6);
  gtk_dialog_add_action_widget (GTK_DIALOG (input->dialog), input->ok, -5);
  
  gtk_widget_show_all (input->dialog);

  if (gtk_dialog_run (GTK_DIALOG (input->dialog)) == -5)
  {
    text = g_strdup ((gchar*) gtk_entry_get_text (GTK_ENTRY (input->entry)));
    gtk_widget_destroy (input->dialog);
    g_slice_free1 (sizeof (InputDialog), input);
    return text;
  }
  else
  {
    gtk_widget_destroy (input->dialog);
    g_slice_free1 (sizeof (InputDialog), input);
    return "EMPTY";
  }
}
