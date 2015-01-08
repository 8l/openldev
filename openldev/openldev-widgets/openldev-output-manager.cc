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

#include "openldev-output-manager.h"

static void output_manager_class_init (OutputManagerClass *klass);
static void output_manager_init (OutputManager *manager);

GType
output_manager_get_type ()
{
  static GType manager_type = 0;

  if (!manager_type)
  {
    static const GTypeInfo manager_info =
    {
	    sizeof (OutputManagerClass),
	    NULL, NULL,
	    (GClassInitFunc) output_manager_class_init,
      NULL, NULL,
      sizeof (OutputManager), 0,
	    (GInstanceInitFunc) output_manager_init,
    };

    manager_type = g_type_register_static (GTK_TYPE_NOTEBOOK, "OutputManager", 
                                           &manager_info, (GTypeFlags) 0);
  }

  return manager_type;
}

static void
output_manager_class_init (OutputManagerClass *klass)
{
}

static void
output_manager_init (OutputManager *manager)
{
}

GtkWidget* 
output_manager_new (EnvironmentSettings *env,
                    MenuHandle *menus)
{
  OutputManager *manager;
  
  manager = OUTPUT (g_object_new (output_manager_get_type (), NULL));
  manager->build = output_build_new (env, menus);
  manager->search = output_list_new ();
  manager->terminal = terminal_new (env);
  
  manager->compiler = gtk_hbox_new (FALSE, 0);
  manager->compiler_label = gtk_label_new ("Compiler");
  manager->compiler_image = gtk_image_new_from_stock ("gtk-execute", GTK_ICON_SIZE_MENU);
  manager->findtab = gtk_hbox_new (FALSE, 0);
  manager->findtab_label = gtk_label_new ("Search");
  manager->findtab_image = gtk_image_new_from_stock ("gtk-find", GTK_ICON_SIZE_MENU);
  manager->terminaltab = gtk_hbox_new (FALSE, 0);
  manager->terminaltab_label = gtk_label_new ("Terminal");
  manager->terminaltab_image = gtk_image_new_from_stock ("gtk-justify-left", GTK_ICON_SIZE_MENU);
  
  gtk_box_set_spacing (GTK_BOX (manager->compiler), 5);
  gtk_box_set_spacing (GTK_BOX (manager->findtab), 5);
  gtk_box_set_spacing (GTK_BOX (manager->terminaltab), 5);
  gtk_box_pack_start (GTK_BOX (manager->terminaltab), manager->terminaltab_image, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (manager->terminaltab), manager->terminaltab_label, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (manager->findtab), manager->findtab_image, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (manager->findtab), manager->findtab_label, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (manager->compiler), manager->compiler_image, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (manager->compiler), manager->compiler_label, FALSE, FALSE, 0);
  
  output_manager_add_tab (manager, output_build_get_scrolled_window (OUTPUT_BUILD 
                          (manager->build)), manager->compiler);
  output_manager_add_tab (manager, output_list_get_scrolled_window (LIST 
                          (manager->search)), manager->findtab);
  output_manager_add_tab (manager, terminal_get_hbox (TERMINAL 
                          (manager->terminal)), manager->terminaltab);
  
  gtk_widget_show_all (manager->compiler);
  gtk_widget_show_all (manager->findtab);
  gtk_widget_show_all (manager->terminaltab);
  gtk_widget_show_all (GTK_WIDGET (manager));

  return GTK_WIDGET (manager);
}

// Add a new tab to the browser
void 
output_manager_add_tab (OutputManager *manager, 
                        GtkWidget *child, 
                        GtkWidget *tab_label)
{ 
  gtk_notebook_append_page (GTK_NOTEBOOK (manager), child, tab_label);
}

GtkWidget* 
output_manager_get_build (OutputManager *manager)
{ return manager->build;
}

GtkWidget* 
output_manager_get_search (OutputManager *manager)
{ return manager->search;
}

GtkWidget* 
openldev_output_manager_get_terminal (OutputManager *manager)
{ return manager->terminal;
}
