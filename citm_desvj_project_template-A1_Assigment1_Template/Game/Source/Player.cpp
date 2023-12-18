#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Map.h"
#include "Animation.h"
#include "Bullet.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {


    //Import Parameters
    initX = parameters.attribute("x").as_int();
    initY = parameters.attribute("y").as_int();
    position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
    deathSFXPath = parameters.attribute("deathsfxpath").as_string();

    //Set animations
    pugi::xml_node animNode = parameters.first_child();
    while (animNode) {
        Animation* currentAnim = nullptr;
        if (std::string(animNode.name()) == "idle") {
            currentAnim = &idleAnim;
        }
        else if (std::string(animNode.name()) == "run") {
            currentAnim = &runAnim;
        }
        else if (std::string(animNode.name()) == "jump") {
            currentAnim = &JumpAnim;
        }
        else if (std::string(animNode.name()) == "death") {
            currentAnim = &DeathAnim;
        }

        if (currentAnim) {
            for (pugi::xml_node node = animNode.child("pushback"); node; node = node.next_sibling("pushback")) {
                currentAnim->PushBack({ node.attribute("x").as_int(),
                                        node.attribute("y").as_int(),
                                        node.attribute("width").as_int(),
                                        node.attribute("height").as_int() });
            }
            currentAnim->speed = animNode.attribute("animspeed").as_float();
            currentAnim->loop = animNode.attribute("loop").as_bool();
        }

        animNode = animNode.next_sibling();
    }

    currentAnimation = &idleAnim;

	return true;
}

