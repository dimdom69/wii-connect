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
void getuserdata(struct emsg *ms);

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


enum _netaction{
	SEND_EMAIL,
	RECV_EMAIL,
	NONE
};


#endif
