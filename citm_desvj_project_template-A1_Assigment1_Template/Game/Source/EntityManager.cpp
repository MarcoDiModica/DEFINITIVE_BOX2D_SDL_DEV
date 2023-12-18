#include "EntityManager.h"
#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Scene.h"
#include "Enemy.h"
#include "EnemyFly.h"
#include "Bullet.h"
#include "Physics.h"

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

bool EntityManager::LoadState(pugi::xml_node node)
{
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;
	
	pugi::xml_node player = node.child("Player");
	pugi::xml_node items = node.child("Items");
	pugi::xml_node walkingEnemies = node.child("Walking_Enemies");
	pugi::xml_node flyingEnemies = node.child("Flying_Enemies");

	int playerNo = 1;
	int itemNo = 1;
	int wEnemyNo = 1;
	int fEnemyno = 1;

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

	int playerNo = 1;
	int itemNo = 1;
	int wEnemyNo = 1;
	int fEnemyno = 1;

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
		default:
			break;
		}

	}

	return ret;
}