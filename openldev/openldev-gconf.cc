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

#include "openldev-gconf.h"

#define BASE "/apps/openldev"

void openldev_gconf_add_dir (GConfClient*, gchar*);
void openldev_notify_tab_size (GConfClient*, guint, GConfEntry*, gpointer);
void openldev_notify_replace_tabs (GConfClient*, guint, GConfEntry*, gpointer);
void openldev_notify_auto_indent (GConfClient*, guint, GConfEntry*, gpointer);
void openldev_notify_undo_actions (GConfClient*, guint, GConfEntry*, gpointer);
void openldev_notify_wrap (GConfClient*, guint, GConfEntry*, gpointer);
void openldev_notify_brackets (GConfClient*, guint, GConfEntry*, gpointer);
void openldev_notify_right_margin (GConfClient*, guint, GConfEntry*, gpointer);
void openldev_notify_font (GConfClient*, guint, GConfEntry*, gpointer);
void openldev_notify_output_font (GConfClient*, guint, GConfEntry*, gpointer);
void openldev_notify_terminal_font (GConfClient*, guint, GConfEntry*, gpointer);
void openldev_notify_terminal_colors (GConfClient*, guint, GConfEntry*, gpointer);
void openldev_notify_scrollback (GConfClient*, guint, GConfEntry*, gpointer);
void openldev_notify_scrolling (GConfClient*, guint, GConfEntry*, gpointer);

void 
openldev_initialize_gconf (OpenLDev *openldev)
{
  EnvironmentSettings *env;
  
  g_object_get (openldev, "env-settings", &env, NULL);
  openldev_gconf_add_dir (env->client, BASE);
  
  gconf_client_notify_add (env->client, BASE"/tab_size", openldev_notify_tab_size,
				                   openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/replace_tabs", openldev_notify_replace_tabs,
				                   (gpointer) openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/auto_indent", openldev_notify_auto_indent,
				                   (gpointer) openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/undo_actions", openldev_notify_undo_actions,
				                   (gpointer) openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/wrap_enable", openldev_notify_wrap,
				                   (gpointer) openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/wrap_split", openldev_notify_wrap,
				                   (gpointer) openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/brackets", openldev_notify_brackets,
				                   (gpointer) openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/right_margin", openldev_notify_right_margin,
				                   (gpointer) openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/font_face", openldev_notify_font,
				                   (gpointer) openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/font_points", openldev_notify_font,
				                   (gpointer) openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/output_font", openldev_notify_output_font,
				                   (gpointer) openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/terminal_font", openldev_notify_terminal_font,
				                   (gpointer) openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/terminal_background", openldev_notify_terminal_colors,
				                   (gpointer) openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/terminal_foreground", openldev_notify_terminal_colors,
				                   (gpointer) openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/scrollback", openldev_notify_scrollback,
				                   (gpointer) openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/scroll_output", openldev_notify_scrolling,
				                   (gpointer) openldev, NULL, NULL);
  gconf_client_notify_add (env->client, BASE"/scroll_keystroke", openldev_notify_scrolling,
				                   (gpointer) openldev, NULL, NULL);
}

// Add a directory to watch all of the keys
void 
openldev_gconf_add_dir (GConfClient *client,
                        gchar *dir)
{
  gconf_client_add_dir (client, dir, GCONF_CLIENT_PRELOAD_NONE, NULL);
}

void
openldev_notify_tab_size (GConfClient *client,
                          guint cnxn_id,
                          GConfEntry *entry,
                          gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  EnvironmentSettings *env;
  FileManager *files;
  gint tab_size;
  
  g_object_get (openldev, "env-settings", &env, "file-browser", &files, NULL);
  g_object_get (env, "tab-size", &tab_size, NULL);
  for (int i=0; i < files->tab_count; i++)
  {
    GtkSourceView *view = GTK_SOURCE_VIEW (files->tabs[i]);
    gtk_source_view_set_tabs_width (view, tab_size);
  }
}

void
openldev_notify_replace_tabs (GConfClient *client,
                              guint cnxn_id,
                              GConfEntry *entry,
                              gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  EnvironmentSettings *env;
  FileManager *files;
  gboolean replace_tabs;
  
  g_object_get (openldev, "env-settings", &env, "file-browser", &files, NULL);
  g_object_get (env, "replace-tabs", &replace_tabs, NULL);
  for (int i=0; i < files->tab_count; i++)
  {
    GtkSourceView *view = GTK_SOURCE_VIEW (files->tabs[i]);
    gtk_source_view_set_insert_spaces_instead_of_tabs (view, replace_tabs);
  }
}

void
openldev_notify_auto_indent (GConfClient *client,
                             guint cnxn_id,
                             GConfEntry *entry,
                             gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  EnvironmentSettings *env;
  FileManager *files;
  gboolean auto_indent;
  
  g_object_get (openldev, "env-settings", &env, "file-browser", &files, NULL);
  g_object_get (env, "auto-indent", &auto_indent, NULL);
  for (int i=0; i < files->tab_count; i++)
  {
    GtkSourceView *view = GTK_SOURCE_VIEW (files->tabs[i]);
    gtk_source_view_set_auto_indent (view, auto_indent);
  }
}

void
openldev_notify_undo_actions (GConfClient *client,
                              guint cnxn_id,
                              GConfEntry *entry,
                              gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  EnvironmentSettings *env;
  FileManager *files;
  gint undo_actions;
  
  g_object_get (openldev, "env-settings", &env, "file-browser", &files, NULL);
  g_object_get (env, "undo-actions", &undo_actions, NULL);
  for (int i=0; i < files->tab_count; i++)
  {
    GtkSourceView *view = GTK_SOURCE_VIEW (files->tabs[i]);
    GtkSourceBuffer *buffer = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer 
                                                (GTK_TEXT_VIEW (view)));
    gtk_source_buffer_set_max_undo_levels (buffer, undo_actions);
  }
}

