#include "Entity.h"


Entity::Entity() : mPosition {0.0f, 0.0f}, mMovement {0.0f, 0.0f}, 
                   mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f},
                   mScale {DEFAULT_SIZE, DEFAULT_SIZE},
                   mColliderDimensions {DEFAULT_SIZE, DEFAULT_SIZE}, 
                   mTexture {NULL}, mTextureType {SINGLE}, mAngle {0.0f},
                   mSpriteSheetDimensions {}, mDirection {RIGHT}, 
                   mAnimationAtlas {{}}, mAnimationIndices {}, mFrameSpeed {0},
                   mEntityType {NONE} { }

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
    EntityType entityType, BallStatus ballStatus = UNUSED) : mPosition {position}, mVelocity {0.0f, 0.0f}, 
    mAcceleration {0.0f, 0.0f}, mScale {scale}, mMovement {0.0f, 0.0f}, 
    mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)}, 
    mTextureType {SINGLE}, mDirection {RIGHT}, mAnimationAtlas {{}}, 
    mAnimationIndices {}, mFrameSpeed {0}, mSpeed {DEFAULT_SPEED}, 
    mAngle {0.0f}, mEntityType {entityType}, mBallStatus {ballStatus} { }

Entity::~Entity() { UnloadTexture(mTexture); };

/**
 * Iterates through a list of collidable entities, checks for collisions with
 * the player entity, and resolves any vertical overlap by adjusting the 
 * player's position and velocity accordingly.
 * 
 * @param collidableEntities An array of pointers to `Entity` objects that 
 * represent the entities that the current `Entity` instance can potentially
 * collide with. The `collisionCheckCount` parameter specifies the number of
 * entities in the `collidableEntities` array that need to be checked for
 * collision.
 * @param collisionCheckCount The number of entities that the current entity
 * (`Entity`) should check for collisions with. This parameter specifies how
 * many entities are in the `collidableEntities` array that need to be checked
 * for collisions with the current entity.
 */


bool Entity::isEdgeCollidingX() const 
{
    if (mPosition.x - mScale.x/2 < 0 || mPosition.x + mScale.x/2 > getScreenWidth()) return true;
    return false;
}

bool Entity::isEdgeCollidingY() const 
{
    if (mPosition.y - mScale.y/2 < 0 || mPosition.y + mScale.y/2 > getScreenHeight()) return true;
    return false;
}

void Entity::checkEdgeCollisionX() {
    // bascially just for balls
    if (isEdgeCollidingX()) {
        if (mPosition.x > getOrigin().x) addP1Score();
        else if (mPosition.x < getOrigin().x) addP2Score();
        deactivate();
        LOG(getGameStatus());
        LOG("P1, P2 scores:");
        LOG(getP1Score());
        LOG(getP2Score());
        
        // mMovement.x *= -1;
        // if (mPosition.x > getOrigin().x) 
        //     {
        //         float xDistance = getScreenWidth() - mPosition.x;
        //         float xOverlap = mScale.x / 2 - xDistance;
        //         mPosition.x -= xOverlap;
        //     }
        // else if (mPosition.x < getOrigin().x) 
        //     {
        //         float xDistance = mPosition.x - 0;
        //         float xOverlap = mScale.x / 2 - xDistance;
        //         mPosition.x += xOverlap;
        //     }
    }
}

void Entity::checkEdgeCollisionY() {
    if (isEdgeCollidingY()) {
        switch (mEntityType) {
            case PADDLE:
                mVelocity.y = 0;
                break;
            case BALL:
                mMovement.y *= -1;
                break;
            default:
                break;
        }
        if (mPosition.y > getOrigin().y) 
            {
                float yDistance = fabs(getScreenHeight() - mPosition.y);
                float yOverlap = fabs(mScale.y / 2 - yDistance);
                mPosition.y -= yOverlap + 5;
                // mIsCollidingTop = true;
            }
        else if (mPosition.y < getOrigin().y) 
            {
                float yDistance = fabs(mPosition.y - 0);
                float yOverlap = fabs(mScale.y / 2 - yDistance);
                mPosition.y += yOverlap + 5;
                // mIsCollidingBottom = true;
            }
    }
}

void Entity::autoPlay() {
    
    if (mPosition.y < mScale.y / 2) {
        mPosition.y = mScale.y / 2;
        mMovement.y = 1;
    }
    if (mPosition.y > static_cast<float>(getScreenHeight()) - mScale.y / 2) {
        mPosition.y = static_cast<float>(getScreenHeight()) - mScale.y / 2;
        mMovement.y = -1;
    }
}

/**
 * Checks if two entities are colliding based on their positions and collider 
 * dimensions.
 * 
 * @param other represents another Entity with which you want to check for 
 * collision. It is a pointer to the Entity class.
 * 
 * @return returns `true` if the two entities are colliding based on their
 * positions and collider dimensions, and `false` otherwise.
 */
