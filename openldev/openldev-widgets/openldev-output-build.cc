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

#include "openldev-output-build.h"

void erase (OutputBuild*);
void add_row (OutputBuild*, string, string);
void update_output (OutputBuild*);

static void output_build_class_init (OutputBuildClass *klass);
static void output_build_init (OutputBuild *build);

GType
output_build_get_type ()
{
  static GType build_type = 0;

  if (!build_type)
  {
    static const GTypeInfo build_info =
    {
	    sizeof (OutputBuildClass),
	    NULL, NULL,
	    (GClassInitFunc) output_build_class_init,
      NULL, NULL,
      sizeof (OutputBuild), 0,
	    (GInstanceInitFunc) output_build_init,
    };

    build_type = g_type_register_static (GTK_TYPE_TREE_VIEW, "OutputBuild", 
                                         &build_info, (GTypeFlags) 0);
  }

  return build_type;
}

static void
output_build_class_init (OutputBuildClass *klass)
{
}

static void
output_build_setup_tree_columns (GtkTreeView *treeview)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Build Results", 
                   renderer, "text", 0, "foreground-gdk", 1, "wrap-mode", 2,
                   "wrap-width", 3, NULL);
  gtk_tree_view_append_column (treeview, column);
}

static void
output_build_init (OutputBuild *build)
{

  build->swin = gtk_scrolled_window_new (NULL, NULL);
  build->channel = NULL;
  build->channelerr = NULL;
  build->messages = NULL;
  build->location = NULL;
  
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (build), FALSE);
  gtk_container_add (GTK_CONTAINER (build->swin), GTK_WIDGET (build));
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (build->swin),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  
  output_build_setup_tree_columns (GTK_TREE_VIEW (build));
}

GtkWidget*
output_build_new (EnvironmentSettings *env,
                  MenuHandle *menus)
{
  PangoFontDescription *fd;
  GtkWidget *build;
  gchar *font;
  
  build = GTK_WIDGET (g_object_new (output_build_get_type (), NULL));
  OUTPUT_BUILD (build)->env = env;
  OUTPUT_BUILD (build)->menus = menus;
  
  g_object_get (OUTPUT_BUILD (build)->env, "output-font", &font, NULL);                         
  fd = pango_font_description_from_string (font);
  gtk_widget_modify_font (build, fd);
  pango_font_description_free (fd);
  g_free (font);

  return build;
}

GtkWidget* 
output_build_get_scrolled_window (OutputBuild *build)
{
  return build->swin;
}

static void
parse_output (OutputBuild *build,
              gchar *line)
{
  gchar **lines, **ptr;
  GtkWidget *treeview;
  GtkListStore *store;
  GtkTreeIter iter;
  gchar *error, *warning;
  gint i, j;
  string text;
  
  /* Load the list store, split the string into lines and create a pointer. */
  g_object_get (build->env, "error-color", &error, "warning-color", &warning, NULL);
  store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (build)));
  lines = g_strsplit (line, "\n", -1);
  ptr = lines;
  
  /* Loop through each of the lines, parsing its content. */
  while (*ptr != NULL)
  {  
    text = *ptr;
    if (text == "")
    {
      ++ptr;
      continue;
    }
    
    /* Check for a directory move or error/warning message. */
    if (g_ascii_strncasecmp ("make[", text.c_str(), 5) == 0)
    {
      if (text.find ("Entering directory `", 0) != string::npos)
      {
        gchar *str;
      
        i = text.find ("`", 20);
        j = text.find ("'", i);
        str = g_strconcat (text.substr (i + 1, j - i - 1).c_str(), "/", NULL);
        build->location = g_list_prepend (build->location, (gpointer) str);
      }
      else if (text.find ("Leaving directory `", 0) != string::npos)
      {
        g_free (build->location->data);
        build->location = g_list_delete_link (build->location, build->location);
      }
    }
    else if (text.find ("error:", 0) != string::npos)
    {
      BuildMessage *message;
        
      message = g_slice_new (BuildMessage);
      message->type = OUTPUT_ERROR;
      message->line = openldev_get_file_name_and_line (text);
      message->location = g_strconcat ((gchar*) build->location->data, text.c_str(), NULL);
      build->messages = g_list_prepend (build->messages, (gpointer) message);
    }
    else if (text.find ("warning:", 0) != string::npos)
    {
      BuildMessage *message;
        
      message = g_slice_new (BuildMessage);
      message->type = OUTPUT_WARNING;
      message->line = openldev_get_file_name_and_line (text);
      message->location = g_strconcat ((gchar*) build->location->data, text.c_str(), NULL);
      build->messages = g_list_prepend (build->messages, (gpointer) message);
    }
    
    /* Apply Error or Warning colors. */
    text = *ptr;
    if (text.find ("error: ", 0) != string::npos ||
        (text.find ("***", 0) != string::npos && text.find ("Error ", 0) != string::npos) ||
        text.find ("undefined reference", 0) != string::npos ||
        (text.find ("***", 0) != string::npos && text.find ("Error ", 0) != string::npos))
    {
      add_row (build, text, error);
      build->successful = FALSE;
    }
    else if (text.find (": ", 0) != string::npos)
      add_row (build, text, warning);
    else
      add_row (build, text, "#000000");
      
    // Update the build_box when there is output
    update_output (build);
    
    ++ptr;
  }
  
  g_strfreev (lines);
  g_free (error);
  g_free (warning);
}

