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

#include "openldev.h"
#include <glade/glade.h>
#include <cstring>

static void on_destroy (GtkWidget*, gpointer);
static void on_find_activate (GtkEntry*, OpenLDev*);
static void on_resize_window (GtkWidget*, GdkEvent*, OpenLDev*);
static void on_resize_pane (GtkWidget*, GdkEvent*, OpenLDev*);
static void on_close_toolbars (GtkWidget*, OpenLDev*);
static void on_goto_line (GtkWidget*, OpenLDev*);
static void on_find (GtkWidget*, OpenLDev*);
static void openldev_tip_of_the_day (OpenLDev*);
static gboolean on_delete_event (GtkWidget*, GdkEvent*, gpointer);
static gboolean on_auto_save (OpenLDev*);
static gboolean on_button_press_event_file_browser (GtkWidget*, GdkEventButton*, OpenLDev*);
static gboolean on_build_box_selected (GtkWidget*, GdkEventButton*, OpenLDev*);
static gboolean on_search_box_selected (GtkWidget*, GdkEventButton*, OpenLDev*);
static gboolean on_goto_line_key (GtkWidget*, GdkEventKey*, OpenLDev*);
GtkWidget* show_splash_screen ();

// The magic all begins here...
int main (int argc, 
          char *argv[])
{
  GtkWidget *hpaned, *vpaned, *sidebar, *output;
  EnvironmentSettings *env;
  GotoLineToolbar *gototoolbar;
  FindToolbar *findtoolbar;
  gchar *icon, *last_project;
  gboolean auto_load;
  gint auto_save;

#ifdef ENABLE_NLS
  setlocale (LC_ALL, "");
  bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

  if (argc > 1)
  {
    if (string (argv[1]) == "--help" || string (argv[1]) == "-?")
    {
      g_print ("For help information, run 'man openldev'.\n");
      return EXIT_SUCCESS;
    }
    else if (string (argv[1]) == "--version")
    {
      g_print ("%s %s %s", "OpenLDev", OPENLDEV_VERSION, "(Rockhopper)\n");
      return EXIT_SUCCESS;
    }
  }
  
  gnome_program_init ("openldev", OPENLDEV_VERSION, LIBGNOMEUI_MODULE, 
                      argc, argv, GNOME_PARAM_NONE);
  GtkWidget *splash = show_splash_screen ();
  
  // Create the OpenLDev window
  OpenLDev *openldev = OPENLDEV (openldev_new ());
  icon = openldev_add_prefix ("/share/pixmaps/openldev/openldev24.png");
  gtk_window_set_icon_from_file (GTK_WINDOW (openldev), icon, NULL);

  g_object_get (openldev, "hpaned", &hpaned, "vpaned", &vpaned, "sidebar", &sidebar,
                "output-pane", &output, "goto-toolbar", &gototoolbar,
                "find-toolbar", &findtoolbar, "env-settings", &env, NULL);

  // Connect all of the main window signals
  g_signal_connect (G_OBJECT (openldev), "destroy",
		                G_CALLBACK (on_destroy), NULL);
  g_signal_connect (G_OBJECT (openldev), "delete-event",
                    G_CALLBACK (on_delete_event), openldev);
  g_signal_connect (G_OBJECT (openldev), "event-after",
                    G_CALLBACK (on_resize_window), (gpointer) openldev);
  g_signal_connect (G_OBJECT (hpaned), "event-after",
                    G_CALLBACK (on_resize_pane), (gpointer) openldev);
  g_signal_connect (G_OBJECT (vpaned), "event-after",
                    G_CALLBACK (on_resize_pane), (gpointer) openldev);
  g_signal_connect (G_OBJECT (sidebar_get_file_browser (SIDEBAR (sidebar))), 
                    "button-press-event", G_CALLBACK (on_button_press_event_file_browser), 
                    (gpointer) openldev);
  g_signal_connect (G_OBJECT (OUTPUT (output)->search), "button-press-event", 
                    G_CALLBACK (on_search_box_selected), (gpointer) openldev);
  g_signal_connect (G_OBJECT (OUTPUT (output)->build), "button-press-event",
                    G_CALLBACK (on_build_box_selected), (gpointer) openldev);
  g_signal_connect (G_OBJECT (findtoolbar->button_close), "clicked",
                    G_CALLBACK (on_close_toolbars), (gpointer) openldev);
  g_signal_connect (G_OBJECT (findtoolbar->button_find), "clicked",
                    G_CALLBACK (on_find), (gpointer) openldev);
  g_signal_connect (G_OBJECT (gtk_bin_get_child (GTK_BIN (findtoolbar->find_text))), 
                    "activate", G_CALLBACK (on_find_activate), (gpointer) openldev);
  
  // Load the last project or just set the directory to the home directory
  g_object_get (env, "last-project", &last_project, "auto-load", &auto_load,
                "auto-save", &auto_save, NULL);
  
  // Enable automatic saving if the user wants
  if (auto_save > 0)
  {
    gint time = auto_save * 1000 * 60;
    g_timeout_add (time, GSourceFunc (on_auto_save), (gpointer) openldev);
  }
 
  // Hide the splash screen & start the main look
  gtk_widget_destroy (splash);
  
  if ((strlen (last_project) > 0) && auto_load)
    if (g_ascii_strcasecmp (last_project, "EMPTY") != 0)
      openldev_open_project (openldev, last_project);
        
  openldev_populate_treeview (openldev);
  openldev_tip_of_the_day (openldev);
  g_free (last_project);
  g_free (icon);
  
  gtk_main ();
	
  return EXIT_SUCCESS;
}

