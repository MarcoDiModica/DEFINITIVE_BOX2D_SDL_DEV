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
#include "Title.h"
#include "Defs.h"
#include "Log.h"

Dead::Dead() : Module()
{
	name.Create("scene");
}

// Destructor
Dead::~Dead()
{}

// Called before render is available
bool Dead::Awake(pugi::xml_node& config)
{
	LOG("Loading Dead");
	bool ret = true;


	mynode = config;
	//musicPath = config.child("Music").attribute("musicpath").as_string();

	return ret;
}

// Called before the first frame
bool Dead::Start()
{
	if (active)
	{
		//app->audio->PlayMusic(musicPath);
		img = app->tex->Load("Assets/GUI/die.png");

		//Get the size of the window
		app->win->GetWindowSize(windowW, windowH);

		//Get the size of the texture
		app->tex->GetSize(img, texW, texH);

		textPosX = (float)windowW / 2 - (float)texW / 2;
		textPosY = (float)windowH / 2 - (float)texH / 2;
	}

	return true;
}

// Called each loop iteration
bool Dead::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Dead::Update(float dt)
{
	if (startTime == 0) startTime = SDL_GetTicks();

	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || SDL_GetTicks() - startTime >= 3000)
	{
		app->title->active = true;
		app->title->Awake(mynode);
		app->title->Start();
		app->dead->active = false;

		startTime = 0;
	}

	return true;
}

// Called each loop iteration
bool Dead::PostUpdate()
{
	bool ret = true;

	//blit texture
	if(active) app->render->DrawTexture(img, 0, 0 , NULL);

	return ret;
}

// Called before quitting
bool Dead::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

bool Dead::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	return true;
}