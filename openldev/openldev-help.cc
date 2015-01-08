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

#include "openldev-help.h"

#define BROWSER_KEY "/desktop/gnome/url-handlers/http/command"

// Show the documentation by using the default graphical browser.
void openldev_menu_help_documentation (OpenLDev *openldev)
{
  string command;
  EnvironmentSettings *env;
  
  g_object_get (openldev, "env-settings", &env, NULL);
  command += gconf_client_get_string (env->client, BROWSER_KEY , NULL);
  command += " http://docs.openldev.org";
  if (command.find ("%s", 0) != string::npos)
    command.erase (command.find ("%s", 0), 2);
  
  openldev_fork_new_process (command.c_str());
}

// Allow the user to submit bug reports by using the default graphical browser.
void openldev_menu_help_report_bug (OpenLDev *openldev)
{
  string command;
  EnvironmentSettings *env;
  
  g_object_get (openldev, "env-settings", &env, NULL);
  command += gconf_client_get_string (env->client, BROWSER_KEY , NULL);
  command += " http://bugs.openldev.org";
  if (command.find ("%s", 0) != string::npos)
    command.erase (command.find ("%s", 0), 2);
  
  openldev_fork_new_process (command.c_str());
}

// Create an object of the OpenLDev about dialog that shows the copyright
// information and current version.
void openldev_menu_help_about (OpenLDev *openldev)
{
  openldev_about_dialog_new (OPENLDEV_VERSION);
}
