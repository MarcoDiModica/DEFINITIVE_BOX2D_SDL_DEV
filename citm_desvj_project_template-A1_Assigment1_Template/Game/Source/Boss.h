#pragma once
#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "Box2D/Box2D/Box2D.h"

struct SDL_Texture;

class Boss : public Entity
{
public:

	Boss();

	virtual ~Boss();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void Death();

	void Attack();

	void StartFight();

	bool LoadState(pugi::xml_node node, int num);

	bool SaveState(pugi::xml_node node, int num);
public:
	const char* texturePath;
	const char* attackSFXPath;
	uint attackSFX;
	bool audiohasplayed = false;
	SDL_Texture* texture = nullptr;
	SDL_Texture* t3 = nullptr;
	SDL_Texture* t2 = nullptr;
	SDL_Texture* t1 = nullptr;
	PhysBody* sensorUp = nullptr;
	PhysBody* sensorDown = nullptr;
	PhysBody* entranceSensor = nullptr;
	float timer = 0;
	bool damage;
	bool aggro = false;
	bool attackhasfinished = false;
	uint32 startTime;
	int hp;

	


	
};