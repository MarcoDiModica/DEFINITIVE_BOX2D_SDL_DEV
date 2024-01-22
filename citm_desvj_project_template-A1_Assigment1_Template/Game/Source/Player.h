#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include <list>

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();

	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void UpdateCamera();

	void Death();

	void Respawn();

	bool LoadState(pugi::xml_node node, int num);

	bool SaveState(pugi::xml_node node, int num);

public:
	float speed = 0.4f;
	const char* texturePath;
	const char* deathSFXPath;
	SDL_Texture* texture = nullptr;
	SDL_Texture* texture2 = nullptr;
	Animation* currentAnimation = nullptr;

	SDL_RendererFlip lastDirection = SDL_FLIP_NONE;
	SDL_RendererFlip flipHorizontal = lastDirection;
	SDL_RendererFlip flipVertical = SDL_FLIP_NONE;

	std::list<Entity*> bullets;

	Animation idleAnim;
	Animation runAnim;
	Animation JumpAnim;
	Animation DeathAnim;

	int pickCoinFxId;
	bool isTouchingGround;
	bool death = false;
	bool debug = false;
	bool canDash = true;
	float jumpImpulse = 2.0f;
	float jumpIncrement = 1.0f;
	float maxJumpImpulse = 10.0f;
	float initialJumpImpulse = 2.0f;
	float dashMultiplier = 2.0f;
	bool isMoving = false;
	bool isInAir = false;
	bool isDashing = false;
	float dashTime = 0.0f;
	float maxDashTime = 4.0f;
	float dashSpeed = 200.0f;
	int dashDirection = 0;

	float gravityScale = 1.0f;
	int initX;
	int initY;
	uint deathSFX;
	bool audiohasplayed = false;
};

#endif // __PLAYER_H__