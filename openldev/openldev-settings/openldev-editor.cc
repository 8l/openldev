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
 
#include "openldev-editor.h"

// Create a new editor properties structure
EditorProperties* 
openldev_editor_new ()
{
  EditorProperties *editor = g_slice_new (EditorProperties);
  
  openldev_editor_set_toolbar (editor, TRUE);
  openldev_editor_set_browser (editor, TRUE);
  openldev_editor_set_statusbar (editor, TRUE);
  openldev_editor_set_outputmanager (editor, TRUE);
  openldev_editor_set_autocomplete_active (editor, FALSE);
  editor->current_autocomplete = NULL;
  editor->build_dir = NULL;
  
  return editor;
}

void 
openldev_editor_set_browser (EditorProperties *editor,
                             gboolean value)
{ editor->browser = value;
}

void 
openldev_editor_set_outputmanager (EditorProperties *editor,
                                   gboolean value)
{ editor->outputmanager = value;
}

void 
openldev_editor_set_statusbar (EditorProperties *editor,
                               gboolean value)
{ editor->statusbar = value;
}

void 
openldev_editor_set_toolbar (EditorProperties *editor,
                             gboolean value)
{ editor->toolbar = value;
}

void 
openldev_editor_set_autocomplete_active (EditorProperties *editor,
                                         gboolean value)
{ editor->autocomplete_active = value;
}
