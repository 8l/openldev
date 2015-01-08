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

#ifndef FILE_TAB_H
#define FILE_TAB_H

/*!
  \file openldev-file-tab.h
  \brief FileTab
*/

#include "../openldev-utils.h"

#include <gtksourceview/gtksourceview.h>
#include <gtk/gtk.h>
#include <iostream>
#include <glib.h>
#include <glib-object.h>
using namespace std;

G_BEGIN_DECLS

#define FILE_TAB_TYPE            (file_tab_get_type ())
#define FILE_TAB(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), FILE_TAB_TYPE, FileTab))
#define FILE_TAB_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), FILE_TAB_TYPE, FileTabClass))
#define IS_FILE_TAB(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FILE_TAB_TYPE))
#define IS_FILE_TAB_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), FILE_TAB_TYPE))

/*! \brief Widgets and information for one open file. */
struct FileTab
{
  GtkSourceView view;
  
  /*! GtkSourceBuffer that is used for this GtkSourceView. */
  GtkSourceBuffer *buffer;

  /*! GtkScrolledWindow that contains the textview. */
  GtkWidget *swin;
  /*! GtkLabel shown in the tab as the short file name. */
  GtkWidget *label;
  /*! GtkButton that can be pressed to close the tab. */
  GtkWidget *button;
  /*! Close button GtkImage for the tab. */
  GtkWidget *image;
  /*! GtkHBox that holds the tab label and close button. */
  GtkWidget *hbox;

  /*! The full file name used by the tab. */
  gchar *filename;
  /*! TRUE if the file has been modified since the last save. */
  bool modified;
};

struct FileTabClass
{
  GtkSourceViewClass parent_class;
};

// Internal function: Do not document!
GType file_tab_get_type ();

/*! Create a new FileTab object with the content of the specified file. This should be added to the FileManager tab vector.
  \param file The full file name of a file to open.
  \return A new FileTab object.
*/
GtkWidget* file_tab_new (const char *file);

/*! Retrieve the file name for the tab. You should use the functions provided by FileManager instead.
  \param tab A new FileTab object
  \return The full file name of the tab.
*/
string file_tab_get_filename_short (FileTab *tab);

/*! Give the file a new name. This does not rename the actual file on the system. You should use the functions provided by FileManager instead.
  \param tab A new FileTab object
  \param file The new full file name for the tab.
*/
void file_tab_set_label (FileTab *tab, const char *file);

G_END_DECLS

#endif
