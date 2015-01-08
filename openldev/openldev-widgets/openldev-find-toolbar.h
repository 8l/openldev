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

#ifndef FIND_TOOLBAR_H
#define FIND_TOOLBAR_H

/*!
  \file openldev-find-toolbar.h
  \brief FindToolbar
*/

#include <gtk/gtk.h>
#include <iostream>
#include <vector>
using namespace std;

/*! \brief Find toolbar widgets including an error message */
struct FindToolbar
{
  /*! GtkLabel that tells the user what to enter in the find_text GtkEntry. */
  GtkWidget *label_find;
  /*! GtkLabel error message. */
  GtkWidget *label_error;
  /*! GtkComboBoxEntry that holds the text the user wants to search for. */
  GtkWidget *find_text;
  /*! GtkButton that is pressed to start searching. */
  GtkWidget *button_find;
  /*! GtkButton that is pressed to hide the toolbar. */
  GtkWidget *button_close;
  /*! GtkImage set to gtk-close. */
  GtkWidget *image_close;
  /*! GtkImage set to gtk-error. */
  GtkWidget *image_error;
  /*! GtkCheckButton that when checked, causes OpenLDev to search all source files for the specified text.*/
  GtkWidget *search_files;
  /*! GtkHBox that holds all of the find toolbar widgets. */
  GtkWidget *hbox;
  
  vector<string> choices;
};

// Internal function: Do not document!
FindToolbar* openldev_find_toolbar_new ();

/*! Show or hide the error message on the toolbar. When there is an unsuccessful search, this should be set to TRUE.
  \param toolbar A FindToolbar widget.
  \param show TRUE if the error message should be shown.
*/
void openldev_find_toolbar_show_error (FindToolbar *toolbar, gboolean show);

/*! Get whether the user wants to search in all files for their text.
  \param toolbar A FindToolbar widget.
  \return TRUE if the search_files GtkCheckButton is active.
*/
gboolean openldev_find_toolbar_search_files_is_active (FindToolbar *toolbar);

/*! Add an option to the drop down list of past searches. This is limited to 10 options, so if there are already 10 in the list, the last one will be removed before the new one is added. This will automatically update the GtkComboBox, so there is no need to take further action.
  \param toolbar A FindToolbar widget.
  \param choice The text to add to the list of choices.
*/
void openldev_find_toolbar_add_choice (FindToolbar *toolbar, string choice);

#endif
