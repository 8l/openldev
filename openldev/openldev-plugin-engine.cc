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

#include "openldev-plugin-engine.h"

vector<string> openldev_get_installed_plugins (string);
string openldev_get_plugin_attribute (string, string);

OpenLDevPluginList* openldev_load_plugins_all (OpenLDev *openldev)
{
  g_assert (g_module_supported());
  
  OpenLDevPluginList *pl = new OpenLDevPluginList;
  string text, basedir = openldev_add_prefix ("/lib/openldev/");
  vector<string> files = openldev_get_installed_plugins (basedir);
  EnvironmentSettings *env;
  
  g_object_get (openldev, "env-settings", &env, NULL);
  
  for (unsigned int i=0; i < files.size(); i++)
  {
    OpenLDevPlugin *plugin = new OpenLDevPlugin;
    text = openldev_read_file (g_strconcat ((gchar*) basedir.c_str(), (gchar*) files[i].c_str(), NULL));
    
    plugin->name = openldev_get_plugin_attribute (text, "Name");
    plugin->author = openldev_get_plugin_attribute (text, "Author");
    plugin->copyright = openldev_get_plugin_attribute (text, "Copyright");
    plugin->description = openldev_get_plugin_attribute (text, "Description");
    plugin->library = basedir + "lib" + openldev_get_plugin_attribute 
                      (text, "Location") + ".la";
    
    plugin->file = files[i];
    plugin->enabled = TRUE;
    plugin->plugin_id = i;
    plugin->state = PLUGIN_OK;
    
    plugin->module = g_module_open (plugin->library.c_str(), G_MODULE_BIND_LAZY);
    if (!plugin->module)
      openldev_message ("OpenLDev Error", (gchar*) g_module_error (), GTK_MESSAGE_ERROR);
    else
    {
      if (!g_module_symbol (plugin->module, "init", (gpointer*) &plugin->init) ||
          !g_module_symbol (plugin->module, "destroy", (gpointer*) &plugin->destroy) ||
          !g_module_symbol (plugin->module, "reload", (gpointer*) &plugin->reload))
        openldev_message ("OpenLDev Error", (gchar*) g_module_error (), GTK_MESSAGE_ERROR);
      else
      {
        pl->plugins.push_back (plugin);
        pl->num_of_plugins++;
        plugin->init ((gpointer) openldev);
        if (!environment_get_plugin (env, (gchar*) plugin->file.c_str()))
        {
          plugin->destroy ((gpointer) openldev);
          plugin->enabled = FALSE;
        }
      }
    }
  }
  
  return pl;
}

string openldev_get_plugin_attribute (string text, string attr)
{
  int location = text.find (attr, 0) + attr.length() + 1;
  return text.substr (location, text.find (";", location) - location);
}

// Get a list of plugin files in the directory
vector<string> openldev_get_installed_plugins (string dir)
{
  vector<string> files;
  struct dirent **namelist;
  int n = scandir (dir.c_str(), &namelist, 0, alphasort), i;
  
  // Get the list of files by parsing through the list
  if (n >= 0)
  {
    while (n--) 
    {
      if (string (namelist[n]->d_name).find (".plugin", 0) != string::npos)
        files.push_back (namelist[n]->d_name);
      free (namelist[n]);
    } 
    free (namelist);
  }
  
  // Sort the vector in the opposite direction
  string temp;
  for (i=0; i < (signed)files.size() / 2; i++)
  {
    temp = files[i];
    files[i] = files[files.size() - i - 1];
    files[files.size() - i - 1] = temp;
  }
  
  return files;
}
