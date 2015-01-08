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

#include "openldev-status-bar.h"

// Create a new gnome application bar
StatusBar* 
openldev_status_bar_new ()
{
  StatusBar *obj = new StatusBar();
  
  obj->status1 = gtk_statusbar_new ();
  obj->status2 = gtk_statusbar_new ();
  obj->context1 = gtk_statusbar_get_context_id (GTK_STATUSBAR (obj->status1), "OpenLDev");
  obj->context2 = gtk_statusbar_get_context_id (GTK_STATUSBAR (obj->status2), "OpenLDev");
  obj->locked1 = FALSE;
  obj->locked2 = FALSE;
  
  obj->bar = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (obj->bar), obj->status1, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (obj->bar), obj->status2, TRUE, TRUE, 0);
  gtk_widget_set_size_request (obj->status1, 200, -1);
  gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (obj->status1), FALSE);
  
  return obj;
}

// Set the status bar to have a new message
void 
openldev_status_bar_set_message (StatusBar *statusbar, 
                                 gint bar,
                                 gchar *message)
{
  if (bar == STATUSBAR_LEFT)
  {
    if (!statusbar->locked1)
    {
      gtk_statusbar_pop (GTK_STATUSBAR (statusbar->status1), statusbar->context1);
      gtk_statusbar_push (GTK_STATUSBAR (statusbar->status1), statusbar->context1, message);
    }
  }
  else
  {
    if (!statusbar->locked2)
    {
      gtk_statusbar_pop (GTK_STATUSBAR (statusbar->status2), statusbar->context2);
      gtk_statusbar_push (GTK_STATUSBAR (statusbar->status2), statusbar->context2, message);
    }
  }
}

void 
openldev_status_bar_lock_message (StatusBar *statusbar,
                                  gint bar,
                                  gchar *message)
{
  if (bar == STATUSBAR_LEFT)
  {
    if (!statusbar->locked1)
    {
      gtk_statusbar_push (GTK_STATUSBAR (statusbar->status1), statusbar->context1, message);
      statusbar->locked1 = TRUE;
    }
  }
  else
  {
    if (!statusbar->locked2)
    {
      gtk_statusbar_push (GTK_STATUSBAR (statusbar->status2), statusbar->context2, message);
      statusbar->locked2 = TRUE;
    }
  }
}

void 
openldev_status_bar_unlock_message (StatusBar *statusbar,
                                    gint bar)
{
  if (bar == STATUSBAR_LEFT)
  {
    gtk_statusbar_pop (GTK_STATUSBAR (statusbar->status1), statusbar->context1);
    statusbar->locked1 = FALSE;
  }
  else
  {
    gtk_statusbar_pop (GTK_STATUSBAR (statusbar->status2), statusbar->context2);
    statusbar->locked2 = FALSE;
  }
}
