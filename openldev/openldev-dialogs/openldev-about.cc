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
 
#include "openldev-about.h"

static void 
about_dialog_response (GtkDialog *dialog,
                       gint response)
{
  gtk_widget_hide (GTK_WIDGET (dialog));
}

// Create the OpenLDev about dialog
void
openldev_about_dialog_new (gchar *version)
{
  gchar *logo_location;
  GdkPixbuf *logo;

  static GtkAboutDialog *dialog = NULL;
  
  if (dialog == NULL)
  {
    dialog = GTK_ABOUT_DIALOG (gtk_about_dialog_new ());
    g_signal_connect (G_OBJECT (dialog), "response",
                      G_CALLBACK (about_dialog_response), NULL);
  
    const gchar *authors[] = {
      "Andrew Krause <andrew openldev org>", "",
      "Patch Contributors:",
      "  Aaron Sebold <bugs invigorated org>",
      "  Ben Romer <benjamin romer@unisys com>",
      "  Jarmo Hekkanen <jarski 2ndpoint fi>",
      "  Jim Hayward <jimhayward linuxexperience net>",
      "  Uwe Grawert <uwegrawe@paju.oulu.fi>",
      NULL
    };

    logo_location = openldev_add_prefix ("/share/pixmaps/openldev/openldev96.png");
    logo = gdk_pixbuf_new_from_file (logo_location, NULL);
  
    /* Populate and show the about dialog. */
    gtk_about_dialog_set_logo (dialog, logo);
    gtk_about_dialog_set_comments (dialog, "Integrated Development Environment");
    gtk_about_dialog_set_authors (dialog, authors);
    gtk_about_dialog_set_copyright (dialog, "(c) 2004-2007 Andrew Krause");
    gtk_about_dialog_set_name (dialog, "OpenLDev");
    gtk_about_dialog_set_version (dialog, version);
    gtk_about_dialog_set_website (dialog, "http://www.openldev.org");
  }
  
  gtk_widget_show (GTK_WIDGET (dialog));
}
