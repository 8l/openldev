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
 
#include "openldev-project.h"

static void openldev_project_settings_get_makefiles (vector<string>&, string);
static vector<string> openldev_project_settings_get_subdirs (string);
static vector<string> openldev_project_settings_get_makefile_sources (gchar*);
static gchar* openldev_project_settings_load_version (gchar*);

// Open the project located at the specified file name
ProjectSettings* 
openldev_project_settings_new (gchar *filename)
{
  ProjectSettings* settings = g_slice_new (ProjectSettings);
  settings->open = FALSE;

  if (filename == NULL)
    settings->name = NULL;
  else
    openldev_project_settings_load_project_file (settings, filename);
  
  return settings;
}

// Close the project by setting all of the values to empty strings, but
// make sure to write the project before closing.
void openldev_project_settings_close_project (ProjectSettings* settings)
{
  settings->open = TRUE;
  g_slist_free (settings->lastfiles);
  settings->lastfiles = NULL;
  openldev_xml_document_close (&settings->xml);
  settings->template_name = NULL;
  
  g_free (settings->language);
  g_free (settings->compile);
  g_free (settings->build);
  g_free (settings->autogen);
  g_free (settings->configure);
  g_free (settings->clean);
  g_free (settings->location);
  g_free (settings->name);
  g_free (settings->version);
  g_free (settings->fullname);
  
  settings->language = NULL;
  settings->compile = NULL;
  settings->build = NULL;
  settings->autogen = NULL;
  settings->configure = NULL;
  settings->clean = NULL;
  settings->location = NULL;
  settings->name = NULL;
  settings->version = NULL;
  settings->fullname = NULL;
}

// Load the project specified by the file name
gboolean 
openldev_project_settings_load_project_file (ProjectSettings* settings, 
                                             gchar *file)
{
  gchar *find;
  if (!g_file_test (file, G_FILE_TEST_EXISTS))
    return FALSE;
  
  settings->xml = openldev_xml_document_open (file);
  
  if (g_ascii_strcasecmp (openldev_xml_document_root_attribute 
                          (settings->xml, "version"), "0.6+") == 0)
  {
    settings->fullname = g_strdup (file);
    find = g_strrstr (file, "/");

    // Get all of the attributes
    settings->location = g_strndup (file, (gint) (find - file) + 1);
    
    find++;
    settings->name = g_strndup (find, strlen (find) - 4);
    settings->version = g_strdup (openldev_project_settings_load_version (settings->location));
    
    settings->lastfiles = openldev_xml_document_element_values (settings->xml, "OpenFile");
    settings->template_name = openldev_xml_document_element_value (settings->xml, "ProjectTemplate");
    settings->language = openldev_xml_document_element_value (settings->xml, "Language");
    settings->compile = openldev_xml_document_element_value (settings->xml, "Compile");
    settings->build = openldev_xml_document_element_value (settings->xml, "Build");
    settings->autogen = openldev_xml_document_element_value (settings->xml, "AutoGenerate");
    settings->configure = openldev_xml_document_element_value (settings->xml, "Configure");
    settings->clean = openldev_xml_document_element_value (settings->xml, "Clean");
    settings->execute = openldev_xml_document_element_value (settings->xml, "Execute");
  }
  else
  {
    openldev_message ("OpenLDev Error", "This project file format is not compatible with your version of OpenLDev!", GTK_MESSAGE_ERROR);
    return FALSE;
  }
  
  settings->open = TRUE;
  return TRUE;
}

gboolean
openldev_project_is_open (ProjectSettings *settings)
{
  return settings->open;
}

// Check whether the specified file is in the project
gboolean 
openldev_project_settings_get_file_in_project (ProjectSettings *settings, 
                                               gchar *filename)
{
  gint i;
  gchar *location;
  vector<string> sources;
  
  location = g_strndup (filename, (gint) (g_strrstr (filename, "/") - filename) + 1);
  sources = openldev_project_settings_get_makefile_sources 
                                    (g_strconcat (location, "Makefile.am", NULL));
  
  for (i = 0; i < sources.size(); i++)
  {
    if (g_ascii_strcasecmp ((gchar*) sources[i].c_str(), filename) == 0)
    {
      sources.clear ();
      return TRUE;
    }
  }
  
  sources.clear ();
  return FALSE;
}

