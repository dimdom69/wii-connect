/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * menu.h
 * Menu flow routines - handles all menu logic
 ***************************************************************************/

#ifndef _MENU_H_
#define _MENU_H_

#include <ogcsys.h>

void InitGUIThreads();
void MainMenu (int menuitem);

enum
{
	MENU_EXIT = -1,
	FRIEND_MENU,
	MENU_NONE,
	MAIN_SCREEN
};

struct frienddata{
	char *ip;
	char *name;
};


#endif
