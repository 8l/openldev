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

#include "openldev-environment.h"
#include <cstdlib>

static void plugin_toggled (GtkCellRendererToggle*, gchar*, EnvironmentDialog*);
static void plugin_chosen (GtkTreeSelection*, gpointer);
static void openldev_environment_initialize_data (EnvironmentDialog*);
static void openldev_environment_initialize_plugins (EnvironmentDialog*);
static void openldev_environment_connect_signals (EnvironmentDialog*);
static void openldev_environment_close_clicked (GtkButton*, EnvironmentDialog*);

static void openldev_environment_scroll_output_changed (GtkToggleButton*, EnvironmentDialog*);
static void openldev_environment_scroll_keystroke_changed (GtkToggleButton*, EnvironmentDialog*);
static void openldev_environment_replace_tabs_changed (GtkToggleButton*, EnvironmentDialog*);
static void openldev_environment_autoindent_changed (GtkToggleButton*, EnvironmentDialog*);
static void openldev_environment_load_on_launch_changed (GtkToggleButton*, EnvironmentDialog*);
static void openldev_environment_wrap_changed (GtkToggleButton*, EnvironmentDialog*);
static void openldev_environment_split_words_changed (GtkToggleButton*, EnvironmentDialog*);
static void openldev_environment_bracket_changed (GtkToggleButton*, EnvironmentDialog*);
static void openldev_environment_right_margin_changed (GtkToggleButton*, EnvironmentDialog*);
static void openldev_environment_tab_size_changed (GtkSpinButton*, EnvironmentDialog*);
static void openldev_environment_autosave_changed (GtkSpinButton*, EnvironmentDialog*);
static void openldev_environment_scrollback_changed (GtkSpinButton*, EnvironmentDialog*);
static void openldev_environment_undo_actions_changed (GtkSpinButton*, EnvironmentDialog*);
static void openldev_environment_error_changed (GtkColorButton*, EnvironmentDialog*);
static void openldev_environment_warning_changed (GtkColorButton*, EnvironmentDialog*);
static void openldev_environment_terminal_fg_changed (GtkColorButton*, EnvironmentDialog*);
static void openldev_environment_terminal_bg_changed (GtkColorButton*, EnvironmentDialog*);
static void openldev_environment_font_changed (GtkFontButton*, EnvironmentDialog*);
static void openldev_environment_output_font_changed (GtkFontButton*, EnvironmentDialog*);
static void openldev_environment_terminal_font_changed (GtkFontButton*, EnvironmentDialog*);
static gboolean openldev_environment_shell_changed (GtkEntry*, GdkEventFocus*, EnvironmentDialog*);

