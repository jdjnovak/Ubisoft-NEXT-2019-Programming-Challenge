#include "stdafx.h"
#include "Enemy.h"

static float p_enemyDef[4][4] = {
	{1.0f, 0.0f, 0.0f, 1.0f}, //RED - Basic AI
    {1.0f, 0.5f, 0.0f, 1.0f}, //ORANGE - Walljump AI
    {0.0f, 1.0f, 1.0f, 1.0f}, //CYAN - Dashing AI
    {1.0f, 0.5f, 0.75f, 1.0f}, //PINK - Flashbang AI
};

static int p_dir[4][2] = {
	{-1, 0}, //LEFT
    {1, 0}, //RIGHT
    {0, 1}, //UP
    {0, -1} // DOWN
};

void CEnemy::SetX(float newX) {
	p_positionX = newX;
}

void CEnemy::SetY(float newY) {
	p_positionY = newY;
}

void CEnemy::MoveX(float X) {
	p_positionX += X;
}

void CEnemy::MoveY(float Y) {
	p_positionY += Y;
}

void CEnemy::SetColour(int newColour) {
	p_colour = newColour;
}

void CEnemy::SetAI(AI_TYPE type) {
	p_ai = type;
}

void CEnemy::SetDirection(int direction) {
	p_direction = direction;
}

float CEnemy::GetDistance(float sX, float sY, float fX, float fY) {
	return ((fY - sY) * (fY - sY)  + (fX - sX) * (fX - sX));
}

bool CEnemy::IsNextTileGround(float nextX, float nextY, CSimpleTileMap *map) {
	return (map->GetTileMapValue(nextX, nextY) == EMapValue::FLOOR ||
		map->GetTileMapValue(nextX, nextY) == EMapValue::PILL ||
		map->GetTileMapValue(nextX, nextY) == EMapValue::PIP);
}

bool CEnemy::CheckIfIntersection(float X, float Y, CSimpleTileMap *map) {
	int numValid = 0;
	for (int direction = 0; direction < 4; direction++) {
		if (map->GetTileMapValue(X + p_dir[direction][0], Y + p_dir[direction][1]) == EMapValue::FLOOR ||
		    map->GetTileMapValue(X + p_dir[direction][0], Y + p_dir[direction][1]) == EMapValue::PILL ||
		    map->GetTileMapValue(X + p_dir[direction][0], Y + p_dir[direction][1]) == EMapValue::PIP) {
			numValid++;
		}
	}
	return numValid >= 3;
}

DIRECTION CEnemy::GetClosest1ThicknessWall(CSimpleTileMap *map, CPlayer *player) {
	for (int dir = 0; dir < 4; dir++) {
		float oneX = p_positionX + (p_dir[dir][0] * ((float)APP_VIRTUAL_WIDTH / (float)map->GetMapSize()));
		float oneY = p_positionY + (p_dir[dir][1] * ((float)APP_VIRTUAL_HEIGHT / (float)map->GetMapSize()));
		float twoX = p_positionX + (2 * p_dir[dir][0] * ((float)APP_VIRTUAL_WIDTH / (float)map->GetMapSize()));
		float twoY = p_positionY + (2 * p_dir[dir][1] * ((float)APP_VIRTUAL_HEIGHT / (float)map->GetMapSize()));
		if (map->GetTileMapValue(oneX, oneY) == EMapValue::WALL &&
			map->GetTileMapValue(twoX, twoY) != EMapValue::WALL &&
			IsNextTileGround(twoX, twoY, map) &&
			GetDistance(twoX, twoY, player->GetX(), player->GetY()) < GetDistance(p_positionX, p_positionY, player->GetX(), player->GetY()) + 10.0f) {
			return DIRECTION(dir);
		}
	}
	return DIRECTION(-1);
}

void CEnemy::Jump(float newX, float newY) {
	MoveX(newX);
	MoveY(newY);
}

//-------------------------------------------------------------------
//                     Update functions for the AI
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//  Use this while running away from the player.
//-------------------------------------------------------------------
void CEnemy::AI_Running(CSimpleTileMap *map, int timer) {
	// If distance to player >= something, turn and run other way
	// Find  Y - Player Y > 0
	// Find  X - Player X > 0
	// Move the chosen direction
}

