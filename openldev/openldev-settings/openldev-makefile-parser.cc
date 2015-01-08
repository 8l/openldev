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
 
#include "openldev-makefile-parser.h"

bool openldev_makefile_buffer_new_variable (MakefileBuffer*, string&);
void openldev_makefile_buffer_new_script (MakefileBuffer*, string&);
void openldev_makefile_buffer_parse_variable (MakefileBuffer*, string&);
vector<string> openldev_makefile_buffer_find_element (MakefileBuffer*, string);

void openldev_makefile_parse (Makefile*, MakefileBuffer*);
void openldev_makefile_parse_targets (Makefile*, MakefileBuffer*);
void openldev_makefile_parse_prefixes (Makefile*, MakefileBuffer*, vector<string>);
vector<string> openldev_makefile_parse_not_targets (Makefile*, MakefileBuffer*);

void openldev_makefile_clean_string (string&);
string openldev_makefile_get_target_prefix (MakefileTarget*, string);
int openldev_makefile_get_target_type (string);
string openldev_makefile_fix_target_name (string);

void openldev_makefile_add_variable (string&, string, vector<string>&);
void openldev_makefile_add_script (string&, string, vector<string>&);
void openldev_makefile_add_variable1 (string&, string, string);

Makefile* 
openldev_makefile_new (string file)
{
  Makefile *mf = new Makefile;
  MakefileBuffer *buffer = openldev_makefile_buffer_read (file);
  openldev_makefile_parse (mf, buffer);
  mf->file = file;
  delete buffer;
  return mf;
}

void
openldev_makefile_parse (Makefile *mf,
                         MakefileBuffer *buffer)
{
  vector<string> pfxs = openldev_makefile_parse_not_targets (mf, buffer);
  
  if (pfxs.size() > 0)
    openldev_makefile_parse_prefixes (mf, buffer, pfxs);
  openldev_makefile_parse_targets (mf, buffer);
}

vector<string>
openldev_makefile_parse_not_targets (Makefile *mf,
                                     MakefileBuffer *buffer)
{
  unsigned int i, j;
  vector<string> prefixes;
  MakefileBuffer *temp = new MakefileBuffer;

  mf->subdirs = openldev_makefile_buffer_find_element (buffer, "SUBDIRS");
  mf->includes = openldev_makefile_buffer_find_element (buffer, "INCLUDES");

  for (i=0; i < buffer->name.size(); i++)
  {
    if (buffer->name[i].find (":", 0) != string::npos)
    {
      MakefileScript *script = new MakefileScript;
      script->header = buffer->name[i];
      for (j=0; j < buffer->content[i].size(); j++)
        script->content.push_back (buffer->content[i].at(j));
      mf->scripts.push_back (script);
    }
    else if (buffer->name[i].find ("dir", 0) == buffer->name[i].length()-3)
      prefixes.push_back (buffer->name[i].substr (0, buffer->name[i].size()-3));
    else if (buffer->name[i] != "EXTRA_DIST" &&
             buffer->name[i] != "SUBDIRS" && buffer->name[i] != "INCLUDES" &&
             buffer->name[i].find ("_DATA", 0) == string::npos &&
             buffer->name[i].find ("_PROGRAMS", 0) == string::npos &&
             buffer->name[i].find ("_LIBRARIES", 0) == string::npos &&
             buffer->name[i].find ("_LTLIBRARIES", 0) == string::npos &&
             buffer->name[i].find ("_LDFLAGS", 0) == string::npos &&
             buffer->name[i].find ("_LDADD", 0) == string::npos &&
             buffer->name[i].find ("_LIBADD", 0) == string::npos &&
             buffer->name[i].find ("_SOURCES", 0) == string::npos)
    {
      MakefileVariable *var = new MakefileVariable;
      var->name = buffer->name[i];
      for (j=0; j < buffer->content[i].size(); j++)
        var->content.push_back (buffer->content[i].at(j));
      mf->variables.push_back (var);
    }
  }
  
  return prefixes;
}