// Create the environment settings dialog and make sure all of fonts are correct.
void
openldev_env_dialog_new (OpenLDevPluginList *list,
                         EnvironmentSettings *settings)
{
  static EnvironmentDialog *env = NULL;
  gchar *gladefile;
  
  if (env == NULL)
  {
    env = g_slice_new (EnvironmentDialog);
    gladefile = openldev_add_prefix ("/share/openldev/glade/environment.glade");
    env->list = list;
    env->settings = settings;
  
    env->xml = glade_xml_new (gladefile, NULL, NULL);
    env->autoindent = glade_xml_get_widget (env->xml, "auto_indent");
    env->autosave = glade_xml_get_widget (env->xml, "auto_save");
    env->bracket = glade_xml_get_widget (env->xml, "highlight_brackets");
    env->dialog = glade_xml_get_widget (env->xml, "dialog");
    env->error = glade_xml_get_widget (env->xml, "error_color");
    env->fontbutton = glade_xml_get_widget (env->xml, "fontbutton");
    env->load_on_launch = glade_xml_get_widget (env->xml, "load_on_launch");
    env->replace_tabs = glade_xml_get_widget (env->xml, "insert_spaces");
    env->right_margin = glade_xml_get_widget (env->xml, "display_margin");
    env->split_words = glade_xml_get_widget (env->xml, "split_words");
    env->tab_size = glade_xml_get_widget (env->xml, "tab_width");
    env->undo_actions = glade_xml_get_widget (env->xml, "undo_actions");
    env->warning = glade_xml_get_widget (env->xml, "warning_color");
    env->wrap = glade_xml_get_widget (env->xml, "text_wrapping");
    env->name = glade_xml_get_widget (env->xml, "name");
    env->author = glade_xml_get_widget (env->xml, "author");
    env->description = glade_xml_get_widget (env->xml, "description");
    env->plugins = glade_xml_get_widget (env->xml, "plugins");
    env->close = glade_xml_get_widget (env->xml, "button_close");
    env->fontbutton2 = glade_xml_get_widget (env->xml, "fontbutton2");
    env->terminal_font = glade_xml_get_widget (env->xml, "terminal_font");
    env->terminal_fg = glade_xml_get_widget (env->xml, "terminal_fg");
    env->terminal_bg = glade_xml_get_widget (env->xml, "terminal_bg");
    env->shell = glade_xml_get_widget (env->xml, "shell");
    env->scrollback = glade_xml_get_widget (env->xml, "scrollback");
    env->scroll_keystroke = glade_xml_get_widget (env->xml, "scroll_keystroke");
    env->scroll_output = glade_xml_get_widget (env->xml, "scroll_output");
  
    gtk_spin_button_set_range (GTK_SPIN_BUTTON (env->scrollback), 0.0, 10000.0);
    openldev_environment_connect_signals (env);
    openldev_environment_initialize_plugins (env);
  }
  
  openldev_environment_initialize_data (env);
  gtk_widget_show_all (env->dialog);
}

static void 
openldev_environment_initialize_data (EnvironmentDialog *env)
{
  gchar *fontname, *warning, *error, *font, *fg, *bg, *termfont, *outfont, *shell;
  gint int_value;
  gboolean bool_value;
  
  g_object_get (env->settings, "auto-save", &int_value, NULL);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (env->autosave), int_value);
  g_object_get (env->settings, "undo-actions", &int_value, NULL);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (env->undo_actions), int_value);
  g_object_get (env->settings, "tab-size", &int_value, NULL);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (env->tab_size), int_value);
  g_object_get (env->settings, "terminal-scrollback", &int_value, NULL);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (env->scrollback), int_value);

  g_object_get (env->settings, "auto-indent", &bool_value, NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (env->autoindent), bool_value);
  g_object_get (env->settings, "brackets", &bool_value, NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (env->bracket), bool_value);
  g_object_get (env->settings, "auto-load", &bool_value, NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (env->load_on_launch), bool_value);
  g_object_get (env->settings, "replace-tabs", &bool_value, NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (env->replace_tabs), bool_value);
  g_object_get (env->settings, "right-margin", &bool_value, NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (env->right_margin), bool_value);
  g_object_get (env->settings, "wrap-split", &bool_value, NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (env->split_words), bool_value);
  g_object_get (env->settings, "wrap-enable", &bool_value, NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (env->wrap), bool_value);
  g_object_get (env->settings, "scroll-keystroke", &bool_value, NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (env->scroll_keystroke), bool_value);
  g_object_get (env->settings, "scroll-output", &bool_value, NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (env->scroll_output), bool_value);

  GdkColor color = { 0, 0, 0, 0, }, color2 = { 0, 0, 0, 0, },
           color3 = { 0, 0, 0, 0, }, color4 = { 0, 0, 0, 0, };
  
  g_object_get (env->settings, "warning-color", &warning, "error-color", &error, 
                "terminal-foreground", &fg, "terminal-background", &bg, NULL);

  gdk_color_parse (error, &color);
  gdk_color_parse (warning, &color2);
  gdk_color_parse (fg, &color3);
  gdk_color_parse (bg, &color4);
  
  gtk_color_button_set_color (GTK_COLOR_BUTTON (env->error), &color);
  gtk_color_button_set_color (GTK_COLOR_BUTTON (env->warning), &color2);
  gtk_color_button_set_color (GTK_COLOR_BUTTON (env->terminal_fg), &color3);
  gtk_color_button_set_color (GTK_COLOR_BUTTON (env->terminal_bg), &color4);

  g_object_get (env->settings, "font-face", &font, "font-size", &int_value,
                "terminal-font", &termfont, "output-font", &outfont,
                "terminal-shell", &shell, NULL);
  fontname = g_strdup_printf ("%s %i", font, int_value);
  gtk_font_button_set_font_name (GTK_FONT_BUTTON (env->fontbutton), fontname);
  gtk_font_button_set_font_name (GTK_FONT_BUTTON (env->fontbutton2), outfont);
  gtk_font_button_set_font_name (GTK_FONT_BUTTON (env->terminal_font), termfont);
  gtk_entry_set_text (GTK_ENTRY (env->shell), shell);
  
  g_free (fontname);
  g_free (font);
  g_free (warning);
  g_free (error);
  g_free (termfont);
  g_free (outfont);
  g_free (shell);
  g_free (fg);
  g_free (bg);
}

