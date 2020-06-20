

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label


    // add "HelloWorld" splash screen"

    _tileMap = new CCTMXTiledMap();
    // _tileMap->initWithTMXFile("TileMap.tmx");
    _tileMap->initWithTMXFile("Multi-Enemy-Chase.tmx");
    //_tileMap->setScale(0.8f);

    _background = _tileMap->layerNamed("Background");
    _foreground = _tileMap->layerNamed("Foreground");

    this->addChild(_tileMap);

    // Add Soldier sprite
    CCTMXObjectGroup *objectGroup = _tileMap->objectGroupNamed("ObjectLayer");
    auto spawnPoints = objectGroup->objectNamed("SpawnPoint");

    int x = spawnPoints.at("x").asInt();
    int y = spawnPoints.at("y").asInt();

    //initalizePhysics();
    _player = Sprite::create("monkey.png");
    _player->setPosition(ccp(x,y));
    this->addChild(_player);

    this->setViewPointCenter(_player->getPosition());
    initTouch();

    // Add enemies
    for (auto& eSpawnPoint: objectGroup->getObjects()){
        ValueMap& dict = eSpawnPoint.asValueMap();
        if(dict["Enemy"].asInt() == 1){
            x = dict["x"].asInt();
            y = dict["y"].asInt();
            this->addEnemyAtPos(Point(x, y));
        }
    }


    return true;
}

void HelloWorld::addEnemyAtPos(Point pos)
{
    auto enemy = Sprite::create("Player.png");
    enemy->setPosition(pos);
    //enemy->setScale(0.5);
    this->animateEnemy(enemy);
    this->addChild(enemy);

    _enemies.push_back(enemy);
}

void HelloWorld::animateEnemy(Sprite *enemy)
{
    float actualDuration = 3.0f;
    auto position = (_player->getPosition() - enemy->getPosition());
    auto actionMove = MoveBy::create(actualDuration, position);
    auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyMoveFinished, this));
    enemy->runAction(Sequence::create(actionMove, actionMoveDone, NULL));
}

void HelloWorld::enemyMoveFinished(Object *pSender)
{
    Sprite *enemy = (Sprite *)pSender;
    this->animateEnemy(enemy);
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

void HelloWorld::setPlayerPosition(CCPoint position) {

    _player->setPosition(position);
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

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
