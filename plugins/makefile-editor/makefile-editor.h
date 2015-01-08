/***************************************************************************
 *   Copyright (C) 2005-2006 by Andrew Krause                              *
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
 
#ifndef MAKEFILE_EDITOR_H
#define MAKEFILE_EDITOR_H

#include "openldev/openldev-settings/openldev-makefile-parser.h"
#include "openldev/openldev-settings/openldev-project.h"
#include "openldev/openldev-dialogs/openldev-input.h"
#include "openldev/openldev-plugin.h"
#include "openldev/openldev.h"

#include <gtk/gtk.h>
#include <glade/glade.h>

enum 
{
  MAKEFILE_EDITOR_BASE,
  MAKEFILE_EDITOR_MOVE_UP,
  MAKEFILE_EDITOR_SUBPROJECT,
  MAKEFILE_EDITOR_PREFIX,
  MAKEFILE_EDITOR_TARGET,
  MAKEFILE_EDITOR_SCRIPT,
  MAKEFILE_EDITOR_VARIABLE,
  MAKEFILE_EDITOR_FILE
};

typedef struct _MakefileEditor  MakefileEditor;

struct _MakefileEditor
{  
  GtkWidget *vbox;
  GtkWidget *scrolledwindow;
  GtkWidget *content;
  
  GtkToolItem *button;
  GtkWidget *toolbar;
  GtkWidget *menu;
  GtkWidget *add;
  GtkWidget *edit;
  GtkWidget *remove;
  
  ProjectSettings *project;
  string fullname;
  string current_dir;
  string current_path;
};

MakefileEditor* makefile_editor_init (OpenLDev*);
void makefile_editor_populate (MakefileEditor*);

void makefile_editor_add_row (GtkTreeModel*, GtkTreeIter&, GtkTreeIter&,
                              GdkPixbuf*, const char*, int);

gboolean makefile_editor_button_press (GtkWidget*, GdkEventButton*);
void makefile_editor_add (GtkMenuItem*, gpointer);
void makefile_editor_remove (GtkMenuItem*, gpointer);

void makefile_editor_edit (MakefileEditor*);

#endif
