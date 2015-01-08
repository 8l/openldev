/***************************************************************************
 *   Copyright (C) 2006 by Andrew Krause                                   *
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
 
#ifndef INSERT_INFO_H
#define INSERT_INFO_H

#include "openldev/openldev-plugin.h"
#include "openldev/openldev.h"
#include <glade/glade.h>

struct InsertInfoWidgets
{
  GtkWidget *menu_user;
  GtkWidget *item_user;
  GtkWidget *item_username;
  GtkWidget *item_realname;
  GtkWidget *item_homedir;
  GtkWidget *item_gpl;
  GtkWidget *item_date;
  GtkWidget *item_sep;
};

InsertInfoWidgets *insert_info;

#endif