bool Entity::isColliding(Entity *other) const 
{
    if (!other->isActive()) return false;

    float xDistance = fabs(mPosition.x - other->getPosition().x) - 
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) - 
        ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

void Entity::checkCollisionX(Entity *other) {
    float xDistance = fabs(mPosition.x - other->getPosition().x) - 
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    if (xDistance < 0.0f) {
        // meaning x-collision!
        //"unclip"
        if (mVelocity.x > 0) {
            mPosition.x     -= fabs(xDistance);
        } else if (mVelocity.x < 0) {
            mPosition.x    += fabs(xDistance);
        }
        mMovement.x     *= -1;
    }
}

// void Entity::checkCollisionX(Entity *other)
// {
//     if (isColliding(other))
//         {   
//             LOG("collidedX");
//             float xDistance = fabs(mPosition.x - other->mPosition.x);
//             float xOverlap  = fabs(xDistance - (mColliderDimensions.x / 2.0f) - (other->mColliderDimensions.x / 2.0f));

//             if (mVelocity.x > 0) {
//                 mPosition.x     -= xOverlap;
//             } else if (mVelocity.x < 0) {
//                 mPosition.x    += xOverlap;
//             }
//             mVelocity.x     *= -1;
//         }
// }

void Entity::checkCollisionX(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = &collidableEntities[i];
        
        if (isColliding(collidableEntity))
        {            

            // float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            // float yOverlap  = fabs(yDistance - (mColliderDimensions.y / 2.0f) - (collidableEntity->mColliderDimensions.y / 2.0f));

            // // Skip if barely touching vertically (standing on platform)
            // if (yOverlap < Y_COLLISION_THRESHOLD) continue;

            float xDistance = fabs(mPosition.x - collidableEntity->mPosition.x);
            float xOverlap  = fabs(xDistance - (mColliderDimensions.x / 2.0f) - (collidableEntity->mColliderDimensions.x / 2.0f));

            if (mVelocity.x > 0) {
                mPosition.x     -= xOverlap;
                mVelocity.x      = 0;

                // Collision!
                mIsCollidingRight = true;
            } else if (mVelocity.x < 0) {
                mPosition.x    += xOverlap;
                mVelocity.x     = 0;
 
                // Collision!
                mIsCollidingLeft = true;
            }
        }
    }
}

void Entity::checkCollisionY(Entity *other) {
    if (isColliding(other)) {
        LOG("collidedY");
        float yDistance = fabs(mPosition.y - other->mPosition.y);
        float yOverlap  = fabs(yDistance - (mColliderDimensions.y / 2.0f) - (other->mColliderDimensions.y / 2.0f));
            // STEP 3: "Unclip" ourselves from the other entity, and flip our
            //         vertical velocity.
            if (mVelocity.y > 0) {
                mPosition.y -= yOverlap;
            } else if (mVelocity.y < 0) {
                mPosition.y += yOverlap;
            }
            mVelocity.y  *= -1;
    }

}

void Entity::checkCollisionY(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        // STEP 1: For every entity that our player can collide with...
        Entity *collidableEntity = &collidableEntities[i];
        
        if (isColliding(collidableEntity))
        {
            // STEP 2: Calculate the distance between its centre and our centre
            //         and use that to calculate the amount of overlap between
            //         both bodies.
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(yDistance - (mColliderDimensions.y / 2.0f) - 
                              (collidableEntity->mColliderDimensions.y / 2.0f));
            
            // STEP 3: "Unclip" ourselves from the other entity, and zero our
            //         vertical velocity.
            if (mVelocity.y > 0) 
            {
                mPosition.y -= yOverlap;
                mVelocity.y  = 0;
                mIsCollidingBottom = true;
            } else if (mVelocity.y < 0) 
            {
                mPosition.y += yOverlap;
                mVelocity.y  = 0;
                mIsCollidingTop = true;
            }
        }
    }
}
/*
gBalls = new Entity(
        {ORIGIN.x, static_cast<float>(GetRandomValue(ORIGIN.y - SPAWN_RANGE, ORIGIN.y + SPAWN_RANGE))}, // position
        {50.0f, 50.0f},  // scale
        SCREENSHOT,        // texture file address
        BALL                         // entity type
    );
    gBalls->setSpeed(200.0f + gDifficultyLevel * 200.0f);
    gBalls->setBounceAngle(GetRandomValue(-45, 45));
    // gBalls->setBounceAngle(30);
    gBalls->setMovement({ static_cast<float>(((gPlayerTurn == ONE) ? -1 : 1)), 1});


    getOrigin().x
    getDifficultyLevel()
    getPlayerTurn()
    */
void Entity::spawn(int spawnRange) {
    mEntityStatus = ACTIVE;
    // mBallStatus = USED;
    mPosition.x = getOrigin().x;
    mPosition.y = static_cast<float>(GetRandomValue(getOrigin().y - spawnRange, getOrigin().y + spawnRange));
    mSpeed = 200.0f + getDifficultyLevel() * 200.0f;
    mBounceAngle = GetRandomValue(-30, 30);
    mMovement = { static_cast<float>(((getPlayerTurn() == ONE) ? -1 : 1)), 1};
    nextPlayerTurn();
}

