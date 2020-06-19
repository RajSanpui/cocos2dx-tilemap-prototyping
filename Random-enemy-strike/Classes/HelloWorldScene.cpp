#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "../cocos2d/cocos/deprecated/CCDeprecated.h"

USING_NS_CC;

enum class PhysicsCategory {
    None = 0,
    Soldier = (1 << 1),   // 2
    //All = PhysicsCategory::Boulder | PhysicsCategory::Jewels// 5
};

Scene* HelloWorld::createScene()
{
    //return HelloWorld::create();
    // 'scene' is an autorelease object

    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setGravity(Vect(0, -900));
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    //CCScene *scene = CCScene::create();

    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
    //return HelloWorld::create();
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    //if ( !Scene::init() )
    if ( !CCLayer::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("collect-coin.ogg");

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    _tileMap = new CCTMXTiledMap();
    // _tileMap->initWithTMXFile("TileMap.tmx");
    _tileMap->initWithTMXFile("xmas-candy-1.tmx");
    //_tileMap->setScale(0.8f);

    _background = _tileMap->layerNamed("Background");
    _foreground = _tileMap->layerNamed("Ground");
    //_obstacles = _tileMap->layerNamed("Obstacles");

    //_meta = _tileMap->layerNamed("Meta");
    //_background->setVisible(true);
    //_foreground->setVisible(true);

    this->addChild(_tileMap);

    CCTMXObjectGroup *objectGroup = _tileMap->objectGroupNamed("ObjectLayer");

    if(objectGroup == NULL){
        CCLOG("tile map has no objects object layer");
        return false;
    }
    //https://stackoverflow.com/questions/32981246/cocos2d-error-no-suitable-conversion-function-from-cocos2dvaluemap-to-cocos2d
    auto spawnPoints = objectGroup->objectNamed("SpawnPoint");

    int x = spawnPoints.at("x").asInt();
    int y = spawnPoints.at("y").asInt();

    initalizePhysics();
    _player = Sprite::create("monkey.png");
    _player->setPosition(ccp(x,y));

    auto circle = PhysicsBody::createBox(_player -> getContentSize());
    circle->setContactTestBitmask(true);
    circle->setDynamic(true);
    _player->setPhysicsBody(circle);

    _player->getPhysicsBody()->setCategoryBitmask((int)PhysicsCategory::Soldier);
    //_player->getPhysicsBody()->setCollisionBitmask((int)PhysicsCategory::None);
    //_player->getPhysicsBody()->setContactTestBitmask((int)PhysicsCategory::Obstacles);

    this->addChild(_player);



    // Touch init, add boulders and event listeners
    initTouch();
    addEnemy(0.0);
    //schedule(schedule_selector(HelloWorld::throwProjectile), 2);
    schedule(schedule_selector(HelloWorld::throwProjectileRandom), 2);

    this->setViewPointCenter(_player->getPosition());
    schedule(schedule_selector(HelloWorld::update));

    return true;
}

void HelloWorld::update(float dt)
{
    this->setViewPointCenter(_player->getPosition());
}

void HelloWorld::addEnemy(float dt) {
    CCTMXObjectGroup * objectGroup = _tileMap->objectGroupNamed("ObjectLayer");

    {
        auto enemyPoints = objectGroup->objectNamed("EnemyPoint4");
        //auto enemyPoints = objectGroup->objectNamed(enemy);

        int x = enemyPoints.at("x").asInt();
        int y = enemyPoints.at("y").asInt();

        _enemy4 = Sprite::create("Player.png");
        _enemy4->setPosition(ccp(x, y));
        this->addChild(_enemy4);

    }

    {
        auto enemyPoints = objectGroup->objectNamed("EnemyPoint3");
        //auto enemyPoints = objectGroup->objectNamed(enemy);

        int x = enemyPoints.at("x").asInt();
        int y = enemyPoints.at("y").asInt();

        _enemy3 = Sprite::create("Player.png");
        _enemy3->setPosition(ccp(x, y));
        this->addChild(_enemy3);

    }

    {
        auto enemyPoints = objectGroup->objectNamed("EnemyPoint2");
        //auto enemyPoints = objectGroup->objectNamed(enemy);

        int x = enemyPoints.at("x").asInt();
        int y = enemyPoints.at("y").asInt();

        _enemy2 = Sprite::create("Player.png");
        _enemy2->setPosition(ccp(x, y));
        this->addChild(_enemy2);

    }

    {
        auto enemyPoints = objectGroup->objectNamed("EnemyPoint1");
        //auto enemyPoints = objectGroup->objectNamed(enemy);

        int x = enemyPoints.at("x").asInt();
        int y = enemyPoints.at("y").asInt();

        _enemy1 = Sprite::create("Player.png");
        _enemy1->setPosition(ccp(x, y));
        this->addChild(_enemy1);

    }
}

void HelloWorld::throwProjectileRandom(float dt)
{
    //Determine which enemy is closest to soldier out of all
    Sprite *enemy[4] = {_enemy4, _enemy3, _enemy2, _enemy1};
    Sprite *closestEnemy = enemy[random(0,3)];


    auto projectile = Sprite::create("bomba.png");
    projectile->setPosition(closestEnemy->getPosition());
    this->addChild(projectile);

    int realX;
    auto diff =  _player->getPosition() - closestEnemy->getPosition();
    if (diff.x > 0)
    {
        realX = (_tileMap->getMapSize().width * _tileMap->getTileSize().width) +
                (projectile->getContentSize().width / 2);
    }
    else {
        realX = -(_tileMap->getMapSize().width * _tileMap->getTileSize().width) -
                (projectile->getContentSize().width / 2);
    }
    float ratio = (float)diff.y / (float)diff.x;
    int realY = ((realX - projectile->getPosition().x) * ratio) +projectile->getPosition().y;
    auto realDest = Point(realX, realY);
    int offRealX = realX - projectile->getPosition().x;
    int offRealY = realY - projectile->getPosition().y;
    float length = sqrtf((offRealX*offRealX) + (offRealY*offRealY));
    float velocity = 480 / 1; // 480pixels/1sec
    float realMoveDuration = length / velocity;

    auto actionMoveDone =CallFuncN::create(CC_CALLBACK_1(HelloWorld::projectileMoveFinished, this));
    projectile->runAction(Sequence::create(MoveTo::create(realMoveDuration,realDest), actionMoveDone, NULL));
    //auto circle = PhysicsBody::createBox(_player -> getContentSize());
    //circle->setContactTestBitmask(true);
    //circle->setDynamic(true);
    //_player->setPhysicsBody(circle);


}

// The enemy closest to soldier will throw projectile.
void HelloWorld::throwProjectile(float dt)
{
    //Determine which enemy is closest to soldier out of all
    Sprite *closestEnemy;

    auto closest = abs(-_player->getPosition().x + _enemy4->getPosition().x);
    auto temp = abs(-_player->getPosition().x + _enemy3->getPosition().x);

    if (temp < closest)
    {
        closest = temp;
        closestEnemy = _enemy3;
    }
    else {
        closestEnemy = _enemy4;
    }

    temp = abs(- _player->getPosition().x + _enemy2->getPosition().x);
    if (temp < closest)
    {
        closest = temp;
        closestEnemy = _enemy2;
    }

    temp = abs(- _player->getPosition().x + _enemy1->getPosition().x);
    if (temp < closest)
    {
        closest = temp;
        closestEnemy = _enemy1;
    }


    auto projectile = Sprite::create("bomba.png");
    projectile->setPosition(closestEnemy->getPosition());
    this->addChild(projectile);

    int realX;
    auto diff =  _player->getPosition() - closestEnemy->getPosition();
    if (diff.x > 0)
    {
        realX = (_tileMap->getMapSize().width * _tileMap->getTileSize().width) +
                (projectile->getContentSize().width / 2);
    }
    else {
        realX = -(_tileMap->getMapSize().width * _tileMap->getTileSize().width) -
                (projectile->getContentSize().width / 2);
    }
    float ratio = (float)diff.y / (float)diff.x;
    int realY = ((realX - projectile->getPosition().x) * ratio) +projectile->getPosition().y;
    auto realDest = Point(realX, realY);
    int offRealX = realX - projectile->getPosition().x;
    int offRealY = realY - projectile->getPosition().y;
    float length = sqrtf((offRealX*offRealX) + (offRealY*offRealY));
    float velocity = 480 / 1; // 480pixels/1sec
    float realMoveDuration = length / velocity;

    auto actionMoveDone =CallFuncN::create(CC_CALLBACK_1(HelloWorld::projectileMoveFinished, this));
    projectile->runAction(Sequence::create(MoveTo::create(realMoveDuration,realDest), actionMoveDone, NULL));
    //auto circle = PhysicsBody::createBox(_player -> getContentSize());
    //circle->setContactTestBitmask(true);
    //circle->setDynamic(true);
    //_player->setPhysicsBody(circle);


}

void HelloWorld::projectileMoveFinished(Object *pSender)
{
    Sprite *sprite = (Sprite *)pSender;
    this->removeChild(sprite);
    //this->removeChild(_enemy2);
}


void HelloWorld::initalizePhysics() {

    TMXObjectGroup *collision_rectangles_object_layer = _tileMap->getObjectGroup("GroundObject");
    ValueVector &rectangles_array = collision_rectangles_object_layer->getObjects();
    for (cocos2d::Value &rectangle_box : rectangles_array) {
        cocos2d::ValueMap rectangle_box_properties = rectangle_box.asValueMap();


        // First we have to create a Node. Without it we cannot use the physics body.
        Node *node = Node::create();

        // Now create and attach the PhysicsBody to the node
        PhysicsBody *box = PhysicsBody::createEdgeBox(
                Size(rectangle_box_properties["width"].asInt(),
                     rectangle_box_properties["height"].asInt()), PhysicsMaterial(1.0f, 0.0f, 1.0f));
        box->setGravityEnable(false);
        box->setDynamic(false);
        node->setPhysicsBody(box);


        node->setPosition(Vec2(rectangle_box_properties["x"].asInt() +
                               rectangle_box_properties["width"].asInt() / 2,
                               rectangle_box_properties["y"].asInt() +
                               rectangle_box_properties["height"].asInt() / 2));


        this->addChild(node, 20);
    }
}
void HelloWorld::setViewPointCenter(CCPoint position) {

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();

    int x = MAX(position.x, winSize.width/2);
    int y = MAX(position.y, winSize.height/2);
    x = MIN(x, (_tileMap->getMapSize().width * this->_tileMap->getTileSize().width) - winSize.width / 2);
    y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height/2);
    CCPoint actualPosition = ccp(x, y);

    CCPoint centerOfView = ccp(winSize.width/2, winSize.height/2);
    CCPoint viewPoint = ccpSub(centerOfView, actualPosition);
    this->setPosition(viewPoint);
}

