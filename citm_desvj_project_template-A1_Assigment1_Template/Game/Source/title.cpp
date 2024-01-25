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
#include "Physics.h"
#include "Map.h"

#include "Defs.h"
#include "Log.h"

Title::Title() : Module()
{
	name.Create("scene");
}

// Destructor
Title::~Title()
{}

// Called before render is available
bool Title::Awake(pugi::xml_node& config)
{
	LOG("Loading Title");
	bool ret = true;

	//musicPath = config.child("Music").attribute("musicpath").as_string();

	mynode = config;
	return ret;
}

// Called before the first frame
bool Title::Start()
{
	if (active)
	{
		img = app->tex->Load("Assets/GUI/title.png");
		selector = app->tex->Load("Assets/GUI/selector.png");

		//app->audio->PlayMusic(musicPath);

		//Get the size of the window
		app->win->GetWindowSize(windowW, windowH);

		//Get the size of the texture
		app->tex->GetSize(img, texW, texH);

		textPosX = (float)windowW / 2 - (float)texW / 2;
		textPosY = (float)windowH / 2 - (float)texH / 2;

		SDL_Rect playPos = { windowW / 2 - 60, windowH / 2 + 10, 100,50 };
		SDL_Rect quitPos = { windowW / 2 - 60, windowH / 2 + 180, 100,50 };
		SDL_Rect optionsPos = { windowW / 2 - 110, windowH / 2 + 90 , 200,50 };

		start = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "   Play   ", playPos, this);
		quit = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "   Quit   ", quitPos, this);
		options = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "   Options   ", optionsPos, this);

		start->state = GuiControlState::NORMAL;
		quit->state = GuiControlState::NORMAL;
		options->state = GuiControlState::NORMAL;

	}
	
	return true;
}

// Called each loop iteration
bool Title::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Title::Update(float dt)
{
	if (comenzar)
	{
		app->map->active = false;
		app->physics->active = true;
		app->physics->Start();
		pugi::xml_node n = mynode;
		app->scene->active = true;
		app->scene->Awake(n);
		app->scene->Start();
		app->map->active = true;
		app->map->Start();

		app->guiManager->active = true;
		app->guiManager->Start();

		start->state = GuiControlState::DISABLED;
		quit->state = GuiControlState::DISABLED;
		options->state = GuiControlState::DISABLED;

		app->title->active = false;
		comenzar = false;
	}

	return true;
}

// Called each loop iteration
bool Title::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || exitgame)
		ret = false;

	//blit texture
	app->render->DrawTexture(img, textPosX, textPosY, NULL);

	return ret;
}

// Called before quitting
bool Title::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

bool Title::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	if (control->id == 1)
	{
		comenzar = true; //start->state = GuiControlState::FOCUSED;
	}
	else if (control->id == 2)
	{
		exitgame = true;
	}
	else if (control->id == 3)
	{
		iraconfiguracion = true;
	}

	return true;
}