//-------------------------------------------------------------------
//  Walk towards the player. Moves at a steady speed.
//-------------------------------------------------------------------
void CEnemy::AI_Basic(CSimpleTileMap *map, CPlayer *player, int timer) {
	if (timer % 50 == 0) {
		float nextX = (float)p_dir[p_direction][0] * ((float)APP_VIRTUAL_WIDTH / (float)map->GetMapSize());
		float nextY = (float)p_dir[p_direction][1] * ((float)APP_VIRTUAL_HEIGHT / (float)map->GetMapSize());
		if (IsNextTileGround(p_positionX + nextX, p_positionY + nextY, map)) {
		    MoveX((float)nextX);
		    MoveY((float)nextY);
		} else {
			SetDirection(rand() % 4);
		}
	}
}

//-------------------------------------------------------------------
//  Wander aimlessly, but it has a chance to jump 1-thickness wall 
//  to get closer to the player
//-------------------------------------------------------------------
void CEnemy::AI_WallJump(CSimpleTileMap *map, CPlayer *player, int timer) {
	int moveOrJump = rand() % 100;
	if (moveOrJump > 70 && p_canJump) {
		if (timer % 20 == 0) {
			int dir = rand() % 4;
			float jumpX = 2 * p_dir[dir][0] * ((float)APP_VIRTUAL_WIDTH / (float)map->GetMapSize());
			float jumpY = 2 * p_dir[dir][1] * ((float)APP_VIRTUAL_HEIGHT / (float)map->GetMapSize());
			if (IsNextTileGround(p_positionX + jumpX, p_positionY + jumpY, map)) {
				Jump(jumpX, jumpY);
			}
			p_canJump = false;
		}
	} else {
	    if (timer % 20 == 0) {
		    float nextX = (float)p_dir[p_direction][0] * ((float)APP_VIRTUAL_WIDTH / (float)map->GetMapSize());
		    float nextY = (float)p_dir[p_direction][1] * ((float)APP_VIRTUAL_HEIGHT / (float)map->GetMapSize());
		    if (IsNextTileGround(p_positionX + nextX, p_positionY + nextY, map)) {
		        MoveX((float)nextX);
		        MoveY((float)nextY);
		    } else {
		    	SetDirection(rand() % 4);
		    }
			p_canJump = true;
	    }
	}
}

//-------------------------------------------------------------------
//  Wanders aimlessly, but dashes down a single corridor at high speeds
//-------------------------------------------------------------------
void CEnemy::AI_Dashing(CSimpleTileMap *map, int timer) {
    if (timer % 5 == 0) {
	    float nextX = (float)p_dir[p_direction][0] * ((float)APP_VIRTUAL_WIDTH / (float)map->GetMapSize());
	    float nextY = (float)p_dir[p_direction][1] * ((float)APP_VIRTUAL_HEIGHT / (float)map->GetMapSize());
	    if (IsNextTileGround(p_positionX + nextX, p_positionY + nextY, map)) {
	        MoveX((float)nextX);
	        MoveY((float)nextY);
		} else {
			SetDirection(rand() % 4);
		}
    }
}

//-------------------------------------------------------------------
//  Wander aimlessly around the map. Change directions at each wall.
//-------------------------------------------------------------------
void CEnemy::AI_Flashbang(CSimpleTileMap *map, CPlayer *player, int timer) {
	// TODO: Implement this properly
	// Note: For now, just using AI_Basic();
	AI_Basic(map, player, timer);
}

// This is called and then it goes to the specific function
void CEnemy::UpdateAI(AI_TYPE ai, CSimpleTileMap *map, CPlayer *player, int timer) {
	switch (ai) {
	case AI_TYPE::BASIC:
	{
		AI_Basic(map, player, timer);
		break;
	}
	case AI_TYPE::WALLJUMP:
	{
		AI_WallJump(map, player, timer);
		break;
	}
	case AI_TYPE::DASHING:
	{
		AI_Dashing(map, timer);
		break;
	}
	case AI_TYPE::FLASHBANG:
	{
		AI_Flashbang(map, player, timer);
		break;
	}
	case AI_TYPE::RUNNING:
	{
		AI_Running(map, timer);
		break;
	}
	}
}
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------

void CEnemy::Create(float spawnX, float spawnY) {
	//srand(time(NULL));
	p_colour = rand() % 4;
	p_direction = rand() % 4;
	p_ai = AI_TYPE(p_colour);
	p_positionX = spawnX;
	p_positionY = spawnY;
	p_canJump = true;
}

void CEnemy::Render(const float tW, const float tH) {
	App::DrawQuad(
		p_positionX - 0.25f * tW,
		p_positionY - 0.35f * tH,
		p_positionX + 0.25f * tW,
		p_positionY + 0.35f * tH,
		p_enemyDef[p_colour][0],
		p_enemyDef[p_colour][1],
		p_enemyDef[p_colour][2]
	);
}