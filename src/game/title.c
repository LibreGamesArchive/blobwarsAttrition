/*
Copyright (C) 2018 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "title.h"

static void logic(void);
static void draw(void);
static int getRecentSave(void);
static void populateSaveSlotWidgets(void);
static void doNewGame(void);
static void doLoadGame(void);
static void doContinueGame(void);
static void doOptions(void);
static void doCredits(void);
static void doQuit(void);
static void doSaveSlot(void);
static void doLoadCancel(void);
static void doOK(void);
static void doCancel(void);

static Texture *atlasTexture;
static Atlas *title;
static int recentSaveSlot;
static int saveAction;
static Widget *newGame;
static Widget *load;
static Widget *continueGame;
static Widget *options;
static Widget *credits;
static Widget *quit;
static Widget *save[MAX_SAVE_SLOTS];
static Widget *loadCancel;
static Widget *ok;
static Widget *cancel;

void initTitle(void)
{
	startSectionTransition();
	
	atlasTexture = getTexture("gfx/atlas/atlas.png");
	
	title = getImageFromAtlas("gfx/main/title.png");
	
	newGame = getWidget("new", "title");
	newGame->action = &doNewGame;

	load = getWidget("load", "title");
	load->action = &doLoadGame;

	continueGame = getWidget("continue", "title");
	continueGame->action = &doContinueGame;

	options = getWidget("options", "title");
	options->action = &doOptions;

	credits = getWidget("credits", "title");
	credits->action = &doCredits;

	quit = getWidget("exit", "title");
	quit->action = &doQuit;

	populateSaveSlotWidgets();

	loadCancel = getWidget("cancel", "load");
	loadCancel->action = doLoadCancel;

	ok = getWidget("ok", "destroy");
	ok->action = doOK;

	cancel = getWidget("cancel", "destroy");
	cancel->action = doCancel;
	
	recentSaveSlot = getRecentSave();
	
	showWidgetGroup("title");
	
	if (recentSaveSlot != -1)
	{
		setSelectedWidget("continue", "title");
	}
	else
	{
		load->disabled = 1;
		continueGame->disabled = 1;
	}
	
	app.delegate.logic = &logic;
	app.delegate.draw = &draw;
	
	endSectionTransition();
}

static void logic(void)
{
	doWidgets();
}

static void draw(void)
{
	blitRect(atlasTexture->texture, SCREEN_WIDTH / 2, 175, &title->rect, 1);
	
	drawText(10, SCREEN_HEIGHT - 30, 16, TA_LEFT, colors.white, "Copyright 2014, 2018 Parallel Realities");
	drawText(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 30, 16, TA_RIGHT, colors.white, "Version %.2f.%d", VERSION, REVISION);
	
	drawWidgets();
}

static int getRecentSave(void)
{
	char filename[MAX_FILENAME_LENGTH];
	int i, slot, curModTime, modTime;
	
	slot = -1;
	modTime = 0;
	
	for (i = 0 ; i < MAX_SAVE_SLOTS ; i++)
	{
		sprintf(filename, "%s/%d/game.json", app.saveDir, i);
		
		if (fileExists(filename))
		{
			curModTime = getFileModTime(filename);
			
			if (curModTime > modTime)
			{
				modTime = curModTime;
				slot = i;
			}
		}
	}
	
	return slot;
}

static void populateSaveSlotWidgets(void)
{
	int i;
	char name[MAX_NAME_LENGTH], filename[MAX_FILENAME_LENGTH];

	for (i = 0 ; i < MAX_SAVE_SLOTS ; i++)
	{
		sprintf(name, "save%d", i);

		save[i] = getWidget(name, "saveSlot");

		sprintf(filename, "%s/%d/game.json", app.saveDir, i);

		if (fileExists(filename))
		{
			strcpy(save[i]->label, getSaveWidgetLabel(filename));
			save[i]->value[0] = 1;
		}
		else
		{
			strcpy(save[i]->label, "(empty)");
			save[i]->value[0] = 0;
		}
		
		save[i]->value[1] = i;

		save[i]->action = &doSaveSlot;
	}
}

static void doNewGame(void)
{
	saveAction = SA_DELETE;
	
	destroyGame();
}

static void doLoadGame(void)
{
	saveAction = SA_LOAD;
	
	showWidgetGroup("saveSlot");
	
	loadCancel->visible = 1;
}

static void doContinueGame(void)
{
	game.saveSlot = continueGame->value[1];
	
	loadGame();
		
	initHub();
}

static void doOptions(void)
{

}

static void doCredits(void)
{

}

static void doQuit(void)
{
	exit(1);
}

static void doSaveSlot(void)
{
	Widget *w;
	
	w = getSelectedWidget();
	
	game.saveSlot = w->value[1];
	
	if (saveAction == SA_LOAD)
	{
		loadGame();
		
		initHub();
	}
	else if (saveAction == SA_DELETE)
	{
		
	}
}

static void doLoadCancel(void)
{
	showWidgetGroup("title");
}

static void doOK(void)
{

}

static void doCancel(void)
{

}