void HelloWorld::initTouch()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener -> onTouchBegan = [] (Touch* touch, Event* event) { return true;};
    //listener -> onTouchEnded = CC_CALLBACK_2(HelloWorld::moveSoldier, this);
    listener -> onTouchMoved = CC_CALLBACK_2(HelloWorld::moveSoldier, this);
    //listener -> onTouchEnded = [=] (Touch* touch, Event* event) {};
    _eventDispatcher -> addEventListenerWithSceneGraphPriority(listener, this);
}

void HelloWorld::setPlayerPosition(CCPoint position) {

    _player->setPosition(position);
}


void HelloWorld::moveSoldier(cocos2d::Touch *touch, cocos2d::Event *event)
{
    auto actionTo1 = RotateTo::create(0, 0, 180);
    auto actionTo2 = RotateTo::create(0, 0, 0);
    auto touchLocation = touch->getLocation();

    touchLocation = this->convertToNodeSpace(touchLocation);

    auto playerPos = _player->getPosition();
    auto diff = touchLocation - playerPos;
    if (abs(diff.x) > abs(diff.y)) {
        if (diff.x > 0) {
            playerPos.x += _tileMap->getTileSize().width / 2;
            _player->runAction(actionTo2);
        }
        else {
            playerPos.x -= _tileMap->getTileSize().width / 2;
            _player->runAction(actionTo1);
        }
    }
    else {
        if (diff.y > 0) {
            playerPos.y += _tileMap->getTileSize().height / 2;
        }
        else {
            playerPos.y -= _tileMap->getTileSize().height / 2;
        }
    }

    if (playerPos.x <= (_tileMap->getMapSize().width * _tileMap->getMapSize().width) &&
        playerPos.y <= (_tileMap->getMapSize().height * _tileMap->getMapSize().height) &&
        playerPos.y >= 0 &&
        playerPos.x >= 0)
    {
        this->setPlayerPosition(playerPos);

    }

    this->setViewPointCenter(_player->getPosition());
}

CCPoint HelloWorld::tileCoordForPosition(CCPoint position)
{
    int x = position.x / _tileMap->getTileSize().width;
    int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - position.y) / _tileMap->getTileSize().height;
    return ccp(x, y);
}