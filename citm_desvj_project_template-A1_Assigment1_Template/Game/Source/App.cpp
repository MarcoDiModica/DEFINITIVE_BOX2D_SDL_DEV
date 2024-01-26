#include "App.h"
#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Scene.h"
#include "Map.h"
#include "Physics.h"
#include "Pathfinding.h"
#include "GuiManager.h"
#include "dead.h"
#include "win.h"
#include "winboss.h"
#include "group.h"
#include "title.h"
#include "Optick/include/optick.h"

#include "Defs.h"
#include "Log.h"

#include <iostream>
#include <sstream>

// Constructor
App::App(int argc, char* args[]) : argc(argc), args(args)
{
	Timer timer = Timer();
	startupTime = Timer();
	frameTime = PerfTimer();
	lastSecFrameTime = PerfTimer();

	frames = 0;

	win = new Window();
	input = new Input();
	render = new Render();
	tex = new Textures();
	audio = new Audio();
	physics = new Physics();
	scene = new Scene();
	map = new Map();
	entityManager = new EntityManager();
	guiManager = new GuiManager();
	dead = new Dead();
	victory = new Victory();
	victory_boss = new VictoryBoss();
	group = new Group();
	title = new Title();


	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(win);
	AddModule(input);
	AddModule(tex);
	AddModule(audio);
	AddModule(physics, false);
	AddModule(map, false);
	AddModule(scene, false);
	AddModule(dead, false);
	AddModule(victory, false);
	AddModule(victory_boss, false);
	AddModule(group, true);
	AddModule(title, false);
	
	AddModule(entityManager, true);
	AddModule(guiManager, true);

	// Render last to swap buffer
	AddModule(render);

	LOG("Timer App Constructor: %f", timer.ReadMSec());
}

// Destructor
App::~App()
{
	// Release modules
	ListItem<Module*>* item = modules.end;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.Clear();
}

void App::AddModule(Module* module, bool isActive)
{
	module->Init(isActive);
	modules.Add(module);
}

// Called before render is available
bool App::Awake()
{
	Timer timer = Timer();

	bool ret = LoadConfig();

	if(ret == true)
	{
		gameTitle = configNode.child("app").child("title").child_value();
		win->SetTitle(gameTitle.GetString());
		maxFrameDuration = configFile.child("config").child("app").child("maxFrameDuration").attribute("value").as_int();

		ListItem<Module*>* item;
		item = modules.start;

		while(item != NULL && ret == true)
		{
			// If the section with the module name exists in config.xml, fill the pointer with the valid xml_node
			// that can be used to read all variables for that module.
			// Send nullptr if the node does not exist in config.xml
			pugi::xml_node node = configNode.child(item->data->name.GetString());
			ret = item->data->Awake(node);
			item = item->next;
		}
	}

	LOG("Timer App Awake(): %f", timer.ReadMSec());

	return ret;
}

// Called before the first frame
bool App::Start()
{
	Timer timer = Timer();

	bool ret = true;
	ListItem<Module*>* item;
	item = modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}

	LOG("Timer App Start(): %f", timer.ReadMSec());

	return ret;
}

// Called each loop iteration
bool App::Update()
{
	OPTICK_FRAME("Main Loop");
	
	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();

	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {
		if (maxFrameDuration == 1000 / 60) {
			maxFrameDuration = 1000 / 30;
		}
		else {
			maxFrameDuration = 1000 / 60;
		}
	}

	return ret;
}

// Load config from XML file
bool App::LoadConfig()
{
	bool ret = false;
	pugi::xml_parse_result parseResult = configFile.load_file("config.xml");

	if (parseResult) {
		ret = true;
		configNode = configFile.child("config");
	}
	else {
		LOG("Error in App::LoadConfig(): %s", parseResult.description());
	}

	return ret;
}

// ---------------------------------------------
void App::PrepareUpdate()
{
	OPTICK_EVENT();
	
	frameTime.Start();
}

