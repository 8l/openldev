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

#include "makefile-editor-add.h"

gboolean makefile_editor_remove_script (MakefileEditor*, GtkTreeIter, GtkTreeModel*);
gboolean makefile_editor_remove_variable (MakefileEditor*, GtkTreeIter, GtkTreeModel*);
gboolean makefile_editor_remove_target (MakefileEditor*, GtkTreeIter, GtkTreeModel*);
gboolean makefile_editor_remove_prefix (MakefileEditor*, GtkTreeIter, GtkTreeModel*);

void makefile_editor_add (GtkMenuItem *item, 
                          gpointer data)
{
  MakefileEditor *editor = (MakefileEditor*) data;
  
  // Make sure that there is a Makefile to add something to
  if (gtk_tree_model_iter_n_children (gtk_tree_view_get_model 
      (GTK_TREE_VIEW (editor->content)), NULL) > 0)
  {
    Makefile *mf = openldev_makefile_new (editor->project->location +
                                          editor->current_path + "Makefile.am");
    GladeXML *xml = glade_xml_new (openldev_add_prefix 
                    ("/share/openldev/glade/mfeditor-add.glade"), NULL, NULL);
    GtkWidget *dialog = glade_xml_get_widget (xml, "dialog");
    GtkWidget *combobox = glade_xml_get_widget (xml, "type");
    GtkWidget *entry = glade_xml_get_widget (xml, "name");
    gtk_combo_box_set_active (GTK_COMBO_BOX (combobox), 0);
    
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
    {
      string type = gtk_combo_box_get_active_text (GTK_COMBO_BOX (combobox)),
             name = gtk_entry_get_text (GTK_ENTRY (entry));
      
      if (name.empty())
      {
        gtk_widget_destroy (dialog); 
        openldev_message ("Makefile Editor Error", "You must specify a name for the new element!", GTK_MESSAGE_ERROR);
        return;
      }
      else if (type == "Script")
      {
        if (name.find (":", 0) == string::npos)
        {
          gtk_widget_destroy (dialog); 
          openldev_message ("Makefile Editor Error", "Script names must include a colon character!", GTK_MESSAGE_ERROR);
          return;
        }
        
        MakefileScript *script = new MakefileScript;
        script->header = name;
        script->content.push_back ("Enter script content here.");
        mf->scripts.push_back (script);
      }
      else if (type == "Variable")
      {
        MakefileVariable *variable = new MakefileVariable;
        variable->name = name;
        variable->content.push_back ("CONTENT");
        mf->variables.push_back (variable);
      }
      else if (type == "Prefix")
      {
        MakefilePrefix *prefix = new MakefilePrefix;
        prefix->name = name;
        prefix->location = "/usr";
        mf->prefixes.push_back (prefix);
      }
      else if (type == "Target")
      {
        MakefileTarget *target = new MakefileTarget;
        target->name = name;
        target->type = TARGET_PROGRAM;
        target->user_prefix = FALSE;
        target->prefix = "bin";
        mf->targets.push_back (target);
      }
      else if (type == "Subproject")
      {
        bool added = FALSE;
        string dir = editor->project->location + editor->current_path + name + "/";
        mkdir (dir.c_str(), 0777);
        mf->subdirs.push_back (name);
      }
    }
    
    openldev_makefile_write (mf);
    makefile_editor_populate (editor);
    gtk_widget_destroy (dialog);    
  }
}

