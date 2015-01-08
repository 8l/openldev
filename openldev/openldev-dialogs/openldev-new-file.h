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

#ifndef NEW_FILE_DIALOG_H
#define NEW_FILE_DIALOG_H

#include "../openldev-utils.h"

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>

struct NewFileDialog
{
  GladeXML *xml;

  GtkWidget *dialog;
  GtkWidget *cancel;
  GtkWidget *apply;
  GtkWidget *name;
  GtkWidget *filechooser;
  
  gchar *file_location;
};

NewFileDialog* openldev_new_file_dialog_new (gchar*);
void openldev_new_file_dialog_run (NewFileDialog*, gchar*&);

#endif
