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

#include "tankCommander.h"

static void activate(int activate);
static void tick(void);
static void walk(void);
static void attackPistol(void);
static void attackMissile(void);
static void die1(void);
static void die2(void);
static void attack(void);
static void applyDamage(int amount);
static SDL_Rect *getBounds(void);
static void preFire(void);
static void selectWeapon(void);
static void moveTowardsPlayer(void);

static Entity *tankTrack;
static int brakingTimer;
static int missileSprite[2];
static int aimedSprite;

void initTankCommander(Entity *e)
{
	Boss *b;
	
	initBoss(e);
	
	b = (Boss*)e;

	STRNCPY(e->name, "Tank Commander", MAX_NAME_LENGTH);

	b->sprite[FACING_LEFT] = getSpriteIndex("TankCommanderLeft");
	b->sprite[FACING_RIGHT] = getSpriteIndex("TankCommanderRight");
	b->sprite[FACING_DIE] = getSpriteIndex("TankCommanderDie");

	b->flags |= EF_EXPLODES;

	b->health = e->healthMax = 400;

	b->activate = activate;
	b->walk = walk;
	b->tick = tick;
	b->die = die1;
	b->applyDamage = applyDamage;
	b->getBounds = getBounds;

	brakingTimer = 0;

	world.boss = b;
	
	aimedSprite = getSpriteIndex("AimedShot");

	missileSprite[0] = getSpriteIndex("MissileRight");
	missileSprite[1] = getSpriteIndex("MissileLeft");

	initTankTrack(tankTrack);
}

static void activate(int activate)
{
	Boss *b;
	
	b = (Boss*)self;
	
	b->flags &= ~EF_GONE;
	tankTrack->flags &= ~EF_GONE;

	world.isBossActive = 1;

	addTeleportStars(self);
	addTeleportStars(tankTrack);

	playMusic("", 1);
}

static void tick(void)
{
	Boss *b;
	
	b = (Boss*)self;
	
	if (b->health > 0)
	{
		b->facing = (world.bob->x < b->x) ? FACING_LEFT : FACING_RIGHT;

		b->reload = limit(b->reload - 1, 0, FPS);

		brakingTimer = limit(brakingTimer - 1, 0, FPS);

		if (brakingTimer > 0)
		{
			b->dx *= 0.9;
			b->dy *= 0.9;
		}
	}
}

static void lookForPlayer(void)
{
	Boss *b;
	
	b = (Boss*)self;
	
	b->thinkTime = rrnd(0, FPS / 2);

	if (rand() % 10 == 0)
	{
		brakingTimer = rrnd(60, 120);
	}

	if (getDistance(world.bob->x, world.bob->y, b->x, b->y) > 650)
	{
		moveTowardsPlayer();
		return;
	}

	if (!enemyCanSeePlayer(self))
	{
		moveTowardsPlayer();
		return;
	}

	if (rand() % 100 <15)
	{
		selectWeapon();
	}

	moveTowardsPlayer();
}

static void walk(void)
{
	self->action = (self->health > 0) ? lookForPlayer : die1;
}

static void moveTowardsPlayer(void)
{
	if (brakingTimer == 0)
	{
		if (world.bob->x < self->x)
		{
			self->dx -= 0.5;
		}

		if (world.bob->x > self->x)
		{
			self->dx += 0.5;
		}
	}

	self->dx = limit(self->dx, -7.5, 7.5);
}

static void selectWeapon(void)
{
	Boss *b;
	
	b = (Boss*)self;
	
	if (abs(b->y - world.bob->y) > 64)
	{
		b->weaponType = WPN_AIMED_PISTOL;
		b->shotsToFire = rrnd(4, 12);
		b->action = preFire;
	}
	else
	{
		b->weaponType = WPN_MISSILE;
		b->shotsToFire = rrnd(1, 3);
		b->action = preFire;
	}
}