void makefile_editor_remove (GtkMenuItem *item, 
                             gpointer data)
{
  MakefileEditor *editor = (MakefileEditor*) data;
  
  GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (editor->content));
  GtkTreeModel *model;
  GtkTreeIter iter;

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
    int type;
    gtk_tree_model_get (model, &iter, 2, &type, -1);
    
    // Call the edit function
    if (type == MAKEFILE_EDITOR_VARIABLE)
    {
      if (makefile_editor_remove_variable (editor, iter, model))
        makefile_editor_populate (editor);
    }
    else if (type == MAKEFILE_EDITOR_SCRIPT)
    {
      if (makefile_editor_remove_script (editor, iter, model))
        makefile_editor_populate (editor);
    }
    else if (type == MAKEFILE_EDITOR_TARGET)
    {
      if (makefile_editor_remove_target (editor, iter, model))
        makefile_editor_populate (editor);
    }
    else if (type == MAKEFILE_EDITOR_PREFIX)
    {
      if (makefile_editor_remove_prefix (editor, iter, model))
        makefile_editor_populate (editor);
    }
    else
      openldev_message ("Makefile Editor Error", "This element must be removed manually!", GTK_MESSAGE_ERROR);
  }
}

gboolean 
makefile_editor_remove_script (MakefileEditor *editor, 
                               GtkTreeIter iter, 
                               GtkTreeModel *model)
{
  unsigned int i;
  string makefile = editor->project->location 
                  + editor->current_path + "Makefile.am";
  Makefile *mf = openldev_makefile_new (makefile);
  vector<MakefileScript*>::iterator mfiter = mf->scripts.begin();
  
  gchar *script;
  gtk_tree_model_get (model, &iter, 1, &script, -1);
  
  // Find where it is in the Makefile
  for (i=0; i < mf->scripts.size(); i++)
  {
    if (mf->scripts[i]->header == script)
    {
      mf->scripts.erase (mfiter);
      openldev_makefile_write (mf);
      return TRUE;
    }
    mfiter++;
  }
  
  return FALSE;
}

gboolean 
makefile_editor_remove_variable (MakefileEditor *editor, 
                                 GtkTreeIter iter, 
                                 GtkTreeModel *model)
{
  unsigned int i;
  string makefile = editor->project->location 
                  + editor->current_path + "Makefile.am";
  Makefile *mf = openldev_makefile_new (makefile);
  vector<MakefileVariable*>::iterator mfiter = mf->variables.begin();
  
  gchar *variable;
  gtk_tree_model_get (model, &iter, 1, &variable, -1);
  
  // Find where it is in the Makefile
  for (i=0; i < mf->variables.size(); i++)
  {
    if (mf->variables[i]->name == variable)
    {
      mf->variables.erase (mfiter);
      openldev_makefile_write (mf);
      return TRUE;
    }
    mfiter++;
  }
  
  return FALSE;
}

gboolean 
makefile_editor_remove_target (MakefileEditor *editor, 
                               GtkTreeIter iter, 
                               GtkTreeModel *model)
{
  unsigned int i;
  string makefile = editor->project->location 
                  + editor->current_path + "Makefile.am";
  Makefile *mf = openldev_makefile_new (makefile);
  vector<MakefileTarget*>::iterator mfiter = mf->targets.begin();
  
  gchar *target;
  gtk_tree_model_get (model, &iter, 1, &target, -1);
  
  // Find where it is in the Makefile
  for (i=0; i < mf->targets.size(); i++)
  {
    if (mf->targets[i]->name == target)
    {
      mf->targets.erase (mfiter);
      openldev_makefile_write (mf);
      return TRUE;
    }
    mfiter++;
  }
  
  return FALSE;
}

gboolean 
makefile_editor_remove_prefix (MakefileEditor *editor, 
                               GtkTreeIter iter, 
                               GtkTreeModel *model)
{
  unsigned int i;
  string makefile = editor->project->location 
                  + editor->current_path + "Makefile.am";
  Makefile *mf = openldev_makefile_new (makefile);
  vector<MakefilePrefix*>::iterator mfiter = mf->prefixes.begin();
  
  gchar *prefix;
  gtk_tree_model_get (model, &iter, 1, &prefix, -1);
  
  // Find where it is in the Makefile
  for (i=0; i < mf->prefixes.size(); i++)
  {
    if (mf->prefixes[i]->name == prefix)
    {
      mf->prefixes.erase (mfiter);
      openldev_makefile_write (mf);
      return TRUE;
    }
    mfiter++;
  }
  
  return FALSE;
}
