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

#include "insert-info.h"

void insert_info_add_menu_items (OpenLDev*);
void insert_info_user_name (GtkMenuItem*, gpointer);
void insert_info_real_name (GtkMenuItem*, gpointer);
void insert_info_home_directory (GtkMenuItem*, gpointer);
void insert_info_do_insertion (OpenLDev*, string);
void insert_info_gpl_header (GtkMenuItem*, gpointer);
void insert_info_date_and_time (GtkMenuItem*, gpointer);

static void refresh_date (const char*, GtkWidget*);
gboolean insert_info_format_event_after (GtkWidget*, GdkEvent*, gpointer);

extern "C" 
{
// Initialize the plugin
G_MODULE_EXPORT PluginState init (gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  insert_info = new InsertInfoWidgets;
  insert_info_add_menu_items (openldev);
  
  return PLUGIN_OK;
} 

// This actually will just hide the plugin for later use
G_MODULE_EXPORT PluginState destroy (gpointer data)
{
  gtk_widget_hide (insert_info->menu_user);
  gtk_widget_hide (insert_info->item_user);
  gtk_widget_hide (insert_info->item_username);
  gtk_widget_hide (insert_info->item_realname);
  gtk_widget_hide (insert_info->item_homedir);
  gtk_widget_hide (insert_info->item_gpl);
  gtk_widget_hide (insert_info->item_date);
  gtk_widget_hide (insert_info->item_sep);
  
  return PLUGIN_OK;
}

// Show the plugin
G_MODULE_EXPORT PluginState reload (gpointer data)
{
  gtk_widget_show (insert_info->menu_user);
  gtk_widget_show (insert_info->item_user);
  gtk_widget_show (insert_info->item_username);
  gtk_widget_show (insert_info->item_realname);
  gtk_widget_show (insert_info->item_homedir);
  gtk_widget_show (insert_info->item_gpl);
  gtk_widget_show (insert_info->item_date);
  gtk_widget_show (insert_info->item_sep);
  
  return PLUGIN_OK;
}
};

// Add the menu items & connect signals for the Insert Info plugin
void insert_info_add_menu_items (OpenLDev *openldev)
{
  MenuHandle *menus = openldev->menus;
  
  insert_info->menu_user = gtk_menu_new ();
  insert_info->item_user = gtk_menu_item_new_with_mnemonic ("Insert User Info ...");
  insert_info->item_username = gtk_menu_item_new_with_mnemonic ("Current _User Name");
  insert_info->item_realname = gtk_menu_item_new_with_mnemonic ("User's _Real Name");
  insert_info->item_homedir = gtk_menu_item_new_with_mnemonic ("_Home Directory");
  insert_info->item_gpl = gtk_menu_item_new_with_mnemonic ("Insert _GPL Header");
  insert_info->item_date = gtk_menu_item_new_with_mnemonic ("Insert Date and _Time");
  insert_info->item_sep = gtk_separator_menu_item_new ();
  GtkWidget *menu = gtk_menu_item_get_submenu (GTK_MENU_ITEM (gtk_ui_manager_get_widget 
                                               (menus->uimanager, "/MenuBar/Edit")));
  
  gtk_menu_attach (GTK_MENU (insert_info->menu_user), 
                   insert_info->item_username, 0, 1, 0, 1);
  gtk_menu_attach (GTK_MENU (insert_info->menu_user), 
                   insert_info->item_realname, 0, 1, 1, 2);
  gtk_menu_attach (GTK_MENU (insert_info->menu_user), 
                   insert_info->item_homedir, 0, 1, 2, 3);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (insert_info->item_user), 
                             insert_info->menu_user);
  
  g_signal_connect (G_OBJECT (insert_info->item_username), "activate",
                    G_CALLBACK (insert_info_user_name), (gpointer) openldev);
  g_signal_connect (G_OBJECT (insert_info->item_realname), "activate",
                    G_CALLBACK (insert_info_real_name), (gpointer) openldev);
  g_signal_connect (G_OBJECT (insert_info->item_homedir), "activate",
                    G_CALLBACK (insert_info_home_directory), (gpointer) openldev);
  g_signal_connect (G_OBJECT (insert_info->item_gpl), "activate",
                    G_CALLBACK (insert_info_gpl_header), (gpointer) openldev);
  g_signal_connect (G_OBJECT (insert_info->item_date), "activate",
                    G_CALLBACK (insert_info_date_and_time), (gpointer) openldev);

  gtk_menu_attach (GTK_MENU (menu), insert_info->item_sep, 0, 1, 10, 11);
  gtk_menu_attach (GTK_MENU (menu), insert_info->item_user, 0, 1, 11, 12);
  gtk_menu_attach (GTK_MENU (menu), insert_info->item_date, 0, 1, 12, 13);
  gtk_menu_attach (GTK_MENU (menu), insert_info->item_gpl, 0, 1, 13, 14);
  
  gtk_widget_show (insert_info->menu_user);
  gtk_widget_show (insert_info->item_user);
  gtk_widget_show (insert_info->item_username);
  gtk_widget_show (insert_info->item_realname);
  gtk_widget_show (insert_info->item_homedir);
  gtk_widget_show (insert_info->item_gpl);
  gtk_widget_show (insert_info->item_date);
  gtk_widget_show (insert_info->item_sep);
}