/* This is a watch function that is called when the IO channel has data to read. */
static gboolean
read_build_output (GIOChannel *channel,
		               GIOCondition condition,
		               gpointer data)
{
  gchar *line, *error_color, *warning_color;
	GError *error = NULL;
	OutputBuild *build;
	GIOStatus status;
	gsize len, term;

  /* Read the current line of data from the IO channel. */
  build = (OutputBuild*) data;
  if (channel->is_readable)
	  status = g_io_channel_read_line (channel, &line, &len, &term, &error);
	else
	  return FALSE;
	
	/* If some type of error has occurred, handle it. */
	if (status != G_IO_STATUS_NORMAL || line == NULL || error != NULL) 
	{
	  GtkAdjustment *vadj;
    gchar time[10];
    string ttime;
    
		if (error) 
		{
			g_warning ("Error reading IO channel: %s", error->message);
			g_error_free (error);
		}
    
    g_timer_stop (build->timer);
    g_object_get (build->env, "error-color", &error_color,
                  "warning-color", &warning_color, NULL);
    if (build->successful && !build->terminated)
      add_row (build, "Completed ... Successful", "#000000");
    else if (build->terminated)
      add_row (build, "Incomplete ... Terminated", error_color);
    else
      add_row (build, "Completed ... Failed", error_color);
  
    snprintf (time, sizeof (time), "%f", g_timer_elapsed (build->timer, NULL));
    ttime = time;
    if (ttime.find (".", 0) != string::npos)
      ttime= ttime.substr (0, ttime.find (".", 0));
    if (ttime == "0")
      ttime = "1";
  
    add_row (build, string ("Total time elapsed: " + ttime + " second(s)"), 
             warning_color);
  
    g_timer_destroy (build->timer);
    build->timer = NULL;
    update_output (build);
	  menu_handle_manage_build_items (build->menus, FALSE);

    if (build->channel != NULL)
      g_io_channel_shutdown (build->channel, TRUE, NULL);
    build->channel = NULL;
    
    if (build->channelerr != NULL)
      g_io_channel_shutdown (build->channelerr, TRUE, NULL);
    build->channelerr = NULL;
    
    if (build->loop != NULL)
      if (g_main_loop_is_running (build->loop))
        g_main_loop_quit (build->loop);
    
    g_free (error_color);
    g_free (warning_color);
    
		return FALSE;
	}

  /* Parse the line if an error has not occurred. */
  parse_output (build, line);  
	g_free (line);

	return TRUE;
}

