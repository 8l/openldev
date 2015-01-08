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

#include "openldev-new-project.h"

static void openldev_new_project_dialog_populate_combobox (NewProjectDialog*);
static void prep_assistant (GtkAssistant*, GtkWidget*, NewProjectDialog*);
static gboolean openldev_new_project_dialog_check_for_errors (NewProjectDialog*);
static void page2_changed (GtkEditable*, NewProjectDialog*);

static NewProjectDialog *npd = NULL;
static NewProjectDialogPage page[3] = {
  { NULL, -1, "Basic Information", GTK_ASSISTANT_PAGE_CONTENT, FALSE },
  { NULL, -1, "Additional Options", GTK_ASSISTANT_PAGE_CONTENT, TRUE },
  { NULL, -1, "Summary", GTK_ASSISTANT_PAGE_CONFIRM, TRUE },
};

// Load the new project dialog, connect the signals
NewProjectDialog*
openldev_new_project_dialog_new (gpointer data)
{
  gint i;

  if (npd != NULL)
  {
    g_slice_free (NewProjectDialog, npd);
    npd = NULL;
  }
  
  npd = g_slice_new (NewProjectDialog);
  npd->assistant = gtk_assistant_new ();
  g_object_set_data (G_OBJECT (npd->assistant), "openldev", data);
  g_object_set_data (G_OBJECT (npd->assistant), "npd", npd);
  gtk_widget_set_size_request (npd->assistant, 450, 300);
  gtk_window_set_title (GTK_WINDOW (npd->assistant), "Create a New Project");

  page[0].widget = gtk_table_new (4, 2, FALSE);
  page[1].widget = gtk_table_new (3, 2, FALSE);
  page[2].widget = gtk_label_new ("Confirm");

  npd->name = gtk_entry_new ();
  npd->version = gtk_entry_new ();
  npd->combobox = gtk_combo_box_new_text ();
  npd->location = gtk_file_chooser_button_new ("Choose a Project Location",
                                               GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);

  gtk_container_set_border_width (GTK_CONTAINER (page[0].widget), 5);
  gtk_table_set_row_spacings (GTK_TABLE (page[0].widget), 7);
  gtk_table_set_col_spacings (GTK_TABLE (page[0].widget), 10);

  gtk_table_attach (GTK_TABLE (page[0].widget), gtk_label_new ("Project Type:"), 0, 1, 0, 1,
                    GTK_SHRINK, GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (page[0].widget), gtk_label_new ("Location:"), 0, 1, 1, 2,
                    GTK_SHRINK, GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (page[0].widget), gtk_label_new ("Name:"), 0, 1, 2, 3,
                    GTK_SHRINK, GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (page[0].widget), gtk_label_new ("Version:"), 0, 1, 3, 4,
                    GTK_SHRINK, GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (page[0].widget), npd->combobox, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (page[0].widget), npd->location, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (page[0].widget), npd->name, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (page[0].widget), npd->version, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), GTK_FILL, 0, 0);

  npd->author = gtk_entry_new ();
  npd->email = gtk_entry_new ();
  npd->homepage = gtk_entry_new ();

  gtk_container_set_border_width (GTK_CONTAINER (page[1].widget), 5);
  gtk_table_set_row_spacings (GTK_TABLE (page[1].widget), 7);
  gtk_table_set_col_spacings (GTK_TABLE (page[1].widget), 10);

  gtk_table_attach (GTK_TABLE (page[1].widget), gtk_label_new ("Author Name:"), 0, 1, 0, 1,
                    GTK_SHRINK, GTK_SHRINK, 0, 0);
  gtk_table_attach (GTK_TABLE (page[1].widget), gtk_label_new ("E-mail:"), 0, 1, 1, 2,
                    GTK_SHRINK, GTK_SHRINK, 0, 0);
  gtk_table_attach (GTK_TABLE (page[1].widget), gtk_label_new ("Homepage:"), 0, 1, 2, 3,
                    GTK_SHRINK, GTK_SHRINK, 0, 0);
  gtk_table_attach (GTK_TABLE (page[1].widget), npd->author, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), GTK_SHRINK, 0, 0);
  gtk_table_attach (GTK_TABLE (page[1].widget), npd->email, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), GTK_SHRINK, 0, 0);
  gtk_table_attach (GTK_TABLE (page[1].widget), npd->homepage, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), GTK_SHRINK, 0, 0);

  for (i = 0; i < 3; i++)
  {
    page[i].index = gtk_assistant_append_page (GTK_ASSISTANT (npd->assistant),
                                               page[i].widget);
    gtk_assistant_set_page_title (GTK_ASSISTANT (npd->assistant),
                                  page[i].widget, page[i].title);
    gtk_assistant_set_page_type  (GTK_ASSISTANT (npd->assistant),
                                  page[i].widget, page[i].type);

    gtk_assistant_set_page_complete (GTK_ASSISTANT (npd->assistant),
                                     page[i].widget, page[i].complete);
  }

  openldev_new_project_dialog_populate_combobox (npd);
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (npd->location),
                                       g_get_home_dir());

  g_signal_connect (G_OBJECT (npd->name), "changed",
                    G_CALLBACK (page2_changed), (gpointer) npd);
  g_signal_connect (G_OBJECT (npd->version), "changed",
                    G_CALLBACK (page2_changed), (gpointer) npd);
  g_signal_connect (G_OBJECT (npd->assistant), "prepare",
                    G_CALLBACK (prep_assistant), (gpointer) npd);

  gtk_widget_show_all (npd->assistant);
  return npd;
}

