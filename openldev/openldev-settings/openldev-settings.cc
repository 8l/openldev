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
 
#include "openldev-settings.h"

#define PATH "/apps/openldev"
#define CURRENT_VERSION "1.0"

enum 
{
  ENVIRONMENT_PROPERTIES,
  PROP_AUTO_SAVE,
  PROP_SIDEBAR_WIDTH,
  PROP_OUTPUT_HEIGHT,
  PROP_WINDOW_WIDTH,
  PROP_WINDOW_HEIGHT,
  PROP_UNDO_ACTIONS,
  PROP_TAB_SIZE,
  PROP_AUTO_INDENT,
  PROP_AUTO_LOAD,
  PROP_BRACKETS,
  PROP_LINE_NUMBERS,
  PROP_REPLACE_TABS,
  PROP_RIGHT_MARGIN,
  PROP_WRAP_ENABLE,
  PROP_WRAP_SPLIT,
  PROP_ERROR_COLOR,
  PROP_WARNING_COLOR,
  PROP_FONT_FACE,
  PROP_FONT_SIZE,
  PROP_LAST_PROJECT,
  PROP_RECENT_FILES,
  PROP_PLUGINS,
  PROP_OUTPUT_FONT,
  PROP_TERMINAL_FONT,
  PROP_TERMINAL_FG,
  PROP_TERMINAL_BG,
  PROP_TERMINAL_SHELL,
  PROP_TERMINAL_SCROLLBACK,
  PROP_SCROLL_KEYSTROKE,
  PROP_SCROLL_OUTPUT,
  PROP_FIRST_BOOT,
  PROP_SHOW_TIP
};

static void environment_class_init (EnvironmentSettingsClass *klass);
static void environment_init (EnvironmentSettings *env);
static void environment_get_property (GObject*, guint, GValue*, GParamSpec*);
static void environment_set_property (GObject*, guint, const GValue*, GParamSpec*);

GType
environment_get_type (void)
{
  static GType env_type = 0;

  if (!env_type)
  {
    static const GTypeInfo env_info =
    {
	    sizeof (EnvironmentSettingsClass),
	    NULL, NULL,
	    (GClassInitFunc) environment_class_init,
      NULL, NULL,
      sizeof (EnvironmentSettings), 0,
	    (GInstanceInitFunc) environment_init,
    };

    env_type = g_type_register_static (G_TYPE_OBJECT, "EnvironmentSettings", 
                                       &env_info, (GTypeFlags) 0);
  }

  return env_type;
}