// Do the actual insertion of whatever the text is
void insert_info_do_insertion (OpenLDev *openldev,
                               string text)
{
  FileManager *files = FILE_MANAGER (openldev->files);
  
  if (file_manager_get_current_tab (files) != -1)
    gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER (file_manager_get_current_buffer
                                      (files)), (gchar*) text.c_str(), text.length());
  else
    openldev_message ("OpenLDev Error", "You must first open a document for this action to work!", GTK_MESSAGE_ERROR);
}

// Insert date and time
void 
insert_info_date_and_time (GtkMenuItem *menuitem,
                           gpointer data)
{  
  string gladefile = string(INSTALL_PREFIX) + "/share/openldev/glade/inserttimedate.glade";
  GladeXML *xml = glade_xml_new (gladefile.c_str(), NULL, NULL);
  GtkWidget *dialog = glade_xml_get_widget (xml, "dialog");
  GtkWidget *format = glade_xml_get_widget (xml, "format");
  GtkWidget *preview = glade_xml_get_widget (xml, "preview");
  
  g_signal_connect (G_OBJECT (format), "event-after",
                    G_CALLBACK (insert_info_format_event_after),
                    (gpointer) preview);
  gtk_entry_set_text (GTK_ENTRY (format), "%B %d, %Y %H:%M:%S");
  refresh_date ("%B %d, %Y %H:%M:%S", preview);
  
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
    insert_info_do_insertion ((OpenLDev*) data, gtk_label_get_text (GTK_LABEL (preview)));
  
  gtk_widget_destroy (dialog);
}

static void
refresh_date (const char *format,
              GtkWidget *preview)
{
  char date[256];
  time_t current = time (NULL);
  struct tm *local = localtime (&current);
  
  strftime (date, sizeof (date), format, local);
  gtk_label_set_text (GTK_LABEL (preview), date);
}

gboolean 
insert_info_format_event_after (GtkWidget *widget,
                                GdkEvent *event,
                                gpointer data)
{
  if (event->type == GDK_KEY_PRESS ||
      event->type == GDK_KEY_RELEASE)
  refresh_date (gtk_entry_get_text (GTK_ENTRY (widget)), (GtkWidget*) data);
}

// Insert user name
void insert_info_user_name (GtkMenuItem *menuitem,
                            gpointer data)
{ insert_info_do_insertion ((OpenLDev*) data, g_get_user_name ());
}

// Insert real name
void insert_info_real_name (GtkMenuItem *menuitem,
                            gpointer data)
{ insert_info_do_insertion ((OpenLDev*) data, g_get_real_name ());
}

// Insert home directory
void insert_info_home_directory (GtkMenuItem *menuitem,
                                 gpointer data)
{ insert_info_do_insertion ((OpenLDev*) data, g_get_home_dir ());
}

// Insert GPL Header
void insert_info_gpl_header (GtkMenuItem *menuitem,
                             gpointer data)
{ 
  string header = 
    "/***************************************************************************\n" \
    " *   Copyright (C) YYYY by                                                 *\n" \
    " *                                                                         *\n" \
    " *   This program is free software; you can redistribute it and/or modify  *\n" \
    " *   it under the terms of the GNU General Public License as published by  *\n" \
    " *   the Free Software Foundation; either version 2 of the License, or     *\n" \
    " *   (at your option) any later version.                                   *\n" \
    " *                                                                         *\n" \
    " *   This program is distributed in the hope that it will be useful,       *\n" \
    " *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *\n" \
    " *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *\n" \
    " *   GNU General Public License for more details.                          *\n" \
    " *                                                                         *\n" \
    " *   You should have received a copy of the GNU General Public License     *\n" \
    " *   along with this program; if not, write to the                         *\n" \
    " *   Free Software Foundation, Inc.,                                       *\n" \
    " *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *\n" \
    " ***************************************************************************/\n";

  // Get the full date
  int location = 0;
  time_t current;
  time (&current);
  string date = ctime (&current);
  if (date.length () >= 24)
  {  
    date.erase (0, 20);
    location = header.find ("YYYY", 0);
    header.erase (location, 4);
    if (date.length () >= 4)
      header.insert (location, date.substr (0, 4));
  }
  
  // Insert username and remove traling spaces
  location += 8;
  string username = g_get_real_name ();
  header.erase (location, username.length());
  header.insert (location, username);
  
  insert_info_do_insertion ((OpenLDev*) data, header);
}