GtkWidget*
show_splash_screen ()
{
  gchar *image_file = openldev_add_prefix ("/share/pixmaps/openldev/splash.png");
  GtkWidget *splash = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  GtkWidget *image = gtk_image_new_from_file (image_file);
  
  gtk_container_add (GTK_CONTAINER (splash), image);
  gtk_window_set_type_hint (GTK_WINDOW (splash), GDK_WINDOW_TYPE_HINT_SPLASHSCREEN);
  gtk_window_set_position (GTK_WINDOW (splash), GTK_WIN_POS_CENTER_ALWAYS);
  gtk_window_set_keep_above (GTK_WINDOW (splash), TRUE);
  gtk_window_set_decorated (GTK_WINDOW (splash), FALSE);
  gtk_window_set_has_frame (GTK_WINDOW (splash), FALSE);
  gtk_window_set_skip_taskbar_hint (GTK_WINDOW (splash), TRUE);
  gtk_widget_show_all (splash);
  
  // Make sure the splash screen is shown
  while (gtk_events_pending())
    gtk_main_iteration ();
  
  return splash;
}

static void
toggle_show_tip (GtkToggleButton *show,
                 EnvironmentSettings *env)
{
  g_object_set (env, "show-tip", gtk_toggle_button_get_active (show), NULL);
}

static void
openldev_tip_of_the_day (OpenLDev *openldev)
{
  GtkWidget *dialog, *tip, *show;
  gboolean first_boot, show_tip;
  gchar *message, *gladefile;
  GladeXML *xml;
  
  g_object_get (openldev->env, "first-boot", &first_boot, "show-tip", &show_tip, NULL);
  if (!show_tip || !first_boot)
    return;

  gladefile = openldev_add_prefix ("/share/openldev/glade/tip.glade");
  xml = glade_xml_new (gladefile, NULL, NULL);
  dialog = glade_xml_get_widget (xml, "dialog");
  tip = glade_xml_get_widget (xml, "tip");
  show = glade_xml_get_widget (xml, "show");  
  gtk_widget_show_all (dialog);
  
  message = "Welcome to OpenLDev! To begin, you should create a new project by using the item provided in the File menu. From there, you can run Auto Generate on the project to produce all of the necessary files before building.\n\nYou can also find a number of plugins in the Preferences dialog, found under the Edit menu. This is where you can customize and expand OpenLDev.\n\nYou will not see this dialog again until the next time you upgrade OpenLDev.";
  
  gtk_label_set_text (GTK_LABEL (tip), message);
  g_object_set (openldev->env, "first-boot", FALSE, NULL);
  g_signal_connect (G_OBJECT (show), "toggled", 
                    G_CALLBACK (toggle_show_tip), openldev->env);
  
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

static gboolean 
on_delete_event (GtkWidget *widget, 
                 GdkEvent *event, 
                 gpointer data)
{
  OpenLDev *openldev = OPENLDEV (data);
  gboolean is_modified = FALSE;
  FileManager *files;
  
  g_object_get (openldev, "file-browser", &files, NULL);
  for (int i=0; i < files->tab_count; i++)
    if (FILE_TAB (files->tabs[i])->modified)
      is_modified = TRUE;
  
  if (!is_modified)
  {
    gtk_main_quit();
    return FALSE;
  }
  else if (openldev_question ("You have unsaved files! Are you sure you want to close?"))
  {
    gtk_main_quit();
    return FALSE;
  }
  
  return TRUE;
}

static void 
on_destroy (GtkWidget *widget,
            gpointer data)
{
  gtk_main_quit ();
}

// Autosave all of the files when called
static gboolean 
on_auto_save (OpenLDev *openldev)
{
  openldev_menu_file_save_all (openldev);
  return TRUE;
}

// When the window is resized, this checks the width and height and stores
// them so the same size can be restored on restart.
static void 
on_resize_window (GtkWidget *widget,
                  GdkEvent *event,
                  OpenLDev *openldev)
{
  EnvironmentSettings *env;
  
  g_object_get (openldev, "env-settings", &env, NULL);
  if (event->type == GDK_CONFIGURE)
  {
    // Get the current height & width and store them
    int width=0, height=0;
    width = widget->allocation.width;
    height = widget->allocation.height;
    
    if (width > 0 && height > 0)
      g_object_set (env, "window-width", width, "window-height", height, NULL);
  }
}

// When one of the panes is resized, this checks the width and height of each
// pane so the same sizes can be restored on restart.
static void 
on_resize_pane (GtkWidget *widget,
                GdkEvent *event,
                OpenLDev *openldev)
{
  EnvironmentSettings *env;
  SideBar *sidebar;
  OutputManager *output;
  
  g_object_get (openldev, "env-settings", &env, "sidebar", &sidebar,
                "output-pane", &output, NULL);
  if (event->type == GDK_MOTION_NOTIFY)
  {   
    // Get the Project File & Output panes' width/height & store them
    g_object_set (env, "sidebar-width", 
                  FILE_BROWSER (sidebar->filebrowser)->swin->allocation.width,
                  "output-height", OUTPUT_BUILD (output->build)->swin->allocation.height, NULL);
  }
}

// Catch button press event & open popup menu or handle selection. This function
// receives the button event. If it is a right click, then the popup menu is
// shown. If it is a double click, the selected file is opened
static gboolean 
on_button_press_event_file_browser (GtkWidget *widget,
                                    GdkEventButton *event,
                                    OpenLDev *openldev)
{
  MenuHandle *menus;
  SideBar *sidebar;
  
  g_object_get (openldev, "menus", &menus, "sidebar", &sidebar, NULL);
  
  if((event->type == GDK_BUTTON_PRESS) && (event->button == 3))  
    gtk_menu_popup (GTK_MENU (menus->popup_filebrowser), 
                    NULL, NULL, NULL, (gpointer) openldev, 0, event->time);
  else if (event->type == GDK_2BUTTON_PRESS)
  {
    GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
      gchar *fn, *file;
      gtk_tree_model_get (model, &iter, 1, &fn, -1);
   
      // If the text is not a group title, open it
      file = g_strdup_printf ("%s%s%s", FILE_BROWSER (sidebar_get_file_browser (sidebar))->base,
                              FILE_BROWSER (sidebar_get_file_browser (sidebar))->current, fn);
      
      if (g_ascii_strcasecmp (fn, "..") == 0)
        file_browser_move_up (FILE_BROWSER (sidebar_get_file_browser (sidebar)));
      else if (g_file_test (file, G_FILE_TEST_IS_DIR))
        file_browser_move (FILE_BROWSER (sidebar_get_file_browser (sidebar)), fn);
      else
        openldev_open_file (openldev, file);
      
      g_free (fn);
      g_free (file);
    }
  }
  else
    return FALSE;
  
  return TRUE;
}

