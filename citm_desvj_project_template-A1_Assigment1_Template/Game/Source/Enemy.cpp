#include "Enemy.h"
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

Enemy::Enemy() : Entity(EntityType::WALKING_ENEMY)
{
	name.Create("Enemy");
}

Enemy::~Enemy() {

}

bool Enemy::Awake() {

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

bool Enemy::Start()
{
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateGroundEnemy(position.x, position.y, 34, 58, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;
	return true;
}

bool Enemy::Update(float dt)
{
    if (death)
    {
        currentAnimation = &DeathAnim;

        if (currentAnimation->HasFinished())
        {
            b2Vec2 stop(0, 0);
            pbody->body->SetLinearVelocity(stop);
        }
    }

    b2Vec2 vel = pbody->body->GetLinearVelocity();

    if (path.x != 0 && path.y != 0 && !death) {
        if (position.x < path.x) {
			vel.x = enemySpeed;
			flipHorizontal = SDL_FLIP_NONE;
			isMoving = true;
		}
        else if (position.x > path.x) {
			vel.x = -enemySpeed;
			flipHorizontal = SDL_FLIP_HORIZONTAL;
			isMoving = true;
		}
        else if (position.x == path.x) {
			vel.x = 0;
			isMoving = false;
		}
	}
    else if (!death) {
		vel.x = 0;
		isMoving = false;
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

bool Enemy::CleanUp()
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

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {

    switch (physB->ctype)
    {
    case ColliderType::UNKNOWN:
        LOG("Collision UNKNOWN");
        break;
    case ColliderType::DEATH:
        LOG("Collision DEATH");
        Death();
        break;
    }
}

void Enemy::Death()
{
    if(!debug)
        death = true;
}