static void
page2_changed (GtkEditable *entry,
               NewProjectDialog *npd)
{
  gboolean result = openldev_new_project_dialog_check_for_errors (npd);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (npd->assistant),
                                   page[0].widget, result);
}

static void
prep_assistant (GtkAssistant *assistant,
                GtkWidget *current,
                NewProjectDialog *npd)
{
  gchar *l;

  if (current == page[2].widget)
  {
    l = g_strdup_printf ("Confirm the following information:\n\n" \
                         "Project Type: %s\nLocation: %s\nName: %s\nVersion: %s" \
                         "\n\nAuthor: %s\nE-mail: %s\nHomepage: %s",
                         gtk_combo_box_get_active_text (GTK_COMBO_BOX (npd->combobox)),
                         gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (npd->location)),
                         gtk_entry_get_text (GTK_ENTRY (npd->name)),
                         gtk_entry_get_text (GTK_ENTRY (npd->version)),
                         gtk_entry_get_text (GTK_ENTRY (npd->author)),
                         gtk_entry_get_text (GTK_ENTRY (npd->email)),
                         gtk_entry_get_text (GTK_ENTRY (npd->homepage)));
    gtk_label_set_text (GTK_LABEL (page[2].widget), l);
  }
}

// This adds the different types of projects to the combobox
static void
openldev_new_project_dialog_populate_combobox (NewProjectDialog *project)
{
  gchar *directory, *fn;
  GList *entries = NULL, *real_entries = NULL, *ptr;

  directory = openldev_add_prefix ("/share/openldev/templates/");
  GDir *dir = g_dir_open (directory, 0, NULL);
  while ((fn = (gchar*) g_dir_read_name (dir)) != NULL)
    entries = g_list_prepend (entries, (gpointer) g_strdup (fn));

  for (ptr = entries; ptr != NULL; ptr = ptr->next)
    if (g_strrstr ((gchar*) ptr->data, ".pl") != NULL)
      real_entries = g_list_prepend (real_entries, ptr->data);

	for (ptr = real_entries; ptr != NULL; ptr = ptr->next)
  {
    gchar buff[8], *command;
    GString *text;
    FILE *in;
    gint i;

    command = g_strdup_printf ("perl %s%s name", directory, (gchar*) ptr->data);
    text = g_string_new (NULL);
    in = popen (command, "r");
    while (fgets (buff, sizeof (buff), in))
    {
      i = 0;
      while (buff[i] != '\0' && i < 8)
      {
        text = g_string_append_c (text, buff[i]);
        i++;
      }
    }
    pclose (in);

    gtk_combo_box_append_text (GTK_COMBO_BOX (project->combobox), text->str);
    g_string_free (text, TRUE);
	}

  gtk_combo_box_set_active (GTK_COMBO_BOX (project->combobox), 0);
  g_list_foreach (entries, (GFunc) g_free, NULL);
  g_list_free (entries);
  g_list_free (real_entries);
}

static gboolean
openldev_new_project_dialog_check_for_errors (NewProjectDialog* project)
{
  const gchar *ntxt = gtk_entry_get_text (GTK_ENTRY (project->name)),
              *vtxt = gtk_entry_get_text (GTK_ENTRY (project->version));
  guint i;

  // Check for error when creating a project or file
  if (strlen (ntxt) == 0)
    return FALSE;
  else if (!isalpha (ntxt[0]))
    return FALSE;

  for (i = 1; i < strlen (ntxt); i++)
    if (!isalnum (ntxt[i]) && ntxt[i] != '_')
      return FALSE;

  if (strlen (vtxt) == 0)
    return FALSE;

  return TRUE;
}

static gboolean
openldev_new_project_dialog_space (GtkWidget *widget,
                                   GdkEventKey *event,
                                   gpointer data)
{
  if (event->keyval == GDK_space ||
      event->keyval == GDK_KP_Space)
    return TRUE;

  return FALSE;
}
