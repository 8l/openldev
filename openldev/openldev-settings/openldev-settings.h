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
 
#ifndef OPENLDEV_ENVIRONMENT_H
#define OPENLDEV_ENVIRONMENT_H

/*! \file openldev-settings.h
    \brief EnvironmentSettings
*/

#include <gconf/gconf-client.h>
#include <glib-object.h>
#include <glib.h>
#include <cstdlib>
#include <cstring>

G_BEGIN_DECLS

#define ENVIRONMENT_TYPE            (environment_get_type ())
#define ENVIRONMENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), ENVIRONMENT_TYPE, EnvironmentSettings))
#define ENVIRONMENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), ENVIRONMENT_TYPE, EnvironmentSettingsClass))
#define IS_ENVIRONMENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ENVIRONMENT_TYPE))
#define IS_ENVIRONMENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ENVIRONMENT_TYPE))

typedef struct _EnvironmentSettings       EnvironmentSettings;
typedef struct _EnvironmentSettingsClass  EnvironmentSettingsClass;

struct _EnvironmentSettings
{
  GObject object;
  
  GConfClient *client;
};

struct _EnvironmentSettingsClass
{
  GObjectClass parent_class;
};

GType environment_get_type ();
EnvironmentSettings* environment_new ();

/*! Add a file that was just opened to the recent files list. If there are 5 files already on the list, the last one will be deleted.
  \param settings A non-NULL EnvironmentSettings object.
  \param file The full path of a file that is currently opened.
*/
void environment_add_recent_file (EnvironmentSettings *settings, gchar *file);

/*! Get the short file name of a plugin located at position id. This helps when trying to make a menu list of recent files.
  \param settings A non-NULL EnvironmentSettings object.
  \param id The id of the recent file to obtain, from 0 to 4.
  \return The short name of a recent file located at id.
*/
gchar* environment_get_recent_short (EnvironmentSettings *settings, gint id);

/*! Check whether a plugin is currently enabled. You must pass the short name of the plugin, not the full file name.
  \param settings A non-NULL EnvironmentSettings object.
  \param plugin A short plugin name.
  \return TRUE if the plugin is enabled.
*/
gboolean environment_get_plugin (EnvironmentSettings *settings, gchar *plugin);

/*! Remove a plugin from the list of enabled. The changes will be saved to GConf so it can be disabled in the future.
  \param settings A non-NULL EnvironmentSettings object.
  \param plugin A short plugin name.
*/
void environment_remove_plugin (EnvironmentSettings *settings, gchar *plugin);

/*! Add a plugin to the list of enabled. The changes will be saved to GConf so it can be enabled in the future.
  \param settings A non-NULL EnvironmentSettings object.
  \param plugin A short plugin name.
*/
void environment_add_plugin (EnvironmentSettings *settings, gchar *plugin);

G_END_DECLS

#endif
