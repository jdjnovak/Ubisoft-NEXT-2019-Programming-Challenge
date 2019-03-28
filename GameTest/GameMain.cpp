//------------------------------------------------------------------------
// GameTest.cpp
//------------------------------------------------------------------------
#include "stdafx.h"
//------------------------------------------------------------------------
#include <windows.h> 
#include <math.h>  
#include <stdlib.h>
#include <time.h>
//------------------------------------------------------------------------
#include "app\app.h"
#include "SimpleTileMap.h"
#include "Player.h"
#include "Enemy.h"

#define NUM_GHOSTS 4
#define MIN_DISTANCE_TO_PLAYER 5

bool IsNextTileGround(float, float);
void ChangePillsToFloor();

void UpdatePlayerScore();
void UpdatePlayerLives();
void MovePlayer();
void ReadPlayerInput();

bool CheckDistanceToPlayer(int sX, int sY, int fX, int fY);

void InitGhosts();
bool CheckGhostSpawns(float x, float y);
void RenderGhosts();
void UpdateGhosts(int timerUpdate);
bool CheckGhostCollisions();

static const int MAP_SIZE = 21; // Default 16
static const int TUNNEL_LEN = 19; // Default 12
static const float TUNNEL_FILL_PERCENT = 70; // Default 80
int score;
int lives;
int timer;
float spawns[NUM_GHOSTS][2] = { 0 };

CSimpleTileMap g_map(MAP_SIZE);
CPlayer p_player;
CEnemy p_enemy[NUM_GHOSTS];

static const float TILEWIDTH = (float)APP_VIRTUAL_WIDTH / (float)g_map.GetMapSize();
static const float TILEHEIGHT = (float)APP_VIRTUAL_HEIGHT / (float)g_map.GetMapSize();


//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
    srand(time(NULL));
	timer = 0;
    g_map.RandomMap(TUNNEL_FILL_PERCENT, TUNNEL_LEN);
	InitGhosts();
}

//------------------------------------------------------------------------
// Update your simulation here. deltaTime is the elapsed time since the last update in ms.
// This will be called at no greater frequency than the value of APP_MAX_FRAME_RATE
//------------------------------------------------------------------------
void Update(float deltaTime)
{
    srand(time(NULL));
	timer++;
	ChangePillsToFloor();

    static const float MIN_FRAMERATE = (1000 / 20);
    if (deltaTime > MIN_FRAMERATE)
        deltaTime = MIN_FRAMERATE;

	ReadPlayerInput();
	UpdateGhosts(timer);
	UpdatePlayerScore();
	UpdatePlayerLives();
	CheckGhostCollisions();
}

bool IsNextTileGround(float nextX, float nextY) {
	if (g_map.GetTileMapValue(nextX, nextY) == EMapValue::PILL ||
		g_map.GetTileMapValue(nextX, nextY) == EMapValue::FLOOR ||
		g_map.GetTileMapValue(nextX, nextY) == EMapValue::PIP) {
		return true;
	}
	return false;
}

void UpdatePlayerScore() {
	score = p_player.GetPillCount();
}

void UpdatePlayerLives() {
	lives = p_player.GetLifeCount();
}

void ChangePillsToFloor() {
	if (g_map.GetTileMapValue(p_player.GetX(), p_player.GetY()) == EMapValue::PILL) {
		p_player.IncPillCount(1);
	} else if (g_map.GetTileMapValue(p_player.GetX(), p_player.GetY()) == EMapValue::PIP) {
		p_player.IncPillCount(10);
	}
	g_map.SetTileMapValue((int)p_player.GetX() / (int)TILEWIDTH, (int)p_player.GetY() / (int)TILEHEIGHT, EMapValue::FLOOR);
}

void MovePlayer() {
	if (p_player.CanPlayerMove()) {
		if (App::IsKeyPressed('A') && IsNextTileGround(p_player.GetX() - TILEWIDTH, p_player.GetY())) {
			p_player.MoveX(-TILEWIDTH);
		} else if (App::IsKeyPressed('D') && IsNextTileGround(p_player.GetX() + TILEWIDTH, p_player.GetY())) {
			p_player.MoveX(TILEWIDTH);
		} else if (App::IsKeyPressed('W') && IsNextTileGround(p_player.GetX(), p_player.GetY() + TILEHEIGHT)) {
			p_player.MoveY(TILEHEIGHT);
		} else if (App::IsKeyPressed('S') && IsNextTileGround(p_player.GetX(), p_player.GetY() - TILEHEIGHT)) {
			p_player.MoveY(-TILEHEIGHT);
		}
	} else {
		if (!(App::IsKeyPressed('A') || App::IsKeyPressed('D') || App::IsKeyPressed('S') || App::IsKeyPressed('W'))) {
			p_player.SetCanMove(true);
		}
	}
}

void ReadPlayerInput() {
    if (App::GetController().CheckButton(VK_PAD_A, true)) {
        g_map.RandomMap(TUNNEL_FILL_PERCENT, TUNNEL_LEN);
		InitGhosts();
    }
	MovePlayer();
}

void UpdateGhosts(int timerUpdate) {
	for (int ghost = 0; ghost < NUM_GHOSTS; ghost++) {
		p_enemy[ghost].UpdateAI(p_enemy[ghost].GetAI(), &g_map, &p_player, timerUpdate);
	}
}

