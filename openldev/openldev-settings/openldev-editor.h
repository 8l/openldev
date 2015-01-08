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
 
#ifndef OPENLDEV_EDITOR_H
#define OPENLDEV_EDITOR_H

/*!
  \file openldev-editor.h
  \brief EditorProperties
*/

#include <glib.h>

/*! \brief Structure that holds basic information about the current editor. */
struct EditorProperties
{
  /*! TRUE if the project file browser notebook is currently visible in the editor. */
  gboolean browser;
  /*! TRUE if the output manager notebook is currently visible in the editor. */
  gboolean outputmanager;
  /*! TRUE if the status bar is currently visible in the editor. */
  gboolean statusbar;
  /*! TRUE if the toolbar is currently visible in the editor. */
  gboolean toolbar;
  /*! Whether automatic completion is currently being shown to the user. */
  gboolean autocomplete_active;
  /*! The currently shown automatic completion string. This should not be changed. */
  gchar *current_autocomplete;
  gchar *build_dir;
};

// Internal function: Do not document!
EditorProperties* openldev_editor_new ();

/*! This just stores the current values for reference by OpenLDev. Calling this will not toggle visibility!
  \param editor An EditorProperties object.
  \param show TRUE if the project file browser is currently visible.
*/
void openldev_editor_set_browser (EditorProperties *editor, gboolean show);

/*! This just stores the current values for reference by OpenLDev. Calling this will not toggle visibility!
  \param editor An EditorProperties object.
  \param show TRUE if the output manager notebook is currently visible.
*/
void openldev_editor_set_outputmanager (EditorProperties *editor, gboolean show);

/*! This just stores the current values for reference by OpenLDev. Calling this will not toggle visibility!
  \param editor An EditorProperties object.
  \param show TRUE if the status bar is currently visible.
*/
void openldev_editor_set_statusbar (EditorProperties *editor, gboolean show);

/*! This just stores the current values for reference by OpenLDev. Calling this will not toggle visibility!
  \param editor An EditorProperties object.
  \param show TRUE if the toolbar is currently visible.
*/
void openldev_editor_set_toolbar (EditorProperties *editor, gboolean show);

/*! This just stores the current values for reference by OpenLDev. Calling this will not toggle visibility!
  \param editor An EditorProperties object.
  \param active TRUE if the automatic completion is currently active.
*/
void openldev_editor_set_autocomplete_active (EditorProperties *editor, gboolean active);

#endif
