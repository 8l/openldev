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

#include "makefile-editor-edit.h"

void makefile_editor_vector_to_entry (GtkEntry*, vector<string>&);

gboolean makefile_editor_edit_prefix (MakefileEditor*, GtkTreeIter, GtkTreeModel*);
gboolean makefile_editor_edit_script (MakefileEditor*, GtkTreeIter, GtkTreeModel*);
gboolean makefile_editor_edit_subproject (MakefileEditor*, GtkTreeIter, GtkTreeModel*);
gboolean makefile_editor_edit_target (MakefileEditor*, GtkTreeIter, GtkTreeModel*);
gboolean makefile_editor_edit_variable (MakefileEditor*, GtkTreeIter, GtkTreeModel*);

vector<string> user_prefixes;

// This handles finding the type of object selected & calling the correct edit function
void makefile_editor_edit (MakefileEditor *editor)
{
  GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (editor->content));
  GtkTreeModel *model;
  GtkTreeIter iter;

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
    int type;
    gtk_tree_model_get (model, &iter, 2, &type, -1);
    
    // Call the edit function
    switch (type)
    {
      case (MAKEFILE_EDITOR_VARIABLE):
      {
        if (makefile_editor_edit_variable (editor, iter, model))
          makefile_editor_populate (editor);
        break;
      }
      case (MAKEFILE_EDITOR_SCRIPT):
      {
        if (makefile_editor_edit_script (editor, iter, model))
          makefile_editor_populate (editor);
        break;
      }
      case (MAKEFILE_EDITOR_BASE):
      case (MAKEFILE_EDITOR_SUBPROJECT):
      {
        if (makefile_editor_edit_subproject (editor, iter, model))
          makefile_editor_populate (editor);
        break;
      }
      case (MAKEFILE_EDITOR_PREFIX):
      {
        if (makefile_editor_edit_prefix (editor, iter, model))
          makefile_editor_populate (editor);
        break;
      }
      case (MAKEFILE_EDITOR_TARGET):
      {
        if (makefile_editor_edit_target (editor, iter, model))
          makefile_editor_populate (editor);
        break;
      }
    }
  }
}

void 
makefile_editor_vector_to_entry (GtkEntry *entry,
                                 vector<string> &content)
{
  if (content.size() > 0)
    gtk_entry_set_text (entry, content[0].c_str());
  for (unsigned int i=1; i < content.size(); i++)
    gtk_entry_set_text (entry, g_strconcat (gtk_entry_get_text (entry), " ", 
                        content[i].c_str(), NULL));
}

/************************  Edit Target  ************************/

void makefile_editor_populate_sources (GtkWidget*, Makefile*, string, gint);
vector<string> makefile_editor_get_sources (Makefile*, string);
gboolean makefile_editor_get_source_in_target (Makefile*, string, gint);
void makefile_editor_source_toggled (GtkCellRendererToggle*, gchar*, GtkTreeModel*);
void makefile_editor_type_changed (GtkComboBox*, gpointer);

