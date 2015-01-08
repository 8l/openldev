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

#ifndef OPENLDEV_MENU_CALLBACKS_H
#define OPENLDEV_MENU_CALLBACKS_H

#include <gtk/gtk.h>

typedef struct _OpenLDev  OpenLDev;

// Popup menu action handlers
void openldev_on_find_and_replace (GtkWidget*, OpenLDev*);
void openldev_refresh_file_browser (GtkMenuItem*, OpenLDev*);
void openldev_open_file_browser (GtkMenuItem*, OpenLDev*);
void openldev_clear_build_output (GtkMenuItem*, OpenLDev*);
void openldev_clear_search_output (GtkMenuItem*, OpenLDev*);
void openldev_copy_output (GtkMenuItem*, OpenLDev*);
void openldev_jump_to_build_output (GtkMenuItem*, OpenLDev*);
void openldev_jump_to_search_output (GtkMenuItem*, OpenLDev*);

// Menu handler functions
void openldev_file_close_project (GtkMenuItem*, OpenLDev*);
void openldev_file_import_project(GtkMenuItem*, OpenLDev*);
void openldev_file_new_file      (GtkMenuItem*, OpenLDev*);
void openldev_file_new_project   (GtkMenuItem*, OpenLDev*);
void openldev_file_open          (GtkMenuItem*, OpenLDev*);
void openldev_file_open_project  (GtkMenuItem*, OpenLDev*);
void openldev_file_print         (GtkMenuItem*, OpenLDev*);
void openldev_file_print_preview (GtkMenuItem*, OpenLDev*);
void openldev_file_revert        (GtkMenuItem*, OpenLDev*);
void openldev_file_save          (GtkMenuItem*, OpenLDev*);
void openldev_file_save_all      (GtkMenuItem*, OpenLDev*);
void openldev_file_save_as       (GtkMenuItem*, OpenLDev*);
void openldev_file_quit          (GtkMenuItem*, OpenLDev*);

void openldev_edit_copy        (GtkMenuItem*, OpenLDev*);
void openldev_edit_cut         (GtkMenuItem*, OpenLDev*);
void openldev_edit_deselect    (GtkMenuItem*, OpenLDev*);
void openldev_edit_paste       (GtkMenuItem*, OpenLDev*);
void openldev_edit_preferences (GtkMenuItem*, OpenLDev*);
void openldev_edit_redo        (GtkMenuItem*, OpenLDev*);
void openldev_edit_select_all  (GtkMenuItem*, OpenLDev*);
void openldev_edit_undo        (GtkMenuItem*, OpenLDev*);
void openldev_edit_find        (GtkMenuItem*, OpenLDev*);
void openldev_edit_find_next   (GtkMenuItem*, OpenLDev*);
void openldev_edit_goto_line   (GtkMenuItem*, OpenLDev*);
void openldev_edit_indent      (GtkMenuItem*, OpenLDev*);
void openldev_edit_unindent    (GtkMenuItem*, OpenLDev*);

void openldev_project_info        (GtkMenuItem*, OpenLDev*);
void openldev_project_preferences (GtkMenuItem*, OpenLDev*);

void openldev_build_auto_generate (GtkMenuItem*, OpenLDev*);
void openldev_build_clean         (GtkMenuItem*, OpenLDev*);
void openldev_build_compile       (GtkMenuItem*, OpenLDev*);
void openldev_build_configure     (GtkMenuItem*, OpenLDev*);
void openldev_build_execute       (GtkMenuItem*, OpenLDev*);
void openldev_build_project       (GtkMenuItem*, OpenLDev*);
void openldev_build_stop          (GtkMenuItem*, OpenLDev*);

void openldev_view_browser        (GtkMenuItem*, OpenLDev*);
void openldev_view_line_numbering (GtkMenuItem*, OpenLDev*);
void openldev_view_output         (GtkMenuItem*, OpenLDev*);
void openldev_view_statusbar      (GtkMenuItem*, OpenLDev*);
void openldev_view_toolbar        (GtkMenuItem*, OpenLDev*);
void openldev_view_fullscreen     (GtkMenuItem*, OpenLDev*);

void openldev_help_about         (GtkMenuItem*, OpenLDev*);
void openldev_help_documentation (GtkMenuItem*, OpenLDev*);
void openldev_help_report_bug    (GtkMenuItem*, OpenLDev*);