void
openldev_makefile_parse_prefixes (Makefile *mf,
                                  MakefileBuffer *buffer,
                                  vector<string> pfxs)
{
  vector<string> prefixes;
  unsigned int i = pfxs.size();
  sort (pfxs.begin(), pfxs.end());
  
  if (i > 1)
  {
    for (i=0; i < pfxs.size()-1; i++)
      if (pfxs[i] != pfxs[i+1])
        prefixes.push_back (pfxs[i]);
    prefixes.push_back (pfxs[pfxs.size()-1]);
  }
  else
    prefixes.push_back (pfxs[0]);

  for (i=0; i < prefixes.size(); i++)
  {
    MakefilePrefix *prefix = new MakefilePrefix;
    vector<string> temp = openldev_makefile_buffer_find_element (buffer,
                          prefixes[i] + "dir");
    prefix->name = prefixes[i];
    if (temp.size() > 0)
      prefix->location = temp[0];
    else
      prefix->location = "UNDEFINED";
    prefix->data = openldev_makefile_buffer_find_element (buffer,
                   prefixes[i] + "_DATA");
    mf->prefixes.push_back (prefix);
  }
}

void
openldev_makefile_parse_targets (Makefile *mf,
                                 MakefileBuffer *buffer)
{
  for (unsigned int i=0; i < buffer->name.size(); i++)
  {
    if (buffer->name[i].find ("_PROGRAMS", 0) != string::npos ||
        buffer->name[i].find ("_LIBRARIES", 0) != string::npos ||
        buffer->name[i].find ("_LTLIBRARIES", 0) != string::npos)
    {
      
      for (unsigned int j=0; j < buffer->content[i].size(); j++)
      {
        MakefileTarget *target = new MakefileTarget;
        target->name = buffer->content[i][j];
        target->prefix = openldev_makefile_get_target_prefix (target, 
                         buffer->name[i]);
        target->type = openldev_makefile_get_target_type (buffer->name[i]);
        
        string name = openldev_makefile_fix_target_name (target->name);
        target->sources = openldev_makefile_buffer_find_element (buffer,
                          name + "_SOURCES");
        target->ldflags = openldev_makefile_buffer_find_element (buffer,
                          name + "_LDFLAGS");
        
        if (target->type == TARGET_PROGRAM)
          target->ldadd = openldev_makefile_buffer_find_element (buffer,
                            name + "_LDADD");
        else
          target->ldadd = openldev_makefile_buffer_find_element (buffer,
                            name + "_LIBADD");
        
        mf->targets.push_back (target);
      }
    }
  }
}

string
openldev_makefile_fix_target_name (string name)
{
  for (int i=0; i < name.length(); i++)
    if (name[i] == '-' || name[i] == '.')
      name[i] = '_';
  
  return name;
}

string
openldev_makefile_get_target_prefix (MakefileTarget *target,
                                     string name)
{ 
  string pfx = name.substr (0, name.find ("_", 0));
  
  if (pfx == "bin" || pfx == "sbin" || pfx == "libexec" || pfx == "lib" || 
      pfx == "noinst" || pfx == "pkglib" || pfx == "check")
    target->user_prefix = false;
  else
    target->user_prefix = true;
  
  return pfx;
}

int
openldev_makefile_get_target_type (string name)
{ 
  if (name.find ("_PROGRAMS", 0) != string::npos)
    return TARGET_PROGRAM;
  else if (name.find ("_LTLIBRARIES", 0) != string::npos)
    return TARGET_LTLIBRARY;
  else
    return TARGET_LIBRARY;
}

