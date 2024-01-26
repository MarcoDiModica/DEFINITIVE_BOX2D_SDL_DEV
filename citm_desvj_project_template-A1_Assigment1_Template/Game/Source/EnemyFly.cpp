#include "EnemyFly.h"
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
#include "Player.h"
#include "EntityManager.h"

EnemyFLY::EnemyFLY() : Entity(EntityType::FLYING_ENEMY)
{
    name.Create("EnemyFly");
}

EnemyFLY::~EnemyFLY() {

}

bool EnemyFLY::Awake() {

    initX = parameters.attribute("x").as_int();
    initY = parameters.attribute("y").as_int();
    position.x = parameters.attribute("x").as_int();
    position.y = parameters.attribute("y").as_int();
    texturePath = parameters.attribute("texturepath").as_string();
    deathSFXPath = parameters.attribute("deathsfxpath").as_string();
    //path = parameters.attribute("path").as_string();

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

bool EnemyFLY::Start()
{
    audiohasplayed = false;
    death = false;
    DeathAnim.Reset();
    texture = app->tex->Load(texturePath);
    deathSFX = app->audio->LoadFx(deathSFXPath);
    pathTexture = app->tex->Load("Assets/Textures/player1.png");
    pbody = app->physics->CreateFlyingEnemy(position.x, position.y, 34, 58, bodyType::DYNAMIC);
    //app->physics->CreatePathForGroundEnemy(pbody, ? ? ? , ? ? ? , position.y);
    pbody->listener = this;
    pbody->ctype = ColliderType::ENEMY;
    return true;
}

bool EnemyFLY::Update(float dt)
{
    if (firstload)
    {
        this->Awake();
        this->Start();
        firstload = false;
    }
    
    b2Vec2 vel = pbody->body->GetLinearVelocity();

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
            Disable();
            return true;
        }
    }
    else
    {
        //// Si el jugador está a la vista, mueve al enemigo hacia el jugador
        //if (position.DistanceTo(app->scene->player->position) < 200)
        //{
        //    if (app->scene->player->position.x > position.x)
        //    {
        //        // Move the enemy to the right
        //        vel.x = enemySpeed * dt;
        //        flipHorizontal = SDL_FLIP_NONE;
        //        isMoving = true;
        //    }
        //    else if (app->scene->player->position.x < position.x)
        //    {
        //        // Move the enemy to the left
        //        vel.x = -enemySpeed * dt;
        //        flipHorizontal = SDL_FLIP_HORIZONTAL;
        //        isMoving = true;
        //    }

        //    if (app->scene->player->position.y > position.y)
        //    {
        //        // Move the enemy upwards
        //        vel.y = -enemySpeed * dt;
        //    }
        //    else if (app->scene->player->position.y < position.y)
        //    {
        //        // Move the enemy downwards
        //        vel.y = enemySpeed * dt;
        //    }

        //    // If there is an obstacle in front, make the enemy jump
        //    /*if (ObstacleInFront())
        //    {
        //        Jump();
        //    }*/
        //}
        //else
        //{
        //    vel.x = 0;
        //    vel.y = 0;
        //    isMoving = false;
        //}

        iPoint enemyPos = app->map->WorldToMap(position.x, position.y);
        iPoint playerPos = app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y);

        if (position.DistanceTo(app->scene->player->position) < 700) {
            app->map->pathfinding->CreatePath(enemyPos, playerPos);

            const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
            b2Vec2 posi;
            if (app->physics->debug && !death) {
                for (uint i = 0; i < path->Count(); ++i)
                {
                    iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
                    app->render->DrawTexture(pathTexture, pos.x, pos.y, false);
                }
            }
            if (path->Count() > 1 && app->map->pathfinding->CreatePath(enemyPos, playerPos) != -1 && path->Count() < 25) {

                iPoint pos = app->map->MapToWorld(path->At(1)->x, path->At(1)->y);

                float dirx = position.x - pos.x;
                float diry = position.y - pos.y;

                b2Vec2 vel;
                vel.x = dirx;
                vel.y = diry;
                vel.Normalize();

                pbody->body->SetLinearVelocity(b2Vec2(-vel.x * 2, -vel.y * 2));
            }
            else
            {
                pbody->body->SetLinearVelocity(b2Vec2(0, 0));
                pbody->body->SetLinearDamping(0);
            }
        }
    }
    
    //pbody->body->SetLinearVelocity(vel);

    position.x = METERS_TO_PIXELS(pbody->body->GetPosition().x) - 44;
    position.y = METERS_TO_PIXELS(pbody->body->GetPosition().y) - 42;

    if (vel.y != 0 && !death) {
        currentAnimation = &JumpAnim;
    }
    if (isMoving && vel.y == 0 && !death) {
        currentAnimation = &JumpAnim;
    }
    if (!isMoving && isTouchingGround && !death)
    {
        currentAnimation = &JumpAnim;
    }
    currentAnimation->Update();

    SDL_RendererFlip flips = (SDL_RendererFlip)(flipHorizontal | flipVertical);

    SDL_Rect currentFrame = currentAnimation->GetCurrentFrame();
    SDL_Rect destRect = { position.x - 5, position.y - 8, currentFrame.w, currentFrame.h };
    //SDL_RenderCopyEx(app->render->renderer, texture, &currentFrame, &destRect, 0.0, NULL, flips);
    app->render->DrawTexture(texture, destRect.x, destRect.y, &currentFrame, flips);

    return true;
}

