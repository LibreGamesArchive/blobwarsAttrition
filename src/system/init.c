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

#include "init.h"

static void initJoypad(void);

void init18N(int argc, char *argv[])
{
	int i;
	int languageId = -1;

	setlocale(LC_NUMERIC, "");

	for (i = 1 ; i < argc ; i++)
	{
		if (strcmp(argv[i], "-language") == 0)
		{
			languageId = i + 1;

			if (languageId >= argc)
			{
				SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "You must specify a language to use with -language. Using default.");
			}
		}
	}

	setLanguage("blobwarsAttrition", languageId == -1 ? NULL : argv[languageId]);

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Numeric is %s", setlocale(LC_NUMERIC, "C"));
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "atof(2.75) is %f", atof("2.75"));
}

void initSDL(void)
{
	int rendererFlags, windowFlags;
	
	/* done in src/plat/ */
	createSaveFolder();
	
	app.winWidth = SCREEN_WIDTH;
	app.winHeight = SCREEN_HEIGHT;

	rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	
	windowFlags = 0;

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0)
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
    {
        printf("Couldn't initialize SDL Mixer\n");
		exit(1);
    }

    Mix_AllocateChannels(CH_MAX);

	app.window = SDL_CreateWindow("Blob Wars : Attrition", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, app.winWidth, app.winHeight, windowFlags);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	if (TTF_Init() < 0)
	{
		printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
		exit(1);
	}

	initJoypad();
}

static void initJoypad(void)
{
	int i, n;

	n = SDL_NumJoysticks();

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "%d joypads available", n);

	for (i = 0 ; i < n ; i++)
	{
    	if (SDL_IsGameController(i))
    	{
    		app.joypad = SDL_GameControllerOpen(i);
    		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Using joypad '%s'", SDL_GameControllerName(app.joypad));
    		return;
        }
	}
}

void initGameSystem(void)
{
	int i, numInitFuns;
	void (*initFuncs[]) (void) = {
		initLookups,
		initGraphics,
		initFonts,
		initAtlas,
		initSounds,
		initSprites,
		initEntityFactory
	};

	numInitFuns = sizeof(initFuncs) / sizeof(void*);

	for (i = 0 ; i < numInitFuns ; i++)
	{
		/*showLoadingStep(i + 1, numInitFuns);*/

		initFuncs[i]();
	}
}

void cleanup(void)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Cleaning up ...");
	
	destroyLookups();
	
	destroyFonts();
	
	destroyTextures();
	
	expireTexts(1);
	
	destroyGame();

	if (app.joypad != NULL) {
		SDL_GameControllerClose(app.joypad);
	}
	
	SDL_DestroyRenderer(app.renderer);
	
	SDL_DestroyWindow(app.window);
	
	TTF_Quit();
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Done.");
	
	SDL_Quit();
}
