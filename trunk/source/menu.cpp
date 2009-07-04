/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * menu.cpp
 * Menu flow routines - handles all menu logic
 ***************************************************************************/

#include <gccore.h>
#include <ogcsys.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <fstream>
#include <debug.h>
#include <iostream>


#include "libwiigui/gui.h"
#include "menu.h"
#include "demo.h"
#include "input.h"
#include "filelist.h"
#include "internet.h"

using namespace std;

extern void startnetwork();
static GuiImageData * pointer[4];
static GuiImageData * ibg = NULL;
static GuiImageData * isidebar = NULL;
static GuiImageData * iwiiconnect = NULL;
static GuiImage * bg = NULL;
static GuiImage * sidebar = NULL;
static GuiImage * wiiconnect = NULL;
static GuiSound * bgMusic = NULL;
static GuiWindow * mainWindow = NULL;
static GuiButton * bmainWindow = NULL;
static lwp_t guithread = LWP_THREAD_NULL;
static bool guiHalt = true;
extern _netaction netaction;
struct emsg *ms;
extern email *eml;


/****************************************************************************
 * ResumeGui
 *
 * Signals the GUI thread to start, and resumes the thread. This is called
 * after finishing the removal/insertion of new elements, and after initial
 * GUI setup.
 ***************************************************************************/
static void
ResumeGui()
{
	guiHalt = false;
	LWP_ResumeThread (guithread);
}

/****************************************************************************
 * HaltGui
 *
 * Signals the GUI thread to stop, and waits for GUI thread to stop
 * This is necessary whenever removing/inserting new elements into the GUI.
 * This eliminates the possibility that the GUI is in the middle of accessing
 * an element that is being changed.
 ***************************************************************************/
static void
HaltGui()
{
	guiHalt = true;

	// wait for thread to finish
	while(!LWP_ThreadIsSuspended(guithread))
		usleep(50);
}














/****************************************************************************
 * WindowPrompt
 *
 * Displays a prompt window to user, with information, an error message, or
 * presenting a user with a choice
 ***************************************************************************/
int
WindowPrompt(const char *title, const char *msg, const char *btn1Label, const char *btn2Label)
{
	int choice = -1;

	GuiWindow promptWindow(448,288);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);
	GuiSound btnSoundOver(button_over_pcm, button_over_pcm_size, SOUND_PCM);
	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiImageData dialogBox(dialogue_box_png);
	GuiImage dialogBoxImg(&dialogBox);

	GuiText titleTxt(title, 26, (GXColor){0, 0, 0, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0,40);
	GuiText msgTxt(msg, 22, (GXColor){0, 0, 0, 255});
	msgTxt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	msgTxt.SetPosition(0,-20);
	msgTxt.SetMaxWidth(430);

	GuiText btn1Txt(btn1Label, 22, (GXColor){0, 0, 0, 255});
	GuiImage btn1Img(&btnOutline);
	GuiImage btn1ImgOver(&btnOutlineOver);
	GuiButton btn1(btnOutline.GetWidth(), btnOutline.GetHeight());

	if(btn2Label)
	{
		btn1.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
		btn1.SetPosition(20, -25);
	}
	else
	{
		btn1.SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
		btn1.SetPosition(0, -25);
	}

	btn1.SetLabel(&btn1Txt);
	btn1.SetImage(&btn1Img);
	btn1.SetImageOver(&btn1ImgOver);
	btn1.SetSoundOver(&btnSoundOver);
	btn1.SetTrigger(&trigA);
	btn1.SetState(STATE_SELECTED);
	btn1.SetEffectGrow();

	GuiText btn2Txt(btn2Label, 22, (GXColor){0, 0, 0, 255});
	GuiImage btn2Img(&btnOutline);
	GuiImage btn2ImgOver(&btnOutlineOver);
	GuiButton btn2(btnOutline.GetWidth(), btnOutline.GetHeight());
	btn2.SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
	btn2.SetPosition(-20, -25);
	btn2.SetLabel(&btn2Txt);
	btn2.SetImage(&btn2Img);
	btn2.SetImageOver(&btn2ImgOver);
	btn2.SetSoundOver(&btnSoundOver);
	btn2.SetTrigger(&trigA);
	btn2.SetEffectGrow();

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);
	promptWindow.Append(&msgTxt);
	promptWindow.Append(&btn1);

	if(btn2Label)
		promptWindow.Append(&btn2);

	promptWindow.SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_IN, 50);
	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	while(choice == -1)
	{
		VIDEO_WaitVSync();

		if(btn1.GetState() == STATE_CLICKED)
			choice = 1;
		else if(btn2.GetState() == STATE_CLICKED)
			choice = 0;
	}

	promptWindow.SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
	while(promptWindow.GetEffect() > 0) usleep(50);
	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	return choice;
}

