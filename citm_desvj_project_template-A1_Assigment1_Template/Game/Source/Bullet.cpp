#include "Bullet.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Map.h"
#include "Animation.h"
#include "EntityManager.h"


Bullet::Bullet(b2Vec2* position, b2Vec2* direction) : Entity(EntityType::BULLET)
{
	name.Create("Bullet");
	if (position != nullptr)
	{
		spawnpos = *position;
	}
	else
	{
		spawnpos.x = 0;
		spawnpos.y = 0;
	}
	if (direction != nullptr)
	{
		this->direction = *direction;
	}
	else
	{
		this->direction.x = 0;
		this->direction.y = 0;
	}
	pbody = app->physics->CreateBullet(spawnpos.x, spawnpos.y, 5, 5, DYNAMIC, ColliderType::WEAPON, 15, this->direction);
	pbody->listener = this;
}



Bullet::~Bullet()
{

}

bool Bullet::Awake()
{
	return true;
}

bool Bullet::Start()
{
	pbody = app->physics->CreateBullet(spawnpos.x, spawnpos.y, 5, 5, DYNAMIC, ColliderType::WEAPON, 15, direction);
    

   
	return true;

}

bool Bullet::Update(float dt)
{
	return true;

}

bool Bullet::CleanUp()
{
	return true;

}

void Bullet::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::UNKNOWN:
		pendingtodestroy = true;
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::DEATH:
		pendingtodestroy = true;
		LOG("Collision DEATH");
		break;
	case ColliderType::PLATFORM:
		pendingtodestroy = true;
		LOG("Collision PLATFORM");
		break;
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		break;
	case ColliderType::WEAPON:
		LOG("Collision WEAPON");
		break;
	}

}

bool Bullet::LoadState(pugi::xml_node node, int num)
{
	return true;

}

bool Bullet::SaveState(pugi::xml_node node, int num)
{
	return true;

}