// Edit a target object
gboolean 
makefile_editor_edit_target (MakefileEditor *editor,
                             GtkTreeIter iter,
                             GtkTreeModel *model)
{
  unsigned int i;
  int l = -1;
  string makefile = g_strconcat (editor->project->location, (gchar*) editor->current_path.c_str(),
                                 "Makefile.am", NULL);
  Makefile *mf = openldev_makefile_new (makefile);
  
  gchar *target;
  gtk_tree_model_get (model, &iter, 1, &target, -1);
  
  // Find where it is in the Makefile
  for (i=0; i < mf->targets.size(); i++)
    if (mf->targets[i]->name == target)
      l = i;

  if (l >= 0)
  {
    // Load the glade file & widgets
    string gladefile = openldev_add_prefix ("/share/openldev/glade/mfeditor-target.glade");
    GladeXML *xml = glade_xml_new (gladefile.c_str(), NULL, NULL);
    GtkWidget *dialog = glade_xml_get_widget (xml, "dialog");
    GtkWidget *name = glade_xml_get_widget (xml, "name");
    GtkWidget *type = glade_xml_get_widget (xml, "type");
    GtkWidget *prefix = glade_xml_get_widget (xml, "prefix");
    GtkWidget *ldflags = glade_xml_get_widget (xml, "ldflags");
    GtkWidget *ldadd = glade_xml_get_widget (xml, "ldadd");
    GtkWidget *sources = glade_xml_get_widget (xml, "sources");
    
    // Set the information for the variable
    gtk_entry_set_text (GTK_ENTRY (name), target);
    makefile_editor_vector_to_entry (GTK_ENTRY (ldflags), mf->targets[l]->ldflags);
    makefile_editor_vector_to_entry (GTK_ENTRY (ldadd), mf->targets[l]->ldadd);
    makefile_editor_populate_sources (sources, mf, mf->file.substr (0, mf->file.length()-11), l);
    
    user_prefixes.clear();
    for (i=0; i < mf->prefixes.size(); i++)
      user_prefixes.push_back (mf->prefixes[i]->name);
    
    gtk_combo_box_remove_text (GTK_COMBO_BOX (prefix), 0);
    gtk_combo_box_append_text (GTK_COMBO_BOX (prefix), mf->targets[l]->prefix.c_str());
    gtk_combo_box_set_active (GTK_COMBO_BOX (prefix), 0);
    
    g_signal_connect (G_OBJECT (type), "changed",
                      G_CALLBACK (makefile_editor_type_changed), 
                      (gpointer) prefix);
    
    if (mf->targets[l]->type == TARGET_PROGRAM)
      gtk_combo_box_set_active (GTK_COMBO_BOX (type), 0);
    else if (mf->targets[l]->type == TARGET_LIBRARY)
      gtk_combo_box_set_active (GTK_COMBO_BOX (type), 1);
    else if (mf->targets[l]->type == TARGET_LTLIBRARY)
      gtk_combo_box_set_active (GTK_COMBO_BOX (type), 2);
    
    // Run the dialog & apply the changes if the user doesn't Cancel
    int result = gtk_dialog_run (GTK_DIALOG (dialog));
    while (result == GTK_RESPONSE_APPLY)
    {    
      if (gtk_combo_box_get_active_text (GTK_COMBO_BOX (type)) == NULL)
        openldev_message ("Makefile Editor Error", "You must specify a target type!", GTK_MESSAGE_ERROR);
      else if (gtk_combo_box_get_active_text (GTK_COMBO_BOX (prefix)) == NULL)
        openldev_message ("Makefile Editor Error", "You must specify a prefix!", GTK_MESSAGE_ERROR);
      else if (gtk_entry_get_text (GTK_ENTRY (name)) == NULL)
        openldev_message ("Makefile Editor Error", "You must specify a name!", GTK_MESSAGE_ERROR);
      else
      {
        string type_text = gtk_combo_box_get_active_text (GTK_COMBO_BOX (type)),
               prefix_text = gtk_combo_box_get_active_text (GTK_COMBO_BOX (prefix)),
               name_text = gtk_entry_get_text (GTK_ENTRY (name)),
               ldadd_text = gtk_entry_get_text (GTK_ENTRY (ldadd)),
               ldflags_text = gtk_entry_get_text (GTK_ENTRY (ldflags));
      
        mf->targets[l]->name = name_text;
        mf->targets[l]->prefix = prefix_text;
        
        if (type_text == "Program")
          mf->targets[l]->type = TARGET_PROGRAM;
        else if (type_text == "Library")
          mf->targets[l]->type = TARGET_LIBRARY;
        else if (type_text == "Libtool Library")
          mf->targets[l]->type = TARGET_LTLIBRARY;
        
        mf->targets[l]->sources.clear();
        mf->targets[l]->ldflags.clear();
        mf->targets[l]->ldadd.clear();
        
        if (ldflags_text.length() > 0)
          mf->targets[l]->ldflags.push_back (ldflags_text);
        if (ldadd_text.length() > 0)
          mf->targets[l]->ldadd.push_back (ldadd_text);
        
        GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (sources));
        GtkTreeIter iter;
        gboolean show;
        for (i=0; i < gtk_tree_model_iter_n_children (model, NULL); i++)
        {
          gchar *file;
          gtk_tree_model_iter_nth_child (model, &iter, NULL, i);
          gtk_tree_model_get (model, &iter, 0, &show, 3, &file, -1);
      
          if (show)
            mf->targets[l]->sources.push_back (file);
        }
        
        openldev_makefile_write (mf);
        break;
      }
      
      result = gtk_dialog_run (GTK_DIALOG (dialog));
    }
    
    gtk_widget_destroy (dialog);
  }
  
  g_free (target);
  return TRUE;
}

