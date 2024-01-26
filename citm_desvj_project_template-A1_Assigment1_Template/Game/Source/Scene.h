#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"
#include "Enemy.h"
#include "EnemyFly.h"
#include "Coin.h"
#include "Heart.h"
#include "GuiControl.h"
#include "GuiManager.h"
#include "GuiControlButton.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

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


	Player* player;
	Enemy* enemyground1;
	EnemyFLY* enemyfly1;
	Enemy* enemyground2;
	EnemyFLY* enemyfly2;
	Coin* coin1;
	Coin* coin2;
	Coin* coin3;
	Coin* coin4;
	Coin* coin5;
	Heart* heart1;
	Heart* heart2;
	Heart* heart3;
	Heart* heart4;
	Heart* heart5;

	const char* musicPath;
	float volume = 90;
	std::string lifetext;
	std::string cointext;
	std::string timer;
	Uint32 startTime = 0;
	bool firstTime = true;
	bool secondTime = false;
	bool pausemenu = false;
	pugi::xml_node mynode;

private:
	SDL_Texture* img;
	float textPosX, textPosY = 0;
	uint texW, texH;
	uint windowW, windowH;
	
	SString bgPath;
	SString bgName;
	
};

#endif // __SCENE_H__