/****************************************************************************
 * UpdateGUI
 *
 * Primary thread to allow GUI to respond to state changes, and draws GUI
 ***************************************************************************/

static void *
UpdateGUI (void *arg)
{
	while(1)
	{
		if(guiHalt)
		{
			LWP_SuspendThread(guithread);
		}
		else
		{
			mainWindow->Draw();

			#ifdef HW_RVL
			for(int i=3; i >= 0; i--) // so that player 1's cursor appears on top!
			{
				if(userInput[i].wpad.ir.valid)
					Menu_DrawImg(userInput[i].wpad.ir.x-48, userInput[i].wpad.ir.y-48,
						96, 96, pointer[i]->GetImage(), userInput[i].wpad.ir.angle, 1, 1, 255);
				DoRumble(i);
			}
			#endif

			Menu_Render();

			for(int i=0; i < 4; i++)
				mainWindow->Update(&userInput[i]);

			if(ExitRequested)
			{
				for(int a = 0; a < 255; a += 15)
				{
					mainWindow->Draw();
					Menu_DrawRectangle(0,0,screenwidth,screenheight,(GXColor){0, 0, 0, a},1);
					Menu_Render();
					
				}
				ExitApp();
			}
		}
	}
	return NULL;
}

/****************************************************************************
 * InitGUIThread
 *
 * Startup GUI threads
 ***************************************************************************/
void
InitGUIThreads()
{
	LWP_CreateThread (&guithread, UpdateGUI, NULL, NULL, 0, 70);
}

/****************************************************************************
 * OnScreenKeyboard
 *
 * Opens an on-screen keyboard window, with the data entered being stored
 * into the specified variable.
 ***************************************************************************/
static void OnScreenKeyboard(char * var, u16 maxlen)
{
	int save = -1;

	GuiKeyboard keyboard(var, maxlen);

	GuiSound btnSoundOver(button_over_pcm, button_over_pcm_size, SOUND_PCM);
	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiText okBtnTxt("OK", 22, (GXColor){0, 0, 0, 255});
	GuiImage okBtnImg(&btnOutline);
	GuiImage okBtnImgOver(&btnOutlineOver);
	GuiButton okBtn(btnOutline.GetWidth(), btnOutline.GetHeight());

	okBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	okBtn.SetPosition(25, -25);

	okBtn.SetLabel(&okBtnTxt);
	okBtn.SetImage(&okBtnImg);
	okBtn.SetImageOver(&okBtnImgOver);
	okBtn.SetSoundOver(&btnSoundOver);
	okBtn.SetTrigger(&trigA);
	okBtn.SetEffectGrow();

	GuiText cancelBtnTxt("Cancel", 22, (GXColor){0, 0, 0, 255});
	GuiImage cancelBtnImg(&btnOutline);
	GuiImage cancelBtnImgOver(&btnOutlineOver);
	GuiButton cancelBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	cancelBtn.SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
	cancelBtn.SetPosition(-25, -25);
	cancelBtn.SetLabel(&cancelBtnTxt);
	cancelBtn.SetImage(&cancelBtnImg);
	cancelBtn.SetImageOver(&cancelBtnImgOver);
	cancelBtn.SetSoundOver(&btnSoundOver);
	cancelBtn.SetTrigger(&trigA);
	cancelBtn.SetEffectGrow();

	keyboard.Append(&okBtn);
	keyboard.Append(&cancelBtn);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&keyboard);
	mainWindow->ChangeFocus(&keyboard);
	ResumeGui();

	while(save == -1)
	{
		VIDEO_WaitVSync();

		if(okBtn.GetState() == STATE_CLICKED)
			save = 1;
		else if(cancelBtn.GetState() == STATE_CLICKED)
			save = 0;
	}

	if(save)
	{
		snprintf(var, maxlen, "%s", keyboard.kbtextstr);
	}

	HaltGui();
	mainWindow->Remove(&keyboard);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
}


/****************************************************************************
 * FriendMenu
 * *************************************************************************/