#define NUM_ENTRIES 65
static GtkActionEntry entries[] = {
  { "File", NULL, "_File" },
  { "Edit", NULL, "_Edit" },
  { "View", NULL, "_View" },
  { "Search", NULL, "_Search" },
  { "Project", NULL, "_Project" },
  { "Build", NULL, "_Build" },
  { "Help", NULL, "_Help" },
  
  { "FileNewFile", GTK_STOCK_NEW, "_New File", "<control>n", 
    "Create a new file", G_CALLBACK (openldev_file_new_file) },
  { "FileNewFileToolbar", GTK_STOCK_NEW, " New File", NULL, 
    "Create a new file", G_CALLBACK (openldev_file_new_file) },
  { "FileNewProject", "NEWPROJECT", "New Project", "<control><shift>n", 
    "Create a new project", G_CALLBACK (openldev_file_new_project) },
  { "FileOpen", GTK_STOCK_OPEN, "_Open", "<control>o", 
    "Open an existing file", G_CALLBACK (openldev_file_open) },
  { "FileOpenProject", GTK_STOCK_OPEN, "Open Pro_ject", "<control><shift>o", 
    "Open an existing project", G_CALLBACK (openldev_file_open_project) },
  { "FileImportProject", GTK_STOCK_CONVERT, "_Import Project", NULL, 
    "Import a new OpenLDev project", G_CALLBACK (openldev_file_import_project) },
  { "FileOpenToolbar", GTK_STOCK_OPEN, " Open", NULL, 
    "Open an existing file", G_CALLBACK (openldev_file_open) },
  { "FileSave", GTK_STOCK_SAVE, "_Save", "<control>s", 
    "Save current file", G_CALLBACK (openldev_file_save) },
  { "FileSaveAs", GTK_STOCK_SAVE_AS, "Save _As", "<control><shift>s", 
    "Save file by new name", G_CALLBACK (openldev_file_save_as) },
  { "FileSaveAll", NULL, "Save All", "<control><shift>l", 
    "Save all open files", G_CALLBACK (openldev_file_save_all) },
  { "FileRevert", GTK_STOCK_REVERT_TO_SAVED, "_Reload File", NULL, 
    "Revert to saved file", G_CALLBACK (openldev_file_revert) },
  { "FilePrint", GTK_STOCK_PRINT, "_Print", "<control>p", 
    "Print the current file", G_CALLBACK (openldev_file_print) },
  { "FilePrintPreview", GTK_STOCK_PRINT_PREVIEW, "Print Preview", "<control><shift>p", 
    "Preview the current file", G_CALLBACK (openldev_file_print_preview) },
  { "FileClose", GTK_STOCK_CLOSE, "_Close Project", "<control><shift>w", 
    "Close current project", G_CALLBACK (openldev_file_close_project) },
  { "FileQuit", GTK_STOCK_QUIT, "_Quit", "<control>q", 
    "Quit the application", G_CALLBACK (openldev_file_quit) },
  { "FileRecent1", NULL, "", NULL, "", NULL },
  { "FileRecent2", NULL, "", NULL, "", NULL },
  { "FileRecent3", NULL, "", NULL, "", NULL },
  { "FileRecent4", NULL, "", NULL, "", NULL },
  { "FileRecent5", NULL, "", NULL, "", NULL },

  { "EditUndo", GTK_STOCK_UNDO, "_Undo", "<control>z", 
    "Undo last action", G_CALLBACK (openldev_edit_undo) },
  { "EditRedo", GTK_STOCK_REDO, "_Redo", "<control><shift>z", 
    "Redo last undone action", G_CALLBACK (openldev_edit_redo) },
  { "EditCut", GTK_STOCK_CUT, "C_ut", "<control>x", 
    "Cut selected text", G_CALLBACK (openldev_edit_cut) },
  { "EditCopy", GTK_STOCK_COPY, "_Copy", "<control>c", 
    "Copy selected text", G_CALLBACK (openldev_edit_copy) },
  { "EditPaste", GTK_STOCK_PASTE, "_Paste", "<control>v", 
    "Paste text from clipboard", G_CALLBACK (openldev_edit_paste) },
  { "EditSelectAll", NULL, "Select _All", "<control>a", 
    "Select all text", G_CALLBACK (openldev_edit_select_all) },
  { "EditDeselect", NULL, "_Deselect", NULL, 
    "Deselect selected text", G_CALLBACK (openldev_edit_deselect) },
  { "EditFind", GTK_STOCK_FIND, "_Find", "<control>f", 
    "Open the Find toolbar", G_CALLBACK (openldev_edit_find) },
  { "EditFindNext", GTK_STOCK_FIND, "Find _Next", "<control>g", 
    "Find next match", G_CALLBACK (openldev_edit_find_next) },
  { "EditReplace", GTK_STOCK_FIND_AND_REPLACE, "Replace", "<control>r", 
    "Find and replace", G_CALLBACK (openldev_on_find_and_replace) },
  { "EditGotoLine", GTK_STOCK_JUMP_TO, "_Goto Line", "<control>i", 
    "Open Goto Line toolbar", G_CALLBACK (openldev_edit_goto_line) },
  { "EditIndent", GTK_STOCK_INDENT, "Indent", "<control>t", 
    "Indent the current line", G_CALLBACK (openldev_edit_indent) },
  { "EditUnIndent", GTK_STOCK_UNINDENT, "Unindent", "<control><shift>t", 
    "Unindent the current line", G_CALLBACK (openldev_edit_unindent) },
  { "EditSettings", GTK_STOCK_PREFERENCES, "Preferences", NULL, 
    "Edit Environment settings", G_CALLBACK (openldev_edit_preferences) },
  
  { "ViewFiles", GTK_STOCK_JUSTIFY_LEFT, "_Project Files", NULL, 
    "Show or hide project files", G_CALLBACK (openldev_view_browser) },
  { "ViewOutput", GTK_STOCK_JUSTIFY_LEFT, "_Build Output", NULL, 
    "Show or hide build output", G_CALLBACK (openldev_view_output) },
  
  { "ProjectConfigure", GTK_STOCK_PREFERENCES, "_Configure Project", NULL, 
    "Edit project properties", G_CALLBACK (openldev_project_preferences) },
  { "ProjectInfo", "PROJECTINFO", "_Project Info", NULL, 
    "View project info", G_CALLBACK (openldev_project_info) },
  
  { "BuildCompile", "COMPILE_SMALL", "_Compile", "F9", 
    "Compile current file", G_CALLBACK (openldev_build_compile) },
  { "BuildCompileToolbar", "COMPILE", "_Compile", NULL, 
    "Compile current file", G_CALLBACK (openldev_build_compile) },
  { "BuildBuild", "BUILD_SMALL", "_Build", "F11", 
    "Build project tree", G_CALLBACK (openldev_build_project) },
  { "BuildBuildToolbar", "BUILD", "_Build", NULL, 
    "Build project tree", G_CALLBACK (openldev_build_project) },
  { "BuildExecute", GTK_STOCK_EXECUTE, "_Execute", "F3", 
    "Execute project", G_CALLBACK (openldev_build_execute) },
  { "BuildAutogen", GTK_STOCK_CONVERT, "_Auto Generate", NULL, 
    "Auto generate and configure", G_CALLBACK (openldev_build_auto_generate) },
  { "BuildConfigure", NULL, "Con_figure", NULL, 
    "Run configure script", G_CALLBACK (openldev_build_configure) },
  { "BuildClean", GTK_STOCK_CLEAR, "_Make Clean", NULL, 
    "Clean distribution tree", G_CALLBACK (openldev_build_clean) },
  { "BuildStop", GTK_STOCK_STOP, "_Stop", "F12", 
    "Stop current action", G_CALLBACK (openldev_build_stop) },
  
  { "HelpDocs", GTK_STOCK_HELP, "_Documentation", "F1", 
    "View project documentation", G_CALLBACK (openldev_help_documentation) },
  { "HelpBugs", NULL, "_Report Bugs", NULL, 
    "Report bugs", G_CALLBACK (openldev_help_report_bug) },
  { "HelpAbout", GTK_STOCK_ABOUT, "_About", "<alt>a",
    "About OpenLDev", G_CALLBACK (openldev_help_about) },
  
  { "SearchOutputGo", GTK_STOCK_JUMP_TO, "Go To Match", NULL, 
    "Go to line of match", G_CALLBACK (openldev_jump_to_search_output) },
  { "SearchOutputCopy", GTK_STOCK_COPY, "Copy", NULL, 
    "Copy search text", G_CALLBACK (openldev_copy_output) },
  { "SearchOutputClear", GTK_STOCK_CLEAR, "Clear", NULL, 
    "Clear serach output", G_CALLBACK (openldev_clear_search_output) },
  
  { "BuildOutputGo", GTK_STOCK_JUMP_TO, "Go To Error", NULL, 
    "Go to line of error", G_CALLBACK (openldev_jump_to_build_output) },
  { "BuildOutputCopy", GTK_STOCK_COPY, "Copy", NULL, 
    "Copy error text", G_CALLBACK (openldev_copy_output) },
  { "BuildOutputClear", GTK_STOCK_CLEAR, "Clear", NULL, 
    "Clear build output", G_CALLBACK (openldev_clear_build_output) },
  
  { "OpenFileBrowser", GTK_STOCK_OPEN, "_Open File", NULL, 
    "Open an existing file", G_CALLBACK (openldev_open_file_browser) },
  { "RefreshFileBrowser", GTK_STOCK_REFRESH, "Refresh", NULL, 
    "Refresh file browser", G_CALLBACK (openldev_refresh_file_browser) }
};

#define NUM_ENTRIES_TOGGLE 4
static GtkToggleActionEntry toggle_entries[] = {
  { "ViewToolbar", NULL, "_Toolbar", NULL, 
    "Show or hide toolbar", G_CALLBACK (openldev_view_toolbar), TRUE },
  { "ViewStatusBar", NULL, "_Status Bar", NULL, 
    "Show or hide status bar", G_CALLBACK (openldev_view_statusbar), TRUE },
  { "ViewLineNumbering", NULL, "_Line Numbering", NULL, 
    "Show or hide line numbers", G_CALLBACK (openldev_view_line_numbering), TRUE },
  { "ViewFullscreen", NULL, "_Full Screen", NULL, 
    "Set the window as fullscreen", G_CALLBACK (openldev_view_fullscreen), FALSE }
};

#endif
