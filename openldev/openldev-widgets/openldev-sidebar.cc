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

#include "openldev-sidebar.h"

static void sidebar_class_init (SideBarClass *klass);
static void sidebar_init (SideBar *sidebar);

GType
sidebar_get_type ()
{
  static GType sidebar_type = 0;

  if (!sidebar_type)
  {
    static const GTypeInfo sidebar_info =
    {
	    sizeof (SideBarClass),
	    NULL, NULL,
	    (GClassInitFunc) sidebar_class_init,
      NULL, NULL,
      sizeof (SideBar), 0,
	    (GInstanceInitFunc) sidebar_init,
    };

    sidebar_type = g_type_register_static (GTK_TYPE_NOTEBOOK, "SideBar", 
                                           &sidebar_info, (GTypeFlags) 0);
  }

  return sidebar_type;
}

static void
sidebar_class_init (SideBarClass *klass)
{
}

static void
sidebar_init (SideBar *sidebar)
{
  GtkWidget *widget = GTK_WIDGET (sidebar);

  sidebar->filebrowser = file_browser_new ();
  sidebar_add_tab (sidebar, FILE_BROWSER (sidebar->filebrowser)->swin, 
                   gtk_label_new ("Files"));
}

GtkWidget*
sidebar_new ()
{
  return GTK_WIDGET (g_object_new (sidebar_get_type (), NULL));
}

// Add a new tab to the browser
void sidebar_add_tab (SideBar *sidebar, 
                               GtkWidget *child, 
                               GtkWidget *tab_label)
{ 
  gtk_notebook_append_page (GTK_NOTEBOOK (sidebar), 
                            child, tab_label);
}

FileBrowser* sidebar_get_file_browser (SideBar *sidebar)
{ return FILE_BROWSER (sidebar->filebrowser);
}
