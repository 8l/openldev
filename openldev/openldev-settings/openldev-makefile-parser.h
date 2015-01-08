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

#ifndef OPENLDEV_MAKEFILE_PARSER_H
#define OPENLDEV_MAKEFILE_PARSER_H

/*! \file openldev-makefile-parser.h
    \brief Makefile, MakefileBuffer, MakefileVariable, MakefileScript, MakefileTarget, MakefilePrefix
*/

#include "../openldev-utils.h"

#include <glib.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

/*! \brief Specify the target type as PROGRAMS, LIBRARIES or LTLIBRARIES. */
enum TargetType { TARGET_PROGRAM, TARGET_LIBRARY, TARGET_LTLIBRARY };

/*! \brief Simple list of all of the elements in a Makefile.am. */
struct MakefileBuffer
{
  /*! List of identification names given to each of the elements. */
  vector<string> name;
  /*! The content for each of the elements. This includes variable content or lines of a script. */
  vector<vector<string> > content;
};

/*! \brief Variable element in a Makefile.am (Used by Makefile). */
struct MakefileVariable
{
  /*! The displayed name of the variable. This name can be alphanumeric and include the underscore character. All other characters are not allowed. */
  string name;
  /*! List of content that the variable will be set to. This can include other variables by placing them between identifiers. For example: \$(VARIABLE). */
  vector<string> content;
};

/*! \brief Script element in a Makefile.am (Used by Makefile). */
struct MakefileScript
{
  /*! The header for a script element. This must include a colon (:) character! */
  string header;
  /*! Lines of content for the script. A tab prefix will be added to each when it is written to a file, so it is not necessary to include them in this list. */
  vector<string> content;
};

/*! \brief Prefix element in a Makefile.am (Used by Makefile). */
struct MakefilePrefix
{
  /*! The prefix name that can be used as an alternative prefix for targets. */
  string name;
  /*! Location where all of the files with this prefix will be installed, including any data elements. */
  string location;
  /*! An optional list of non-binary data files that can be installed in to the prefix. */
  vector<string> data;
};

/*! \brief Target element in a Makefile.am (Used by Makefile). */
struct MakefileTarget
{
  /*! The type of target defined by the TargetType enumeration. This can be TARGET_PROGRAM, TARGET_LIBRARY or TARGET_LTLIBRARY. */
  int type;
  /*! TRUE if this target is going to use a user-defined prefix. */
  bool user_prefix;
  
  /*! The name of the target. It can include alphanumeric characters, dashes and periods. */
  string name;
  /*! The full text version of the prefix to be used. These can include, but are not limited to a user-defined prefix, bin, sbin, lib, etc. */
  string prefix;
  
  /*! List of sources (SOURCES) that should be compiled to create the target. */
  vector<string> sources;
  /*! List of linker flags (LDFLAGS) used when linking the target. */
  vector<string> ldflags;
  /*! List of libraries (LDADD or LIBADD) included when building the target. */
  vector<string> ldadd;
};

/*! \brief Fully parsed structure of all of the elements in a Makefile.am */
struct Makefile
{
  /*! The full path location of the Makefile.am that the elements were loaded from. */
  string file;

  /*! List of elements from the SUBDIRS variable if it exists. */
  vector<string> subdirs;
  /*! List of elements from the INCUDES variable if it exists. */
  vector<string> includes;
  
  /*! List of general variable elements in the Makefile.am. This excludes any elements that are targets, SUBDIRS, INCLUDES or prefixes. */
  vector<MakefileVariable*> variables;
  /*! List of scripts and their content in the Makefile.am. */
  vector<MakefileScript*> scripts;
  /*! List of target elements in the Makefile.am. Each of these can be linked to a custom prefix. */
  vector<MakefileTarget*> targets;
  /*! List of custom prefixes. There is also the option of installing data at each of the prefixes as well. */
  vector<MakefilePrefix*> prefixes;
};

/*! This will create a new MakefileBuffer object out of the Makefile.am file. This is useful if you want to parse manually through the list of objects.
  \param file The location of the Makefile.am file.
  \return A newly loaded MakefileBuffer object or an empty one if the file is not found.
*/
MakefileBuffer* openldev_makefile_buffer_read (string file);

/*! This will create a new Makefile object out of the specified Makefile.am. This object has already sorted out the different types of elements, so it is a lot easier to work with than MakefileBuffer objects.
  \param file The location of the Makefile.am file.
  \return A newly loaded Makefile object or an empty one if the file is not found.
*/
Makefile* openldev_makefile_new (string file);

/*! Write the current content of a Makefile object to the file it was loaded from. Use this so changes are not lost.
  \param mf A Makefile object.
*/
void openldev_makefile_write (Makefile *mf);

/*! You can use this function to retrieve the list of SUBDIRS in a Makfile.am, although it is somewhat arbitrary since that object is now public.
  \param mf A Makefile object.
  \return The list of subdirectories in the Makefile.am.
*/
vector<string> openldev_makefile_get_subdirs (Makefile *mf);

/*! This will retrieve all of the sources from each of the targets in the project. You can also achieve this result by parsing manually through all of the target objects.
  \param mf A Makefile object.
  \return A full list of sources in the Makefile.am.
*/
vector<string> openldev_makefile_get_sources (Makefile *mf);

#endif
