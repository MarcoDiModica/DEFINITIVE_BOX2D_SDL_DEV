#ifndef __VICTORYBOSS_H__
#define __VICTORYBOSS_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"
#include "Enemy.h"
#include "EnemyFly.h"
#include "Coin.h"
#include "Heart.h"

struct SDL_Texture;

class VictoryBoss : public Module
{
public:

	VictoryBoss();

	// Destructor
	virtual ~VictoryBoss();

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

private:
	SDL_Texture* img;
	float textPosX, textPosY = 0;
	uint texW, texH;
	uint windowW, windowH;

	SString bgPath;
	SString bgName;

};

#endif // __VICTORYBOSS_H__