// The type has changed! That means we must perform some actions.
void 
makefile_editor_type_changed (GtkComboBox *type,
                              gpointer data)
{
  GtkComboBox *prefixes = GTK_COMBO_BOX (data);
  string type_text = gtk_combo_box_get_active_text (type),
         current_prefix = gtk_combo_box_get_active_text (prefixes);
  gtk_list_store_clear (GTK_LIST_STORE (gtk_combo_box_get_model (prefixes)));
  guint count, i;
  
  if (type_text == "Program")
  {
    gtk_combo_box_append_text (prefixes, "bin");
    gtk_combo_box_append_text (prefixes, "sbin");
    gtk_combo_box_append_text (prefixes, "libexec");
    count = 3;
  }
  else
  {
    gtk_combo_box_append_text (prefixes, "lib");
    count = 1;
  }
  
  gtk_combo_box_append_text (prefixes, "noinst");
  gtk_combo_box_append_text (prefixes, "pkglib");
  gtk_combo_box_append_text (prefixes, "check");
  
  for (i = 0; i < user_prefixes.size(); i++)
    gtk_combo_box_append_text (prefixes, user_prefixes[i].c_str());
  
  if (current_prefix == "bin")
    gtk_combo_box_set_active (prefixes, 0);
  else if (current_prefix == "sbin")
    gtk_combo_box_set_active (prefixes, 1);
  else if (current_prefix == "libexec")
    gtk_combo_box_set_active (prefixes, 2);
  else if (current_prefix == "lib")
    gtk_combo_box_set_active (prefixes, 0);
  else if (current_prefix == "noinst")
    gtk_combo_box_set_active (prefixes, count);
  else if (current_prefix == "pkglib")
    gtk_combo_box_set_active (prefixes, count + 1);
  else if (current_prefix == "check")
    gtk_combo_box_set_active (prefixes, count + 2);
  else
  {
    for (i = 0; i < user_prefixes.size(); i++)
      if (current_prefix == user_prefixes[i])
        gtk_combo_box_set_active (prefixes, count + 3 + i);
  }
}

// Add the list of sources and whether they are a part of the current target
void 
makefile_editor_populate_sources (GtkWidget *sources, 
                                  Makefile *mf, 
                                  string base,
                                  gint l)
{
  vector<string> files = makefile_editor_get_sources (mf, base);
  GtkTreeIter row;
  GtkCellRenderer *renderer;
  GtkTreeModel *model = GTK_TREE_MODEL (gtk_tree_store_new (4, G_TYPE_BOOLEAN,
                                        G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING));
                                                          
  gtk_tree_view_set_model (GTK_TREE_VIEW (sources), model);
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (sources));

  // This gave me a lot of trouble because it is late right now, and therefore, I am stupid
  renderer = gtk_cell_renderer_toggle_new ();
  g_object_set (renderer, "xalign", 0.5, NULL);
  g_signal_connect (renderer, "toggled", 
                    G_CALLBACK (makefile_editor_source_toggled), model);
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (sources), 0, "",
							      renderer, "active", 0, "visible", 1, "activatable", 2, NULL);

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (sources), 1, "Source Files",
							      renderer, "text", 3, NULL);
  
  // Make sure to check whether or not to set the target active
  for (unsigned int i=0; i < files.size(); i++)
  {
    gtk_tree_store_append (GTK_TREE_STORE (model), &row, NULL);
    gtk_tree_store_set (GTK_TREE_STORE (model), &row, 0,
                        makefile_editor_get_source_in_target (mf, files[i], l),
                        1, TRUE, 2, TRUE, 3, files[i].c_str(), -1);
  }
  
  g_object_unref (model);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (sources), FALSE);
}

