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

    initX = parameters.attribute("x").as_int();
    initY = parameters.attribute("y").as_int();
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

	pbody = app->physics->CreatePlayer(position.x, position.y, 28, 28, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update(float dt)
{
    b2Vec2 vel = pbody->body->GetLinearVelocity();

    if (isDashing) {
        dashTime += dt;
        if (dashTime >= maxDashTime) {
            isDashing = false;
            canDash = false;
            dashTime = 0.0f;
        }
        else {
            vel.x += dashDirection * dashSpeed * dt;
        }
    }

    SDL_RendererFlip flip = lastDirection;
    if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
        vel.x = -speed * dt;
        dashDirection = -1;
        isMoving = true;
        flipHorizontal = SDL_FLIP_HORIZONTAL;  
    }
    else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
        vel.x = speed * dt;
        isMoving = true;
        dashDirection = 1;
        flipHorizontal = SDL_FLIP_NONE;
    }
    else {
        vel.x = 0;
        isMoving = false;
    }

    lastDirection = flipHorizontal;

    if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isTouchingGround && vel.y == 0) {
        if (gravityScale == 1.0f)
        {
            pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, 0.0f));
            pbody->body->ApplyForce(b2Vec2(0, -450.0f), pbody->body->GetWorldCenter(), true);
            isTouchingGround = false;
        }
        else if (gravityScale == -1.0f)
        {
            pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, 0.0f));
            pbody->body->ApplyForce(b2Vec2(0, 450.0f), pbody->body->GetWorldCenter(), true);
            isTouchingGround = false;
        }
        
    }

    if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && gravityScale == 1.0f) {
        pbody->body->GetWorld()->SetGravity(b2Vec2(GRAVITY_X, GRAVITY_Y));
        pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, 0.0f));
        pbody->body->ApplyForce(b2Vec2(0, -1.0f), pbody->body->GetWorldCenter(), true);
        flipVertical = SDL_FLIP_VERTICAL;
        gravityScale = -1.0f;
        isTouchingGround = false;
    }
    else if (app->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN && gravityScale == -1.0f) {
        pbody->body->GetWorld()->SetGravity(b2Vec2(GRAVITY_X, -GRAVITY_Y));
        pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, 0.0f));
        pbody->body->ApplyForce(b2Vec2(0, 1.0f), pbody->body->GetWorldCenter(), true);
        flipVertical = SDL_FLIP_NONE;
        gravityScale = 1.0f;
        isTouchingGround = false;
    }

    if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN && canDash)
    {
        if (lastDirection == SDL_FLIP_NONE)
        {
            pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, pbody->body->GetLinearVelocity().y));
            pbody->body->ApplyForce(b2Vec2(5000.0f, 0.0f), pbody->body->GetWorldCenter(), true);
        }
        else if (lastDirection == SDL_FLIP_HORIZONTAL)
        {
            pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, pbody->body->GetLinearVelocity().y));
            pbody->body->ApplyForce(b2Vec2(-5000.0f, 0.0f), pbody->body->GetWorldCenter(), true);
        }
    }

    pbody->body->SetLinearVelocity(vel);

    position.x = METERS_TO_PIXELS(pbody->body->GetPosition().x) - 16;
    position.y = METERS_TO_PIXELS(pbody->body->GetPosition().y) - 16;

    if (!isTouchingGround) {
        currentAnimation = &JumpAnim;
    }
    if (isMoving && isTouchingGround) {
        currentAnimation = &runAnim;
    }
    if (!isMoving && isTouchingGround)
    {
        currentAnimation = &idleAnim;
    }
    currentAnimation->Update();

    SDL_RendererFlip flips = (SDL_RendererFlip)(flipHorizontal | flipVertical);

    SDL_Rect currentFrame = currentAnimation->GetCurrentFrame();
    SDL_Rect destRect = { position.x - 5, position.y - 8, currentFrame.w, currentFrame.h };
    SDL_RenderCopyEx(app->render->renderer, texture, &currentFrame, &destRect, 0.0, NULL, flips);

    UpdateCamera();

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
        break;
    case ColliderType::UNKNOWN:
        LOG("Collision UNKNOWN");
        break;
    case ColliderType::DEATH:
        LOG("Collision DEATH");
        Death();
        break;
    }
}

void Player::Death()
{
    
}

void Player::UpdateCamera()
{
    /*app->render->camera.x = position.x + app->render->camera.w / 2;
    app->render->camera.y = position.y - app->render->camera.h / 2;*/

    //if (app->render->camera.x < 0) app->render->camera.x = 0;
    //if (app->render->camera.y < 0) app->render->camera.y = 0;
    //if (app->render->camera.x > app->map->mapData.width - app->render->camera.w) app->render->camera.x = app->map->mapData.width - app->render->camera.w;
    //if (app->render->camera.y > app->map->mapData.height - app->render->camera.h) app->render->camera.y = app->map->mapData.height - app->render->camera.h;
}