// Run the specified command and print the output to the build output treeview
gboolean 
output_build_run_command (OutputBuild *build,
                          gchar *command,
                          gchar *base_dir)
{
  GtkTreeSelection *selection;
  GtkListStore *store;
  gint argc, fout, ferr, ret;
  GtkTreeIter iter;
  GList *temp;
  gchar **argv;
  
  store = gtk_list_store_new (4, G_TYPE_STRING, GDK_TYPE_COLOR, G_TYPE_INT, G_TYPE_INT);
  gtk_tree_view_set_model (GTK_TREE_VIEW (build), GTK_TREE_MODEL (store));
  g_object_unref (store);
  
  if (build->location != NULL)
  {
    g_list_foreach (build->location, (GFunc) g_free, NULL);
    g_list_free (build->location);
    build->location = NULL;
  }
  
  build->location = g_list_prepend (NULL, (gpointer) g_strdup (base_dir));
  
  if (build->messages != NULL)
  {
    for (temp = build->messages; temp != NULL; temp = temp->next)
    {
      g_free (((BuildMessage*) temp->data)->location);
      g_slice_free (BuildMessage, temp->data);
    }
    
    g_list_free (build->messages);
    build->messages = NULL;
  }
  
  build->successful = TRUE;
  build->terminated = FALSE;
  build->timer = g_timer_new ();
  g_timer_start (build->timer);
  erase (build);
  
  add_row (build, command, "#000000");
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (build));
  gtk_tree_model_get_iter (GTK_TREE_MODEL (store), &iter, gtk_tree_path_new_from_string ("0"));
  gtk_tree_selection_select_iter (selection, &iter);
  
  /* Parse the command and launch the process, monitoring standard output. */
  g_shell_parse_argv (command, &argc, &argv, NULL);
  ret = g_spawn_async_with_pipes (NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL,
                                  &build->pid, NULL, &fout, &ferr, NULL);
  
  if (!ret)
  {
    g_warning ("The fork instruction has failed!");
    return FALSE;
  }
  else
  {	  
	  /* Create a new IO channel and monitor it for data to read. */
	  menu_handle_manage_build_items (build->menus, TRUE);
	  build->channel = g_io_channel_unix_new (fout);
    g_io_add_watch (build->channel, (GIOCondition) (G_IO_IN), 
                    read_build_output, (gpointer) build);
	  g_io_channel_unref (build->channel);
	  
	  build->channelerr = g_io_channel_unix_new (ferr);
	  g_io_add_watch (build->channelerr, (GIOCondition) (G_IO_IN | G_IO_ERR | G_IO_HUP), 
                    read_build_output, (gpointer) build);
	  g_io_channel_unref (build->channelerr);
	  
	  build->loop = g_main_loop_new (NULL, FALSE);
	  g_main_loop_run (build->loop);
	  
	  if (build->loop != NULL)
	    g_main_loop_unref (build->loop);
	  build->loop = NULL;
  }
  
  g_strfreev (argv);
  return !build->terminated;
}

/* Stop any processes currently occuring. */
void 
output_build_stop_build (OutputBuild *build)
{
  if (build->pid != 0)
  {
    kill (build->pid, SIGTERM);
    g_spawn_close_pid (build->pid);
    build->terminated = TRUE;
  }
  
  if (build->loop != NULL)
    if (g_main_loop_is_running (build->loop))
      g_main_loop_quit (build->loop);
}

void 
erase (OutputBuild *build)
{
  output_build_clear (build);
}

// Add a row to the build output
void 
add_row (OutputBuild *build,
         string text, 
         string color)
{
  GtkTreeIter iter;
  GdkColor gcolor = { 0, 0, 0, 0 };
  GtkTreeModel *model;

  if (text.find ("make  all-recursive", 0) == string::npos)
  {
    gdk_color_parse (color.c_str(), &gcolor);
    model = gtk_tree_view_get_model (GTK_TREE_VIEW (build));
    gtk_list_store_append (GTK_LIST_STORE (model), &iter);
	  gtk_list_store_set (GTK_LIST_STORE (model), &iter, 
                        0, text.c_str(), 1, &gcolor, 2, PANGO_WRAP_WORD,
                        3, GTK_WIDGET(build)->allocation.width - 10, -1);
  }
}

// Update the build output to enable scrolling to the last row
void 
update_output (OutputBuild *build)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  gint rows;
  
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (build));
  rows = gtk_tree_model_iter_n_children (model, NULL);
  gtk_tree_model_iter_nth_child (model, &iter, NULL, rows - 1);
  gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (build), gtk_tree_model_get_path 
                                (model, &iter), NULL, FALSE, 0.0, 0.0);
  
  while (gtk_events_pending())
    gtk_main_iteration ();
}

void 
output_build_clear (OutputBuild *build)
{
  GtkTreeModel *model;
  
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (build));
  if (GTK_IS_LIST_STORE (model))
    gtk_list_store_clear (GTK_LIST_STORE (model));
}
