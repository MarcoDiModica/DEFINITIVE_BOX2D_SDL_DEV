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

	pbody = app->physics->CreateBullet(spawnpos.x, spawnpos.y, 18, 18, DYNAMIC, ColliderType::WEAPON, 15, this->direction);
	pbody->listener = this;
	texture2 = app->tex->Load("Assets/Textures/marco.png");
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
	pbody = app->physics->CreateBullet(spawnpos.x, spawnpos.y, 18, 18, DYNAMIC, ColliderType::WEAPON, 15, direction);
    texture2 = app->tex->Load("Assets/Textures/marco.png");

   
	return true;

}

bool Bullet::Update(float dt)
{
	
	
	position.x = METERS_TO_PIXELS(pbody->body->GetPosition().x) - 10;
	position.y = METERS_TO_PIXELS(pbody->body->GetPosition().y) - 10;

	//hacer flip a la textura si direction.x < 0
	if (direction.x < 0)
	{
		app->render->DrawTexture(texture2, position.x, position.y, NULL, SDL_FLIP_HORIZONTAL);
	}
	else
	{
		app->render->DrawTexture(texture2, position.x, position.y, NULL, SDL_FLIP_NONE);
	}

	app->render->DrawTexture(texture2,position.x, position.y);
	
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
		//pendingtodestroy = true;
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
	case ColliderType::ENEMY:
		pendingtodestroy = true;
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