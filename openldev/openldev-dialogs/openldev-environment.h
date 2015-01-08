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

#ifndef ENVIRONMENT_DIALOG_H
#define ENVIRONMENT_DIALOG_H

#include "../openldev-utils.h"
#include "../openldev-plugin.h"
#include "../openldev-settings/openldev-settings.h"

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <cstdlib>

struct EnvironmentDialog
{
  GladeXML *xml;
  OpenLDevPluginList *list;
  EnvironmentSettings *settings;

  GtkWidget *dialog;
  GtkWidget *name;
  GtkWidget *author;
  GtkWidget *description;
  GtkWidget *plugins;
  GtkWidget *close;
  
  GtkWidget *autosave;
  GtkWidget *autoindent;
  GtkWidget *bracket;
  GtkWidget *error;
  GtkWidget *fontbutton;
  GtkWidget *load_on_launch;
  GtkWidget *replace_tabs;
  GtkWidget *right_margin;
  GtkWidget *split_words;
  GtkWidget *tab_size;
  GtkWidget *undo_actions;
  GtkWidget *warning;
  GtkWidget *wrap;
  GtkWidget *fontbutton2;
  GtkWidget *terminal_font;
  GtkWidget *terminal_fg;
  GtkWidget *terminal_bg;
  GtkWidget *shell;
  GtkWidget *scrollback;
  GtkWidget *scroll_keystroke;
  GtkWidget *scroll_output;
};

void openldev_env_dialog_new (OpenLDevPluginList*, EnvironmentSettings*);

#endif