bool CheckDistanceToPlayer(int startingTileX, int startingTileY, int finalTileX, int finalTileY) {
	return abs(sqrt((finalTileY - startingTileY)^2 + (finalTileX - startingTileX)^2)) >= 5.0;
}

bool CheckGhostSpawns(float x, float y) {
	for (int ghost = 0; ghost < NUM_GHOSTS; ghost++) {
		if (x == spawns[ghost][0] && y == spawns[ghost][1]) {
			return false;
		}
	}
	return true;
}

void InitGhosts() {
	srand(time(NULL));
	for (int ghost = 0; ghost < NUM_GHOSTS; ghost++) {
	    float spawnX = ((rand() % (MAP_SIZE - 1)) + 1) * TILEWIDTH;
	    float spawnY = ((rand() % (MAP_SIZE - 1)) + 1) * TILEHEIGHT;
		while(g_map.GetTileMapValue(spawnX + TILEWIDTH/2, spawnY + TILEHEIGHT/2) == EMapValue::WALL ||
			g_map.GetTileMapValue(spawnX + TILEWIDTH/2, spawnY + TILEHEIGHT/2) == EMapValue::BORDER ||
			!CheckGhostSpawns(spawnX, spawnY)) {
	        spawnX = ((rand() % (MAP_SIZE - 1)) + 1) * TILEWIDTH;
	        spawnY = ((rand() % (MAP_SIZE - 1)) + 1) * TILEHEIGHT;
		}
		CEnemy enemy(APP_VIRTUAL_WIDTH / 2, APP_VIRTUAL_HEIGHT / 2);
		enemy.SetX(spawnX + TILEWIDTH / 2);
		enemy.SetY(spawnY + TILEHEIGHT / 2);
		p_enemy[ghost] = enemy;
		spawns[ghost][0] = spawnX;
		spawns[ghost][1] = spawnY;
	}
}

void RenderGhosts() {
	for (int ghost = 0; ghost < NUM_GHOSTS; ghost++) {
		p_enemy[ghost].Render(TILEWIDTH, TILEHEIGHT);
	}
}

bool CheckGhostCollisions() {
	for (int ghost = 0; ghost < NUM_GHOSTS; ghost++) {
		if ((int)p_enemy[ghost].GetX() == (int)p_player.GetX() &&
			(int)p_enemy[ghost].GetY() == (int)p_player.GetY()) {
			p_player.DecLifeCount();
			g_map.RandomMap(TUNNEL_FILL_PERCENT, TUNNEL_LEN);
			p_player.SetX((float)APP_VIRTUAL_WIDTH / 2);
			p_player.SetY((float)APP_VIRTUAL_HEIGHT / 2);
			InitGhosts();
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine or Print) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{	 
    g_map.Render();
	if (p_player.GetLifeCount() >= 0) {
	    p_player.Render(TILEWIDTH, TILEHEIGHT);
	}
	RenderGhosts();
	App::Print(25.0f, (g_map.GetMapSize() - 1) * TILEHEIGHT + (TILEHEIGHT / 2), "Use W, A, S, D to move.", 1.0, 1.0, 1.0);
	App::Print((g_map.GetMapSize() - 3) * TILEWIDTH, (g_map.GetMapSize() - 1) * TILEHEIGHT, "Score: ", 1.0, 1.0, 1.0);
	App::Print((g_map.GetMapSize() - 1) * TILEWIDTH, (g_map.GetMapSize() - 1) * TILEHEIGHT, std::to_string(score).c_str() , 1.0, 1.0, 1.0);
	App::Print((g_map.GetMapSize() - 6) * TILEWIDTH, (g_map.GetMapSize() - 1) * TILEHEIGHT, "Lives: ", 1.0, 1.0, 1.0);
	App::Print((g_map.GetMapSize() - 4) * TILEWIDTH, (g_map.GetMapSize() - 1) * TILEHEIGHT, std::to_string(lives).c_str() , 1.0, 1.0, 1.0);
	if (p_player.GetLifeCount() < 0) {
		App::DrawQuad(0.0f, 0.0f, (float)APP_VIRTUAL_WIDTH, (float)APP_VIRTUAL_HEIGHT, 0.0f, 0.0f, 0.5f);
		App::Print(((float)APP_VIRTUAL_WIDTH / 2) - TILEWIDTH / 2, ((float)APP_VIRTUAL_HEIGHT / 2), "GAME OVER", 1.0f, 1.0f, 1.0f);
		App::Print(((float)APP_VIRTUAL_WIDTH / 2) - TILEWIDTH, ((float)APP_VIRTUAL_HEIGHT / 2) - TILEHEIGHT, "Highest Score: ", 1.0f, 1.0f, 1.0f);
		App::Print(((float)APP_VIRTUAL_WIDTH / 2) + TILEWIDTH * 2, ((float)APP_VIRTUAL_HEIGHT / 2) - TILEHEIGHT, std::to_string(p_player.GetHighestScore()).c_str(), 1.0f, 1.0f, 1.0f);
	}
}

//------------------------------------------------------------------------
// Add your shutdown code here. Called when the APP_QUIT_KEY is pressed.
// Just before the app exits.
//------------------------------------------------------------------------
void Shutdown()
{
}