/**
 * Updates the current frame index of an entity's animation based on the 
 * elapsed time and frame speed.
 * 
 * @param deltaTime represents the time elapsed since the last frame update.
 */
void Entity::animate(float deltaTime)
{
    mAnimationIndices = mAnimationAtlas.at(mDirection);

    mAnimationTime += deltaTime;
    float secondsPerFrame = 1.0f / mFrameSpeed;

    if (mAnimationTime >= secondsPerFrame)
    {
        mAnimationTime = 0.0f;

        mCurrentFrameIndex++;
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}

void Entity::displayCollider() 
{
    // draw the collision box
    Rectangle colliderBox = {
        mPosition.x - mColliderDimensions.x / 2.0f,  
        mPosition.y - mColliderDimensions.y / 2.0f,  
        mColliderDimensions.x,                        
        mColliderDimensions.y                        
    };

    DrawRectangleLines(
        colliderBox.x,      // Top-left X
        colliderBox.y,      // Top-left Y
        colliderBox.width,  // Width
        colliderBox.height, // Height
        GREEN               // Color
    );
}

void Entity::update(float deltaTime, std::vector<Entity*> paddles) // Ball version
{
    if(mBallStatus == UNUSED) return;

    // if(mEntityStatus == INACTIVE) return;
    if (mEntityStatus == INACTIVE)
    {
        spawn(SPAWN_RANGE);
        return;
    }

    resetColliderFlags();

    // check for colliding edge, update points
    checkEdgeCollisionX();
    checkEdgeCollisionY();

    // check for colliding paddles, then bounce (+ change speed???)
    for (Entity* p : paddles) {
        if (isColliding(p)) {
            // if left/right colliding, flip x
            checkCollisionX(p);
            // if top/bottom colliding, flip y
            // checkCollisionY(p);
        }
    }


    // Lastly, update the velocity and position
    mVelocity.x = mMovement.x * mSpeed;
    mVelocity.y = mMovement.y * mSpeed;

    mPosition.x += mVelocity.x * fabs(cos(mBounceAngle)) * deltaTime;
    mPosition.y += mVelocity.y * fabs(sin(mBounceAngle)) * deltaTime;
}

void Entity::update(float deltaTime) // Paddle version
{
    if(mEntityStatus == INACTIVE) return;

    resetColliderFlags();


    checkEdgeCollisionX();
    checkEdgeCollisionY();

    mVelocity.x = mMovement.x * mSpeed;
    mVelocity.y = mMovement.y * mSpeed;

    mPosition.y += mVelocity.y * deltaTime;
    // switch (mEntityType) {
    //     case PADDLE:
    //         mPosition.y += mVelocity.y * deltaTime;
    //         break;
    //     case BALL:
    //         mPosition.x += mVelocity.x * fabs(cos(mBounceAngle)) * deltaTime;
    //         mPosition.y += mVelocity.y * fabs(sin(mBounceAngle)) * deltaTime;
    //         break;
    // }
}

void Entity::update(float deltaTime, Entity *collidableEntities,
    int collisionCheckCount, Entity* blocks, int blockCount)
{
    if(mEntityStatus == INACTIVE) return;

    resetColliderFlags();

    checkEdgeCollisionX();
    checkEdgeCollisionY();

    mVelocity.x = mMovement.x * mSpeed;
    mVelocity.y = mMovement.y * mSpeed;

    switch (mEntityType) {
        case PADDLE:
            mPosition.y += mVelocity.y * deltaTime;
            break;
        case BALL:
            mPosition.x += mVelocity.x * fabs(cos(mBounceAngle)) * deltaTime;
            mPosition.y += mVelocity.y * fabs(sin(mBounceAngle)) * deltaTime;
            break;
    }
}

void Entity::render()
{
    if(mEntityStatus == INACTIVE) return;

    Rectangle textureArea;

    switch (mTextureType)
    {
        case SINGLE:
            // Whole texture (UV coordinates)
            textureArea = {
                // top-left corner
                0.0f, 0.0f,

                // bottom-right corner (of texture)
                static_cast<float>(mTexture.width),
                static_cast<float>(mTexture.height)
            };
            break;
        case ATLAS:
            textureArea = getUVRectangle(
                &mTexture, 
                mAnimationIndices[mCurrentFrameIndex], 
                mSpriteSheetDimensions.x, 
                mSpriteSheetDimensions.y
            );
        
        default: break;
    }

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        mPosition.x,
        mPosition.y,
        static_cast<float>(mScale.x),
        static_cast<float>(mScale.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(mScale.x) / 2.0f,
        static_cast<float>(mScale.y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        mTexture, 
        textureArea, destinationArea, originOffset,
        mAngle, WHITE
    );

    // displayCollider();
}