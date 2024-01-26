#include "EntityManager.h"
#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Scene.h"
#include "Enemy.h"
#include "EnemyFly.h"
#include "Bullet.h"
#include "Physics.h"
#include "Coin.h"
#include "Heart.h"

#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager() : Module()
{
	name.Create("entitymanager");
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Awake();
	}

	return ret;

}

bool EntityManager::Start() {

	bool ret = true; 

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Start();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;
	ListItem<Entity*>* item;
	item = entities.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	entities.Clear();

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type, b2Vec2* position, b2Vec2* direction)
{
	Entity* entity = nullptr; 

	switch (type)
	{
	case EntityType::PLAYER:
		entity = new Player();
		break;
	case EntityType::ITEM:
		entity = new Item();
		break;
	case EntityType::WALKING_ENEMY:
		entity = new Enemy();
		break;
	case EntityType::FLYING_ENEMY:
		entity = new EnemyFLY();
		break;
	case EntityType::BULLET:
		entity = new Bullet(position,direction);
		break;
	case EntityType::COIN:
		entity = new Coin();
		break;
	case EntityType::HEART:
		entity = new Heart();
		break;
	case EntityType::BOSS:
		entity = new Boss();
		break;
	default:
		break;
	}

	entities.Add(entity);

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data == entity) entities.Del(item);
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	if ( entity != nullptr) entities.Add(entity);
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if(pEntity->pendingtodestroy)
		{
			app->physics->DestroyObject(pEntity->pbody);
			DestroyEntity(pEntity);
		}

		if (pEntity->active == false) continue;
		ret = item->data->Update(dt);
	}

	return ret;
}


void EntityManager::DisableAllItems()
{
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->type == EntityType::HEART || pEntity->type == EntityType::COIN)
		{
			pEntity->Disable();
		}

	}
	DestroyAllBullets();
}

void EntityManager::ResetAllItems()
{
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->type == EntityType::HEART || pEntity->type == EntityType::COIN)
		{
			pEntity->Disable();
			pEntity->Enable();
		}

	}
	DestroyAllBullets();
}



void EntityManager::DisableAllEnemies()
{
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->type == EntityType::WALKING_ENEMY || pEntity->type == EntityType::FLYING_ENEMY)
		{
			pEntity->Disable();
		}

	}
	DestroyAllBullets();
}


void EntityManager::ResetAllEnemies()
{
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->type == EntityType::WALKING_ENEMY || pEntity->type == EntityType::FLYING_ENEMY)
		{
			pEntity->Disable();
			pEntity->Enable();
		}
		
	}
	DestroyAllBullets();
}


void EntityManager::DestroyAllBullets()
{
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->type == EntityType::BULLET)
		{
			app->physics->DestroyObject(pEntity->pbody);
			DestroyEntity(pEntity);
		}

	}
}




bool EntityManager::LoadState(pugi::xml_node node)
{
	DestroyAllBullets();
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;
	
	pugi::xml_node player = node.child("Player");
	pugi::xml_node items = node.child("Items");
	pugi::xml_node walkingEnemies = node.child("Walking_Enemies");
	pugi::xml_node flyingEnemies = node.child("Flying_Enemies");
	//pugi::xml_node coins = node.child("Coins");
	//pugi::xml_node hearts = node.child("Hearts");

	int playerNo = 1;
	int itemNo = 1;
	int wEnemyNo = 1;
	int fEnemyno = 1;
	//int fCoinsno = 1;
	//int fHeartsno = 1;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;
		EntityType type = item->data->type;
		switch (type)
		{
		case EntityType::PLAYER:
			ret = pEntity->LoadState(player, playerNo);
			playerNo++;
			break;
		case EntityType::ITEM:
			ret = pEntity->LoadState(items, itemNo);
			wEnemyNo ++;
			break;
		case EntityType::WALKING_ENEMY:
			ret = pEntity->LoadState(walkingEnemies, wEnemyNo);
			wEnemyNo++;
			break;
		case EntityType::FLYING_ENEMY:
			ret = pEntity->LoadState(flyingEnemies, fEnemyno);
			fEnemyno++;
			break;
		/*case EntityType::COIN:
			ret = pEntity->LoadState(coins, fCoinsno);
			fCoinsno++;
			break;
		case EntityType::HEART:
			ret = pEntity->LoadState(hearts, fHeartsno);
			fHeartsno++;
			break;*/
		default:
			break;
		}
		
	}
	

	return ret;
}

bool EntityManager::SaveState(pugi::xml_node node)
{

	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	pugi::xml_node player = node.append_child("Player");
	pugi::xml_node items = node.append_child("Items");
	pugi::xml_node walkingEnemies = node.append_child("Walking_Enemies");
	pugi::xml_node flyingEnemies = node.append_child("Flying_Enemies");
	//pugi::xml_node coins = node.append_child("Coins");
	//pugi::xml_node hearts = node.append_child("Hearts");

	int playerNo = 1;
	int itemNo = 1;
	int wEnemyNo = 1;
	int fEnemyno = 1;
	//int fHeartsno = 1;
	//int fCoinsno = 1;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;
		EntityType type = item->data->type;
		switch (type)
		{
		case EntityType::PLAYER:
			ret = pEntity->SaveState(player, playerNo);
			playerNo++;
			break;
		case EntityType::ITEM:
			ret = pEntity->SaveState(items, itemNo);
			wEnemyNo++;
			break;
		case EntityType::WALKING_ENEMY:
			ret = pEntity->SaveState(walkingEnemies, wEnemyNo);
			wEnemyNo++;
			break;
		case EntityType::FLYING_ENEMY:
			ret = pEntity->SaveState(flyingEnemies, fEnemyno);
			fEnemyno++;
			break;
		/*case EntityType::COIN:
			ret = pEntity->SaveState(coins, fCoinsno);
			fCoinsno++;
			break;
		case EntityType::HEART:
			ret = pEntity->SaveState(hearts, fHeartsno);
			fHeartsno++;
			break;*/
		default:
			break;
		}

	}

	return ret;
}

bool EntityManager::goToLevel1()
{
	ResetAllEnemies();
	ResetAllItems();
	app->scene->boss->Disable();
	app->physics->DestroyObject(app->scene->player->pbody);
	Start();
	
	return true;
}

bool EntityManager::goToLevel2()
{
	DisableAllEnemies();
	DisableAllItems();
	app->scene->boss->Enable();
	app->physics->DestroyObject(app->scene->player->pbody);
	Start();


	return true;
}