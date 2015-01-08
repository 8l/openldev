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

enum 
{
  FIRST_OPENLDEV_PROPERTY,
  PROP_SIDEBAR,
  PROP_FILES,
  PROP_OUTPUT,
  PROP_HPANED,
  PROP_VBOX,
  PROP_VPANED,
  PROP_FIND_TOOLBAR,
  PROP_GOTO_TOOLBAR,
  PROP_MENU,
  PROP_STATUSBAR,
  PROP_EDITOR,
  PROP_ENV_SETTINGS,
  PROP_PROJECT,
  PROP_PLUGINS
};

enum 
{
  PROJECT_OPEN,
  FILE_NEW,
  FILE_OPEN,
  PROJECT_NEW,
  PROJECT_IMPORT,
  FILE_SAVE,
  FILE_SAVE_AS,
  FILE_SAVE_ALL,
  FILE_RELOAD,
  PROJECT_CLOSE,
  COMPILE,
  BUILD,
  EXECUTE,
  AUTO_GENERATE,
  CONFIGURE,
  MAKE_CLEAN,
  OPENLDEV_SIGNAL_COUNT
};

// Initialization Functions
void openldev_initialize_objects (OpenLDev*);
void openldev_populate_window (OpenLDev*);
void openldev_create_menus_and_toolbar (OpenLDev*);
void openldev_populate_recent_files (OpenLDev*);

// Signal Handler declarations
void openldev_on_recent_file_activated (GtkMenuItem*, OpenLDev*);

// Class initialization functions
static void openldev_class_init (OpenLDevClass *klass);
static void openldev_init (OpenLDev *openldev);
static void openldev_get_property (GObject*, guint, GValue*, GParamSpec*);
static void openldev_set_property (GObject*, guint, const GValue*, GParamSpec*);

static guint openldev_signals [OPENLDEV_SIGNAL_COUNT] = { 0 };

GType
openldev_get_type (void)
{
  static GType openldev_type = 0;

  if (!openldev_type)
  {
    static const GTypeInfo openldev_info =
    {
	    sizeof (OpenLDevClass),
	    NULL, NULL,
	    (GClassInitFunc) openldev_class_init,
      NULL, NULL,
      sizeof (OpenLDev), 0,
	    (GInstanceInitFunc) openldev_init,
    };

    openldev_type = g_type_register_static (GTK_TYPE_WINDOW, "OpenLDev", 
                                            &openldev_info, (GTypeFlags) 0);
  }

  return openldev_type;
}

