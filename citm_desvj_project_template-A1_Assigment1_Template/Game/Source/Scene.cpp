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
#include "SDL_mixer/include/SDL_mixer.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	mynode = config;
	
	if (active && firstTime)
	{
		LOG("Loading Scene");
		bool ret = true;

		// iterate all objects in the scene
		// Check https://pugixml.org/docs/quickstart.html#access
		if (config.child("Coin1"))
		{
			coin1 = (Coin*)app->entityManager->CreateEntity(EntityType::COIN);
			coin1->parameters = config.child("Coin1");
		}

		if (config.child("Coin2"))
		{
			coin2 = (Coin*)app->entityManager->CreateEntity(EntityType::COIN);
			coin2->parameters = config.child("Coin2");
		}

		if (config.child("Coin3"))
		{
			coin3 = (Coin*)app->entityManager->CreateEntity(EntityType::COIN);
			coin3->parameters = config.child("Coin3");
		}

		if (config.child("Coin4"))
		{
			coin4 = (Coin*)app->entityManager->CreateEntity(EntityType::COIN);
			coin4->parameters = config.child("Coin4");
		}

		if (config.child("Coin5"))
		{
			coin5 = (Coin*)app->entityManager->CreateEntity(EntityType::COIN);
			coin5->parameters = config.child("Coin5");
		}

		if (config.child("Heart1"))
		{
			heart1 = (Heart*)app->entityManager->CreateEntity(EntityType::HEART);
			heart1->parameters = config.child("Heart1");
		}

		if (config.child("Heart2"))
		{
			heart2 = (Heart*)app->entityManager->CreateEntity(EntityType::HEART);
			heart2->parameters = config.child("Heart2");
		}

		if (config.child("Heart3"))
		{
			heart3 = (Heart*)app->entityManager->CreateEntity(EntityType::HEART);
			heart3->parameters = config.child("Heart3");
		}

		if (config.child("Heart4"))
		{
			heart4 = (Heart*)app->entityManager->CreateEntity(EntityType::HEART);
			heart4->parameters = config.child("Heart4");
		}

		if (config.child("player")) {
			player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
			player->parameters = config.child("player");
		}

		if (config.child("map")) {
			//Get the map name from the config file and assigns the value in the module
			app->map->mapName = config.child("map").attribute("name").as_string();
			app->map->path = config.child("map").attribute("path").as_string();
		}
		
		if (config.child("map2")) {
			//Get the map name from the config file and assigns the value in the module
			app->map->mapName2 = config.child("map2").attribute("name").as_string();
		}


		if (config.child("background"))
		{
			app->map->bgPath = config.child("background").attribute("path").as_string();
			app->map->bgName = config.child("background").attribute("name").as_string();

		}

		if (config.child("enemyground")) {
			enemyground1 = (Enemy*)app->entityManager->CreateEntity(EntityType::WALKING_ENEMY);
			enemyground1->parameters = config.child("enemyground");
		}

		if (config.child("enemyfly")) {
			enemyfly1 = (EnemyFLY*)app->entityManager->CreateEntity(EntityType::FLYING_ENEMY);
			enemyfly1->parameters = config.child("enemyfly");
		}

		if (config.child("enemyground2")) {
			enemyground2 = (Enemy*)app->entityManager->CreateEntity(EntityType::WALKING_ENEMY);
			enemyground2->parameters = config.child("enemyground2");
		}

		if (config.child("enemyfly2")) {
			enemyfly2 = (EnemyFLY*)app->entityManager->CreateEntity(EntityType::FLYING_ENEMY);
			enemyfly2->parameters = config.child("enemyfly2");
		}

		//musicPath = config.child("Music").attribute("musicpath").as_string();
		startTime = SDL_GetTicks();
		secondTime = true;
		firstTime = false;
		return ret;
	}

	if (active && secondTime && !firstTime)
	{
		startTime = SDL_GetTicks();
	}
	
	return true;
}

// Called before the first frame
bool Scene::Start()
{
	// NOTE: We have to avoid the use of paths in the code, we will move it later to a config file
	//img = app->tex->Load("Assets/Textures/test.png");
	
	//Music is commented so that you can add your own music
	//add path to config file
	//app->audio->PlayMusic(musicPath);
	
	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	//Get the size of the texture
	app->tex->GetSize(img, texW, texH);

	textPosX = (float)windowW / 2 - (float)texW / 2;
	textPosY = (float)windowH / 2 - (float)texH / 2;

	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{

	float camSpeed = 1; 

	if(app->input->GetKey(SDL_SCANCODE_I) == KEY_REPEAT)
		app->render->camera.y += (int)ceil(camSpeed * dt);

	if(app->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT)
		app->render->camera.y -= (int)ceil(camSpeed * dt);

	if(app->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT)
		app->render->camera.x += (int)ceil(camSpeed * dt);

	if(app->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT)
		app->render->camera.x -= (int)ceil(camSpeed * dt);

	// Renders the image in the center of the screen 
	//app->render->DrawTexture(img, (int)textPosX, (int)textPosY);

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		app->SaveRequest();
	}

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		app->LoadRequest();
	}

	

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	Mix_VolumeMusic(volume);
	Mix_Volume(-1, volume);

	lifetext = "lifes X" + std::to_string(app->lifes);
	app->render->DrawText(lifetext.c_str(), app->scene->windowW / 2 - 480, app->scene->windowH / 2 - 380, 80, 40);

	cointext = "Coins X" + std::to_string(app->coins);
	app->render->DrawText(cointext.c_str(), app->scene->windowW / 2 - 480, app->scene->windowH / 2 - 340, 80, 40);

	timer = "Time: " + std::to_string((SDL_GetTicks() - startTime) / 1000) + " Seconds";
	app->render->DrawText(timer.c_str(), app->scene->windowW / 2 - 480, app->scene->windowH / 2 - 300, 150, 40);

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		pausemenu = !pausemenu;
	}

	if (pausemenu)
	{
		SDL_Rect rect = { 0,0,300,1200 };
		app->render->DrawRectangle(rect, 0, 0, 0, 50);
		app->title->musicbutton->state = GuiControlState::NORMAL;
		app->title->fullbutton->state = GuiControlState::NORMAL;
		app->title->vsyncbutton->state = GuiControlState::NORMAL;
		app->title->titlebutton->state = GuiControlState::NORMAL;
		app->title->resumebutton->state = GuiControlState::NORMAL;
	}
	else
	{
		app->title->musicbutton->state = GuiControlState::DISABLED;
		app->title->fullbutton->state = GuiControlState::DISABLED;
		app->title->vsyncbutton->state = GuiControlState::DISABLED;
		app->title->titlebutton->state = GuiControlState::DISABLED;
		app->title->resumebutton->state = GuiControlState::DISABLED;
	}

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	if (this->active) player->CleanUp();


	return true;
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	

	return true;
}