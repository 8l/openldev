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

#ifndef OUTPUT_SEARCH_H
#define OUTPUT_SEARCH_H

/*!
  \file openldev-output-search.h
  \brief OutputSearch
*/

#include <gtk/gtk.h>
#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define LIST_TYPE            (output_list_get_type ())
#define LIST(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LIST_TYPE, OutputList))
#define LIST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), LIST_TYPE, OutputListClass))
#define IS_LIST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LIST_TYPE))
#define IS_LIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LIST_TYPE))

/*! \brief Widgets for the list output GtkTreeView. */
struct OutputList
{
  GtkTreeView parent;

  /*! GtkScrolledWindow that contains the main GtkTreeView widget. */
  GtkWidget *swin;
};

struct OutputListClass
{
  GtkTreeViewClass parent_class;
};

GType output_list_get_type ();

/*! Create a new OutputList object. This is useful if you want a generic GtkTreeView to show output in.
  \return A new OutputList object.
*/
GtkWidget* output_list_new ();

/*! Get the GtkScrolledWindow widget in the OutputList object.
  \param list An OutputList object.
  \return The OutputList's GtkScrolledWindow.
*/
GtkWidget* output_list_get_scrolled_window (OutputList *list);

/*! Clear the content of the GtkTreeView.
  \param list An OutputList object.
*/
void output_list_clear (OutputList *list);

G_END_DECLS

#endif