// Read a Makefile.am into a MakefileBuffer. This is meant to put the
// Makefile.am in a better format in which to parse.
MakefileBuffer* 
openldev_makefile_buffer_read (string file)
{
  MakefileBuffer *buffer = new MakefileBuffer;
  string line;
  int current = 0;
  bool is_script = false;
  bool is_variable = false;
  
  // Loop for every line in the Makefile.am
  ifstream fin (file.c_str());
  while (getline (fin, line))
  {
    // There is script content to be catched
    if (is_script)
    {
      // Add the script if the line begins with a tab
      if (line[0] == 9)
      {
        line.erase (0, 1);
        buffer->content[current].push_back (line);
      }
      // Otherwise, fall through
      else
        is_script = false;
    }
    // Parse this line as extra variable content
    else if (is_variable)
    {
      openldev_makefile_clean_string (line);
      
      // Check whether we should include the next line too
      if (line[line.length()-1] != '\\')
        is_variable = false;
      else
        line.erase (line.length()-1, 1);
      
      openldev_makefile_buffer_parse_variable (buffer, line);
    }

    // This is used to parse all base elements
    if (!is_variable && !is_script && !line.empty())
    {
      if (line.find ("=", 0) != string::npos)
        is_variable = openldev_makefile_buffer_new_variable (buffer, line);
      else if (line.find (":", 0) != string::npos)
      {
        openldev_makefile_buffer_new_script (buffer, line);
        current = buffer->content.size() - 1;
        is_script = true;
      }
    }
  }
  
  fin.close ();
  return buffer;
}

vector<string>
openldev_makefile_buffer_find_element (MakefileBuffer *buffer,
                                       string element)
{
  for (unsigned int i=0; i < buffer->name.size(); i++)
    if (buffer->name[i] == element)
      return buffer->content[i];
  
  vector<string> empty;
  return empty;
}

// Parse a new variable line.
bool 
openldev_makefile_buffer_new_variable (MakefileBuffer *buffer,
                                       string &line)
{
  bool is_variable = false;
  openldev_makefile_clean_string (line);
  
  // Check whether there is another line of variable content
  if (line[line.length()-1] == '\\')
  {
    is_variable = true;
    line.erase (line.length()-1, 1);
    openldev_makefile_clean_string (line);
  }
  
  // Add the name & add an empty content vector
  vector<string> content;
  string name = line.substr (0, line.find ("=", 0));
  openldev_makefile_clean_string (name);
  buffer->name.push_back (name);
  buffer->content.push_back (content);
  line.erase (0, line.find ("=", 0) + 1);

  // Parse the rest of the line
  openldev_makefile_buffer_parse_variable (buffer, line);
  
  return is_variable;
}

// Parse a line for variable content
void 
openldev_makefile_buffer_parse_variable (MakefileBuffer *buffer,
                                         string &line)
{
  unsigned int location;
  unsigned int current = buffer->content.size() - 1;
  openldev_makefile_clean_string (line);
  
  // Loop through & get all but the last piece of data
  while (line.find (" ", 0) != string::npos)
  {
    location = line.find (" ", 0);
    buffer->content[current].push_back (line.substr (0, location));
    line.erase (0, location + 1);
  }
  
  // Add the last piece of data if the string isn't empty yet
  if (line.length() > 0)
  {
    location = line.find (" ", 0);
    buffer->content[current].push_back (line.substr (0, location));
    line.erase (0, location + 1);
  }
}

// Add a new script element. An empty content vector is added because
// this line doesn't include any script content
void 
openldev_makefile_buffer_new_script (MakefileBuffer *buffer,
                                     string &line)
{
  vector<string> content;
  openldev_makefile_clean_string (line);
  buffer->name.push_back (line);
  buffer->content.push_back (content);
}

// Clean the white space from the front and back of the string
void 
openldev_makefile_clean_string (string &str)
{
  while (str[0] == ' ' || str[0] == '\t')
    str.erase (0, 1);
  
  while (str[str.length()-1] == ' ' || 
         str[str.length()-1] == '\t')
    str.erase (str.length()-1, 1);
}

