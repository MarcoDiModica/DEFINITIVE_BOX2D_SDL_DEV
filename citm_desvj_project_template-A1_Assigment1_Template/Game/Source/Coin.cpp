#include "Coin.h"
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
#include "Player.h"

Coin::Coin(b2Vec2* position) : Entity(EntityType::COIN)
{
	name.Create("Coin");
	if (position != nullptr)
	{
		spawnpos = *position;
	}
	else
	{
		spawnpos.x = 0;
		spawnpos.y = 0;
	}
	
	pbody = app->physics->CreateCoin(position->x,position->y,10,STATIC, ColliderType::COIN);
	pbody->listener = this;
	texture2 = app->tex->Load("Assets/Textures/marco.png");
}

Coin::~Coin()
{

}

bool Coin::Awake()
{
	return true;
}

bool Coin::Start()
{
	pbody = app->physics->CreateCoin(spawnpos.x, spawnpos.y, 10, STATIC, ColliderType::COIN);
	texture2 = app->tex->Load("Assets/Textures/marco.png");


	return true;

}

bool Coin::Update(float dt)
{


	position.x = METERS_TO_PIXELS(pbody->body->GetPosition().x) - 10;
	position.y = METERS_TO_PIXELS(pbody->body->GetPosition().y) - 10;


	app->render->DrawTexture(texture2, position.x, position.y);

	return true;

}

bool Coin::CleanUp()
{
	return true;

}

void Coin::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		pendingtodestroy = true;
		LOG("Collision Player");
		break;
	case ColliderType::WEAPON:
		pendingtodestroy = true;
		LOG("Collision WEAPON");
		break;
	}

}

bool Coin::LoadState(pugi::xml_node node, int num)
{
	return true;
}

bool Coin::SaveState(pugi::xml_node node, int num)
{
	return true;

}