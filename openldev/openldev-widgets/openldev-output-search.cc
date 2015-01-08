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

#include "openldev-output-search.h"

static void output_list_class_init (OutputListClass *klass);
static void output_list_init (OutputList *list);

GType
output_list_get_type ()
{
  static GType list_type = 0;

  if (!list_type)
  {
    static const GTypeInfo list_info =
    {
	    sizeof (OutputListClass),
	    NULL, NULL,
	    (GClassInitFunc) output_list_class_init,
      NULL, NULL,
      sizeof (OutputList), 0,
	    (GInstanceInitFunc) output_list_init,
    };

    list_type = g_type_register_static (GTK_TYPE_TREE_VIEW, "OutputList", 
                                        &list_info, (GTypeFlags) 0);
  }

  return list_type;
}

static void
output_list_class_init (OutputListClass *klass)
{
}

static void
output_list_init (OutputList *list)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
        
  renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Search Results", 
                                                     renderer, "text", 0, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (list), column);

  list->swin = gtk_scrolled_window_new (NULL, NULL);
  
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (list->swin),
                                         GTK_WIDGET (list));
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (list->swin),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (list), FALSE);
}

GtkWidget* 
output_list_new ()
{
  return GTK_WIDGET (g_object_new (output_list_get_type (), NULL));
}

GtkWidget* 
output_list_get_scrolled_window (OutputList *list)
{ return list->swin;
}

void 
output_list_clear (OutputList *list)
{
  GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (list));
  
  if (GTK_IS_LIST_STORE (model))
    gtk_list_store_clear (GTK_LIST_STORE (model));
}