static void 
openldev_environment_initialize_plugins (EnvironmentDialog *env)
{
  GtkCellRenderer *renderer;
  GtkTreeSelection *selection;
  GtkTreeIter row;
  GtkTreeModel *model;
  guint i;
  
  model = GTK_TREE_MODEL (gtk_tree_store_new (4, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, 
                                              G_TYPE_BOOLEAN, G_TYPE_STRING));
  gtk_tree_view_set_model (GTK_TREE_VIEW (env->plugins), model);

  // Create the columns
  renderer = gtk_cell_renderer_toggle_new ();
  g_object_set (renderer, "xalign", 0.5, NULL);
  g_signal_connect (renderer, "toggled", G_CALLBACK (plugin_toggled), env);
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (env->plugins), 0, "Enabled",
							      renderer, "active", 0, "visible", 1, "activatable", 2, NULL);

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (env->plugins), 
                                   1, "Plugin Name", renderer, "text", 3, NULL);
  
  // Make sure to check whether or not to set the target active
  for (i = 0; i < env->list->plugins.size(); i++)
  {
    gtk_tree_store_append (GTK_TREE_STORE (model), &row, NULL);
    gtk_tree_store_set (GTK_TREE_STORE (model), &row, 
                        0, env->list->plugins[i]->enabled, 1, TRUE, 2, TRUE, 
                        3, env->list->plugins[i]->name.c_str(), -1);
  }
  
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (env->plugins));
  g_signal_connect (G_OBJECT (selection), "changed",
                    G_CALLBACK (plugin_chosen), (gpointer) env);

  gtk_tree_model_get_iter (model, &row, gtk_tree_path_new_from_string("0"));
  gtk_tree_selection_select_iter (selection, &row);
  g_object_unref (model);
}

