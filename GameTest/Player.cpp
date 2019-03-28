#include "stdafx.h"
#include "Player.h"

static float p_playerDef[4]{ 1.0f, 1.0f, 0.0f, 0.5f };

void CPlayer::Create() {
	p_positionX = (float)APP_VIRTUAL_WIDTH / 2;
	p_positionY = (float)APP_VIRTUAL_HEIGHT / 2;
	p_pillCount = 0;
	p_lifeCount = 3;
	p_highestScore = 0;
}

void CPlayer::SetX(float newX) {
	p_positionX = newX;
}

void CPlayer::SetY(float newY) {
	p_positionY = newY;
}

void CPlayer::SetPillCount(int newPillCount) {
	p_pillCount = newPillCount;
}

void CPlayer::DecLifeCount() {
	p_lifeCount--;
	if (p_pillCount > p_highestScore) 
		p_highestScore = p_pillCount;
	SetPillCount(0);
}

void CPlayer::MoveX(float X) {
	p_positionX = p_positionX + X;
	canMove = false;
}

void CPlayer::MoveY(float Y) {
	p_positionY = p_positionY + Y;
	canMove = false;
}

void CPlayer::SetCanMove(bool newBool) {
	canMove = newBool;
}

void CPlayer::Render(const float tW, const float tH) const {
	App::DrawQuad(
		p_positionX - 0.25f * tW,
		p_positionY - 0.35f * tH,
		p_positionX + 0.25f * tW,
		p_positionY + 0.35f * tH,
		p_playerDef[0],
		p_playerDef[1],
		p_playerDef[2]
	);
}