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

#ifndef PLUGIN_H
#define PLUGIN_H

/*!
  \file openldev-plugin.h
  Two structures used for the plug-in API. You will usually not access these structures in an plug-in, but they are here just in case.
  \brief OpenLDevPlugin, OpenLDevPluginList
*/

#include <gtk/gtk.h>
#include <iostream>
#include <vector>
using namespace std;

/*! A list of plugin states. These must be returned from each of the loaded functions. These include:\n\n 
  <ul>
    <li><i>PLUGIN_OK</i>: The plug-in is working fine.
    <li><i>PLUGIN_CRASH</i>: The plug-in encountered a problem and cannot continue working.
    <li><i>PLUGIN_ENABLED</i>: The plug-in was just enabled.
    <li><i>PLUGIN_DISABLED</i>: The plug-in was just disabled.
  </ul>
*/
enum PluginState { PLUGIN_OK, PLUGIN_CRASH, PLUGIN_ENABLED, PLUGIN_DISABLED };

/*! \brief Plugin information and functions loaded from the library. */
struct OpenLDevPlugin
{
  /*! The value of the plugin in the list. This corresponds to its location in the vector. */
  int plugin_id;
  /*! Whether the plugin is currently enabled or disabled. */
  gboolean enabled;
  /*! The location of the plugin file on the user's system. */
  string file;
  /*! The location of the plugin library on the user's system. */
  string library;
  
  /*! Full name of the plugin. */
  string name;
  /*! Programmer that created the plugin. */
  string author;
  /*! A short, one line description of the plugin. */
  string description;
  /*! Simple copyright information about the creation of the plugin. */
  string copyright;
  
  /*! The current state of the plugin. See the PluginState enum. */
  PluginState state;
  /*! Loaded initialization function from the plugin library. */
  PluginState (*init)    (gpointer);
  /*! Loaded disable function from the plugin library. */
  PluginState (*destroy) (gpointer);
  /*! Loaded re-enable function from the plugin library. */
  PluginState (*reload)  (gpointer);
  
  /*! A GModule object that contains the links to the plugin library. */
  GModule *module;
};

/*! \brief The list of available plugins in std::vector form. */
struct OpenLDevPluginList
{
  /*! An std::vector list of plugin objects for each plugin that was loaded, whether it is enabled or not. */
  vector<OpenLDevPlugin*> plugins;
  /*! The number of plugins available. You can get this value from the vector, but this is for convenience. */
  int num_of_plugins;
};

#endif
