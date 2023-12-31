#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "Module.h"
#include "Entity.h"
#include "List.h"

struct b2Vec2;

class EntityManager : public Module
{
public:

	EntityManager();

	// Destructor
	virtual ~EntityManager();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Additional methods
	Entity* CreateEntity(EntityType type, b2Vec2* position = nullptr, b2Vec2* direction=nullptr);

	void DestroyEntity(Entity* entity);

	void AddEntity(Entity* entity);

	void RespawnAllEnemies();

	void DestroyAllBullets();
	
	bool LoadState(pugi::xml_node node);

	bool SaveState(pugi::xml_node node);

public:

	List<Entity*> entities;

};

#endif // __ENTITYMANAGER_H__
