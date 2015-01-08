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

#ifndef OUTPUT_TERMINAL_H
#define OUTPUT_TERMINAL_H

/*!
  \file openldev-output-terminal.h
  \brief OutputTerminal
*/

#include "../openldev-settings/openldev-settings.h"
#include <vte/vte.h>
#include <gtk/gtk.h>
#include <cstring>
#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define TERMINAL_TYPE            (terminal_get_type ())
#define TERMINAL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TERMINAL_TYPE, Terminal))
#define TERMINAL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TERMINAL_TYPE, TerminalClass))
#define IS_TERMINAL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TERMINAL_TYPE))
#define IS_TERMINAL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TERMINAL_TYPE))

/*! \brief Widgets for the VTE terminal tab. */
struct Terminal
{
  VteTerminal parent;

  /*! GtkHBox that holds the terminal and its scrollbar. */
  GtkWidget *hbox;
  /*! GtkScrollBar that will help scroll the terminal widget. */
  GtkWidget *scrollbar;
};

struct TerminalClass
{
  VteTerminalClass parent_class;
};

GType terminal_get_type ();

/*! Create a new Terminal object. This is useful if you want a generic VTE terminal widget somewhere else in the application.
  \param env An EnvironmentSettings object with terminal preferences.
  \return A new Terminal object.
*/
GtkWidget* terminal_new (EnvironmentSettings *env);

/*! Run an arbitrary command in the terminal. Use this if the command you want to run needs user interaction. Make sure to place focus on the terminal tab before you run a command.
  \param term A Terminal object.
  \param command The command to run in the terminal.
*/
void terminal_run_command (Terminal *term, const char *command);

/*! Let the terminal grab focus from the rest of the application. You should do this after running a command.
  \param term An Terminal object.
*/
void terminal_grab_focus (Terminal *term);

/*! Get the GtkHBox widget in the Terminal object.
  \param term A Terminal object.
  \return The Terminal's GtkHBox.
*/
GtkWidget* terminal_get_hbox (Terminal *term);

G_END_DECLS

#endif