// Toggle the source check button because the user said to do it
void makefile_editor_source_toggled (GtkCellRendererToggle *renderer, 
                                     gchar *path_string, 
                                     GtkTreeModel *model)
{
  GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
  GtkTreeIter iter;
  gboolean toggled;
  gint *column = (gint*) g_object_get_data (G_OBJECT (renderer), "column");

  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_model_get (model, &iter, column, &toggled, -1);
  gtk_tree_store_set (GTK_TREE_STORE (model), &iter, column, !toggled, -1);
  gtk_tree_path_free (path);
}

// Check if the current source file is a part of the sources list for the target
gboolean makefile_editor_get_source_in_target (Makefile *mf, 
                                               string file,
                                               gint l)
{
  for (unsigned int i=0; i < mf->targets[l]->sources.size(); i++)
    if (mf->targets[l]->sources[i] == file)
      return TRUE;
  
  return FALSE;
}

// Get a list of C/C++ source files in the directory
vector<string> 
makefile_editor_get_sources (Makefile *mf,
                             string base)
{
  string temp;
  vector<string> files;
  GList *dir_entries, *itr;
  GnomeVFSFileInfo *file_info;
  
 	gnome_vfs_init();

  if (gnome_vfs_directory_list_load (&dir_entries, base.c_str(),
      GNOME_VFS_FILE_INFO_DEFAULT) == GNOME_VFS_OK)
  {
    for (itr = dir_entries; itr != NULL; itr = itr->next)
    {
      file_info = (GnomeVFSFileInfo*) itr->data;
      string str = (char*) file_info->name;
      
      if (str.find (".", 0) != string::npos)
      {
        string substr = str.substr (str.find (".", 0), 
                        str.length() - str.find (".", 0));
      
        if (substr == ".c" || substr == ".cc" || substr == ".cxx" ||
            substr == ".cpp" || substr == ".hh" || substr == ".h" ||
            substr == ".hpp" || substr == ".hxx" || substr == ".C")
          files.push_back ((char*) file_info->name);
      }
    }
    gnome_vfs_file_info_list_free (dir_entries);
  }
  
  for (unsigned int i=0; i < files.size() / 2; i++)
  {
    temp = files[i];
    files[i] = files[files.size()-i-1];
    files[files.size()-i-1] = temp;
  }
  
  return files;
}

/************************ Edit Subproject ************************/

gboolean makefile_editor_edit_subproject (MakefileEditor *editor,
                                          GtkTreeIter iter,
                                          GtkTreeModel *model)
{
  unsigned int i, j;
  int lC = -1, lCXX = -1;
  string makefile;
  gchar *subproject;
  gtk_tree_model_get (model, &iter, 1, &subproject, -1);
  
  if (editor->current_dir == subproject)
    makefile = g_strconcat (editor->project->location, (gchar*) editor->current_path.c_str(),
                            "Makefile.am", NULL);
  else
    makefile = editor->project->location + editor->current_path 
             + subproject + "/Makefile.am";
  Makefile *mf = openldev_makefile_new (makefile);
  
  string gladefile = openldev_add_prefix ("/share/openldev/glade/mfeditor-subproject.glade");
  GladeXML *xml = glade_xml_new (gladefile.c_str(), NULL, NULL);
  GtkWidget *dialog = glade_xml_get_widget (xml, "dialog");
  GtkWidget *includes = glade_xml_get_widget (xml, "includes");
  GtkWidget *cflags = glade_xml_get_widget (xml, "cflags");
  GtkWidget *cxxflags = glade_xml_get_widget (xml, "cxxflags");
  
  makefile_editor_vector_to_entry (GTK_ENTRY (includes), mf->includes);
  
  for (i=0; i < mf->variables.size(); i++)
  {
    if (mf->variables[i]->name == "AM_CFLAGS")
    {
      makefile_editor_vector_to_entry (GTK_ENTRY (cflags), mf->variables[i]->content);
      lC = i;
    }
    else if (mf->variables[i]->name == "AM_CXXFLAGS")
    {
      lCXX = i;
      makefile_editor_vector_to_entry (GTK_ENTRY (cxxflags), mf->variables[i]->content);
    }
  }
  
  // Run the dialog & apply the changes if the user doesn't Cancel
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_APPLY)
  {
    string incl = gtk_entry_get_text (GTK_ENTRY (includes));
    string cflg = gtk_entry_get_text (GTK_ENTRY (cflags));
    string cxfl = gtk_entry_get_text (GTK_ENTRY (cxxflags));
    
    mf->includes.clear();
    if (incl.length() > 0)
      mf->includes.push_back (incl);
    
    if (lC >= 0)
    {
      mf->variables[lC]->content.clear();
      if (cflg.length() > 0)
        mf->variables[lC]->content.push_back (cflg);
    }
    else
    {
      if (cflg.length() > 0)
      {
        MakefileVariable *var1 = new MakefileVariable;
        var1->name = "AM_CFLAGS";
        var1->content.push_back (cflg);
        mf->variables.push_back (var1);
      }
    }
    
    if (lCXX >= 0)
    {
      mf->variables[lCXX]->content.clear();
      if (cxfl.length() > 0)
        mf->variables[lCXX]->content.push_back (cxfl);
    }
    else
    {
      if (cflg.length() > 0)
      {
        MakefileVariable *var2 = new MakefileVariable;
        var2->name = "AM_CXXFLAGS";
        var2->content.push_back (cxfl);
        mf->variables.push_back (var2);
      }
    }
    
    openldev_makefile_write (mf);
  }
    
  gtk_widget_destroy (dialog);
  g_free (subproject);
  return TRUE;
}