static int friendmenu(){
	
	int menu = MENU_FRIEND;
	
/*	HaltGui();
	//GuiImage rect(screenwidth,screenheight,(GXColor){0,0,0,0});
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	GuiWindow effect;
	effect.Append(&rect);
	mainWindow->Append(&effect);
	for(int a = 0; a < 129; a += 4)
	{
		for(int x = 0;x<=screenwidth;x++){
			for(int y = 0;y<=screenheight;y++){
				rect.SetPixel(x,y,(GXColor){0,0,0,a});
			}
			mainWindow->Draw();
			Menu_Render();
		}
	}
	_break();
	FILE *fd;
	fd = fopen("/wiiconnect/friendlist.fls","a");
	int datanum = 0;
	int numfriends = 0;
	char *c;
	c = new char [99];
	frienddata fdata[99];
	
	for(int x = 0;(c[x] = (char)fgetc(fd));x++){
		c[x+1] = '\0';
		if(c[x] == '\n'){
			if(c[0] == 'i'){
				fdata[datanum].ip = new char [x+1];
				strcpy(fdata[datanum].ip,c);
				x = 0;
				datanum++;
			}
			if(c[0] == 'n'){
				fdata[datanum].name = new char [x+1];
				strcpy(fdata[datanum].name,c);
				x = 0;
				datanum++;
				numfriends++;
			}
		}
	}
	delete [] c;
	
	GuiWindow w;
	GuiImageData iaddfriend(addfriend_png);
	GuiImageData ipoke(poke_png);
	GuiSound btnSoundOver(button_over_pcm, button_over_pcm_size, SOUND_PCM);
	
	GuiText *frnd;
	frnd = new GuiText [numfriends];
	
	for(int num = 0;num<=numfriends;num++){
		frnd[num].SetText(fdata[num].name);
	}
	
	for(int num = 0;num<=numfriends;num++){
		frnd[num].SetPosition((num*30)+100,200);
		w.Append(&frnd[num]);
	}
	
	GuiImage poke(&ipoke);
	GuiButton bpoke(poke.GetWidth(),poke.GetHeight());
	bpoke.SetPosition(500,100);
	bpoke.SetImage(&poke);
	bpoke.SetTrigger(&trigA);
	bpoke.SetSoundOver(&btnSoundOver);
	bpoke.SetEffectGrow();
	
	w.Append(&bpoke);
	
	mainWindow->Append(&w);
	
	ResumeGui();

	while(menu == MENU_FRIEND)
	{
		VIDEO_WaitVSync ();
		if(bmainWindow->GetState() == STATE_CLICKED)
		{
			menu = MENU_EXIT;
		}
		if(bpoke.GetState() == STATE_CLICKED){
			menu = MENU_NONE;
		}
//		if(baddfriend.GetState() == STATE_CLICKED)
//		{
//			OnScreenKeyboard("",16);
//		}
		
	}
	HaltGui();
	mainWindow->Remove(&w);*/
	return menu;
}

static int emailmenu(){
	int menu = MENU_NONE;
	int ret;
	int i = 0;
	OptionList options;
	sprintf(options.name[i++], "From");
	sprintf(options.name[i++], "To");
	sprintf(options.name[i++], "Subject");
	sprintf(options.name[i++], "Message");
	options.length = i;

	GuiText titleTxt("Email - Send Message", 28, (GXColor){255, 255, 255, 255});
	titleTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	titleTxt.SetPosition(50,50);

	GuiSound btnSoundOver(button_over_pcm, button_over_pcm_size, SOUND_PCM);
	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiText backBtnTxt("Go Back", 22, (GXColor){0, 0, 0, 255});
	GuiImage backBtnImg(&btnOutline);
	GuiImage backBtnImgOver(&btnOutlineOver);
	GuiButton backBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	backBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	backBtn.SetPosition(100, -35);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetSoundOver(&btnSoundOver);
	backBtn.SetTrigger(&trigA);
	backBtn.SetEffectGrow();
	
	
	GuiText sendBtnTxt("Send", 22, (GXColor){0, 0, 0, 255});
	GuiImage sendBtnImg(&btnOutline);
	GuiImage sendBtnImgOver(&btnOutlineOver);
	GuiButton sendBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	sendBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	sendBtn.SetPosition(300, -35);
	sendBtn.SetLabel(&sendBtnTxt);
	sendBtn.SetImage(&sendBtnImg);
	sendBtn.SetImageOver(&sendBtnImgOver);
	sendBtn.SetSoundOver(&btnSoundOver);
	sendBtn.SetTrigger(&trigA);
	sendBtn.SetEffectGrow();

	GuiOptionBrowser optionBrowser(552, 248, &options);
	optionBrowser.SetPosition(0, 108);
	optionBrowser.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	optionBrowser.SetCol2Position(185);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&backBtn);
	w.Append(&sendBtn);
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	ResumeGui();

	while(menu == MENU_NONE)
	{
		VIDEO_WaitVSync ();

		ret = optionBrowser.GetClickedOption();
		
		snprintf (options.value[0], 50, "%s", ms->from);
		snprintf (options.value[1], 50, "%s", ms->to);
		snprintf (options.value[2], 50, "%s", ms->subject);
		snprintf (options.value[3], 200, "%s", ms->message);

		switch (ret)
		{
			case 0:
				OnScreenKeyboard(ms->from,50);
				break;

			case 1:
				OnScreenKeyboard(ms->to,50);
				break;

			case 2:
				OnScreenKeyboard(ms->subject,50);
				break;

			case 3:
				OnScreenKeyboard(ms->message,200);
				break;
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			menu = MAIN_SCREEN;
		}
		if(sendBtn.GetState() == STATE_CLICKED){
			HaltGui();
			printf("Sending...");
			eml->sendemail(ms);
			printf("Done!");
			ResumeGui();
		}
	}
	HaltGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	mainWindow->Remove(&titleTxt);
	return menu;
}