// Connect the signals for all of the widgets
static void 
openldev_environment_connect_signals (EnvironmentDialog *env)
{
  g_signal_connect (G_OBJECT (env->tab_size), "value-changed", G_CALLBACK 
                    (openldev_environment_tab_size_changed), env);
  g_signal_connect (G_OBJECT (env->autosave), "value-changed", G_CALLBACK 
                    (openldev_environment_autosave_changed), env);
  g_signal_connect (G_OBJECT (env->undo_actions), "value-changed", G_CALLBACK 
                    (openldev_environment_undo_actions_changed), env);
  g_signal_connect (G_OBJECT (env->scrollback), "value-changed", G_CALLBACK 
                    (openldev_environment_scrollback_changed), env);
  g_signal_connect (G_OBJECT (env->replace_tabs), "toggled", G_CALLBACK 
                    (openldev_environment_replace_tabs_changed), env);
  g_signal_connect (G_OBJECT (env->autoindent), "toggled", G_CALLBACK 
                    (openldev_environment_autoindent_changed), env);
  g_signal_connect (G_OBJECT (env->load_on_launch), "toggled", G_CALLBACK 
                    (openldev_environment_load_on_launch_changed), env);
  g_signal_connect (G_OBJECT (env->wrap), "toggled", G_CALLBACK 
                    (openldev_environment_wrap_changed), env);
  g_signal_connect (G_OBJECT (env->split_words), "toggled", G_CALLBACK 
                    (openldev_environment_split_words_changed), env);
  g_signal_connect (G_OBJECT (env->bracket), "toggled", G_CALLBACK 
                    (openldev_environment_bracket_changed), env);
  g_signal_connect (G_OBJECT (env->right_margin), "toggled", G_CALLBACK 
                    (openldev_environment_right_margin_changed), env);
  g_signal_connect (G_OBJECT (env->scroll_output), "toggled", G_CALLBACK 
                    (openldev_environment_scroll_output_changed), env);
  g_signal_connect (G_OBJECT (env->scroll_keystroke), "toggled", G_CALLBACK 
                    (openldev_environment_scroll_keystroke_changed), env);
  g_signal_connect (G_OBJECT (env->fontbutton), "font-set", G_CALLBACK 
                    (openldev_environment_font_changed), env);
  g_signal_connect (G_OBJECT (env->fontbutton2), "font-set", G_CALLBACK 
                    (openldev_environment_output_font_changed), env);
  g_signal_connect (G_OBJECT (env->terminal_font), "font-set", G_CALLBACK 
                    (openldev_environment_terminal_font_changed), env);
  g_signal_connect (G_OBJECT (env->error), "color-set", G_CALLBACK 
                    (openldev_environment_error_changed), env);
  g_signal_connect (G_OBJECT (env->warning), "color-set", G_CALLBACK 
                    (openldev_environment_warning_changed), env);
  g_signal_connect (G_OBJECT (env->terminal_fg), "color-set", G_CALLBACK 
                    (openldev_environment_terminal_fg_changed), env);
  g_signal_connect (G_OBJECT (env->terminal_bg), "color-set", G_CALLBACK 
                    (openldev_environment_terminal_bg_changed), env);
  g_signal_connect (G_OBJECT (env->close), "clicked", G_CALLBACK 
                    (openldev_environment_close_clicked), env);
  g_signal_connect (G_OBJECT (env->shell), "focus-out-event", G_CALLBACK 
                    (openldev_environment_shell_changed), env);
}

static void
openldev_environment_close_clicked (GtkButton *close,
                                    EnvironmentDialog *env)
{
  gtk_widget_hide_all (env->dialog);
}

static gchar* 
openldev_environment_dialog_get_color_string (GdkColor &color)
{
  gchar blue[3], green[3], red[3];
  snprintf (blue, sizeof(blue), "%X", color.blue);
  snprintf (red, sizeof(red), "%X", color.red);
  snprintf (green, sizeof(green), "%X", color.green);
  
  gchar *clr = g_strconcat ("#", red, NULL);
  if (color.red == 0)
    clr = g_strconcat (clr, "0", NULL);
  clr = g_strconcat (clr, green, NULL);
  if (color.green == 0)
    clr = g_strconcat (clr, "0", NULL);
  clr = g_strconcat (clr, blue, NULL);
  if (color.blue == 0)
    clr = g_strconcat (clr, "0", NULL);
  
  return clr;
}

static void 
openldev_environment_error_changed (GtkColorButton *widget,
                                    EnvironmentDialog *env)
{
  GdkColor color;
  gchar *str;
  
  gtk_color_button_get_color (widget, &color);
  str = openldev_environment_dialog_get_color_string (color);
  g_object_set (env->settings, "error-color", str, NULL);
}

static void 
openldev_environment_warning_changed (GtkColorButton *widget,
                                      EnvironmentDialog *env)
{
  GdkColor color;
  gchar *str;
  
  gtk_color_button_get_color (widget, &color);
  str = openldev_environment_dialog_get_color_string (color);
  g_object_set (env->settings, "warning-color", str, NULL);
}

