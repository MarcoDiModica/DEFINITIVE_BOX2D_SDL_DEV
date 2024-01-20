#pragma once
#include "Module.h"
#include "Entity.h"

#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -30.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

// types of bodies
enum bodyType {
	DYNAMIC,
	STATIC,
	KINEMATIC
};

enum class ColliderType {
	PLAYER, 
	ITEM,
	PLATFORM,
	DEATH,
	ENEMY,
	WEAPON,
	UNKNOWN,
	COIN,
	HEART
	// ..
};

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL), ctype(ColliderType::UNKNOWN)
	{}

	~PhysBody() {}

	void GetPosition(int& x, int& y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;
	void SetPath(b2Vec2* path, int pathLength);

public:
	int width, height;
	b2Body* body;
	Entity* listener;
	ColliderType ctype;

private:
	b2Vec2* path = nullptr;
	int pathLength = 0;
	int currentTargetIndex = 0;
};

// Module --------------------------------------
class Physics : public Module, public b2ContactListener // TODO
{
public:

	// Constructors & Destructors
	Physics();
	~Physics();

	// Main module steps
	bool Start();
	bool PreUpdate();
	bool PostUpdate();
	bool CleanUp();

	// Create basic physics objects
	PhysBody* CreateRectangle(int x, int y, int width, int height, bodyType type);
	PhysBody* CreatePlayer(int x, int y, int width, int height, bodyType type);
	PhysBody* CreateGroundEnemy(int x, int y, int width, int height, bodyType type);
	void CreatePathForGroundEnemy(PhysBody* enemy, int startX, int endX, int y);
	PhysBody* CreateFlyingEnemy(int x, int y, int width, int height, bodyType type);
	void CreatePathForFlyingEnemy(PhysBody* enemy, int startX, int startY, int endX, int endY);
	PhysBody* CreateBullet(int x, int y, int width, int height, bodyType type, ColliderType collider, int velocity, b2Vec2 direction);
	PhysBody* CreateCoin(int x, int y, int radius, bodyType type, ColliderType collider);

	PhysBody* CreateCircle(int x, int y, int radious, bodyType type);
	PhysBody* CreateCircleNoColision(int x, int y, int radious, bodyType type);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, bodyType type);
	PhysBody* CreateChain(int x, int y, int* points, int size, bodyType type);
	
	// b2ContactListener ---
	void BeginContact(b2Contact* contact);
	void DestroyObject(PhysBody* body);

public:

	float gravity_y = 30.0f;
	bool debug = false;

private:

	// Debug mode
	

	// Box2D World
	b2World* world;

	
};