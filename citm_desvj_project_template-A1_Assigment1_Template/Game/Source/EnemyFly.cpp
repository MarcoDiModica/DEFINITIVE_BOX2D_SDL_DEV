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
    texture = app->tex->Load(texturePath);
    pbody = app->physics->CreateFlyingEnemy(position.x, position.y, 34, 58, bodyType::DYNAMIC);
    //app->physics->CreatePathForGroundEnemy(pbody, ? ? ? , ? ? ? , position.y);
    pbody->listener = this;
    pbody->ctype = ColliderType::ENEMY;
    return true;
}

bool EnemyFLY::Update(float dt)
{
    b2Vec2 vel = pbody->body->GetLinearVelocity();

    if (death)
    {
        currentAnimation = &DeathAnim;

        if (currentAnimation->HasFinished())
        {
            b2Vec2 stop(0, 0);
            pbody->body->SetLinearVelocity(stop);
            app->entityManager->DestroyEntity(this);
        }
    }
    else
    {
        // Si el jugador está a la vista, mueve al enemigo hacia el jugador
        //if (PlayerInSight())
        //{
        //    
        //    
        //    if (player->position.x > position.x)
        //    {
        //        // Mueve al enemigo a la derecha
        //        vel.x = enemySpeed;
        //        flipHorizontal = SDL_FLIP_NONE;
        //        isMoving = true;
        //    }
        //    else if (player->position.x < position.x)
        //    {
        //        // Mueve al enemigo a la izquierda
        //        vel.x = -enemySpeed;
        //        flipHorizontal = SDL_FLIP_HORIZONTAL;
        //        isMoving = true;
        //    }

        //    // Si hay un obstáculo delante, haz que el enemigo salte
        //    if (ObstacleInFront())
        //    {
        //        Jump();
        //    }
        //}
        //else
        //{
        //    vel.x = 0;
        //    isMoving = false;
        //}
    }

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
        delete pbody;
        pbody = nullptr;
    }

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
        Death();
        break;
    case ColliderType::PLAYER:
        LOG("Collision PLAYER");
        Death();
        break;
    case ColliderType::WEAPON:
        LOG("Collision WEAPON");
		Death();
		break;
	}
}

void EnemyFLY::Death()
{
    if (!debug)
        death = true;
}