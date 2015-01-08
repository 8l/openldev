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

#ifndef PROJECT_SETTINGS_DIALOG_H
#define PROJECT_SETTINGS_DIALOG_H

#include "../openldev-utils.h"
#include "../openldev-settings/openldev-project.h"

#include <gtk/gtk.h>
#include <glade/glade.h>

struct ProjectDialog
{
  GladeXML *xml;

  GtkWidget *dialog;
  GtkWidget *files;
  GtkWidget *filecontent;
  GtkWidget *version;
  GtkWidget *configure;
  GtkWidget *clean;
  GtkWidget *compile;
  GtkWidget *build;
  GtkWidget *execute;
  
  ProjectSettings *settings;
};

ProjectDialog* openldev_project_dialog_new (ProjectSettings*);
int            openldev_project_dialog_run (ProjectDialog*);

#endif
