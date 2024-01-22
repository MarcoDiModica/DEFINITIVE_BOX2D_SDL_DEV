#ifndef __COIN_H__
#define __COIN_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "Box2D/Box2D/Box2D.h"

struct SDL_Texture;


class Coin : public Entity
{
public:

	Coin();

	virtual ~Coin();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	bool LoadState(pugi::xml_node node, int num);

	bool SaveState(pugi::xml_node node, int num);
public:
	const char* texturePath;
	SDL_Texture* texture2 = nullptr;
	bool texcreated = false;
	Animation* currentAnimation = nullptr;
	b2Vec2 spawnpos;
	SDL_Rect box = { 0,0,30,30 };

};

#endif // __COIN_H__