bool Player::Start() 
{
	texture = app->tex->Load(texturePath);
    //Add path to config file
    deathSFX = app->audio->LoadFx(deathSFXPath);
    texture2 = app->tex->Load("Assets/Textures/player1.png");
	pbody = app->physics->CreatePlayer(position.x, position.y, 34, 58, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	return true;
}

bool Player::Update(float dt)
{
    if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
        debug = !debug;

    if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
        b2Vec2 stop(0, 0);
        pbody->body->SetLinearVelocity(stop);
        b2Vec2 initpos(PIXEL_TO_METERS(initX), PIXEL_TO_METERS(initY));
        pbody->body->SetTransform(initpos, 0);
        gravityScale = 1.0f;
        pbody->body->GetWorld()->SetGravity(b2Vec2(GRAVITY_X, -GRAVITY_Y));
        pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, 0.0f));
        pbody->body->ApplyForce(b2Vec2(0, 1.0f), pbody->body->GetWorldCenter(), true);
        flipVertical = SDL_FLIP_NONE;
        gravityScale = 1.0f;
        Respawn();
    }

    //Play death animation and respawn
    if (death)
    {
        currentAnimation = &DeathAnim;
        if (!audiohasplayed)
        {
            app->audio->PlayFx(deathSFX);
            audiohasplayed = true;  
        }

        if (currentAnimation->HasFinished())
        {
            b2Vec2 stop(0, 0);
            pbody->body->SetLinearVelocity(stop);
            b2Vec2 initpos(PIXEL_TO_METERS(initX), PIXEL_TO_METERS(initY));
            pbody->body->SetTransform(initpos, 0);
            gravityScale = 1.0f;
            pbody->body->GetWorld()->SetGravity(b2Vec2(GRAVITY_X, -GRAVITY_Y));
            pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, 0.0f));
            pbody->body->ApplyForce(b2Vec2(0, 1.0f), pbody->body->GetWorldCenter(), true);
            flipVertical = SDL_FLIP_NONE;
            gravityScale = 1.0f;
            Respawn();
        }
    }

    //get the velocity from last frame
    b2Vec2 vel = pbody->body->GetLinearVelocity();

    //checks if the character is currently in the air
    if (vel.y != 0)
    {
        isInAir = true;
    }
    else
    {
        isInAir = false;
    }

    SDL_RendererFlip flip = lastDirection;
    if (isInAir)
    {
        //fisicas en el aire
        
        if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !death) {
            vel.x = -speed * dt;
            dashDirection = -1;
            isMoving = true;
            flipHorizontal = SDL_FLIP_HORIZONTAL;
        }
        else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !death) {
            vel.x = speed * dt;
            isMoving = true;
            dashDirection = 1;
            flipHorizontal = SDL_FLIP_NONE;
        }
        else {
            vel.x = 0;
            isMoving = false;
        }
    }
    else
    {
        //fisicas en el suelo
        
        if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !death) {
            vel.x = -speed * dt;
            dashDirection = -1;
            isMoving = true;
            flipHorizontal = SDL_FLIP_HORIZONTAL;
        }
        else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !death) {
            vel.x = speed * dt;
            isMoving = true;
            dashDirection = 1;
            flipHorizontal = SDL_FLIP_NONE;
        }
        else {
            vel.x = 0;
            isMoving = false;
        }
    }


    lastDirection = flipHorizontal;

    //Jump
    if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isTouchingGround && vel.y == 0 && !death) {
        if (gravityScale == 1.0f)
        {
            pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, 0.0f));
            pbody->body->ApplyForce(b2Vec2(0, -1625.0f), pbody->body->GetWorldCenter(), true);
            isTouchingGround = false;
        }
        else if (gravityScale == -1.0f)
        {
            pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, 0.0f));
            pbody->body->ApplyForce(b2Vec2(0, 1625.0f), pbody->body->GetWorldCenter(), true);
            isTouchingGround = false;
        }
        
    }


    //Gravity change
    if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN && gravityScale == 1.0f && (vel.y == 0 || debug) && !death) {
        pbody->body->GetWorld()->SetGravity(b2Vec2(GRAVITY_X, GRAVITY_Y));
        pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, 0.0f));
        pbody->body->ApplyForce(b2Vec2(0, -1.0f), pbody->body->GetWorldCenter(), true);
        flipVertical = SDL_FLIP_VERTICAL;
        gravityScale = -1.0f;
        isTouchingGround = false;
    }
    else if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN && gravityScale == -1.0f && (vel.y == 0 || debug) && !death) {
        pbody->body->GetWorld()->SetGravity(b2Vec2(GRAVITY_X, -GRAVITY_Y));
        pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, 0.0f));
        pbody->body->ApplyForce(b2Vec2(0, 1.0f), pbody->body->GetWorldCenter(), true);
        flipVertical = SDL_FLIP_NONE;
        gravityScale = 1.0f;
        isTouchingGround = false;
    }

    if (app->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_DOWN)
    {
        b2Vec2 bulletDirection;
        b2Vec2 bulletPos;
        if (flipHorizontal == SDL_FLIP_HORIZONTAL) {
            bulletDirection = b2Vec2(-1, 0); // Left
            bulletPos.x = position.x - 22;
            bulletPos.y = position.y + 40;
        }
        else if (flipHorizontal == SDL_FLIP_NONE) {
            bulletDirection = b2Vec2(1, 0); // Right
            bulletPos.x = position.x + 62;
            bulletPos.y = position.y + 40;
        }

        //PhysBody* newBullet = app->physics->CreateBullet(bulletPos.x, bulletPos.y, 5, 5, DYNAMIC, ColliderType::WEAPON, 15, bulletDirection);
        //bullets.push_back(newBullet);

        app->entityManager->CreateEntity(EntityType::BULLET, &bulletPos, &bulletDirection);
    }

    if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT && isTouchingGround && !death)
    {
        vel.x = 0; vel.y = -0.5;
    }

    pbody->body->SetLinearVelocity(vel);

    position.x = METERS_TO_PIXELS(pbody->body->GetPosition().x) - 44;
    position.y = METERS_TO_PIXELS(pbody->body->GetPosition().y) - 42;

    if (vel.y != 0 && !death) {
        currentAnimation = &JumpAnim;
    }
    if (isMoving && vel.y == 0 && !death) {
        currentAnimation = &runAnim;
    }
    if (!isMoving && isTouchingGround && !death)
    {
        currentAnimation = &idleAnim;
    }
    currentAnimation->Update();
    
    SDL_RendererFlip flips = (SDL_RendererFlip)(flipHorizontal | flipVertical);

    SDL_Rect currentFrame = currentAnimation->GetCurrentFrame();
    SDL_Rect destRect = { position.x - 5, position.y - 8, currentFrame.w, currentFrame.h };
    app->render->DrawTexture(texture, destRect.x, destRect.y, &currentFrame, flips);

    UpdateCamera();

    return true;
}



