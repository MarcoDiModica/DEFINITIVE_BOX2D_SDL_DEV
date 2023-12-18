#ifndef __BULLET_H__
#define __BULLET_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "Box2D/Box2D/Box2D.h"

struct SDL_Texture;


class Bullet : public Entity
{
public:

	Bullet(b2Vec2* position=nullptr, b2Vec2* direction=nullptr);


	virtual ~Bullet();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	bool LoadState(pugi::xml_node node, int num);

	bool SaveState(pugi::xml_node node, int num);
public:
	const char* texturePath;
	SDL_Texture* texture = nullptr;
	Animation* currentAnimation = nullptr;
	b2Vec2 spawnpos;
	b2Vec2 direction;


};

#endif // __BULLET_H__