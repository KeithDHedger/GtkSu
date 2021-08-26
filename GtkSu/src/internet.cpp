/*
 *
 * ©K. D. Hedger. Tue 28 Jun 10:44:42 BST 2016 keithdhedger@gmail.com
 
 * Projects is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * Projects is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Projects.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <stdlib.h>

#include "config.h"
#include "internet.h"

#ifndef _USEQT5_
const char	*moreapps[]={
								"\nMore by the same author\n",
								"KKEdit\n" KKEDITPAGE "\n",
								"KKTerminal\n" KKTERMINALPAGE "\n",
								"XDecorations\n" XDECS "\n",
								"Xfce-Theme-Manager\n" THEMEMANAGER "\n",
								"Xfce4-Composite-Editor\n" COMPMANAGER "\n",
								"Manpage Editor\n" MANPAGEPAGE "\n",
								"GtkSu\n" GTKSU "\n",
								"ASpell GUI\n" ASPELLPAGE "\n",
								"Clipboard Viewer\n" CLIPVIEW,
								"\nDevelopment versions can be found here:\n" MYWEBSITE
							};

const char	*authors[]={"K.D.Hedger <" MYEMAIL ">",MYWEBSITE,moreapps[0],moreapps[1],moreapps[2],moreapps[3],moreapps[4],moreapps[5],moreapps[6],moreapps[7],moreapps[8],moreapps[9],NULL};
#else
const char	*authors="K.D.Hedger ©2013-2014<br><a href=\"mailto:" MYEMAIL "\">Email Me</a><br>" \
				"<a href=\"" GLOBALWEBSITE "\">Homepage</a>" \
				"<br><br>More by the same author<br>" \
				
				"<a href=\"" KKEDITPAGE "\">KKEdit<br>" \
				"<a href=\"" XDECS "\">XDecorations<br>" \
				"<a href=\"" THEMEMANAGER "\">Xfce-Theme-Manager<br>" \
				"<a href=\"" COMPMANAGER "\">Xfce4-Composite-Editor<br>" \
				"<a href=\"" MANPAGEPAGE "\">Manpage Editor<br>" \
				"<a href=\"" GTKSU "\">GtkSu<br>" \
				"<a href=\"" ASPELLPAGE "\">ASpell<br>" \
				"<a href=\"" CLIPVIEW "\">Clipboard Viewer<br>";
#endif