// Load the version from configure.in
static gchar*
openldev_project_settings_load_version (gchar *file)
{
  string fn = g_strconcat (file, "configure.in", NULL);
  
  if (!g_file_test (fn.c_str(), G_FILE_TEST_EXISTS))
  {
    fn = g_strconcat (file, "configure.ac", NULL);
    if (!g_file_test (fn.c_str(), G_FILE_TEST_EXISTS))
      return "NONE";
  }

  string text = openldev_read_file ((gchar*) fn.c_str());
  int l1, l2 = text.find ("AM_INIT_AUTOMAKE", 0);
  
  if (l2 == string::npos)
    return "NONE";
  
  l1 = text.find (",", l2) + 1;
  l2 = text.find (")", l1);
  
  text = text.substr (l1, l2-l1);
  while (text.find(" ", 0) != string::npos)
    text.erase (text.find(" ", 0), 1);
  
  return (gchar*) text.c_str(); 
}

void openldev_project_settings_add_lastfile (ProjectSettings *settings,
                                             gchar *value)
{
  GSList *list = NULL;

  for (list = settings->lastfiles; list != NULL; list = list->next)
    if (g_ascii_strcasecmp (value, (gchar*) list->data) == 0)
      return;

  settings->lastfiles = g_slist_append (settings->lastfiles, (gpointer) g_strdup (value));
  openldev_xml_document_add_element (settings->xml, "OpenFile", value);
  openldev_xml_document_save (&settings->xml);
}

void openldev_project_settings_remove_lastfile (ProjectSettings *settings,
                                                gchar *value)
{
  settings->lastfiles = g_slist_remove (settings->lastfiles, (gconstpointer) value);
  openldev_xml_document_remove_element (settings->xml, "OpenFile", value); 
  openldev_xml_document_save (&settings->xml);
}

void openldev_project_settings_set_language (ProjectSettings *settings,
                                             gchar *value)
{ 
  openldev_xml_document_set_element (settings->xml, "Language", value);
  openldev_xml_document_save (&settings->xml);
  
  g_free (settings->language);
  settings->language = NULL;
  settings->language = g_strdup (value);
}

void openldev_project_settings_set_compile (ProjectSettings *settings,
                                            gchar *value)
{ 
  openldev_xml_document_set_element (settings->xml, "Compile", value);
  openldev_xml_document_save (&settings->xml);
  
  g_free (settings->compile);
  settings->compile = NULL;
  settings->compile = g_strdup (value);
}

void openldev_project_settings_set_build (ProjectSettings *settings,
                                          gchar *value)
{ 
  openldev_xml_document_set_element (settings->xml, "Build", value);
  openldev_xml_document_save (&settings->xml);
  
  g_free (settings->build);
  settings->build = NULL;
  settings->build = g_strdup (value);
}

void openldev_project_settings_set_autogen (ProjectSettings *settings,
                                            gchar *value)
{ 
  openldev_xml_document_set_element (settings->xml, "AutoGenerate", value);
  openldev_xml_document_save (&settings->xml);
  
  g_free (settings->autogen);
  settings->autogen = NULL;
  settings->autogen = g_strdup (value);
}

void openldev_project_settings_set_configure (ProjectSettings *settings,
                                              gchar *value)
{ 
  openldev_xml_document_set_element (settings->xml, "Configure", value);
  openldev_xml_document_save (&settings->xml);
  
  g_free (settings->configure);
  settings->configure = NULL;
  settings->configure = g_strdup (value);
}

void openldev_project_settings_set_clean (ProjectSettings *settings,
                                          gchar *value)
{ 
  openldev_xml_document_set_element (settings->xml, "Clean", value);
  openldev_xml_document_save (&settings->xml);
  
  g_free (settings->clean);
  settings->clean = NULL;
  settings->clean = g_strdup (value);
}

void openldev_project_settings_set_execute (ProjectSettings *settings,
                                            gchar *value)
{ 
  openldev_xml_document_set_element (settings->xml, "Execute", value);
  openldev_xml_document_save (&settings->xml);
  
  g_free (settings->execute);
  settings->execute = NULL;
  settings->execute = g_strdup (value);
}

gchar* openldev_project_settings_get_template (ProjectSettings *settings)
{ return settings->template_name;
}

GSList* openldev_project_settings_get_lastfiles (ProjectSettings *settings)
{ return settings->lastfiles;
}

gchar* openldev_project_settings_get_language (ProjectSettings *settings)
{ return settings->language;
}

gchar* openldev_project_settings_get_compile (ProjectSettings *settings)
{ return settings->compile;
}

gchar* openldev_project_settings_get_build (ProjectSettings *settings)
{ return settings->build;
}

