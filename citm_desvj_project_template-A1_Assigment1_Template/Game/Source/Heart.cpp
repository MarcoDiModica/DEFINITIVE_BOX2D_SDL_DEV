#include "Heart.h"
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

Heart::Heart(b2Vec2* position) : Entity(EntityType::HEART)
{
	name.Create("1UP");
	if (position != nullptr)
	{
		spawnpos = *position;
	}
	else
	{
		spawnpos.x = 0;
		spawnpos.y = 0;
	}

	pbody = app->physics->CreateCoin(position->x, position->y, 10, STATIC, ColliderType::HEART);
	pbody->listener = this;
	texture2 = app->tex->Load("Assets/Textures/marco.png");
}

Heart::~Heart()
{

}

bool Heart::Awake()
{
	return true;
}

bool Heart::Start()
{
	pbody = app->physics->CreateCoin(spawnpos.x, spawnpos.y, 10, STATIC, ColliderType::COIN);
	texture2 = app->tex->Load("Assets/Textures/marco.png");


	return true;

}

bool Heart::Update(float dt)
{


	position.x = METERS_TO_PIXELS(pbody->body->GetPosition().x) - 10;
	position.y = METERS_TO_PIXELS(pbody->body->GetPosition().y) - 10;


	app->render->DrawTexture(texture2, position.x, position.y);

	return true;

}

bool Heart::CleanUp()
{
	return true;

}

void Heart::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		//player.lifes++;
		pendingtodestroy = true;
		LOG("Collision Player");
		break;
	case ColliderType::WEAPON:
		//player.lifes++;
		pendingtodestroy = true;
		LOG("Collision WEAPON");
		break;
	}

}

bool Heart::LoadState(pugi::xml_node node, int num)
{
	return true;
}

bool Heart::SaveState(pugi::xml_node node, int num)
{
	return true;

}