/************************ Edit Script ************************/

// Edit a script object
gboolean makefile_editor_edit_script (MakefileEditor *editor,
                                  GtkTreeIter iter,
                                  GtkTreeModel *model)
{
  unsigned int i, j;
  int l = -1;
  gchar *script;
  gtk_tree_model_get (model, &iter, 1, &script, -1);
  
  string makefile = editor->project->location + 
                    editor->current_path + "Makefile.am";
  Makefile *mf = openldev_makefile_new (makefile);
  
  // Load the glade file and widgets
  string gladefile = openldev_add_prefix ("/share/openldev/glade/mfeditor-script.glade");
  GladeXML *xml = glade_xml_new (gladefile.c_str(), NULL, NULL);
  GtkWidget *dialog = glade_xml_get_widget (xml, "dialog");
  GtkWidget *header = glade_xml_get_widget (xml, "header");
  GtkWidget *content = glade_xml_get_widget (xml, "content");
  gtk_entry_set_text (GTK_ENTRY (header), script);
  
  // Get the text for the script
  for (i=0; i < mf->scripts.size(); i++)
    if (mf->scripts[i]->header == script)
    {
      string text;
      if (mf->scripts[i]->content.size() > 0)
        text = mf->scripts[i]->content[0];
      for (j=1; j < mf->scripts[i]->content.size(); j++)
        text += "\n" + mf->scripts[i]->content[j];
      
      // Set the text to the buffer & record the position
      GtkTextBuffer *buffer = gtk_text_buffer_new (NULL);
      gtk_text_buffer_set_text (buffer, text.c_str(), -1);
      gtk_text_view_set_buffer (GTK_TEXT_VIEW (content), buffer);
      l = i;
      break;
    }
  
  // Run the dialog until a button is pressed
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_APPLY)
  {
    // Set the header of the script & reset the attributes
    GtkTextIter start, end;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (content));
    gtk_text_buffer_get_start_iter (buffer, &start);
    gtk_text_buffer_get_end_iter (buffer, &end);
    string txt = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
    mf->scripts[l]->header = gtk_entry_get_text (GTK_ENTRY (header));
    mf->scripts[l]->content.clear();
    
    // Get all but the last line of text
    while (txt.find ("\n", 0) != string::npos)
    {
      mf->scripts[l]->content.push_back (txt.substr (0, txt.find ("\n", 0)));
      txt.erase (0, txt.find ("\n", 0) + 1);
    }
    
    // Add the last line of text & write Makefile.am
    if (txt.length () > 0)
      mf->scripts[l]->content.push_back (txt);
    openldev_makefile_write (mf);
  }
    
  gtk_widget_destroy (dialog);
  g_free (script);
  return TRUE;
}

