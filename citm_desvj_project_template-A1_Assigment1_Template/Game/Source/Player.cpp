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

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

    for (pugi::xml_node node = parameters.child("idle").child("pushback");
        node; node = node.next_sibling("pushback"))
    {
        idleAnim.PushBack({ node.attribute("x").as_int(),
                           node.attribute("y").as_int(),
                           node.attribute("width").as_int(),
                           node.attribute("height").as_int() });
    }
    idleAnim.speed = parameters.child("idle").attribute("animspeed").as_float();
    idleAnim.loop = parameters.child("idle").attribute("loop").as_bool();

    for (pugi::xml_node node = parameters.child("run").child("pushback");
        node; node = node.next_sibling("pushback"))
    {
        runAnim.PushBack({ node.attribute("x").as_int(),
                           node.attribute("y").as_int(),
                           node.attribute("width").as_int(),
                           node.attribute("height").as_int() });
    }
    runAnim.speed = parameters.child("run").attribute("animspeed").as_float();
    runAnim.loop = parameters.child("run").attribute("loop").as_bool();

    for (pugi::xml_node node = parameters.child("jump").child("pushback");
        node; node = node.next_sibling("pushback"))
    {
        JumpAnim.PushBack({ node.attribute("x").as_int(),
                           node.attribute("y").as_int(),
                           node.attribute("width").as_int(),
                           node.attribute("height").as_int() });
    }
    JumpAnim.speed = parameters.child("jump").attribute("animspeed").as_float();
    JumpAnim.loop = parameters.child("jump").attribute("loop").as_bool();

    for (pugi::xml_node node = parameters.child("fall").child("pushback");
        node; node = node.next_sibling("pushback"))
    {
        FallAnim.PushBack({ node.attribute("x").as_int(),
                           node.attribute("y").as_int(),
                           node.attribute("width").as_int(),
                           node.attribute("height").as_int() });
    }
    FallAnim.speed = parameters.child("fall").attribute("animspeed").as_float();
    FallAnim.loop = parameters.child("fall").attribute("loop").as_bool();

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

    if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
        vel.x = -speed * dt;
        isMoving = true;
        currentAnimation = &runAnim;
    }
    else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
        vel.x = speed * dt;
        isMoving = true;
        currentAnimation = &runAnim;
    }
    else {
        vel.x = 0;
        isMoving = false;
        currentAnimation = &idleAnim;
    }

    if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && vel.y == 0) {
        // Incrementa la fuerza del salto basándose en cuánto tiempo se ha mantenido presionada la tecla
        jumpImpulse += dt * jumpIncrement;
        currentAnimation = &JumpAnim;

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

    if (vel.y < 0)
    {
        currentAnimation = &FallAnim;
    }

    if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && canDash && isMoving) {
        vel.x *= dashMultiplier;
        canDash = false;
    }

    pbody->body->SetLinearVelocity(vel);

    //vel.y -= -jumpImpulse;

    position.x = METERS_TO_PIXELS(pbody->body->GetPosition().x) - 16;
    position.y = METERS_TO_PIXELS(pbody->body->GetPosition().y) - 16;

    SDL_Rect currentFrame = currentAnimation->GetCurrentFrame();

    SDL_Rect destRect = { position.x, position.y, currentFrame.w, currentFrame.h };
    SDL_RenderCopy(app->render->renderer, texture, &currentFrame, &destRect);
    
    //app->render->DrawTexture(texture, position.x, position.y);

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
