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

    idleAnim.PushBack({ 138, 154, 32, 29 });

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

    currentAnimation = &idleAnim;

	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update(float dt)
{
    b2Vec2 vel = pbody->body->GetLinearVelocity();

    UpdateCamera();

    if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
        vel.x = -speed * dt;
        isMoving = true;
    }
    else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
        vel.x = speed * dt;
        isMoving = true;
    }
    else {
        vel.x = 0;
        isMoving = false;
    }

    if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && vel.y == 0) {
        // Incrementa la fuerza del salto basándose en cuánto tiempo se ha mantenido presionada la tecla
        jumpImpulse += dt * jumpIncrement;

        // Limita la fuerza del salto a un valor máximo
        if (jumpImpulse > maxJumpImpulse) {
            jumpImpulse = maxJumpImpulse;
        }

        // Aplica la fuerza del salto
        vel.y = -jumpImpulse;
    }
    else {
        // Si se suelta la tecla de salto, resetea la fuerza del salto
        jumpImpulse = initialJumpImpulse;
    }

    if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && canDash && isMoving) {
        vel.x *= dashMultiplier;
        canDash = false;
    }

    pbody->body->SetLinearVelocity(vel);

    //vel.y -= -jumpImpulse;

    position.x = METERS_TO_PIXELS(pbody->body->GetPosition().x) - 16;
    position.y = METERS_TO_PIXELS(pbody->body->GetPosition().y) - 16;

    app->render->DrawTexture(texture, position.x, position.y);

    if (vel.y > 0)
        isTouchingGround = false;

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
