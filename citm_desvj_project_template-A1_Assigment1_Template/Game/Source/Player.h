#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

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

public:
	float speed = 0.3f;
	const char* texturePath;
	SDL_Texture* texture = nullptr;
	Animation* currentAnimation = nullptr;

	Animation idleAnim;
	Animation runAnim;
	Animation JumpAnim;
	Animation FallAnim;

	PhysBody* pbody;
	int pickCoinFxId;
	bool isTouchingGround = false;
	bool canDash = true;
	float jumpImpulse = 2.0f;
	float jumpIncrement = 1.0f;
	float maxJumpImpulse = 10.0f;
	float initialJumpImpulse = 2.0f;
	float dashMultiplier = 2.0f;
	bool isMoving = false;
};

#endif // __PLAYER_H__