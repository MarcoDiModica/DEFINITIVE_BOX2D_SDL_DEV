#ifndef __GROUP_H__
#define __GROUP_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"
#include "Enemy.h"
#include "EnemyFly.h"
#include "Coin.h"
#include "Heart.h"

struct SDL_Texture;

class Group : public Module
{
public:

	Group();

	// Destructor
	virtual ~Group();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool OnGuiMouseClickEvent(GuiControl* control);

	const char* musicPath;

	Uint32 startTime = 0;
	pugi::xml_node mynode;

private:
	SDL_Texture* img;

	float textPosX, textPosY = 0;
	uint texW, texH;
	uint windowW, windowH;

	const char* bgPath;

};

#endif // __GROUP_H__
