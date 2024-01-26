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

Coin::Coin() : Entity(EntityType::COIN)
{
	name.Create("Coin");
}

Coin::~Coin()
{

}

bool Coin::Awake()
{
	spawnpos.x = parameters.attribute("x").as_float();
	spawnpos.y = parameters.attribute("y").as_float();
	
	return true;
}

bool Coin::Start()
{
	pbody = app->physics->CreateCircleNoColision(spawnpos.x, spawnpos.y, 16, bodyType::STATIC);
	pbody->ctype = ColliderType::COIN;
	pbody->listener = this;
	texture2 = app->tex->Load("Assets/Textures/euro.png");


	return true;

}

bool Coin::Update(float dt)
{
	if (firstload)
	{
		this->Awake();
		this->Start();
		firstload = false;
	}

	position.x = METERS_TO_PIXELS(pbody->body->GetPosition().x) - 10;
	position.y = METERS_TO_PIXELS(pbody->body->GetPosition().y) - 10;


	app->render->DrawTexture(texture2, position.x, position.y);

	return true;

}

bool Coin::CleanUp()
{
	if (texture2 != nullptr)
	{
		app->tex->UnLoad(texture2);
		texture2 = nullptr;
	}

	if (pbody != nullptr)
	{
		app->physics->DestroyObject(pbody);

		pbody = nullptr;
	}

	position.x = spawnpos.x;
	position.y = spawnpos.y;
	
	return true;

}

void Coin::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		pbody->ctype = ColliderType::UNKNOWN;
		texture2 = nullptr;
		LOG("Collision Player");
		break;
	case ColliderType::WEAPON:
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