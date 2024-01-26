#ifndef __TITLE_H__
#define __TITLE_H__

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

class Title : public Module
{
public:

	Title();

	// Destructor
	virtual ~Title();

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

	float volume = 90;
	GuiControlButton* start;
	GuiControlButton* quit;
	GuiControlButton* options;
	GuiControlButton* backbutton;
	GuiControlButton* musicbutton;
	GuiControlButton* fullbutton;
	GuiControlButton* vsyncbutton;
	GuiControlButton* resumebutton;
	GuiControlButton* titlebutton;

	pugi::xml_node mynode;

	bool comenzar = false;
	bool exitgame = false;
	bool iraconfiguracion = false;
	bool fondonormal = true;
	bool fondoconfig = false;
	bool full = false;
	bool vsyncactive = false;

private:
	SDL_Texture* img;
	SDL_Texture* img2;
	SDL_Texture* selector;
	float textPosX, textPosY = 0;
	uint texW, texH;
	uint windowW, windowH;

	

	SString bgPath;
	SString bgName;

	
};

#endif // __TITLE_H__
