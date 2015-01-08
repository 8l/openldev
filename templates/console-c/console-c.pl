#!/usr/bin/perl

if ($ARGV[0] eq "name")
{
  print "Terminal Project (C)";
  exit;
}

$location = $ARGV[0];
$name = $ARGV[1];
$author = $ARGV[2];
$homepage = $ARGV[3];
$version = $ARGV[4];
$email = $ARGV[5];
$srcdir = "${location}src/";

print "Creating directory: ${location}\n";
mkdir $location, 0777;
print "Creating directory: ${srcdir}\n";
mkdir $srcdir, 0777;

createfile ("${location}${name}.dev", "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<OpenLDevProject version=\"0.6+\">\n  <ProjectTemplate>console-c</ProjectTemplate>\n  <OpenFile>${srcdir}main.c</OpenFile>\n  <Language>C</Language>\n  <AutoGenerate>./autogen.sh</AutoGenerate>\n  <Build>make</Build>\n  <Clean>make clean</Clean>\n  <Compile>make {FILE}</Compile>\n  <Configure>./configure</Configure>\n  <Execute>{BASE}src/{NAME}</Execute>\n</OpenLDevProject>\n", 0644);
createfile ("${location}AUTHORS", "${author} [${email}]\n", 0644);
createfile ("${location}autogen.sh", "\#!/bin/sh\necho \"Running aclocal\"\naclocal\necho \"Running automake\"\nautomake --add-missing\necho \"Running autoconf\"\nautoconf\necho \"Running configure\"\n./configure\n", 0777);
createfile ("${location}ChangeLog", "\n", 0644);
createfile ("${location}config.h.in", "\#undef PACKAGE\n\#undef PACKAGE_BUGREPORT\n\#undef PACKAGE_NAME\n\#undef PACKAGE_STRING\n\#undef PACKAGE_TARNAME\n\#undef PACKAGE_VERSION\n\#undef STDC_HEADERS\n\#undef VERSION\n", 0644);
createfile ("${location}configure.in", "AC_INIT(configure.in)\nAM_INIT_AUTOMAKE(${name}, ${version})\nAC_PREFIX_DEFAULT(\"/usr\")\nAM_CONFIG_HEADER(config.h)\n\nCFLAGS=\"\"\nAC_SUBST(CFLAGS)\nAC_PROG_CC\nAM_PROG_CC_STDC\nAC_HEADER_STDC\n\nAC_OUTPUT([ Makefile src/Makefile ])\n", 0644);
createfile (" ${location}Makefile.am", "SUBDIRS = src\n\n${name}docdir = \$(prefix)/share/doc/${name}\n${name}doc_DATA = README COPYING AUTHORS ChangeLog INSTALL NEWS TODO\n\nEXTRA_DIST = \$(${name}doc_DATA)\n", 0644);
createfile ("${location}NEWS", "\n", 0644);
createfile ("${location}README", "For more information, visit:\n  ${homepage}", 0644);
createfile ("${location}TODO", "\n", 0644);
createfile ("${srcdir}main.c", "/***************************************************************************\n *   This program is free software; you can redistribute it and/or modify  *\n *   it under the terms of the GNU General Public License as published by  *\n *   the Free Software Foundation; either version 2 of the License, or     *\n *   (at your option) any later version.                                   *\n *                                                                         *\n *   This program is distributed in the hope that it will be useful,       *\n *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *\n *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *\n *   GNU General Public License for more details.                          *\n *                                                                         *\n *   You should have received a copy of the GNU General Public License     *\n *   along with this program; if not, write to the                         *\n *   Free Software Foundation, Inc.,                                       *\n *   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.              *\n ***************************************************************************/\n\n\#include <stdio.h>\n\nint main ()\n{\n	printf (\"Hello world\");\n	return (0);\n}\n", 0644);
createfile ("${srcdir}Makefile.am", "AM_CFLAGS = -Wall -g\n\nbin_PROGRAMS = ${name}\n\n${name}_SOURCES = main.c\n", 0644);
print "You should now run Auto Generate to setup the build tree!\n";

sub createfile
{
  print "Creating file: $_[0]\n";
  open (FILE, ">$_[0]");
  print FILE "$_[1]";
  chmod $_[2], $_[0];
}
