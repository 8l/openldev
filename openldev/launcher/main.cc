/***************************************************************************
 *   Copyright (C) 2005 by Andrew Krause                                   *
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

#include <sys/types.h>
#include <sys/wait.h>
#include <glib.h>
#include <stdlib.h>
#include <iostream>
#include <string>
using namespace std;

int main (int argc, 
          char *argv[])
{
  gint status, i, signal;
  string command;
  pid_t pid;
  
  command = "\0";
  cout << "Executing: " << "`";
  for (i = 1; i < argc; i++)
    command += argv[i] + string(" ");
  if (command.length() > 0)
    command.erase (command.length() - 1, 1);
  cout << command << "`\n"
       << "-------------------------------------------------\n";
  
  status = system (command.c_str());
  pid = wait (&status);
  cout << "\n-------------------------------------------------\n";
  
  waitpid (pid, &status, 0);
  if (WIFSIGNALED (status)) 
  {
		signal = WTERMSIG (status);
    cout << "Program has exited with signal " << signal << " "
         << g_strsignal (signal) << endl;
  }
  else if (WIFEXITED (status))
    cout << "Program exited successfully with code "
         << WEXITSTATUS (status) << endl;

  cout << "Press any key to close ... \n";
  getchar ();
  
	return WEXITSTATUS (status);
}
