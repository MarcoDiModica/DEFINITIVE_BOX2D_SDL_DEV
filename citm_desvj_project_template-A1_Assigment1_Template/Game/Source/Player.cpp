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

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

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
        else if (std::string(animNode.name()) == "fall") {
            currentAnim = &FallAnim;
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

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

    

	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

void Player::UpdateAnimation(b2Vec2 vel, bool isTouchingGround)
{
    if (vel.x != 0) {
        currentAnimation = &runAnim;
    }
    else if (vel.y > 0) {
        currentAnimation = &JumpAnim;
    }
    else if (!isTouchingGround) {
        currentAnimation = &FallAnim;
    }
    else {
        currentAnimation = &idleAnim;
    }
    currentAnimation->Update();
}


bool Player::Update(float dt)
{
    b2Vec2 vel = pbody->body->GetLinearVelocity();

    bool inAir = vel.y != 0;

    SDL_RendererFlip flip = lastDirection;
    if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
        vel.x = -speed * dt;
        isMoving = true;
        flip = SDL_FLIP_HORIZONTAL;  
        lastDirection = flip;
    }
    else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
        vel.x = speed * dt;
        isMoving = true;
        flip = SDL_FLIP_NONE;  
        lastDirection = flip;
    }
    else {
        vel.x = 0;
        isMoving = false;
    }

    if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && vel.y == 0) {
        jumpImpulse += dt * jumpIncrement;

        if (jumpImpulse > maxJumpImpulse) {
            jumpImpulse = maxJumpImpulse;
        }

        vel.y = -jumpImpulse;
    }
    else {
        jumpImpulse = initialJumpImpulse;
    }

    if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && canDash && isMoving) {
        vel.x *= dashMultiplier;
        canDash = false;
    }

    pbody->body->SetLinearVelocity(vel);

    position.x = METERS_TO_PIXELS(pbody->body->GetPosition().x) - 16;
    position.y = METERS_TO_PIXELS(pbody->body->GetPosition().y) - 16;

    UpdateAnimation(vel, isTouchingGround);

    SDL_Rect currentFrame = currentAnimation->GetCurrentFrame();
    SDL_Rect destRect = { position.x - 5, position.y - 8, currentFrame.w, currentFrame.h };
    SDL_RenderCopyEx(app->render->renderer, texture, &currentFrame, &destRect, 0.0, NULL, flip);

    return true;
}



bool Player::CleanUp()
{

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
        canDash = true;
        break;
    case ColliderType::UNKNOWN:
        LOG("Collision UNKNOWN");
        break;
    }
}

void Player::UpdateCamera()
{
    //app->render->camera.x = position.x + app->render->camera.w / 2;
    //app->render->camera.y = position.y - app->render->camera.h / 2;

    /*if (app->render->camera.x < 0) app->render->camera.x = 0;
    if (app->render->camera.y < 0) app->render->camera.y = 0;
    if (app->render->camera.x > app->map->mapData.width - app->render->camera.w) app->render->camera.x = app->map->mapData.width - app->render->camera.w;
    if (app->render->camera.y > app->map->mapData.height - app->render->camera.h) app->render->camera.y = app->map->mapData.height - app->render->camera.h;*/
}
