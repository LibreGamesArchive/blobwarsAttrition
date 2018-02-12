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

#include "atlasTest.h"

void initAtlasTest(void)
{
	dev.cheatStatic = 0;
	dev.cheatBlind = 0;
	dev.cheatNoEnemies = 0;
	dev.cheatKeys = 1;
	dev.cheatPower = 1;
	dev.cheatHealth = 1;
	
	initGame();
	
	initHub();
	
	loadWorld("beachApproach");
	
	initWorld();
	
	initMap();
	
	initEntities();
	
	initRadar();
}