static void
openldev_class_init (OpenLDevClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  /* Override the standard functions for setting and retrieving properties. */
  gobject_class->get_property = openldev_get_property;
  gobject_class->set_property = openldev_set_property;

  /* Register signals. */
  openldev_signals[PROJECT_OPEN] = g_signal_new ("project-open",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, project_open),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  
  openldev_signals[FILE_NEW] = g_signal_new ("file-new",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, file_new),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  
  openldev_signals[FILE_OPEN] = g_signal_new ("file-open",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, file_open),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  
  openldev_signals[PROJECT_NEW] = g_signal_new ("project-new",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, project_new),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  
  openldev_signals[PROJECT_IMPORT] = g_signal_new ("project-import",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, project_import),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  
  openldev_signals[FILE_SAVE] = g_signal_new ("file-save",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, file_save),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  
  openldev_signals[FILE_SAVE_AS] = g_signal_new ("file-save-as",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, file_save_as),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  
  openldev_signals[FILE_SAVE_ALL] = g_signal_new ("file-save-all",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, file_save_all),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  
  openldev_signals[FILE_RELOAD] = g_signal_new ("file-reload",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, file_reload),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  
  openldev_signals[PROJECT_CLOSE] = g_signal_new ("project-close",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, project_close),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  
  openldev_signals[COMPILE] = g_signal_new ("compile",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, compile),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  
  openldev_signals[BUILD] = g_signal_new ("build",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, build),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  
  openldev_signals[EXECUTE] = g_signal_new ("execute",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, execute),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  
  openldev_signals[AUTO_GENERATE] = g_signal_new ("auto-generate",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, auto_generate),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  
  openldev_signals[CONFIGURE] = g_signal_new ("configure",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, configure),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  
  openldev_signals[MAKE_CLEAN] = g_signal_new ("make-clean",
					                         G_TYPE_FROM_CLASS (klass),
	                                 G_SIGNAL_RUN_LAST,
	                                 G_STRUCT_OFFSET (OpenLDevClass, make_clean),
                                   NULL, NULL,                
					                         g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);

  /* Add all properties. */
  g_object_class_install_property (gobject_class, PROP_SIDEBAR,
               g_param_spec_object ("sidebar", NULL, NULL, GTK_TYPE_WIDGET,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_FILES,
               g_param_spec_object ("file-browser", NULL, NULL, GTK_TYPE_WIDGET,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_OUTPUT,
               g_param_spec_object ("output-pane", NULL, NULL, GTK_TYPE_WIDGET,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_HPANED,
               g_param_spec_object ("hpaned", NULL, NULL, GTK_TYPE_WIDGET,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_VBOX,
               g_param_spec_object ("vbox", NULL, NULL, GTK_TYPE_WIDGET,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_VPANED,
               g_param_spec_object ("vpaned", NULL, NULL, GTK_TYPE_WIDGET,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_FIND_TOOLBAR,
              g_param_spec_pointer ("find-toolbar", NULL, NULL,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_GOTO_TOOLBAR,
              g_param_spec_pointer ("goto-toolbar", NULL, NULL,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_MENU,
              g_param_spec_pointer ("menus", NULL, NULL,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_STATUSBAR,
              g_param_spec_pointer ("status-bar", NULL, NULL,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_EDITOR,
              g_param_spec_pointer ("editor-prop", NULL, NULL,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_ENV_SETTINGS,
              g_param_spec_pointer ("env-settings", NULL, NULL,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_PROJECT,
              g_param_spec_pointer ("project", NULL, NULL,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_PLUGINS,
              g_param_spec_pointer ("plugins", NULL, NULL,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
}

/* This function is called when the programmer requests the value of a widget
 * property with g_object_get(). */
static void
openldev_get_property (GObject *object,
                       guint prop_id,
                       GValue *value,
                       GParamSpec *pspec)
{
  OpenLDev *openldev = OPENLDEV (object);

  switch (prop_id)
  {
    case PROP_SIDEBAR:      g_value_set_object (value, openldev->sidebar); break;
    case PROP_FILES:        g_value_set_object (value, openldev->files); break;
    case PROP_OUTPUT:       g_value_set_object (value, openldev->output); break;
    case PROP_HPANED:       g_value_set_object (value, openldev->hpaned); break;
    case PROP_VBOX:         g_value_set_object (value, openldev->vbox); break;
    case PROP_VPANED:       g_value_set_object (value, openldev->vpaned); break;
    case PROP_FIND_TOOLBAR: g_value_set_pointer (value, openldev->findtoolbar); break;
    case PROP_GOTO_TOOLBAR: g_value_set_pointer (value, openldev->gototoolbar); break;
    case PROP_MENU:         g_value_set_pointer (value, openldev->menus); break;
    case PROP_STATUSBAR:    g_value_set_pointer (value, openldev->statusbar); break;
    case PROP_EDITOR:       g_value_set_pointer (value, openldev->editor); break;
    case PROP_ENV_SETTINGS: g_value_set_pointer (value, openldev->env); break;
    case PROP_PROJECT:      g_value_set_pointer (value, openldev->project); break;
    case PROP_PLUGINS:      g_value_set_pointer (value, openldev->pluginlist); break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

/* This function is called when the programmer gives a new value for a widget
 * property with g_object_set(). */
static void
openldev_set_property (GObject *object,
                       guint prop_id,
                       const GValue *value,
                       GParamSpec *pspec)
{
  OpenLDev *openldev = OPENLDEV (object);
  gpointer data;

  switch (prop_id)
  {
    case PROP_SIDEBAR:
      data = g_value_get_object (value); 
      openldev->sidebar = GTK_WIDGET (data); 
      break;
    case PROP_FILES:
      data = g_value_get_object (value); 
      openldev->files = GTK_WIDGET (data); 
      break;
    case PROP_OUTPUT:
      data = g_value_get_object (value); 
      openldev->output = GTK_WIDGET (data); 
      break;
    case PROP_HPANED:
      data = g_value_get_object (value); 
      openldev->hpaned = GTK_WIDGET (data); 
      break;
    case PROP_VBOX:
      data = g_value_get_object (value); 
      openldev->vbox = GTK_WIDGET (data); 
      break;
    case PROP_VPANED:
      data = g_value_get_object (value); 
      openldev->vpaned = GTK_WIDGET (data); 
      break;
    case PROP_FIND_TOOLBAR:
      data = g_value_get_pointer (value); 
      openldev->findtoolbar = (FindToolbar*) data; 
      break;
    case PROP_GOTO_TOOLBAR:
      data = g_value_get_pointer (value); 
      openldev->gototoolbar = (GotoLineToolbar*) data; 
      break;
    case PROP_MENU:
      data = g_value_get_pointer (value); 
      openldev->menus = (MenuHandle*) data; 
      break;
    case PROP_STATUSBAR:
      data = g_value_get_pointer (value); 
      openldev->statusbar = (StatusBar*) data; 
      break;
    case PROP_EDITOR:
      data = g_value_get_pointer (value); 
      openldev->editor = (EditorProperties*) data;
      break;
    case PROP_ENV_SETTINGS:
      data = g_value_get_pointer (value); 
      openldev->env = (EnvironmentSettings*) data; 
      break;
    case PROP_PROJECT:
      data = g_value_get_pointer (value); 
      openldev->project = (ProjectSettings*) data; 
      break;
    case PROP_PLUGINS:
      data = g_value_get_pointer (value); 
      openldev->pluginlist = (OpenLDevPluginList*) data; 
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
openldev_init (OpenLDev *openldev)
{
  gint width, height;

  // Set properties of main window
  gtk_window_set_title (GTK_WINDOW (openldev), 
                        g_strdup_printf ("%s%s", "OpenLDev ", OPENLDEV_VERSION));
  
  // Create settings objects
  openldev->project = openldev_project_settings_new (NULL);
  openldev->env = environment_new ();
  openldev->editor = openldev_editor_new ();
  
  // Call functions to add main window widgets & show them all
  g_object_get (openldev->env, "window-width", &width, "window-height", &height, NULL);
  gtk_window_set_default_size (GTK_WINDOW (openldev), width, height);
  
  openldev_initialize_objects (openldev);
  openldev->pluginlist = openldev_load_plugins_all (openldev);
  gtk_notebook_set_current_page (GTK_NOTEBOOK (openldev->output), 0);
  menu_handle_manage_file_items (openldev->menus, FALSE);
  menu_handle_manage_project_items (openldev->menus, FALSE);
  menu_handle_set_sensitive (openldev->menus->uimanager, "/ToolBar/BuildStop", FALSE);
  menu_handle_set_sensitive (openldev->menus->uimanager, "/MenuBar/Build/BuildStop", FALSE);
  openldev_initialize_gconf (openldev);
}

GtkWidget*
openldev_new ()
{
  return GTK_WIDGET (g_object_new (openldev_get_type (), NULL));
}

void 
openldev_initialize_objects (OpenLDev *openldev)
{
  gint width, height;

  openldev->vpaned = gtk_vpaned_new ();
  openldev->hpaned = gtk_hpaned_new ();
  openldev->vbox = gtk_vbox_new (FALSE, 0);
  openldev->statusbar = openldev_status_bar_new ();
  openldev->files = file_manager_new ();
  openldev->sidebar = sidebar_new ();
  openldev->findtoolbar = openldev_find_toolbar_new ();
  openldev->gototoolbar = openldev_goto_toolbar_new (openldev);
  openldev->menus = menu_handle_new ();
  openldev->output = output_manager_new (openldev->env, openldev->menus);
  
  g_object_get (openldev->env, "sidebar-width", &width, "output-height", &height, NULL);
  gtk_widget_set_size_request (FILE_BROWSER (SIDEBAR (openldev->sidebar)->filebrowser)->swin, width, 0);
  gtk_widget_set_size_request (OUTPUT (openldev->output)->terminal, 0, height);
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK (openldev->output), GTK_POS_LEFT);
  gtk_notebook_set_scrollable (GTK_NOTEBOOK (openldev->output), TRUE);
  
  // Add all of the widgets & show them
  openldev_populate_window (openldev);
  gtk_widget_show_all (GTK_WIDGET (openldev));
  
  openldev_find_toolbar_show_error (openldev->findtoolbar, FALSE);
  gtk_widget_hide (openldev->findtoolbar->hbox);
}

void 
openldev_populate_window (OpenLDev *openldev)
{
  openldev_create_menus_and_toolbar (openldev);
  
  // Add menu & toolbar to main vbox
  gtk_box_pack_start (GTK_BOX (openldev->vbox), openldev->menus->menubar, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (openldev->vbox), openldev->menus->toolbar, FALSE, FALSE, 0);
  gtk_paned_pack1 (GTK_PANED (openldev->hpaned), openldev->sidebar, FALSE, TRUE);
  gtk_paned_pack2 (GTK_PANED (openldev->hpaned), openldev->files, TRUE, TRUE);
  gtk_paned_pack1 (GTK_PANED (openldev->vpaned), openldev->hpaned, TRUE, TRUE);
  gtk_paned_pack2 (GTK_PANED (openldev->vpaned), openldev->output, FALSE, TRUE);

  // Add scrolled_win3 to the second vbox and finish off with the statusbar
  gtk_box_pack_start (GTK_BOX (openldev->vbox), openldev->vpaned, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (openldev->vbox), openldev->findtoolbar->hbox, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (openldev->vbox), openldev->statusbar->bar, FALSE, FALSE, 0);

  // Add v' & h' boxes to the main window
  gtk_container_add (GTK_CONTAINER (openldev), openldev->vbox);
}

void
openldev_populate_treeview (OpenLDev *openldev)
{ 
  FileBrowser *browser;

  browser = FILE_BROWSER (sidebar_get_file_browser (SIDEBAR (openldev->sidebar)));
  
  if (openldev->project->location != NULL)
    file_browser_populate (browser, openldev->project->location);
  else
    file_browser_populate (browser, g_get_home_dir ());
}

void 
openldev_create_menus_and_toolbar (OpenLDev *openldev)
{
  gint ln;

  // Create menu actions and icons
  menu_handle_create_icon_factory ();

  gtk_action_group_add_actions (openldev->menus->group, entries, 
                                NUM_ENTRIES, (gpointer) openldev);
  gtk_action_group_add_toggle_actions (openldev->menus->group, toggle_entries, 
                                       NUM_ENTRIES_TOGGLE, (gpointer) openldev);
  menu_handle_uimanager_init (openldev->menus);
  gtk_window_add_accel_group (GTK_WINDOW (openldev), 
                              gtk_ui_manager_get_accel_group (openldev->menus->uimanager));

  g_object_get (openldev->env, "line-numbers", &ln, NULL);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (gtk_ui_manager_get_widget 
                 (openldev->menus->uimanager, "/MenuBar/View/ViewLineNumbering")), ln);
  
  openldev_populate_recent_files (openldev);
}

void
openldev_populate_recent_files (OpenLDev *openldev)
{
  GSList *recent_files;
  gint i;

  // Hide the recent files until they are needed or populate them
  g_object_get (openldev->env, "recent-files", &recent_files, NULL);
  for (i = 0; i < 5; i++)
  {
    gchar buff[2], *recent;
    sprintf (buff, "%d", i+1);
    recent = g_strdup_printf ("%s%s", "/MenuBar/File/FileRecent", buff);
    
    if (g_slist_length (recent_files) <= i)
      gtk_widget_hide (gtk_ui_manager_get_widget (openldev->menus->uimanager, recent));
    else
    {
      gchar *tooltip = g_strdup_printf ("%s%s%s", "Open \"", (gchar*) g_slist_nth_data 
                                        (recent_files, i), "\"");

      gtk_label_set_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN (gtk_ui_manager_get_widget 
                          (openldev->menus->uimanager, recent)))), g_strconcat (buff, ". ", 
                          environment_get_recent_short (openldev->env, i), NULL));
      gtk_tooltips_set_tip (openldev->menus->tooltips, gtk_ui_manager_get_widget 
                            (openldev->menus->uimanager, recent),
                            tooltip, tooltip);
      g_free (tooltip);
    }
    
    g_signal_connect (G_OBJECT (gtk_ui_manager_get_widget 
                      (openldev->menus->uimanager, recent)), "activate",
                      G_CALLBACK (openldev_on_recent_file_activated), 
                      (gpointer) openldev);
    
    g_free (recent);
  }
}

// Quit the program
void 
openldev_file_quit (GtkMenuItem* menuitem, 
                    OpenLDev *openldev) 
{ 
  gboolean is_modified = FALSE;
  gint i;
  
  for (i = 0; i < FILE_MANAGER (openldev->files)->tab_count; i++)
    if (FILE_TAB (FILE_MANAGER (openldev->files)->tabs[i])->modified)
      is_modified = TRUE;
  
  if (!is_modified)
    gtk_widget_destroy (GTK_WIDGET (openldev));
  else if (openldev_question ("You have unsaved files! Are you sure you want to close?"))
    gtk_widget_destroy (GTK_WIDGET (openldev));

}

void 
openldev_on_recent_file_activated (GtkMenuItem *item,
                                   OpenLDev *openldev)
{
  const gchar *text;
  GtkWidget *label;
  GSList *recent_files;
  gint choice;

  label = gtk_bin_get_child (GTK_BIN (item));
  text = gtk_label_get_text (GTK_LABEL (label));
  choice = text[0] - 49;
  recent_files;

  g_object_get (openldev->env, "recent-files", &recent_files, NULL);
  if (choice >= 0 && choice < 5)
    openldev_open_file (openldev, (gchar*) g_slist_nth_data (recent_files, choice));
}

void 
openldev_open_file_browser (GtkMenuItem *item,
                            OpenLDev *openldev)
{
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;
  gchar *fn, *file;

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (sidebar_get_file_browser 
                                           (SIDEBAR (openldev->sidebar))));
  if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
    gtk_tree_model_get (model, &iter, 1, &fn, -1);
   
    // If the text is not a group title, open it
    file = g_strdup_printf ("%s%s%s", FILE_BROWSER (sidebar_get_file_browser 
                            (SIDEBAR (openldev->sidebar)))->base,
                            FILE_BROWSER (sidebar_get_file_browser 
                            (SIDEBAR (openldev->sidebar)))->current, fn);
    
    if (g_ascii_strcasecmp (fn, "..") == 0)
      file_browser_move_up (FILE_BROWSER (sidebar_get_file_browser (SIDEBAR (openldev->sidebar))));
    else if (g_file_test (file, G_FILE_TEST_IS_DIR))
      file_browser_move (FILE_BROWSER (sidebar_get_file_browser (SIDEBAR (openldev->sidebar))), fn);
    else
      openldev_open_file (openldev, file);
      
    g_free (fn);
    g_free (file);
  }
}

void 
openldev_jump_to_build_output (GtkMenuItem *item, 
                               OpenLDev *openldev)
{ 
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreePath *path;
  GtkTreeIter iter;
  GtkTextIter titer;
  GtkTextBuffer *buffer;
  gint linenumber;
  string dstr, temp;
  gchar *file, *dir;

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (OUTPUT (openldev->output)->build));
  if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
    path = gtk_tree_path_new ();
    gtk_tree_model_get (model, &iter, 0, &file, -1);
    
    // It the output is an error or a warning..
    if (openldev_is_error_or_warning (file))
    {
      temp = file;
      linenumber = openldev_get_file_name_and_line (temp) - 1;
      
      do
      {
        path = gtk_tree_model_get_path (model, &iter);
        if (!gtk_tree_path_prev (path))
          break;
        gtk_tree_model_get_iter (model, &iter, path);
        gtk_tree_model_get (model, &iter, 0, &dir, -1);
        dstr = string (dir);
      } while (dstr.find ("make[", 0) == string::npos);
        
      dstr = dir;
      if (dstr.find ("Entering", 0) != string::npos)
      {
        dstr.erase (0, dstr.find ("Entering", 0) + 20);
        dstr = dstr.substr (0, dstr.find ("'", 0));
      }
      else
        dstr = openldev->editor->build_dir;

      openldev_open_file (openldev, g_strconcat (dstr.c_str(), "/", temp.c_str(), NULL));
        
      // Move the cursor to the line of the error & scroll to the cursor
      buffer = GTK_TEXT_BUFFER (file_manager_get_current_buffer (FILE_MANAGER (openldev->files)));
      gtk_text_buffer_get_iter_at_line (buffer, &titer, linenumber);
      gtk_text_buffer_move_mark (buffer, gtk_text_buffer_get_insert (buffer), &titer);
      gtk_text_buffer_move_mark (buffer, gtk_text_buffer_get_selection_bound (buffer), &titer);
      file_manager_scroll_to_mark (FILE_MANAGER (openldev->files));
      gtk_widget_grab_focus (GTK_WIDGET (file_manager_get_current_view 
                             (FILE_MANAGER (openldev->files))));
      g_free (dir);
    }
      
    g_free (file);
  }
}

void 
openldev_jump_to_search_output (GtkMenuItem *item, 
                                OpenLDev *openldev)
{ 
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;
  gchar *file;
  string temp;
  gint linenumber;
  GtkTextIter titer;
  GtkTextBuffer *buffer;

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (OUTPUT (openldev->output)->search));
  if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
    gtk_tree_model_get (model, &iter, 0, &file, -1);
      
    // Open the file the error was found in
    temp = file;
    linenumber = openldev_get_file_name_and_line (temp) - 1;
    file = g_strconcat (openldev_project_settings_get_location (openldev->project), 
                        temp.c_str(), NULL);
    openldev_open_file (openldev, file);
    
    // Move the cursor to the line of the error & scroll to the cursor
    buffer = GTK_TEXT_BUFFER (file_manager_get_current_buffer (FILE_MANAGER (openldev->files)));
    gtk_text_buffer_get_iter_at_line (buffer, &titer, linenumber);
    gtk_text_buffer_move_mark (buffer, gtk_text_buffer_get_insert (buffer), &titer);
    gtk_text_buffer_move_mark (buffer, gtk_text_buffer_get_selection_bound( buffer), &titer);
    file_manager_scroll_to_mark (FILE_MANAGER (openldev->files));
    gtk_widget_grab_focus (GTK_WIDGET (file_manager_get_current_view 
                           (FILE_MANAGER (openldev->files))));
     
    g_free (file);
  }
}

// Menu handler functions and signal handlers that do nothing but redirect

void openldev_refresh_file_browser (GtkMenuItem *item, OpenLDev *openldev)
{ 
  file_browser_refresh (FILE_BROWSER (sidebar_get_file_browser (SIDEBAR (openldev->sidebar)))); 
}

void openldev_clear_build_output (GtkMenuItem *item, OpenLDev *openldev)
{ 
  output_build_clear (OUTPUT_BUILD (OUTPUT (openldev->output)->build)); 
}

void openldev_clear_search_output (GtkMenuItem *item, OpenLDev *openldev)
{
  output_list_clear (LIST (OUTPUT (openldev->output)->search)); 
}

void openldev_on_find_and_replace (GtkWidget *widget, OpenLDev *openldev)
{ openldev_find_and_replace (openldev); }

void openldev_copy_output (GtkMenuItem *item, OpenLDev *openldev)
{ openldev_menu_edit_copy (openldev); }

void openldev_file_close_project (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_file_close_project (openldev); }

void openldev_file_import_project (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_file_import_project (openldev); }

void openldev_file_new_file (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_file_new_file (openldev); }

void openldev_file_new_project (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_file_new_project (openldev); }

void openldev_file_open (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_file_open (openldev); }

void openldev_file_open_project (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_file_open_project (openldev); }

void openldev_file_revert (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_file_revert (openldev); }

void openldev_file_save (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_file_save (openldev); }

void openldev_file_save_all (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_file_save_all (openldev); }

void openldev_file_save_as (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_file_save_as (openldev); }

void openldev_file_print (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_file_print (openldev); }

void openldev_file_print_preview (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_file_print_preview (openldev); }

void openldev_edit_copy (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_edit_copy (openldev); }

void openldev_edit_cut (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_edit_cut (openldev); }

void openldev_edit_deselect (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_edit_deselect (openldev); }

void openldev_edit_paste (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_edit_paste (openldev); }

void openldev_edit_preferences (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_edit_preferences (openldev); }

void openldev_edit_redo (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_edit_redo (openldev); }

void openldev_edit_select_all (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_edit_select_all (openldev); }

void openldev_edit_undo (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_edit_undo (openldev); }

void openldev_edit_find (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_edit_find (openldev); }

void openldev_edit_find_next (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_edit_find_next (openldev); }

void openldev_edit_goto_line (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_edit_goto_line (openldev); }

void openldev_edit_indent (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_edit_indent (openldev); }

void openldev_edit_unindent (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_edit_unindent (openldev); }

void openldev_project_info (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_project_info (openldev); }

void openldev_project_preferences (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_project_preferences (openldev); }

void openldev_build_auto_generate (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_build_auto_generate (openldev); }

void openldev_build_clean (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_build_clean (openldev); }

void openldev_build_compile (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_build_compile (openldev); }

void openldev_build_configure (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_build_configure (openldev); }

void openldev_build_execute (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_build_execute (openldev); }

void openldev_build_project (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_build_project (openldev); }

void openldev_build_stop (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_build_stop (openldev); }

void openldev_view_browser (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_view_browser (openldev); }

void openldev_view_line_numbering (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_view_line_numbering (openldev); }

void openldev_view_output (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_view_output (openldev); }

void openldev_view_statusbar (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_view_statusbar (openldev); }

void openldev_view_toolbar (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_view_toolbar (openldev); }

void openldev_help_about (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_help_about (openldev); }

void openldev_help_documentation (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_help_documentation (openldev); }

void openldev_help_report_bug (GtkMenuItem *menuitem, OpenLDev *openldev)
{ openldev_menu_help_report_bug (openldev); }