bool EnemyFLY::CleanUp()
{
    if (texture != nullptr)
    {
        app->tex->UnLoad(texture);
        texture = nullptr;
    }

    if (pbody != nullptr)
    {
        app->physics->DestroyObject(pbody);
        pbody = nullptr;
    }

    position.x = initX;
    position.y = initY;

    return true;
}

void EnemyFLY::OnCollision(PhysBody* physA, PhysBody* physB) {

    switch (physB->ctype)
    {
    case ColliderType::UNKNOWN:
        LOG("Collision UNKNOWN");
        break;
    case ColliderType::DEATH:
        LOG("Collision DEATH");
        //Death();
        break;
    case ColliderType::PLAYER:
        LOG("Collision PLAYER");
        break;
    case ColliderType::WEAPON:
        LOG("Collision WEAPON");
		Death();
		break;
	}
}

void EnemyFLY::Death()
{
    death = true;
    pbody->ctype = ColliderType::PLATFORM;

}

bool EnemyFLY::LoadState(pugi::xml_node node, int num)
{

    SString childName("enemy%d", num);


    pugi::xml_node enemy = node.child(childName.GetString());




    pugi::xml_node ppos = enemy.child("PhysicalPosition");
    float32 px = ppos.attribute("x").as_float();
    float32 py = ppos.attribute("y").as_float();
    pugi::xml_node gpos = enemy.child("GraphicPosition");
    float32 gx = gpos.attribute("x").as_float();
    float32 gy = gpos.attribute("y").as_float();

    bool enabled = enemy.attribute("enabled").as_bool();
    bool grounded = enemy.attribute("grounded").as_bool();


    if (enabled)
    {
        if (active)
        {
            pbody->body->SetTransform(b2Vec2(px, py), 0);
        }
        else
        {

            position.x = gx;
            position.y = gy;
            Enable();
        }
        isTouchingGround = grounded;
    }
    else
    {
        if (active)
        {
            Disable();
        }
    }


    return true;
}

bool EnemyFLY::SaveState(pugi::xml_node node, int num)
{

    SString childName("enemy%d", num);


    pugi::xml_node enemy = node.append_child(childName.GetString());

    pugi::xml_node ppos = enemy.append_child("PhysicalPosition");
    pugi::xml_attribute px = ppos.append_attribute("x");
    pugi::xml_attribute py = ppos.append_attribute("y");
    pugi::xml_node gpos = enemy.append_child("GraphicPosition");
    pugi::xml_attribute gx = gpos.append_attribute("x");
    pugi::xml_attribute gy = gpos.append_attribute("y");
    pugi::xml_attribute enabled = enemy.append_attribute("enabled");
    pugi::xml_attribute grounded = enemy.append_attribute("grounded");

    if (active)
    {
        px.set_value(pbody->body->GetPosition().x);
        py.set_value(pbody->body->GetPosition().y);
    }
    else
    {
        px.set_value(0);
        py.set_value(0);
    }

    gx.set_value(position.x + 44);
    gy.set_value(position.y + 42);

    enabled.set_value(active);
    grounded.set_value(isTouchingGround);


    return true;

}