static void
environment_class_init (EnvironmentSettingsClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  /* Override the standard functions for setting and retrieving properties. */
  gobject_class->get_property = environment_get_property;
  gobject_class->set_property = environment_set_property;

  /* Add all properties. */
  g_object_class_install_property (gobject_class, PROP_AUTO_SAVE,
                  g_param_spec_int ("auto-save", NULL, NULL, 0, 100, 10,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_SIDEBAR_WIDTH,
                  g_param_spec_int ("sidebar-width", NULL, NULL, -1, 500, 175,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_OUTPUT_HEIGHT,
                  g_param_spec_int ("output-height", NULL, NULL, -1, 500, 175,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_WINDOW_WIDTH,
                  g_param_spec_int ("window-width", NULL, NULL, -1, 3000, 800,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_WINDOW_HEIGHT,
                  g_param_spec_int ("window-height", NULL, NULL, -1, 3000, 600,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_UNDO_ACTIONS,
                  g_param_spec_int ("undo-actions", NULL, NULL, 0, 500, 25,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_TAB_SIZE,
                  g_param_spec_int ("tab-size", NULL, NULL, 1, 10, 2,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_FONT_SIZE,
                  g_param_spec_int ("font-size", NULL, NULL, 8, 72, 10,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_TERMINAL_SCROLLBACK,
                  g_param_spec_int ("terminal-scrollback", NULL, NULL, 0, 10000, 500,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_AUTO_INDENT,
              g_param_spec_boolean ("auto-indent", NULL, NULL, TRUE,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_AUTO_LOAD,
              g_param_spec_boolean ("auto-load", NULL, NULL, TRUE,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_BRACKETS,
              g_param_spec_boolean ("brackets", NULL, NULL, TRUE,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_LINE_NUMBERS,
              g_param_spec_boolean ("line-numbers", NULL, NULL, TRUE,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_REPLACE_TABS,
              g_param_spec_boolean ("replace-tabs", NULL, NULL, TRUE,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_RIGHT_MARGIN,
              g_param_spec_boolean ("right-margin", NULL, NULL, TRUE,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_WRAP_ENABLE,
              g_param_spec_boolean ("wrap-enable", NULL, NULL, TRUE,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_WRAP_SPLIT,
              g_param_spec_boolean ("wrap-split", NULL, NULL, TRUE,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_SCROLL_KEYSTROKE,
              g_param_spec_boolean ("scroll-keystroke", NULL, NULL, TRUE,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_SCROLL_OUTPUT,
              g_param_spec_boolean ("scroll-output", NULL, NULL, TRUE,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_FIRST_BOOT,
              g_param_spec_boolean ("first-boot", NULL, NULL, TRUE,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_SHOW_TIP,
              g_param_spec_boolean ("show-tip", NULL, NULL, TRUE,
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_ERROR_COLOR,
               g_param_spec_string ("error-color", NULL, NULL, "#00CC00",
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_WARNING_COLOR,
               g_param_spec_string ("warning-color", NULL, NULL, "#CC00CC",
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_FONT_FACE,
               g_param_spec_string ("font-face", NULL, NULL, "Monospace",
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_LAST_PROJECT,
               g_param_spec_string ("last-project", NULL, NULL, "NONE",
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_OUTPUT_FONT,
               g_param_spec_string ("output-font", NULL, NULL, "Monospace 9",
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_TERMINAL_FONT,
               g_param_spec_string ("terminal-font", NULL, NULL, "Monospace 9",
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_TERMINAL_FG,
               g_param_spec_string ("terminal-foreground", NULL, NULL, "#FFFFFF",
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_TERMINAL_BG,
               g_param_spec_string ("terminal-background", NULL, NULL, "#000000",
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_TERMINAL_SHELL,
               g_param_spec_string ("terminal-shell", NULL, NULL, "/bin/bash",
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_RECENT_FILES,
              g_param_spec_pointer ("recent-files", NULL, NULL, 
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
  
  g_object_class_install_property (gobject_class, PROP_PLUGINS,
              g_param_spec_pointer ("plugin-list", NULL, NULL, 
                                    (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)));
}

static void
environment_init (EnvironmentSettings *env)
{
  env->client = gconf_client_get_default ();
  
  if (!gconf_client_dir_exists (env->client, PATH, NULL))
    gconf_client_add_dir (env->client, PATH, GCONF_CLIENT_PRELOAD_NONE, NULL);
}

EnvironmentSettings*
environment_new ()
{
  return ENVIRONMENT (g_object_new (environment_get_type (), NULL));
}

static int 
environment_get_int (EnvironmentSettings *env,
                     gchar *key)
{ 
  return gconf_client_get_int (env->client, key, NULL);
}

static gboolean 
environment_get_boolean (EnvironmentSettings *env, 
                         gchar *key)
{ 
  return gconf_client_get_bool (env->client, key, NULL);
}

static gchar* 
environment_get_string (EnvironmentSettings *env, 
                        gchar *key)
{
  return gconf_client_get_string (env->client, key, NULL);
}

static GSList* 
environment_get_list (EnvironmentSettings *env, 
                      gchar *key)
{
  return gconf_client_get_list (env->client, key, GCONF_VALUE_STRING, NULL);
}

/* This function is called when the programmer requests the value of a widget
 * property with g_object_get(). */
static void
environment_get_property (GObject *object,
                          guint prop_id,
                          GValue *value,
                          GParamSpec *pspec)
{
  EnvironmentSettings *env = ENVIRONMENT (object);

  switch (prop_id)
  {
    case PROP_AUTO_SAVE:
      if (!gconf_client_get (env->client, "/apps/openldev/auto_save", NULL))
        g_value_set_int (value, 10);
      else
        g_value_set_int (value, environment_get_int (env, "/apps/openldev/auto_save"));
      break;
    case PROP_SIDEBAR_WIDTH:
      if (!gconf_client_get (env->client, "/apps/openldev/sidebar_width", NULL))
        g_value_set_int (value, 175);
      else
        g_value_set_int (value, environment_get_int (env, "/apps/openldev/sidebar_width"));
      break;
    case PROP_OUTPUT_HEIGHT:
      if (!gconf_client_get (env->client, "/apps/openldev/output_height", NULL))
        g_value_set_int (value, 175);
      else
        g_value_set_int (value, environment_get_int (env, "/apps/openldev/output_height"));
      break;
    case PROP_WINDOW_WIDTH:
      if (!gconf_client_get (env->client, "/apps/openldev/window_width", NULL))
        g_value_set_int (value, 800);
      else
        g_value_set_int (value, environment_get_int (env, "/apps/openldev/window_width"));
      break;
    case PROP_WINDOW_HEIGHT:
      if (!gconf_client_get (env->client, "/apps/openldev/window_height", NULL))
        g_value_set_int (value, 600);
      else
        g_value_set_int (value, environment_get_int (env, "/apps/openldev/window_height"));
      break;
    case PROP_UNDO_ACTIONS:
      if (!gconf_client_get (env->client, "/apps/openldev/undo_actions", NULL))
        g_value_set_int (value, 25);
      else
        g_value_set_int (value, environment_get_int (env, "/apps/openldev/undo_actions"));
      break;
    case PROP_TAB_SIZE:
      if (!gconf_client_get (env->client, "/apps/openldev/tab_size", NULL))
        g_value_set_int (value, 2);
      else
        g_value_set_int (value, environment_get_int (env, "/apps/openldev/tab_size"));
      break;
    case PROP_AUTO_INDENT:
      if (!gconf_client_get (env->client, "/apps/openldev/auto_indent", NULL))
        g_value_set_boolean (value, TRUE);
      else
        g_value_set_boolean (value, environment_get_boolean (env, "/apps/openldev/auto_indent"));
      break;
    case PROP_AUTO_LOAD:
      if (!gconf_client_get (env->client, "/apps/openldev/auto_load", NULL))
        g_value_set_boolean (value, TRUE);
      else
        g_value_set_boolean (value, environment_get_boolean (env, "/apps/openldev/auto_load"));
      break;
    case PROP_BRACKETS:
      if (!gconf_client_get (env->client, "/apps/openldev/brackets", NULL))
        g_value_set_boolean (value, TRUE);
      else
        g_value_set_boolean (value, environment_get_boolean (env, "/apps/openldev/brackets"));
      break;
    case PROP_LINE_NUMBERS:
      if (!gconf_client_get (env->client, "/apps/openldev/line_numbers", NULL))
        g_value_set_boolean (value, TRUE);
      else
        g_value_set_boolean (value, environment_get_boolean (env, "/apps/openldev/line_numbers"));
      break;
    case PROP_REPLACE_TABS:
      if (!gconf_client_get (env->client, "/apps/openldev/replace_tabs", NULL))
        g_value_set_boolean (value, FALSE);
      else
        g_value_set_boolean (value, environment_get_boolean (env, "/apps/openldev/replace_tabs"));
      break;
    case PROP_RIGHT_MARGIN:
      if (!gconf_client_get (env->client, "/apps/openldev/right_margin", NULL))
        g_value_set_boolean (value, TRUE);
      else
        g_value_set_boolean (value, environment_get_boolean (env, "/apps/openldev/right_margin"));
      break;
    case PROP_WRAP_ENABLE:
      if (!gconf_client_get (env->client, "/apps/openldev/wrap_enable", NULL))
        g_value_set_boolean (value, FALSE);
      else
        g_value_set_boolean (value, environment_get_boolean (env, "/apps/openldev/wrap_enable"));
      break;
    case PROP_WRAP_SPLIT:
      if (!gconf_client_get (env->client, "/apps/openldev/wrap_split", NULL))
        g_value_set_boolean (value, FALSE);
      else
        g_value_set_boolean (value, environment_get_boolean (env, "/apps/openldev/wrap_split"));
      break;
    case PROP_FIRST_BOOT:
      gchar *boot_string;
      if (!gconf_client_get (env->client, "/apps/openldev/first_boot", NULL))
      {
        g_value_set_boolean (value, TRUE);
        break;
      }
      
      boot_string = environment_get_string (env, "/apps/openldev/first_boot");
      if (g_ascii_strcasecmp (boot_string, CURRENT_VERSION) == 0)
        g_value_set_boolean (value, FALSE);
      else
        g_value_set_boolean (value, TRUE);
      
      g_free (boot_string);
      break;
    case PROP_SHOW_TIP:
      if (!gconf_client_get (env->client, "/apps/openldev/show_tip", NULL))
        g_value_set_boolean (value, TRUE);
      else
        g_value_set_boolean (value, environment_get_boolean (env, "/apps/openldev/show_tip"));
      break;
    case PROP_ERROR_COLOR:
      if (!gconf_client_get (env->client, "/apps/openldev/error_color", NULL))
        g_value_set_string (value, "#FF0000");
      else
        g_value_set_string (value, environment_get_string (env, "/apps/openldev/error_color"));
      break;
    case PROP_WARNING_COLOR:
      if (!gconf_client_get (env->client, "/apps/openldev/warning_color", NULL))
        g_value_set_string (value, "#0000CC");
      else
        g_value_set_string (value, environment_get_string (env, "/apps/openldev/warning_color"));
      break;
    case PROP_FONT_FACE:
      if (!gconf_client_get (env->client, "/apps/openldev/font_face", NULL))
        g_value_set_string (value, "Monospace");
      else
        g_value_set_string (value, environment_get_string (env, "/apps/openldev/font_face"));
      break;
    case PROP_FONT_SIZE:
      if (!gconf_client_get (env->client, "/apps/openldev/font_points", NULL))
        g_value_set_int (value, 10);
      else
        g_value_set_int (value, environment_get_int (env, "/apps/openldev/font_points"));
      break;
    case PROP_LAST_PROJECT:
      if (!gconf_client_get (env->client, "/apps/openldev/last_project", NULL))
        g_value_set_string (value, "EMPTY");
      else
        g_value_set_string (value, environment_get_string (env, "/apps/openldev/last_project"));
      break;
    case PROP_RECENT_FILES:
      g_value_set_pointer (value, (gpointer) environment_get_list (env, "/apps/openldev/recent_files"));
      break;
    case PROP_PLUGINS:
      g_value_set_pointer (value, (gpointer) environment_get_list (env, "/apps/openldev/plugin_list"));
      break;
    case PROP_OUTPUT_FONT:
      if (!gconf_client_get (env->client, "/apps/openldev/output_font", NULL))
        g_value_set_string (value, "Monospace 9");
      else
        g_value_set_string (value, environment_get_string (env, "/apps/openldev/output_font"));
      break;
    case PROP_TERMINAL_FONT:
      if (!gconf_client_get (env->client, "/apps/openldev/terminal_font", NULL))
        g_value_set_string (value, "Monospace 9");
      else
        g_value_set_string (value, environment_get_string (env, "/apps/openldev/terminal_font"));
      break;
    case PROP_TERMINAL_FG:
      if (!gconf_client_get (env->client, "/apps/openldev/terminal_foreground", NULL))
        g_value_set_string (value, "#FFFFFF");
      else
        g_value_set_string (value, environment_get_string (env, "/apps/openldev/terminal_foreground"));
      break;
    case PROP_TERMINAL_BG:
      if (!gconf_client_get (env->client, "/apps/openldev/terminal_background", NULL))
        g_value_set_string (value, "#000000");
      else
        g_value_set_string (value, environment_get_string (env, "/apps/openldev/terminal_background"));
      break;
    case PROP_TERMINAL_SHELL:
      if (!gconf_client_get (env->client, "/apps/openldev/terminal_shell", NULL))
        g_value_set_string (value, "/bin/bash");
      else
        g_value_set_string (value, environment_get_string (env, "/apps/openldev/terminal_shell"));
      break;
    case PROP_TERMINAL_SCROLLBACK:
      if (!gconf_client_get (env->client, "/apps/openldev/scrollback", NULL))
        g_value_set_int (value, 500);
      else
        g_value_set_int (value, environment_get_int (env, "/apps/openldev/scrollback"));
      break;
    case PROP_SCROLL_KEYSTROKE:
      if (!gconf_client_get (env->client, "/apps/openldev/scroll_keystroke", NULL))
        g_value_set_boolean (value, TRUE);
      else
        g_value_set_boolean (value, environment_get_boolean (env, "/apps/openldev/scroll_keystroke"));
      break;
    case PROP_SCROLL_OUTPUT:
      if (!gconf_client_get (env->client, "/apps/openldev/scroll_output", NULL))
        g_value_set_boolean (value, FALSE);
      else
        g_value_set_boolean (value, environment_get_boolean (env, "/apps/openldev/scroll_output"));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

/* These three functions set values of their type and then suggest
 * as sync to GConf to make sure the changes are written. */
static void 
environment_set_boolean (EnvironmentSettings *settings,
                         gchar *key,
                         gboolean value)
{
  gconf_client_set_bool (settings->client, key, value,  NULL);
  gconf_client_suggest_sync (settings->client, NULL);
}

static void 
environment_set_string (EnvironmentSettings *settings,
                        gchar *key,
                        const gchar *value)
{
  gconf_client_set_string (settings->client, key, value,  NULL);
  gconf_client_suggest_sync (settings->client, NULL);
}

static void 
environment_set_int (EnvironmentSettings *settings, 
                     gchar *key,
                     gint value)
{
  gconf_client_set_int (settings->client, key, value,  NULL);
  gconf_client_suggest_sync (settings->client, NULL);
}

static void 
environment_set_list (EnvironmentSettings *settings, 
                      gchar *key,
                      GSList *list)
{
  gconf_client_set_list (settings->client, key, GCONF_VALUE_STRING, list,  NULL);
  gconf_client_suggest_sync (settings->client, NULL);
}

/* This function is called when the programmer gives a new value for a widget
 * property with g_object_set(). */
static void
environment_set_property (GObject *object,
                          guint prop_id,
                          const GValue *value,
                          GParamSpec *pspec)
{
  EnvironmentSettings *env = ENVIRONMENT (object);

  switch (prop_id)
  {
    case PROP_AUTO_SAVE:
      environment_set_int (env, "/apps/openldev/auto_save", g_value_get_int (value));
      break;
    case PROP_SIDEBAR_WIDTH:
      environment_set_int (env, "/apps/openldev/sidebar_width", g_value_get_int (value));
      break;
    case PROP_OUTPUT_HEIGHT:
      environment_set_int (env, "/apps/openldev/output_height", g_value_get_int (value));
      break;
    case PROP_WINDOW_WIDTH:
      environment_set_int (env, "/apps/openldev/window_width", g_value_get_int (value));
      break;
    case PROP_WINDOW_HEIGHT:
      environment_set_int (env, "/apps/openldev/window_height", g_value_get_int (value));
      break;
    case PROP_UNDO_ACTIONS:
      environment_set_int (env, "/apps/openldev/undo_actions", g_value_get_int (value));
      break;
    case PROP_TAB_SIZE:
      environment_set_int (env, "/apps/openldev/tab_size", g_value_get_int (value));
      break;
    case PROP_AUTO_INDENT:
      environment_set_boolean (env, "/apps/openldev/auto_indent", g_value_get_boolean (value));
      break;
    case PROP_AUTO_LOAD:
      environment_set_boolean (env, "/apps/openldev/auto_load", g_value_get_boolean (value));
      break;
    case PROP_BRACKETS:
      environment_set_boolean (env, "/apps/openldev/brackets", g_value_get_boolean (value));
      break;
    case PROP_LINE_NUMBERS:
      environment_set_boolean (env, "/apps/openldev/line_numbers", g_value_get_boolean (value));
      break;
    case PROP_REPLACE_TABS:
      environment_set_boolean (env, "/apps/openldev/replace_tabs", g_value_get_boolean (value));
      break;
    case PROP_RIGHT_MARGIN:
      environment_set_boolean (env, "/apps/openldev/right_margin", g_value_get_boolean (value));
      break;
    case PROP_WRAP_ENABLE:
      environment_set_boolean (env, "/apps/openldev/wrap_enable", g_value_get_boolean (value));
      break;
    case PROP_WRAP_SPLIT:
      environment_set_boolean (env, "/apps/openldev/wrap_split", g_value_get_boolean (value));
      break;
    case PROP_FIRST_BOOT:
      if (g_value_get_boolean (value))
        environment_set_string (env, "/apps/openldev/first_boot", "0.0");
      else
        environment_set_string (env, "/apps/openldev/first_boot", CURRENT_VERSION);
      break;
    case PROP_SHOW_TIP:
      environment_set_boolean (env, "/apps/openldev/show_tip", g_value_get_boolean (value));
      break;
    case PROP_ERROR_COLOR:
      environment_set_string (env, "/apps/openldev/error_color", g_value_get_string (value));
      break;
    case PROP_WARNING_COLOR:
      environment_set_string (env, "/apps/openldev/warning_color", g_value_get_string (value));
      break;
    case PROP_FONT_FACE:
      environment_set_string (env, "/apps/openldev/font_face", g_value_get_string (value));
      break;
    case PROP_FONT_SIZE:
      environment_set_int (env, "/apps/openldev/font_points", g_value_get_int (value));
      break;
    case PROP_LAST_PROJECT:
      environment_set_string (env, "/apps/openldev/last_project", g_value_get_string (value));
      break;
    case PROP_RECENT_FILES:
      environment_set_list (env, "/apps/openldev/recent_files", (GSList*) g_value_get_pointer (value));
      break;
    case PROP_PLUGINS:
      environment_set_list (env, "/apps/openldev/plugin_list", (GSList*) g_value_get_pointer (value));
      break;
    case PROP_OUTPUT_FONT:
      environment_set_string (env, "/apps/openldev/output_font", g_value_get_string (value));
      break;
    case PROP_TERMINAL_FONT:
      environment_set_string (env, "/apps/openldev/terminal_font", g_value_get_string (value));
      break;
    case PROP_TERMINAL_FG:
      environment_set_string (env, "/apps/openldev/terminal_foreground", g_value_get_string (value));
      break;
    case PROP_TERMINAL_BG:
      environment_set_string (env, "/apps/openldev/terminal_background", g_value_get_string (value));
      break;
    case PROP_TERMINAL_SHELL:
      environment_set_string (env, "/apps/openldev/terminal_shell", g_value_get_string (value));
      break;
    case PROP_TERMINAL_SCROLLBACK:
      environment_set_int (env, "/apps/openldev/scrollback", g_value_get_int (value));
      break;
    case PROP_SCROLL_KEYSTROKE:
      environment_set_boolean (env, "/apps/openldev/scroll_keystroke", g_value_get_boolean (value));
      break;
    case PROP_SCROLL_OUTPUT:
      environment_set_boolean (env, "/apps/openldev/scroll_output", g_value_get_boolean (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* Add a new recent file to the list, pushing the last one of if needed. */
void 
environment_add_recent_file (EnvironmentSettings *settings, 
                             gchar *value)
{
  GSList *list, *temp;
  gchar *new_value;
  gpointer data;

  g_object_get (settings, "recent_files", &list, NULL);
  temp = list;
  while (temp != NULL)
  {
    if (g_ascii_strcasecmp (value, (gchar*) temp->data) == 0)
      return;
    temp = temp->next;
  }
  
  new_value = g_strdup (value);
  list = g_slist_prepend (list, (gpointer) new_value);
  
  if (g_slist_length (list) > 5)
  {
    data = g_slist_nth_data (list, 5);
    list = g_slist_remove (list, data);
  }
  
  g_object_set (settings, "recent_files", list, NULL);
}

/* Retrive the short version of the chosen recent file. */
gchar* 
environment_get_recent_short (EnvironmentSettings *settings,
                              gint item)
{
  GSList *list;
  GString *temp;
  gchar *find;
  gsize len;
  
  g_object_get (settings, "recent_files", &list, NULL);
  temp = g_string_new ((gchar*) g_slist_nth_data (list, item));
  
  /* Parse through the string, splitting it into a GList at the '/' character. */
  while (temp->str[0] == '/')
  {
    while (temp->str[0] == '/')
      g_string_erase (temp, 0, 1);
    find = g_strstr_len (temp->str, temp->len, "/");
    
    if (find != NULL)
    {
      len = (gsize) (strlen (temp->str) - strlen (find));
      g_string_erase (temp, 0, len);
    }
  }

  return temp->str;
}

/* Remove an active plug-in from the list. */
void 
environment_remove_plugin (EnvironmentSettings *settings, 
                           gchar *plugin)
{
  GSList *list, *temp;

  g_object_get (settings, "plugin-list", &list, NULL);
  temp = list;
  while (temp != NULL)
  {
    if (g_ascii_strcasecmp (plugin, (gchar*) temp->data) == 0)
    {
      list = g_slist_remove (list, temp->data);
      break;
    }
    temp = temp->next;
  }
  
  g_object_set (settings, "plugin-list", list, NULL);
}

/* Add an active plug-in to the list. */
void 
environment_add_plugin (EnvironmentSettings *settings, 
                        gchar *plugin)
{
  GSList *list, *temp;
  gchar *new_value;

  g_object_get (settings, "plugin-list", &list, NULL);
  temp = list;
  while (temp != NULL)
  {
    if (g_ascii_strcasecmp (plugin, (gchar*) temp->data) == 0)
      return;
    temp = temp->next;
  }

  new_value = g_strdup (plugin);
  list = g_slist_prepend (list, (gpointer) new_value);
  g_object_set (settings, "plugin-list", list, NULL);
}

/* Check whether a plug-in is marked as active on the list. */
gboolean 
environment_get_plugin (EnvironmentSettings *settings, 
                        gchar *name)
{
  GSList *temp;
  gchar *new_value;

  g_object_get (settings, "plugin-list", &temp, NULL);
  while (temp != NULL)
  {
    if (g_ascii_strcasecmp (name, (gchar*) temp->data) == 0)
      return TRUE;
    temp = temp->next;
  }

  return FALSE;
}
