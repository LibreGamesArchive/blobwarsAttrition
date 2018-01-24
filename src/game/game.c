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

#include "game.h"

/*
public Map<String, MissionStatus> missionStatuses;

public List<String> mias;
public List<String> targets;
public Map<String, AtomicInteger> keys;
*/

void initGame(void)
{
	memset(&game, 0, sizeof(Game));
	
	game.cells = 5;
	game.hearts = 10;

	game.timePlayed = 0;
}

/*
public void updateTimePlayedString()
{
	final int hours = (int) TimeUnit.MILLISECONDS.toHours(timePlayed);
	final int minutes = (int) TimeUnit.MILLISECONDS.toMinutes(timePlayed) % 60;

	timePlayedString = String.format("%dh %02dm", hours, minutes);
}

public int getShotPercentage(int i)
{
	return BMath.getPercentage(statShotsHit[i], statShotsFired[i]);
}

public void addKey(String key)
{
	AtomicInteger i = keys.get(key);
	if (i == null)
	{
		i = new AtomicInteger();
		keys.put(key, i);
	}
	i.incrementAndGet();
}

public void removeKey(String key)
{
	AtomicInteger i = keys.get(key);
	if (i != null && i.decrementAndGet() == 0)
	{
		keys.remove(key);
	}
}
*/

void destroyGame(void)
{
}
