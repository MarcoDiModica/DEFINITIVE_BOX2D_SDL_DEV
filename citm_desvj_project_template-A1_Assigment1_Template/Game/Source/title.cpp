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
		img2 = app->tex->Load("Assets/GUI/config.jpg");
		selector = app->tex->Load("Assets/GUI/select.png");

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

		SDL_Rect backpos = { windowW / 2 - 70, windowH / 2 + 300, 100,50 };
		SDL_Rect musicpos = { windowW / 2 - 70, windowH / 2 - 330, 100,20 };
		SDL_Rect fullpos = { windowW / 2 - 70, windowH / 2 - 230 , 50,50 };
		SDL_Rect vsyncpos = { windowW / 2 - 70, windowH / 2 - 130 , 50,50 };

		backbutton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "   Back   ", backpos, this);
		musicbutton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 5, "   Volume   ", musicpos, this);
		fullbutton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 6, "   Fullscreen   ", fullpos, this);
		vsyncbutton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 7, "   Vsync   ", vsyncpos, this);

		backbutton->state = GuiControlState::DISABLED;
		musicbutton->state = GuiControlState::DISABLED;
		fullbutton->state = GuiControlState::DISABLED;
		vsyncbutton->state = GuiControlState::DISABLED;

		SDL_Rect resumepos = { windowW / 2 - 70, windowH / 2 + 300, 100,50 };
		SDL_Rect backtitlepos = { windowW / 2 - 70, windowH / 2 + 250, 100,50 };

		resumebutton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, "   Resume   ", resumepos, this);
		titlebutton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, "   Back To Title   ", backtitlepos, this);

		resumebutton->state = GuiControlState::DISABLED;
		titlebutton->state = GuiControlState::DISABLED;
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
	

	if (iraconfiguracion)
	{
		fondonormal = false;
		fondoconfig = true;
		start->state = GuiControlState::DISABLED;
		quit->state = GuiControlState::DISABLED;
		options->state = GuiControlState::DISABLED;

		backbutton->state = GuiControlState::NORMAL;
		musicbutton->state = GuiControlState::NORMAL;
		fullbutton->state = GuiControlState::NORMAL;
		vsyncbutton->state = GuiControlState::NORMAL;
	}
	else if (!iraconfiguracion)
	{
		fondonormal = true;
		fondoconfig = false;
		start->state = GuiControlState::NORMAL;
		quit->state = GuiControlState::NORMAL;
		options->state = GuiControlState::NORMAL;

		backbutton->state = GuiControlState::DISABLED;
		musicbutton->state = GuiControlState::DISABLED;
		fullbutton->state = GuiControlState::DISABLED;
		vsyncbutton->state = GuiControlState::DISABLED;
	}	

	if (comenzar)
	{
		pugi::xml_node n = mynode;
		app->map->active = false;
		app->physics->active = true;
		app->physics->Start();
		app->entityManager->active = true;
		app->entityManager->Awake(n);
		app->entityManager->Start();
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

	if (exitgame)
		ret = false;

	//blit texture
	if (fondonormal)
	{
		app->render->DrawTexture(img, textPosX, textPosY, NULL);
	}
	else if (fondoconfig)
	{
		//app->render->DrawTexture(img2, textPosX, textPosY, NULL);
	}

	if (start->state == GuiControlState::FOCUSED)
	{
		app->render->DrawTexture(selector, 360, 400, NULL);
	}
	else if (quit->state == GuiControlState::FOCUSED)
	{
		app->render->DrawTexture(selector, 360, 580, NULL);
	}
	else if (options->state == GuiControlState::FOCUSED)
	{
		app->render->DrawTexture(selector, 290, 490, NULL);
	}

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
	else if (control->id == 4)
	{
		iraconfiguracion = false;
	}
	else if (control->id == 5)
	{

	}
	else if (control->id == 6 && !full)
	{
		full = true;
		SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_FULLSCREEN);
	}
	else if (control->id == 6 && full)
	{
		full = false;
		SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_SHOWN);
	}
	else if (control->id == 7 && vsyncactive)
	{
		vsyncactive = false;
	}
	else if (control->id == 7 && !vsyncactive)
	{
		vsyncactive = true;
	}
	else if (control->id == 8)
	{
		app->scene->pausemenu = false;
	}
	else if (control->id == 9)
	{
		app->scene->pausemenu = false;
		app->scene->player->followplayer = false;
		app->render->camera.x = 0;
		app->render->camera.y = 0;
		app->title->active = true;
		app->title->Awake(mynode);
		app->title->Start();
		app->scene->active = false;
		app->physics->active = false;
		app->map->active = false;
		app->entityManager->active = false;
	}

	return true;
}