gchar* openldev_project_settings_get_autogen (ProjectSettings *settings)
{ return settings->autogen;
}

gchar* openldev_project_settings_get_configure (ProjectSettings *settings)
{ return settings->configure;
}

gchar* openldev_project_settings_get_clean (ProjectSettings *settings)
{ return settings->clean;
}

gchar* openldev_project_settings_get_location (ProjectSettings *settings)
{ return settings->location;
}

gchar* openldev_project_settings_get_name (ProjectSettings *settings)
{ return settings->name;
}

gchar* openldev_project_settings_get_version (ProjectSettings *settings)
{ return settings->version;
}

gchar* openldev_project_settings_get_fullname (ProjectSettings *settings)
{ return settings->fullname;
}

void openldev_project_settings_set_version (ProjectSettings* settings,
                                            gchar *value)
{
  string fn = g_strconcat (settings->location, "configure.in", NULL);
  
  if (!g_file_test (fn.c_str(), G_FILE_TEST_EXISTS))
  {
    fn = g_strconcat (settings->location, "configure.ac", NULL);
    if (!g_file_test (fn.c_str(), G_FILE_TEST_EXISTS))
      return;
  }

  string text = openldev_read_file ((gchar*) fn.c_str());
  gint l1, l2 = text.find ("AM_INIT_AUTOMAKE", 0);
  
  if (l2 == string::npos)
    return;
  
  g_free (settings->version);
  settings->version = NULL;
  settings->version = g_strdup (value);
  l1 = text.find (",", l2) + 1;
  l2 = text.find (")", l1);
  
  text.erase (l1, l2 - l1);
  text.insert (l1, " ");
  text.insert (l1 + 1, value);
  
  openldev_write_file ((gchar*) fn.c_str(), text);
}

// Get a list of all of the makefiles in the project
vector<string> openldev_project_settings_get_makefile_list (ProjectSettings *project)
{
  vector<string> makefiles;
  openldev_project_settings_get_makefiles (makefiles, project->location);
  
  return makefiles;
}

// Get a list of makefiles starting with the specified basedir. This is
// used recursively by the get_makefile_list function.
static void 
openldev_project_settings_get_makefiles (vector<string> &makefiles,
                                         string base_dir)
{
  string makefile = base_dir + "Makefile.am";
  vector<string> subdirs = openldev_project_settings_get_subdirs (makefile);

  for (unsigned int i=0; i < subdirs.size(); i++)
    openldev_project_settings_get_makefiles (makefiles,
      (base_dir + subdirs.at(i) + "/"));

  makefiles.push_back (makefile);
  subdirs.clear ();
}

// Get a list of all of the sub directories in the project
static vector<string> 
openldev_project_settings_get_subdirs (string makefile)
{
  Makefile *mf = openldev_makefile_new (makefile);
  vector<string> subdirs = openldev_makefile_get_subdirs (mf);
  delete mf;

  return subdirs;
}

// Get a list of all of the sources in the project
vector<string> 
openldev_project_settings_get_sources (ProjectSettings *project, 
                                       gchar *base_dir, 
                                       gboolean subdirs)
{
  vector<string> makefiles, sources;
  
  // If sub-directories are to be included, get a list of all of the makefiles
  if (subdirs)
    openldev_project_settings_get_makefiles (makefiles, base_dir);
  // Otherwise just add the base directory
  else
    makefiles.push_back (string(base_dir) + "Makefile.am");
  
  for (unsigned int i=0; i < makefiles.size(); i++)
  {
    string makefile = makefiles[i].substr (0, makefiles[i].find("Makefile.am"));
    vector<string> temp = openldev_project_settings_get_makefile_sources ((gchar*) makefiles[i].c_str());
    for (unsigned int j=0; j < temp.size(); j++)
    {        
      if (subdirs)
        sources.push_back (makefile + temp[j]);
      else
        sources.push_back (temp[j]);
    }
    temp.clear();
  }
  
  makefiles.clear();
  return sources;
}

// Get a list of all of the sources in the project
vector<string> 
openldev_project_settings_get_sources_all (ProjectSettings *project)
{ 
  return openldev_project_settings_get_sources (project, project->location, TRUE);
}

// Get a list of all of the sources in a specific makefile
static vector<string> 
openldev_project_settings_get_makefile_sources (gchar *file)
{
  Makefile *mf = openldev_makefile_new (file);
  vector<string> files = openldev_makefile_get_sources (mf);
  delete mf;
  
  return files;
}
