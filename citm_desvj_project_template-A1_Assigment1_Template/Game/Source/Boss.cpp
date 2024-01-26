#include "Boss.h"
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


Boss::Boss() : Entity(EntityType::BOSS)
{
	name.Create("Enemy");
}

Boss::~Boss()
{

}

bool Boss::Awake()
{
	return true;
}

bool Boss::Start()
{
	startTime = 0;
	damage = false;
	aggro = false;
	attackhasfinished = false;
	pbody = app->physics->CreateBoss(1920, 384, 128, 640, bodyType::STATIC);
	pbody->listener = this;
	sensorDown = app->physics->CreateRectangleSensor(1536,640,896,128, bodyType::STATIC, ColliderType::UNKNOWN);
	sensorDown->listener = this;
	sensorUp = app->physics->CreateRectangleSensor(1536, 128, 896, 128, bodyType::STATIC, ColliderType::UNKNOWN);
	sensorUp->listener = this;
	entranceSensor = app->physics->CreateRectangleSensor(1280, 384, 128, 640, bodyType::STATIC, ColliderType::SCAN);
	entranceSensor->listener = this;
	timer = 0;
	if (!app->level2)
	{
		Disable();
	}
	texturePath = "Assets/Textures/boss.png";
	texture = app->tex->Load(texturePath);
	t3 = app->tex->Load("Assets/Textures/3.png");
	t2 = app->tex->Load("Assets/Textures/2.png");

	t1 = app->tex->Load("Assets/Textures/1.png");

	return true;

}

bool Boss::Update(float dt)
{
	app->render->DrawTexture(texture, 1856, 64);
	if (aggro)
	{
		if (startTime == 0) startTime = SDL_GetTicks();



		if (SDL_GetTicks() - startTime >= 1000)
		{
			damage = false;
			app->render->DrawTexture(t3, 1408, 352);
		}
		else if (SDL_GetTicks() - startTime >= 2000)
		{
			damage = false;
			app->render->DrawTexture(t2, 1408, 352);

		}
		else if (SDL_GetTicks() - startTime >= 3000)
		{
			//Avisar del ataque
			app->render->DrawTexture(t1, 1408, 352);
			damage = false;
		}
		else
		{
			Attack();
		}
	}
	return true;
}



void Boss::Attack()
{
	
	damage = true;
	startTime = 0;
}

bool Boss::CleanUp()
{

	if (texture != nullptr)
	{
		app->tex->UnLoad(texture);
		texture = nullptr;
	}

	if (pbody != nullptr)
	{
		app->physics->DestroyObject(pbody);

		pbody = nullptr;
	}

	if (entranceSensor != nullptr)
	{
		app->physics->DestroyObject(entranceSensor);
		entranceSensor = nullptr;
	}



	return true;
}

void Boss::StartFight()
{
	if (entranceSensor!=nullptr)
	{
		app->physics->DestroyObject(entranceSensor);
		entranceSensor = nullptr;
	}
	
	aggro = true;

}



void Boss::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physA->ctype==ColliderType::SCAN && physB->ctype==ColliderType::PLAYER)
	{
		StartFight();
	}

	if (physA->ctype == ColliderType::UNKNOWN && physB->ctype == ColliderType::PLAYER)
	{
		if(damage)
		app->scene->player->Death();
	}

}

void Boss::Death()
{

}

bool Boss::LoadState(pugi::xml_node node, int num)
{
	return true;
}

bool Boss::SaveState(pugi::xml_node node, int num)
{
	return true;
}