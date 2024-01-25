#ifndef __ENEMYFLY_H__
#define __ENEMYFLY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "Box2D/Box2D/Box2D.h"

struct SDL_Texture;

class EnemyFLY : public Entity
{
public:

	EnemyFLY();

	virtual ~EnemyFLY();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void Death();

	void SetPath(b2Vec2 path);

	bool LoadState(pugi::xml_node node, int num);

	bool SaveState(pugi::xml_node node, int num);

public:
	const char* texturePath;
	const char* deathSFXPath;
	SDL_Texture* texture = nullptr;
	SDL_Texture* pathTexture = nullptr;
	Animation* currentAnimation = nullptr;
	uint deathSFX;
	bool audiohasplayed = false;
	SDL_RendererFlip lastDirection = SDL_FLIP_NONE;
	SDL_RendererFlip flipHorizontal = lastDirection;
	SDL_RendererFlip flipVertical = SDL_FLIP_NONE;

	Animation idleAnim;
	Animation runAnim;
	Animation JumpAnim;
	Animation DeathAnim;

	
	bool death = false;
	bool debug = false;
	bool isMoving = false;
	bool isTouchingGround = true;


	float gravityScale = 1.0f;
	int initX;
	int initY;

	//b2Vec2 path;
	int currentTargetIndex;
	int enemySpeed = 0.3f;
	bool firstload = true;
};

#endif // __ENEMYFLY_H__