/****************************************************************************
 * MenuSettings
 ***************************************************************************/
static int MainScreen(){
	
	int menu = MENU_NONE;


	GuiImageData iwiiconnect(wiiconnect_png);
	GuiImageData ichat(chat_png);
	GuiImageData idown(down_png);
	GuiImageData iemail(email_png);
	GuiImageData ifolder(folder_png);
	GuiImageData inetwork(network_png);
	GuiImageData inews(news_png);
	GuiImageData irefresh(refresh_png);
	GuiImageData iwiiconnectblack(wiiconnectblack_png);
	GuiImageData ifriendlist(friendlist_png);
	GuiImageData ihbb(hbb_png);
	GuiImageData iexit(exit_png);
	GuiSound btnSoundOver(button_over_pcm, button_over_pcm_size, SOUND_PCM);


	

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	
	
	GuiImage chat(&ichat);
	GuiButton bchat(chat.GetWidth(),chat.GetHeight());
	bchat.SetPosition(100,200);
	bchat.SetImage(&chat);
	bchat.SetSoundOver(&btnSoundOver);
//	bchat.SetTrigger(&trigA);
	bchat.SetEffectGrow();
	
	GuiImage email(&iemail);
	GuiButton bemail(email.GetWidth(),email.GetHeight());
	bemail.SetPosition(150,90);
	bemail.SetImage(&email);
	bemail.SetSoundOver(&btnSoundOver);
	bemail.SetTrigger(&trigA);
	bemail.SetEffectGrow();
	
	GuiImage news(&inews);
	GuiButton bnews(news.GetWidth(),news.GetHeight());
	bnews.SetPosition(150,310);
	bnews.SetImage(&news);
	bnews.SetSoundOver(&btnSoundOver);
//	bnews.SetTrigger(&trigA);
	bnews.SetEffectGrow();
	
	
	GuiImage network(&inetwork);
	GuiButton bnetwork(network.GetWidth(),network.GetHeight());
	bnetwork.SetPosition(565,95);
	bnetwork.SetImage(&network);
	bnetwork.SetSoundOver(&btnSoundOver);
//	bnetwork.SetTrigger(&trigA);
	bnetwork.SetEffectGrow();
	
	GuiImage refresh(&irefresh);
	GuiButton brefresh(refresh.GetWidth(),refresh.GetHeight());
	brefresh.SetPosition(565,155);
	brefresh.SetImage(&refresh);
	brefresh.SetSoundOver(&btnSoundOver);
//	brefresh.SetTrigger(&trigA);
	brefresh.SetEffectGrow();
	
	GuiImage down(&idown);
	GuiButton bdown(down.GetWidth(),down.GetHeight());
	bdown.SetPosition(565,215);
	bdown.SetImage(&down);
	bdown.SetSoundOver(&btnSoundOver);
//	bdown.SetTrigger(&trigA);
	bdown.SetEffectGrow();
	
	GuiImage folder(&ifolder);
	GuiButton bfolder(folder.GetWidth(),folder.GetHeight());
	bfolder.SetPosition(565,270);
	bfolder.SetImage(&folder);
	bfolder.SetSoundOver(&btnSoundOver);
//	bfolder.SetTrigger(&trigA);
	bfolder.SetEffectGrow();
	
	
	GuiText netstatus("Status",22,(GXColor){255,255,255,255});
	netstatus.SetPosition(0,200);
	
	GuiImage friendlist(&ifriendlist);
	GuiButton bfriendlist(friendlist.GetWidth(),friendlist.GetHeight());
	bfriendlist.SetPosition(300,380);
	bfriendlist.SetImage(&friendlist);
	bfriendlist.SetSoundOver(&btnSoundOver);
	bfriendlist.SetTrigger(&trigA);
	bfriendlist.SetEffectGrow();
	
	GuiImage hbb(&ihbb);
	GuiButton bhbb(hbb.GetWidth(),hbb.GetHeight());
	bhbb.SetPosition(300,60);
	bhbb.SetImage(&hbb);
	bhbb.SetSoundOver(&btnSoundOver);
//	bhbb.SetTrigger(&trigA);
	bhbb.SetEffectGrow();
	
	GuiImage exit(&iexit);
	GuiButton bexit(exit.GetWidth(),exit.GetHeight());
	bexit.SetPosition(565,380);
	bexit.SetImage(&exit);
	bexit.SetSoundOver(&btnSoundOver);
	bexit.SetTrigger(&trigA);
	bexit.SetEffectGrow();
	
	
/*	GuiImage addfriend(&iaddfriend);
 *	GuiButton baddfriend(addfriend.GetWidth(),addfriend.GetHeight());
 *	baddfriend.SetPosition(200,200);
 *	baddfriend.SetImage(&addfriend);
 *	baddfriend.SetSoundOver(&btnSoundOver);
 *	baddfriend.SetTrigger(&trigA);
 *	baddfriend.SetEffectGrow();
 */
	
	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	
	w.Append(&bchat);
	w.Append(&bemail);
	w.Append(&bnews);
	w.Append(&bnetwork);
	w.Append(&brefresh);
	w.Append(&bdown);
	w.Append(&bfolder);
	w.Append(&netstatus);
	w.Append(&bfriendlist);
	w.Append(&bhbb);
	w.Append(&bexit);

	mainWindow->Append(&w);

	ResumeGui();

	while(menu == MENU_NONE)
	{
		VIDEO_WaitVSync ();
		if(bmainWindow->GetState() == STATE_CLICKED)
		{
			menu = MENU_EXIT;
		}
		if(bfriendlist.GetState() == STATE_CLICKED){
			menu = friendmenu();
		}
		if(bexit.GetState() == STATE_CLICKED){
			menu = MENU_EXIT;
		}
		if(bemail.GetState() == STATE_CLICKED){
			menu = emailmenu();
		}
//		if(baddfriend.GetState() == STATE_CLICKED)
//		{
//			OnScreenKeyboard("",16);
//		}
	}

	HaltGui();
	mainWindow->Remove(&w);
	return menu;
}

