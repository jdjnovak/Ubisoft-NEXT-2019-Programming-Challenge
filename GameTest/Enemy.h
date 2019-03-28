#ifndef _ENEMY_H
#define _ENEMY_H

#include "stdafx.h"
#include "App/app.h"
#include "SimpleTileMap.h"
#include "Player.h"

enum AI_TYPE {
	RUNNING = -1, // I.e. running away from player
	BASIC,
	WALLJUMP,
	DASHING,
	FLASHBANG,
};

enum DIRECTION {
	NONE = -1,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class CEnemy {
public:
	CEnemy() {
	}

	CEnemy(float spawnX, float spawnY) {
		Create(spawnX, spawnY);
	}

	~CEnemy() {
	}

	void Create(float spawnX, float spawnY);

	float GetX() const { return p_positionX; }
	float GetY() const { return p_positionY; }

	void SetX(float newX);
	void SetY(float newY);

	void MoveX(float X);
	void MoveY(float Y);

	void SetColour(int colour);
	int GetColour() const { return p_colour; }

	void SetDirection(int direction);
	int GetDirection() const { return p_direction; }

	float GetDistance(float sX, float sY, float fX, float fY);

	bool IsNextTileGround(float nextX, float nextY, CSimpleTileMap *map);
	bool CheckIfIntersection(float X, float Y, CSimpleTileMap *map);

	DIRECTION GetClosest1ThicknessWall(CSimpleTileMap *map, CPlayer *player);
	void Jump(float nextX, float nextY);

	AI_TYPE GetAI() const { return p_ai; }
	void SetAI(AI_TYPE type);

	void AI_Running(CSimpleTileMap *map, int timer);
	void AI_Basic(CSimpleTileMap *map, CPlayer *player, int timer);
	void AI_WallJump(CSimpleTileMap *map, CPlayer *player, int timer);
	void AI_Dashing(CSimpleTileMap *map, int timer);
	void AI_Flashbang(CSimpleTileMap *map, CPlayer *player, int timer);
	void UpdateAI(AI_TYPE type, CSimpleTileMap *map, CPlayer *player, int timer);

	void Render(const float tW, const float tH);

	bool p_canJump;
protected:
	float p_positionX;
	float p_positionY;
	int p_colour;
	int p_direction;
	AI_TYPE p_ai;
};

#endif //_ENEMY_H