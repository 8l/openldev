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

#include "openldev-output-terminal.h"

static void terminal_class_init (TerminalClass *klass);
static void terminal_init (Terminal *term);

GType
terminal_get_type ()
{
  static GType terminal_type = 0;

  if (!terminal_type)
  {
    static const GTypeInfo terminal_info =
    {
	    sizeof (TerminalClass),
	    NULL, NULL,
	    (GClassInitFunc) terminal_class_init,
      NULL, NULL,
      sizeof (Terminal), 0,
	    (GInstanceInitFunc) terminal_init,
    };

    terminal_type = g_type_register_static (VTE_TYPE_TERMINAL, "Terminal", 
                                            &terminal_info, (GTypeFlags) 0);
  }

  return terminal_type;
}

static void
terminal_class_init (TerminalClass *klass)
{
}

static void
terminal_init (Terminal *term)
{
  GTK_WIDGET_SET_FLAGS (VTE_TERMINAL (term), GTK_CAN_FOCUS);
  vte_terminal_set_mouse_autohide (VTE_TERMINAL (term), TRUE);
  
  term->scrollbar = gtk_vscrollbar_new (vte_terminal_get_adjustment (VTE_TERMINAL (term)));
  term->hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (term->hbox), GTK_WIDGET (term), TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (term->hbox), term->scrollbar, FALSE, FALSE, 0);  
  
  gtk_widget_show (GTK_WIDGET (term));
  gtk_widget_show (term->scrollbar);
  gtk_widget_show (term->hbox);
}

GtkWidget*
terminal_new (EnvironmentSettings *env)
{
  GdkColor fg = { 0, 0, 0, 0}, bg = { 0, 0, 0, 0 };
  gchar *shell, *font, *front, *back;
  gboolean output, keystroke;
  GtkWidget *term;
  gint value;
  
  term = GTK_WIDGET (g_object_new (terminal_get_type (), NULL));
  
  g_object_get (env, "terminal-shell", &shell, "terminal-font", &font, NULL);
  vte_terminal_set_font_from_string (VTE_TERMINAL (term), font);
  value = vte_terminal_fork_command (VTE_TERMINAL (term), shell, 
                                     NULL, NULL, NULL, FALSE, FALSE, FALSE);
  
  g_object_get (env, "scroll-output", &output, "scroll-keystroke", &keystroke,
                "terminal-scrollback", &value, "terminal-foreground", &front,
                "terminal-background", &back, NULL);
  vte_terminal_set_scroll_on_output (VTE_TERMINAL (term), output);
  vte_terminal_set_scroll_on_keystroke (VTE_TERMINAL (term), keystroke);
  vte_terminal_set_scrollback_lines (VTE_TERMINAL (term), (glong) value);
  
  gdk_color_parse (front, &fg);
  gdk_color_parse (back, &bg);
  vte_terminal_set_colors (VTE_TERMINAL (term), &fg, &bg, NULL, 0);

  g_free (shell);
  g_free (front);
  g_free (back);
  g_free (font);
  
  return term;
}

// Run a commend in the terminal
void 
terminal_run_command (Terminal *term,
                      const char *command)
{ 
  vte_terminal_feed_child (VTE_TERMINAL (term), command, strlen(command));
}

// Let the terminal grab focus from the application
void 
openldev_output_terminal_grab_focus (Terminal *term)
{ 
  gtk_widget_grab_focus (GTK_WIDGET (term));
}

GtkWidget* 
terminal_get_hbox (Terminal *term)
{ 
  return term->hbox;
}