static void 
openldev_environment_terminal_fg_changed (GtkColorButton *widget,
                                          EnvironmentDialog *env)
{
  GdkColor color;
  gchar *str;
  
  gtk_color_button_get_color (widget, &color);
  str = openldev_environment_dialog_get_color_string (color);
  g_object_set (env->settings, "terminal-foreground", str, NULL);
}

static void 
openldev_environment_terminal_bg_changed (GtkColorButton *widget,
                                          EnvironmentDialog *env)
{
  GdkColor color;
  gchar *str;
  
  gtk_color_button_get_color (widget, &color);
  str = openldev_environment_dialog_get_color_string (color);
  g_object_set (env->settings, "terminal-background", str, NULL);
}

static void 
openldev_environment_font_changed (GtkFontButton *widget,
                                   EnvironmentDialog *env)
{
  string size = gtk_font_button_get_font_name (widget);
  string name = "";
  gint i;
  
  i = size.find (" ", 0);
  while (i != string::npos)
  {
    name += size.substr (0, i + 1);
    size.erase (0, size.find (" ", 0) + 1);
    i = size.find (" ", 0);
  }
  
  name[name.length()-1] = 0;
  g_object_set (env->settings, "font-face", (gchar*) name.c_str(), NULL);
  g_object_set (env->settings, "font-size", atoi (size.c_str()), NULL);
}

static void 
openldev_environment_output_font_changed (GtkFontButton *widget,
                                          EnvironmentDialog *env)
{
  const gchar *size = gtk_font_button_get_font_name (widget);
  g_object_set (env->settings, "output-font", size, NULL);
}

static void 
openldev_environment_terminal_font_changed (GtkFontButton *widget,
                                            EnvironmentDialog *env)
{
  const gchar *size = gtk_font_button_get_font_name (widget);
  g_object_set (env->settings, "terminal-font", size, NULL);
}

static void 
openldev_environment_scroll_output_changed (GtkToggleButton *togglebutton,
                                            EnvironmentDialog *env)
{
  gboolean value = gtk_toggle_button_get_active (togglebutton);
  g_object_set (env->settings, "scroll-output", value, NULL);
}

static void 
openldev_environment_scroll_keystroke_changed (GtkToggleButton *togglebutton,
                                               EnvironmentDialog *env)
{
  gboolean value = gtk_toggle_button_get_active (togglebutton);
  g_object_set (env->settings, "scroll-keystroke", value, NULL);
}

static void 
openldev_environment_replace_tabs_changed (GtkToggleButton *togglebutton,
                                           EnvironmentDialog *env)
{
  gboolean value = gtk_toggle_button_get_active (togglebutton);
  g_object_set (env->settings, "replace-tabs", value, NULL);
}

static void 
openldev_environment_autoindent_changed (GtkToggleButton *togglebutton,
                                         EnvironmentDialog *env)
{
  gboolean value = gtk_toggle_button_get_active (togglebutton);
  g_object_set (env->settings, "auto-indent", value, NULL);
}

static void 
openldev_environment_load_on_launch_changed (GtkToggleButton *togglebutton,
                                             EnvironmentDialog *env)
{
  gboolean value = gtk_toggle_button_get_active (togglebutton);
  g_object_set (env->settings, "auto-load", value, NULL);
}

static void 
openldev_environment_wrap_changed (GtkToggleButton *togglebutton,
                                   EnvironmentDialog *env)
{
  gboolean value = gtk_toggle_button_get_active (togglebutton);
  g_object_set (env->settings, "wrap-enable", value, NULL);
}

static void 
openldev_environment_split_words_changed (GtkToggleButton *togglebutton,
                                          EnvironmentDialog *env)
{
  gboolean value = gtk_toggle_button_get_active (togglebutton);
  g_object_set (env->settings, "wrap-split", value, NULL);
}