/****************************************************************************
 * MainMenu
 ***************************************************************************/
void MainMenu(int menu)
{
	int currentMenu = menu;

	#ifdef HW_RVL
	pointer[0] = new GuiImageData(player1_point_png);
	pointer[1] = new GuiImageData(player2_point_png);
	pointer[2] = new GuiImageData(player3_point_png);
	pointer[3] = new GuiImageData(player4_point_png);
	#endif
	
	
	GuiTrigger trigHome;
	trigHome.SetButtonOnlyTrigger(-1, WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME, 0);
	
	mainWindow = new GuiWindow(screenwidth, screenheight);
	bmainWindow = new GuiButton(screenwidth, screenheight);
	bmainWindow->SetTrigger(&trigHome);

	ibg = new GuiImageData(bg_png);
	isidebar = new GuiImageData(sidebar_png);
	iwiiconnect = new GuiImageData(wiiconnect_png);
	
	bg = new GuiImage(ibg);
	bg->SetPosition(0,0);
	
	sidebar = new GuiImage(isidebar);
	sidebar->SetPosition(540,70);
	
	wiiconnect = new GuiImage(iwiiconnect);
	wiiconnect->SetPosition(208,200);
	
	mainWindow->Append(bg);
	mainWindow->Append(sidebar);
	mainWindow->Append(wiiconnect);
	mainWindow->Append(bmainWindow);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	ResumeGui();

	bgMusic = new GuiSound(bg_music_ogg, bg_music_ogg_size, SOUND_OGG);
	bgMusic->SetVolume(50);
	bgMusic->Play(); // startup music

	while(currentMenu != MENU_EXIT)
	{
		switch (currentMenu)
		{
			case MAIN_SCREEN:
				currentMenu = MainScreen();
				break;
			case MENU_FRIEND:
				currentMenu = friendmenu();
				break;
			case MENU_EMAIL:
				currentMenu = emailmenu();
				break;
			default: // unrecognized menu
				currentMenu = MainScreen();
				break;
		}
	}

	ResumeGui();
	ExitRequested = 1;
	while(1) usleep(50);

	HaltGui();

	bgMusic->Stop();
	delete bgMusic;
	delete sidebar;
	delete bg;
	delete mainWindow;

	delete pointer[0];
	delete pointer[1];
	delete pointer[2];
	delete pointer[3];

	mainWindow = NULL;
}
