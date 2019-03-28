#ifndef _PLAYER_H
#define _PLAYER_H

#include "stdafx.h"
#include "App/app.h"
#include "SimpleTileMap.h"

class CPlayer {
public: 
	CPlayer() {
		Create();
	}
	~CPlayer() {
	}

	void Create();

	float GetX() const { return p_positionX; }
	float GetY() const { return p_positionY; }

	void SetX(float newX);
	void SetY(float newY);

	int GetPillCount() const { return p_pillCount; }
	int GetHighestScore() const { return p_highestScore; }

	void SetPillCount(int newPillCount);
	void IncPillCount(int add) { p_pillCount += add; }

	int GetLifeCount() const { return p_lifeCount; }
	void DecLifeCount();

	void MoveX(float X);
	void MoveY(float Y);

	bool CanPlayerMove() const { return canMove; }
	void SetCanMove(bool newBool);

	void Render(const float tW, const float tH) const;
private:
	float p_positionX;
	float p_positionY;
	int p_pillCount;
	int p_lifeCount;
	int p_highestScore;
	bool canMove = false; 
	// canMove is to disallow one to hold a movement key to move. One move per press.
};

#endif //_PLAYER_H