bool Player::CleanUp()
{
    if (texture != nullptr)
    {
        app->tex->UnLoad(texture);
        texture = nullptr;
    }

    if (pbody != nullptr)
    {
        delete pbody;
        pbody = nullptr;
    }
    
    return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

    switch (physB->ctype)
    {
    case ColliderType::ITEM:
        LOG("Collision ITEM");
        app->audio->PlayFx(pickCoinFxId);
        break;
    case ColliderType::PLATFORM:
        LOG("Collision PLATFORM");
        isTouchingGround = true;
        break;
    case ColliderType::UNKNOWN:
        LOG("Collision UNKNOWN");
        break;
    case ColliderType::DEATH:
        LOG("Collision DEATH");
        Death();
        break;
    case ColliderType::ENEMY:
        LOG("Collision ENEMY");
		Death();
		break;
    }
}

void Player::Death()
{
    if(!debug)
        death = true;
}

void Player::Respawn()
{
    death = false;
    DeathAnim.Reset();
    currentAnimation = &idleAnim; 
    audiohasplayed = false;
    app->entityManager->RespawnAllEnemies();
}

void Player::UpdateCamera()
{
    app->render->camera.x = -position.x + app->render->camera.w / 2;
    app->render->camera.y = -position.y + app->render->camera.h / 2;
    if (app->render->camera.x > 0) app->render->camera.x = 0;
    if (app->render->camera.y > 0) app->render->camera.y = 0;
    if (app->render->camera.x < -app->map->mapData.width * app->map->mapData.tileWidth + app->render->camera.w) app->render->camera.x = -app->map->mapData.width * app->map->mapData.tileWidth + app->render->camera.w;
    if (app->render->camera.y < -app->map->mapData.height * app->map->mapData.tileHeight + app->render->camera.h) app->render->camera.y = -app->map->mapData.height * app->map->mapData.tileHeight + app->render->camera.h;
    
}


bool Player::LoadState(pugi::xml_node node, int num)
{
    
    SString childName("player%d", num);
    
    
    pugi::xml_node player = node.child(childName.GetString());
    

    pugi::xml_node pos = player.child("Position");
    float32 x = pos.attribute("x").as_float();
    float32 y = pos.attribute("y").as_float();
    float gravity = player.attribute("gravity").as_float();
    pugi::xml_node velocity = player.child("velocity");
    float32 vx = velocity.attribute("vx").as_float();
    float32 vy = velocity.attribute("vy").as_float();

    pbody->body->SetTransform(b2Vec2(x, y), 0);
    gravityScale = gravity;
    pbody->body->SetLinearVelocity(b2Vec2(vx, vy));
    b2Vec2 vel = pbody->body->GetLinearVelocity();


    return true;
}

bool Player::SaveState(pugi::xml_node node, int num)
{
    SString childName("player%d", num);

    
    pugi::xml_node player = node.append_child(childName.GetString());


    pugi::xml_node pos = player.append_child("Position");
    pugi::xml_attribute x = pos.append_attribute("x");
    pugi::xml_attribute y = pos.append_attribute("y");
    pugi::xml_attribute gravity = player.append_attribute("gravity");
    pugi::xml_node velocity = player.append_child("velocity");
    pugi::xml_attribute vx = velocity.append_attribute("vx");
    pugi::xml_attribute vy = velocity.append_attribute("vy");


    x.set_value(pbody->body->GetPosition().x);
    y.set_value(pbody->body->GetPosition().y);
    gravity.set_value(gravityScale);
    vx.set_value(pbody->body->GetLinearVelocity().x);
    vy.set_value(pbody->body->GetLinearVelocity().y);


    return true;
}


