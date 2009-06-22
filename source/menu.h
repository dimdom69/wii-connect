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
	MENU_FRIEND,
	MENU_EMAIL,
	MENU_NONE,
	MAIN_SCREEN
};

typedef struct frienddata_s{
	char *ip;
	char *name;
}frienddata;


#endif
