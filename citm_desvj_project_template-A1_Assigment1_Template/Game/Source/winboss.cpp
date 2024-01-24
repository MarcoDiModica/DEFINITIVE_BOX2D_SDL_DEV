#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"
#include "Player.h"
#include "EntityManager.h"
#include "Enemy.h"
#include "EnemyFly.h"
#include "Coin.h"
#include "Heart.h"
#include "GuiControl.h"
#include "GuiManager.h"
#include "dead.h"
#include "title.h"
#include "group.h"
#include "win.h"
#include "winboss.h"

#include "Defs.h"
#include "Log.h"

VictoryBoss::VictoryBoss() : Module()
{
	name.Create("VictoryBoss");
}

// Destructor
VictoryBoss::~VictoryBoss()
{}

// Called before render is available
bool VictoryBoss::Awake(pugi::xml_node& config)
{
	LOG("Loading VictoryBoss");
	bool ret = true;

	bgPath = config.child("win").attribute("path").as_string();
	img = app->tex->Load("Assets/Textures/dead.png");

	musicPath = config.child("Music").attribute("musicpath").as_string();

	return ret;
}

// Called before the first frame
bool VictoryBoss::Start()
{
	app->audio->PlayMusic(musicPath);

	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	//Get the size of the texture
	app->tex->GetSize(img, texW, texH);

	textPosX = (float)windowW / 2 - (float)texW / 2;
	textPosY = (float)windowH / 2 - (float)texH / 2;

	return true;
}

// Called each loop iteration
bool VictoryBoss::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool VictoryBoss::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{

	}

	return true;
}

// Called each loop iteration
bool VictoryBoss::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	//blit texture
	app->render->DrawTexture(img, textPosX, textPosY, NULL);

	return ret;
}

// Called before quitting
bool VictoryBoss::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

bool VictoryBoss::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	return true;
}