static void 
openldev_environment_bracket_changed (GtkToggleButton *togglebutton,
                                      EnvironmentDialog *env)
{
  gboolean value = gtk_toggle_button_get_active (togglebutton);
  g_object_set (env->settings, "brackets", value, NULL);
}

static void 
openldev_environment_right_margin_changed (GtkToggleButton *togglebutton,
                                           EnvironmentDialog *env)
{
  gboolean value = gtk_toggle_button_get_active (togglebutton);
  g_object_set (env->settings, "right-margin", value, NULL);
}

static void 
openldev_environment_scrollback_changed (GtkSpinButton *spinbutton,
                                         EnvironmentDialog *env)
{
  gint value = gtk_spin_button_get_value_as_int (spinbutton);
  g_object_set (env->settings, "terminal-scrollback", value, NULL);
}

static void 
openldev_environment_tab_size_changed (GtkSpinButton *spinbutton,
                                       EnvironmentDialog *env)
{
  gint value = gtk_spin_button_get_value_as_int (spinbutton);
  g_object_set (env->settings, "tab-size", value, NULL);
}

static void 
openldev_environment_autosave_changed (GtkSpinButton *spinbutton,
                                       EnvironmentDialog *env)
{
  gint value = gtk_spin_button_get_value_as_int (spinbutton);
  g_object_set (env->settings, "auto-save", value, NULL);
}

static void 
openldev_environment_undo_actions_changed (GtkSpinButton *spinbutton,
                                           EnvironmentDialog *env)
{
  gint value = gtk_spin_button_get_value_as_int (spinbutton);
  g_object_set (env->settings, "undo-actions", value, NULL);
}

static gboolean
openldev_environment_shell_changed (GtkEntry *entry,
                                    GdkEventFocus *event,
                                    EnvironmentDialog *env)
{
  const gchar *text = gtk_entry_get_text (entry);
  g_object_set (env->settings, "terminal-shell", text, NULL);
  return FALSE;
}

// A plugin was chosen from the list, so update the information about it
static void 
plugin_chosen (GtkTreeSelection *selection,
               gpointer data)
{
  EnvironmentDialog *env = (EnvironmentDialog*) data;
  GtkTreeModel *model;
  GtkTreeIter iter;
  gint i;

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
    i = atoi (gtk_tree_path_to_string (gtk_tree_model_get_path (model, &iter)));
    gtk_label_set_text (GTK_LABEL (env->name), 
                        env->list->plugins[i]->name.c_str());
    gtk_label_set_text (GTK_LABEL (env->author), 
                        env->list->plugins[i]->author.c_str());
    gtk_label_set_text (GTK_LABEL (env->description), 
                        env->list->plugins[i]->description.c_str());
  }
}

// Toggle the plugin check button because the user said to do it and perform
// the respective action
static void
plugin_toggled (GtkCellRendererToggle *renderer, 
                gchar *path_string, 
                EnvironmentDialog *env)
{
  GtkTreePath *path;
  GtkTreeModel *model;
  gint ipath, *column;
  GtkTreeIter iter;
  gboolean toggled;

  path = gtk_tree_path_new_from_string (path_string);
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (env->plugins));
  ipath = atoi (path_string);
  column = (gint*) g_object_get_data (G_OBJECT (renderer), "column");

  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_model_get (model, &iter, column, &toggled, -1);
  gtk_tree_store_set (GTK_TREE_STORE (model), &iter, column, !toggled, -1);
  gtk_tree_path_free (path);
  
  if (!toggled)
  {
    env->list->plugins[ipath]->reload (NULL);
    environment_add_plugin (env->settings, (gchar*) env->list->plugins[ipath]->file.c_str());
  }
  else
  {
    env->list->plugins[ipath]->destroy (NULL);
    environment_remove_plugin (env->settings, (gchar*) env->list->plugins[ipath]->file.c_str());
  }
  
  env->list->plugins[ipath]->enabled = !toggled;   
}
