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

Heart::Heart() : Entity(EntityType::HEART)
{
	name.Create("1UP");
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
	spawnpos.x = parameters.attribute("x").as_float();
	spawnpos.y = parameters.attribute("y").as_float();
	pbody = app->physics->CreateCircleNoColision(spawnpos.x, spawnpos.y, 16, bodyType::STATIC);
	pbody->ctype = ColliderType::HEART;
	pbody->listener = this;
	texture2 = app->tex->Load("Assets/Textures/cora.png");


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
		position.x = -100;
		position.y = -100;
		Disable();
		LOG("Collision Player");
		break;
	case ColliderType::WEAPON:
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