// Catch the build box selection & do action. This function receives the button
// event. If it is a double click, get the text of the selection and check if
// it is an error or warning. If it is, then the file containing the error or
// warning is opened. The cursor is then moved line of the error or warning
static gboolean 
on_build_box_selected (GtkWidget *widget, 
                       GdkEventButton *event, 
                       OpenLDev *openldev)
{
  MenuHandle *menus;
  
  g_object_get (openldev, "menus", &menus, NULL);
  if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))  
    gtk_menu_popup (GTK_MENU (menus->popup_buildoutput), 
                    NULL, NULL, NULL, (gpointer) openldev, 0, event->time);
  else if (event->type == GDK_2BUTTON_PRESS)
  {
    openldev_jump_to_build_output (GTK_MENU_ITEM (gtk_menu_item_new()), openldev);    
    return TRUE;
  }
  
  return FALSE;
}

// Catch the search box selection & do action. This function receives the button
// event. If it is a double click, get the text of the selection and then the 
// file containing the search text is opened. The cursor is then moved line of 
// the search result
static gboolean 
on_search_box_selected (GtkWidget *widget,
                        GdkEventButton *event,
                        OpenLDev *openldev)
{
  MenuHandle *menus;
  
  g_object_get (openldev, "menus", &menus, NULL);
  if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))  
    gtk_menu_popup (GTK_MENU (menus->popup_searchoutput), 
                    NULL, NULL, NULL, (gpointer) openldev, 0, event->time);
  else if (event->type == GDK_2BUTTON_PRESS)
  {
    openldev_jump_to_search_output (GTK_MENU_ITEM (gtk_menu_item_new()), openldev);    
    return TRUE;
  }
  
  return FALSE;
}

// Close both toolbars. The find toolbar and the goto line toolbar.
static void 
on_close_toolbars (GtkWidget *widget, 
                   OpenLDev *openldev)
{
  FindToolbar *findtoolbar;
  
  g_object_get (openldev, "find-toolbar", &findtoolbar, NULL);
  gtk_widget_hide (findtoolbar->hbox);
}

static void 
on_find (GtkWidget *widget, 
         OpenLDev *openldev)
{ 
  openldev_menu_edit_find_next (openldev); 
}

static void
on_find_activate (GtkEntry *entry,
                  OpenLDev *openldev)
{ 
  openldev_menu_edit_find_next (openldev);
}
