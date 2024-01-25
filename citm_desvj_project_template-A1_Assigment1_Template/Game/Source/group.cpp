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
#include "group.h"
#include "dead.h"
#include "Physics.h"
#include "title.h"
#include "Defs.h"
#include "Log.h"
#include "SDL_mixer/include/SDL_mixer.h"

Group::Group() : Module()
{
	name.Create("scene");
}

// Destructor
Group::~Group()
{}

// Called before render is available
bool Group::Awake(pugi::xml_node& config)
{
	LOG("Loading Group");
	bool ret = true;

	musicPath = config.child("Music").attribute("musicpath").as_string();

	mynode = config;

	return ret;
}

// Called before the first frame
bool Group::Start()
{
	app->audio->PlayMusic(musicPath);
	
	img = app->tex->Load("Assets/GUI/group.png");
	
	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	//Get the size of the texture
	app->tex->GetSize(img, texW, texH);

	textPosX = (float)windowW / 2 - (float)texW / 2;
	textPosY = (float)windowH / 2 - (float)texH / 2;

	return true;
}

// Called each loop iteration
bool Group::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Group::Update(float dt)
{
	//timer de 3 segundos y pasa a la siguente escena
	if (startTime == 0) startTime = SDL_GetTicks();
	
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || SDL_GetTicks() - startTime >= 3000)
	{
		app->title->active = true;
		app->title->Awake(mynode);
		app->title->Start();
		app->group->active = false;

		startTime = 0;
	}

	return true;
}

// Called each loop iteration
bool Group::PostUpdate()
{
	bool ret = true;

	Mix_VolumeMusic(volume);
	Mix_Volume(-1, volume);

	//blit texture
	app->render->DrawTexture(img, textPosX, textPosY, NULL);

	return ret;
}

// Called before quitting
bool Group::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

bool Group::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	return true;
}