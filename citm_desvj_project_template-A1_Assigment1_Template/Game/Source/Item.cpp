#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

Item::Item() : Entity(EntityType::ITEM)
{
	name.Create("item");
}

Item::~Item() {}

bool Item::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Item::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateCircleNoColision(position.x + 16, position.y + 22, 16, bodyType::STATIC);
	pbody->ctype = ColliderType::ITEM;

	return true;
}

bool Item::Update(float dt)
{
	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	app->render->DrawTexture(texture, position.x, position.y);

	return true;
}

bool Item::CleanUp()
{
	return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collision ITEM");
		pbody->body->GetWorld()->DestroyBody(pbody->body);
		pbody->body = nullptr;
		app->tex->UnLoad(texture);
		texture = nullptr;
		break;
	}
}