static void preFire(void)
{
	Boss *b;
	
	b = (Boss*)self;
	
	if (b->reload > 0)
	{
		moveTowardsPlayer();
		return;
	}

	attack();

	b->shotsToFire--;

	if (b->shotsToFire == 0)
	{
		b->walk();
	}
}

static void attack(void)
{
	Boss *b;
	
	b = (Boss*)self;
	
	switch (b->weaponType)
	{
		case WPN_AIMED_PISTOL:
			attackPistol();
			break;
		case WPN_MISSILE:
			attackMissile();
			break;
		default:
			break;
	}
}

static void attackPistol(void)
{
	int bx, by;
	float dx, dy;
	Bullet *bullet;
	Boss *b;
	
	b = (Boss*)self;
	
	bx = world.bob->x + rrnd(-8, 24);
	by = world.bob->y + rrnd(-8, 24);

	getSlope(bx, by, b->x, b->y, &dx, &dy);

	bullet = createBaseBullet((Unit*)self);
	bullet->x = (b->x + b->w / 2);
	bullet->y = b->y + 30;
	bullet->facing = b->facing;
	bullet->damage = 1;
	bullet->owner = self;
	bullet->health = FPS * 3;
	bullet->weaponType = WPN_AIMED_PISTOL;
	bullet->dx = dx * 12;
	bullet->dy = dy * 12;
	bullet->sprite[0] = bullet->sprite[1] = aimedSprite;

	b->reload = 4;

	playSound(SND_MACHINE_GUN, CH_WEAPON);
}

static void attackMissile(void)
{
	Bullet *missile;
	Boss *b;
	
	b = (Boss*)self;

	missile = createBaseBullet((Unit*)self);
	missile->x = b->x + b->w / 2;
	missile->y = b->y + 30;
	missile->facing = b->facing;
	missile->dx = b->facing == FACING_RIGHT ? 15 : -15;
	missile->dy = world.bob->y - missile->y;
	missile->dy *= 0.01;
	missile->owner = self;
	missile->health = FPS * 3;
	missile->sprite[0] = missileSprite[0];
	missile->sprite[1] = missileSprite[1];

	b->reload = 15;

	playSound(SND_MISSILE, CH_WEAPON);
}

static void die1(void)
{
	Boss *b;
	
	b = (Boss*)self;
	
	b->flags |= EF_BOUNCES;

	b->action = die2;
	b->thinkTime = 0;

	b->spriteTime = 0;
	b->spriteFrame = 0;
}

static void die2(void)
{
	int mx, my;
	Boss *b;
	
	b = (Boss*)self;

	b->health--;

	if (b->health % 3 == 0)
	{
		mx = (int) ((b->x + (b->w / 2)) / MAP_TILE_SIZE);
		my = (int) ((b->y + b->h) / MAP_TILE_SIZE);
		
		addScorchDecal(mx, my);

		addExplosion(b->x + rand() % b->w, b->y + rand() % b->h, 50, self);
	}

	if (b->health <= -100)
	{
		addTeleportStars(self);
		addTeleportStars(tankTrack);

		playSound(SND_APPEAR, CH_ANY);

		b->alive = tankTrack->alive = ALIVE_DEAD;

		updateObjective(b->name);

		addDefeatedTarget(b->name);

		game.enemiesKilled++;
	}
}

static void applyDamage(int amount)
{
	self->health -= amount;

	if (self->health <= 0)
	{
		self->health = 0;
		addExplosion(self->x + self->w / 2, self->y + self->h / 2, 50, self);
	}
}

static SDL_Rect *getBounds(void)
{
	if (self->facing == FACING_LEFT)
	{
		self->bounds.x = self->x + 98;
		self->bounds.y = self->y;
		self->bounds.w = 140;
		self->bounds.h = self->h;
	}
	else
	{
		self->bounds.x = self->x;
		self->bounds.y = self->y;
		self->bounds.w = 140;
		self->bounds.h = self->h;
	}

	return &self->bounds;
}