// ---------------------------------------------
void App::FinishUpdate()
{
	OPTICK_EVENT();
	
	// This is a good place to call Load / Save functions
	double currentDt = frameTime.ReadMs();
	if (maxFrameDuration > 0 && currentDt < maxFrameDuration) {
		uint32 delay = (uint32) (maxFrameDuration - currentDt);

		PerfTimer delayTimer = PerfTimer();
		SDL_Delay(delay);
		//LOG("We waited for %I32u ms and got back in %f ms",delay,delayTimer.ReadMs());
	}

	// Amount of frames since startup
	frameCount++;

	// Amount of time since game start (use a low resolution timer)
	secondsSinceStartup = startupTime.ReadSec();
	
	// Amount of ms took the last update (dt)
	dt = (float) frameTime.ReadMs();

	// Amount of frames during the last second
	lastSecFrameCount++;

	// Average FPS for the whole game life
	if (lastSecFrameTime.ReadMs() > 1000) {
		lastSecFrameTime.Start();
		averageFps = (averageFps + lastSecFrameCount) / 2;
		framesPerSecond = lastSecFrameCount; 
		lastSecFrameCount = 0;
	}


	// Shows the time measurements in the window title
	static char title[256];
	sprintf_s(title, 256, "%s: Av.FPS: %.2f Last sec frames: %i Last dt: %.3f Time since startup: %I32u Frame Count: %I64u Lifes: %d Coins: %d",
		gameTitle.GetString(), averageFps, framesPerSecond, dt, secondsSinceStartup, frameCount, lifes, coins);

	app->win->SetTitle(title);

	if (level1Request)
	{
		ToLevel1();
		level1Request = false;
	}

	if (level2Request)
	{
		ToLevel2();
		level2Request = false;
	}

	if (loadRequest)
	{
		LoadFromFile();
		loadRequest = false;
	}
	
	if (saveRequest)
	{
		SaveToFile();
		saveRequest = false;
	}
}

// Call modules before each loop iteration
bool App::PreUpdate()
{
	OPTICK_EVENT();
	
	bool ret = true;

	ListItem<Module*>* item;
	Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool App::DoUpdate()
{
	OPTICK_EVENT();
	
	bool ret = true;
	ListItem<Module*>* item;
	item = modules.start;
	Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool App::PostUpdate()
{
	OPTICK_EVENT();
	
	bool ret = true;
	ListItem<Module*>* item;
	Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool App::CleanUp()
{
	Timer timer = Timer();

	bool ret = true;
	ListItem<Module*>* item;
	item = modules.end;

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	LOG("Timer App CleanUp(): %f", timer.ReadMSec());

	return ret;
}

// ---------------------------------------
int App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* App::GetTitle() const
{
	return gameTitle.GetString();
}

// ---------------------------------------
const char* App::GetOrganization() const
{
	return organization.GetString();
}

void App::LoadRequest()
{
	loadRequest = true;
}

void App::SaveRequest()
{
	saveRequest = true;
}

void App::Level1Request()
{
	level1Request = true;
}

void App::Level2Request()
{
	level2Request = true;
}

void App::LoadFromFile()
{
	//Open the XML file for reading save_game.xml

	pugi::xml_document saveFile;

	pugi::xml_parse_result result = saveFile.load_file("save_game.xml");
	if (result)
	{
		LOG("save_game.xml parsed without errors");
		//Iterate all modules and get the child of the module and call LoadState

		ListItem<Module*>* item;
		item = modules.start;

		while (item != NULL)
		{

			item->data->LoadState(saveFile.child("game_state").child(item->data->name.GetString()));
			item = item->next;
		}

	}
	else
	{
		LOG("Error loading save_game.xml: %s", result.description());
	}



}

void App::SaveToFile()
{
	pugi::xml_document saveFile;

	ListItem<Module*>* item;
	item = modules.start;

	pugi::xml_node gameState = saveFile.append_child("game_state");

	while (item != NULL)
	{

		if (item->data->name.GetCapacity() > 1)
		{
			pugi::xml_node moduleNode = gameState.append_child((item->data->name.GetString()));
			item->data->SaveState(moduleNode);
			saveFile.save_file("save_game.xml");
		}
		item = item->next;
	}


}

void App::ToLevel1()
{

	level2 = false;

	ListItem<Module*>* item;
	item = modules.start;


	while (item != NULL)
	{
		item->data->goToLevel1();
		item = item->next;
	}

}

void App::ToLevel2()
{

	level2 = true;

	ListItem<Module*>* item;
	item = modules.start;


	while (item != NULL)
	{
		item->data->goToLevel2();
		item = item->next;
	}

}