/************************  Edit Prefix  ************************/

// Edit a prefix object
gboolean 
makefile_editor_edit_prefix (MakefileEditor *editor,
                             GtkTreeIter iter,
                             GtkTreeModel *model)
{
  int l = -1;
  string makefile = g_strconcat (editor->project->location, (gchar*) editor->current_path.c_str(),
                                 "Makefile.am", NULL);
  Makefile *mf = openldev_makefile_new (makefile);
  
  gchar *prefix;
  gtk_tree_model_get (model, &iter, 1, &prefix, -1);
  
  // Find where it is in the Makefile
  for (unsigned int i=0; i < mf->prefixes.size(); i++)
    if (mf->prefixes[i]->name == prefix)
      l = i;

  if (l >= 0)
  {
    // Load the glade file & widgets
    string gladefile = openldev_add_prefix ("/share/openldev/glade/mfeditor-prefix.glade");
    GladeXML *xml = glade_xml_new (gladefile.c_str(), NULL, NULL);
    GtkWidget *dialog = glade_xml_get_widget (xml, "dialog");
    GtkWidget *name = glade_xml_get_widget (xml, "name");
    GtkWidget *data = glade_xml_get_widget (xml, "data");
    GtkWidget *location = glade_xml_get_widget (xml, "location");
    
    // Set the information for the variable
    gtk_entry_set_text (GTK_ENTRY (name), prefix);
    gtk_entry_set_text (GTK_ENTRY (location), mf->prefixes[l]->location.c_str());
    makefile_editor_vector_to_entry (GTK_ENTRY (data), mf->prefixes[l]->data);
    
    // Run the dialog & apply the changes if the user doesn't Cancel
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_APPLY)
    {
      string text = gtk_entry_get_text (GTK_ENTRY (data));
      mf->prefixes[l]->name = gtk_entry_get_text (GTK_ENTRY (name));
      mf->prefixes[l]->location = gtk_entry_get_text (GTK_ENTRY (location));
      mf->prefixes[l]->data.clear();
      if (text.length() > 0)
        mf->prefixes[l]->data.push_back (text);
      openldev_makefile_write (mf);
    }
    
    gtk_widget_destroy (dialog);
  }
  
  g_free (prefix);
  return TRUE;
}

/************************ Edit Variable ************************/

// Edit a variable object
gboolean 
makefile_editor_edit_variable (MakefileEditor *editor,
                               GtkTreeIter iter,
                               GtkTreeModel *model)
{
  int l = -1;
  string makefile = g_strconcat (editor->project->location, (gchar*) editor->current_path.c_str(),
                                 "Makefile.am", NULL);
  Makefile *mf = openldev_makefile_new (makefile);
  
  gchar *variable;
  gtk_tree_model_get (model, &iter, 1, &variable, -1);
  
  // Find where it is in the Makefile
  for (unsigned int i=0; i < mf->variables.size(); i++)
    if (mf->variables[i]->name == variable)
      l = i;

  if (l >= 0)
  {
    // Load the glade file & widgets
    string gladefile = openldev_add_prefix ("/share/openldev/glade/mfeditor-variable.glade");
    GladeXML *xml = glade_xml_new (gladefile.c_str(), NULL, NULL);
    GtkWidget *dialog = glade_xml_get_widget (xml, "dialog");
    GtkWidget *var = glade_xml_get_widget (xml, "variable");
    GtkWidget *content = glade_xml_get_widget (xml, "content");
    
    // Set the information for the variable
    gtk_entry_set_text (GTK_ENTRY (var), variable);
    makefile_editor_vector_to_entry (GTK_ENTRY (content), mf->variables[l]->content);
    
    // Run the dialog & apply the changes if the user doesn't Cancel
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_APPLY)
    {
      mf->variables[l]->name = gtk_entry_get_text (GTK_ENTRY (var));
      mf->variables[l]->content.clear();
      mf->variables[l]->content.push_back (gtk_entry_get_text (GTK_ENTRY (content)));
      openldev_makefile_write (mf);
    }
    
    gtk_widget_destroy (dialog);
  }
  
  g_free (variable);
  return TRUE;
}
