#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "Box2D/Box2D/Box2D.h"

struct SDL_Texture;

class Enemy : public Entity
{
public:

	Enemy();

	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void Death();

	void SetPath(b2Vec2 path);

public:
	const char* texturePath;
	SDL_Texture* texture = nullptr;
	Animation* currentAnimation = nullptr;

	SDL_RendererFlip lastDirection = SDL_FLIP_NONE;
	SDL_RendererFlip flipHorizontal = lastDirection;
	SDL_RendererFlip flipVertical = SDL_FLIP_NONE;

	Animation idleAnim;
	Animation runAnim;
	Animation JumpAnim;
	Animation DeathAnim;

	PhysBody* pbody;
	bool death = false;
	bool debug = false;
	bool isMoving = false;
	bool isTouchingGround = true;
	int width;
	int height;

	

	float gravityScale = 1.0f;
	int initX;
	int initY;

	b2Vec2 path1;
	int currentTargetIndex;
	int enemySpeed = 0.3f;
};

#endif // __ENEMY_H__