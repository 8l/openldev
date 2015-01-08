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

#ifndef STATUS_BAR_H
#define STATUS_BAR_H

/*!
  \file openldev-status-bar.h
  \brief StatusBar
*/

#include <iostream>
#include <vector>
#include <gtk/gtk.h>
#include <libgnomeui/libgnomeui.h>
using namespace std;

enum
{
  STATUSBAR_LEFT,
  STATUSBAR_RIGHT
};

/*! \brief A GtkStatisBar widget to show messages to the user. */
struct StatusBar
{
  GtkWidget *bar;
  GtkWidget *status1;
  GtkWidget *status2;
  gboolean locked1;
  gboolean locked2;
  guint context1;
  guint context2;
};

/*! Create a new StatusBar object. This is useful if you want a generic GtkStatisBar widget somewhere else in the application.
  \return A new StatusBar object.
*/
StatusBar* openldev_status_bar_new ();

/*! Set the current message on the status bar. This is not locked, so it can be changed by other parts of the application.
  \param bar A StatusBar object.
  \param message The message to show on the status bar.
*/
void openldev_status_bar_set_message (StatusBar *bar, gint bar, gchar *message);

/*! Set the current message on the status bar. This message is locked, so other parts of the application cannot edit it. Make sure to unlock the message when you are done! This will append a message on the stack, so when you unlock the message, it will revert to the prior.
  \param bar A StatusBar object.
  \param message The message to show on the status bar.
*/
void openldev_status_bar_lock_message (StatusBar *bar, gint bar, gchar *message);

/*! Unlock the status bar message and revert to the prior message. Make sure you do this when locking is no longer necessary!
  \param bar A StatusBar object.
*/
void openldev_status_bar_unlock_message (StatusBar *bar, gint bar);

#endif