void
openldev_notify_wrap (GConfClient *client,
                      guint cnxn_id,
                      GConfEntry *entry,
                      gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  EnvironmentSettings *env;
  FileManager *files;
  gboolean wrap, split;
  
  g_object_get (openldev, "env-settings", &env, "file-browser", &files, NULL);
  g_object_get (env, "wrap-enable", &wrap, "wrap-split", &split, NULL);
  for (int i=0; i < files->tab_count; i++)
  {
    GtkSourceView *view = GTK_SOURCE_VIEW (files->tabs[i]);
    if (wrap)
    {
      if (split)
        gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view), GTK_WRAP_CHAR);
      else
        gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view), GTK_WRAP_WORD);
    }
    else
        gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view), GTK_WRAP_NONE);
    }
}

void
openldev_notify_brackets (GConfClient *client,
                          guint cnxn_id,
                          GConfEntry *entry,
                          gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  EnvironmentSettings *env;
  FileManager *files;
  gboolean brackets;
  
  g_object_get (openldev, "env-settings", &env, "file-browser", &files, NULL);
  g_object_get (env, "brackets", &brackets, NULL);
  for (int i=0; i < files->tab_count; i++)
  {
    GtkSourceView *view = GTK_SOURCE_VIEW (files->tabs[i]);
    GtkSourceBuffer *buffer = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer 
                                                (GTK_TEXT_VIEW (view)));
    gtk_source_buffer_set_check_brackets (buffer, brackets);
  }
}

void
openldev_notify_right_margin (GConfClient *client,
                              guint cnxn_id,
                              GConfEntry *entry,
                              gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  EnvironmentSettings *env;
  FileManager *files;
  gboolean right_margin;
  
  g_object_get (openldev, "env-settings", &env, "file-browser", &files, NULL);
  g_object_get (env, "right-margin", &right_margin, NULL);
  for (int i=0; i < files->tab_count; i++)
  {
    GtkSourceView *view = GTK_SOURCE_VIEW (files->tabs[i]);
    gtk_source_view_set_show_margin (view, right_margin);
  }
}

void
openldev_notify_font (GConfClient *client,
                      guint cnxn_id,
                      GConfEntry *entry,
                      gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  EnvironmentSettings *env;
  FileManager *files;
  gchar *font, *description;
  PangoFontDescription *fd;
  gint size;
  
  g_object_get (openldev, "env-settings", &env, "file-browser", &files, NULL);
  g_object_get (env, "font-face", &font, "font-size", &size, NULL);
  description = g_strdup_printf ("%s %i", font, size);
  fd = pango_font_description_from_string (description);
  
  for (int i=0; i < files->tab_count; i++)
    gtk_widget_modify_font (GTK_WIDGET (files->tabs[i]), fd);
  
  pango_font_description_free (fd);
  g_free (description);
  g_free (font);
}

void
openldev_notify_output_font (GConfClient *client,
                             guint cnxn_id,
                             GConfEntry *entry,
                             gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  PangoFontDescription *fd;
  gchar *font;
  
  g_object_get (openldev->env, "output-font", &font, NULL);
  fd = pango_font_description_from_string (font);
  gtk_widget_modify_font (OUTPUT (openldev->output)->build, fd);
  
  pango_font_description_free (fd);
  g_free (font);
}

void
openldev_notify_terminal_font (GConfClient *client,
                               guint cnxn_id,
                               GConfEntry *entry,
                               gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  PangoFontDescription *fd;
  gchar *font;
  
  g_object_get (openldev->env, "terminal-font", &font, NULL);
  fd = pango_font_description_from_string (font);
  vte_terminal_set_font (VTE_TERMINAL (OUTPUT (openldev->output)->terminal), fd);
  
  pango_font_description_free (fd);
  g_free (font);
}

void
openldev_notify_terminal_colors (GConfClient *client,
                                 guint cnxn_id,
                                 GConfEntry *entry,
                                 gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  GdkColor front = { 0, 0, 0, 0 }, back = { 0, 0, 0, 0 };
  gchar *fg, *bg;
  
  g_object_get (openldev->env, "terminal-foreground", &fg, 
                "terminal-background", &bg, NULL);
  
  gdk_color_parse (fg, &front);
  gdk_color_parse (bg, &back);
  vte_terminal_set_colors (VTE_TERMINAL (OUTPUT (openldev->output)->terminal), 
                           &front, &back, NULL, 0);
  
  g_free (fg);
  g_free (bg);
}

void
openldev_notify_scrollback (GConfClient *client,
                            guint cnxn_id,
                            GConfEntry *entry,
                            gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  gint value;
  
  g_object_get (openldev->env, "terminal-scrollback", &value, NULL);
  vte_terminal_set_scrollback_lines (VTE_TERMINAL (OUTPUT (openldev->output)->terminal), 
                                     (glong) value);
}

void
openldev_notify_scrolling (GConfClient *client,
                           guint cnxn_id,
                           GConfEntry *entry,
                           gpointer data)
{
  OpenLDev *openldev = (OpenLDev*) data;
  gboolean output, key;
  
  g_object_get (openldev->env, "scroll-output", &output, "scroll-keystroke", &key, NULL);
  vte_terminal_set_scroll_on_output (VTE_TERMINAL (OUTPUT 
                                     (openldev->output)->terminal), output);
  vte_terminal_set_scroll_on_keystroke (VTE_TERMINAL (OUTPUT 
                                        (openldev->output)->terminal), key);
}