// Write the makefile with all of the changes. The default formatting is used.
// It is recommended to continue using this formatting if you make manual changes
// because OpenLDev will work with the file faster.
void 
openldev_makefile_write (Makefile *mf)
{
  string text;
  vector<string> extradists;
  unsigned int i, j;
  
  // Add sub directories and includes
  if (mf->subdirs.size() > 0)
    openldev_makefile_add_variable (text, "SUBDIRS", mf->subdirs);
  if (mf->includes.size() > 0)
    openldev_makefile_add_variable (text, "INCLUDES", mf->includes);
  
  // Add general variables and scripts
  for (i=0; i < mf->variables.size(); i++)
    openldev_makefile_add_variable (text, mf->variables[i]->name, mf->variables[i]->content);
  for (i=0; i < mf->scripts.size(); i++)
    openldev_makefile_add_script (text, mf->scripts[i]->header, mf->scripts[i]->content);
  
  // Add prefixes and check for EXTRA_DIST candidates
  for (i=0; i < mf->prefixes.size(); i++)
  {
    openldev_makefile_add_variable1 (text, mf->prefixes[i]->name + "dir", 
                                     mf->prefixes[i]->location);
    if (mf->prefixes[i]->data.size() > 0)
    {
      openldev_makefile_add_variable (text, mf->prefixes[i]->name + "_DATA",
                                      mf->prefixes[i]->data);
      extradists.push_back ("$(" + mf->prefixes[i]->name + "_DATA)");
    }
  }
  
  // Add target headers and content
  for (i=0; i < mf->targets.size(); i++)
  {
    // Add the headers
    string post;
    if (mf->targets[i]->type == TARGET_LIBRARY)
      post = "_LIBRARIES";
    else if (mf->targets[i]->type == TARGET_LTLIBRARY)
      post = "_LTLIBRARIES";
    else
      post = "_PROGRAMS";
    openldev_makefile_add_variable1 (text, mf->targets[i]->prefix + post,
                                     mf->targets[i]->name);
  
    // Add sources and linker flags
    string base = openldev_makefile_fix_target_name (mf->targets[i]->name);
    if (mf->targets[i]->sources.size() > 0)
      openldev_makefile_add_variable (text, base + "_SOURCES", mf->targets[i]->sources);
    if (mf->targets[i]->ldflags.size() > 0)
      openldev_makefile_add_variable (text, base + "_LDFLAGS", mf->targets[i]->ldflags);
    
    // Add libraries depending on type
    if (mf->targets[i]->ldadd.size() > 0)
    {
      if (mf->targets[i]->type == TARGET_PROGRAM)
        openldev_makefile_add_variable (text, base + "_LDADD", mf->targets[i]->ldadd);
      else
        openldev_makefile_add_variable (text, base + "_LIBADD", mf->targets[i]->ldadd);
    }
  }
  
  // Add extra distributions
  if (extradists.size() > 0)
    openldev_makefile_add_variable (text, "EXTRA_DIST", extradists);
  
  // Write the file
  openldev_write_file (mf->file, text);
}

void 
openldev_makefile_add_variable (string &text,
                                string name,
                                vector<string> &content)
{
  text += name + " = ";
  for (unsigned int i=0; i < content.size(); i++)
    text += "\\\n  " + content[i];
  text += "\n\n";
}

void 
openldev_makefile_add_variable1 (string &text,
                                 string name,
                                 string content)
{ text += name + " = " + content + "\n\n";
}

void 
openldev_makefile_add_script (string &text,
                              string header,
                              vector<string> &content)
{
  text += header + "\n";
  for (unsigned int i=0; i < content.size(); i++)
    text += "\t" + content[i] + "\n";
  text += "\n";
}

// Get a list of all of the sub-directories in the project. 
// This is recursive so it goes through all makefiles.
vector<string> openldev_makefile_get_subdirs (Makefile *mf)
{ return mf->subdirs;
}

// Get a list of all of the sources in the makefile
vector<string> openldev_makefile_get_sources (Makefile *mf)
{ 
  vector<string> sources;
  
  for (unsigned int i=0; i < mf->targets.size(); i++)
    for (unsigned int j=0; j < mf->targets[i]->sources.size(); j++)
      sources.push_back (mf->targets[i]->sources[j]);
  
  return sources;
}
