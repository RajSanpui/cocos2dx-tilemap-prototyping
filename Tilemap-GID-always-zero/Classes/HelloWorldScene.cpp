
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "../cocos2d/cocos/deprecated/CCDeprecated.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
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
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("collect-coin.ogg");


    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    _tileMap = new CCTMXTiledMap();
    // _tileMap->initWithTMXFile("TileMap.tmx");
    _tileMap->initWithTMXFile("TileMap1.tmx");

    _background = _tileMap->layerNamed("Background");
    _foreground = _tileMap->layerNamed("Foreground");

    _meta = _tileMap->layerNamed("Meta");
    _meta->setVisible(true);
    this->addChild(_tileMap);

    //auto groundbody = PhysicsBody::createBox(_meta->getContentSize(),PhysicsMaterial(1.0f, 0.0f, 1.0f));
    //groundbody->setGravityEnable(false);
    //groundbody->setDynamic(false);
    //_meta->setPhysicsBody(groundbody);

    CCTMXObjectGroup *objectGroup = _tileMap->objectGroupNamed("Objects");

    if(objectGroup == NULL){
        CCLOG("tile map has no objects object layer");
        return false;
    }
    //https://stackoverflow.com/questions/32981246/cocos2d-error-no-suitable-conversion-function-from-cocos2dvaluemap-to-cocos2d
    auto spawnPoints = objectGroup->objectNamed("SpawnPoint");

    //int x = spawnPoint->valueForKey("x").asInt();
    //int y = spawnPoint->valueForKey("y").asInt();

    int x = spawnPoints.at("x").asInt();
    int y = spawnPoints.at("y").asInt();

    _player = Sprite::create("Player.png");
    _player->setPosition(ccp(x,y));

    auto circle = PhysicsBody::createBox(_player->getContentSize());

    this->addChild(_player);
    this->setViewPointCenter(_player->getPosition());

    // Touch init, add boulders and event listeners
    initTouch();

    //auto contactListener = EventListenerPhysicsContact::create();
    //contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegan, this);
    //this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

    return true;
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
    listener -> onTouchEnded = CC_CALLBACK_2(HelloWorld::moveSoldier, this);
    //listener -> onTouchEnded = [=] (Touch* touch, Event* event) {};
    _eventDispatcher -> addEventListenerWithSceneGraphPriority(listener, this);
}

void HelloWorld::setPlayerPosition(CCPoint position) {
    Point tileCoord = this->tileCoordForPosition(position);
    int tileGid = _meta->tileGIDAt(tileCoord);
    int tileGid1 = _meta->tileGIDAt(CCPoint(21,10)); // Co-ordinate of meta tile frame - just checking
    CCLOG("*******************  tileGid = %d *****************",tileGid);
    CCLOG("*******************  tileGid = %d *****************",tileGid1);
    //CCLOG("GID:, Properties:%s", _meta->getPropertiesForGID(tileGid).asValueMap()["name"].asString().c_str());
    if (tileGid) // Doesnt enter as GID always zero even if it reaches meta tileframe
    {
        auto properties = _tileMap->getPropertiesForGID(tileGid).asValueMap();
        if (!properties.empty()) {
        //CCString *collision = new CCString();
        auto collision = properties["Collidable"].asString();
        if ("True" == collision) {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("collect-coin.ogg");
            return;
        }
        //auto *collectible = new CCString();
        auto collectible = properties["Collectable"].asString();
        if ("True" == collectible) {
            _meta->removeTileAt(tileCoord);
            _foreground->removeTileAt(tileCoord);
            //CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("collect-coin.ogg");
        }
    }
}
    _player->setPosition(position);
}

void HelloWorld::moveSoldier(cocos2d::Touch *touch, cocos2d::Event *event)
{

    CCPoint touchLocation = touch->getLocationInView();
    touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);
    touchLocation = this->convertToNodeSpace(touchLocation);

    CCPoint playerPos = _player->getPosition();
    CCPoint diff = ccpSub(touchLocation, playerPos);

    if ( abs(diff.x) > abs(diff.y) ) {
        if (diff.x > 0) {
            playerPos.x += _tileMap->getTileSize().width;
            //   _player->runAction(actionTo2);
        } else {
            playerPos.x -= _tileMap->getTileSize().width;
            //   _player->runAction(actionTo1);
        }
    } else {
        if (diff.y > 0) {
            playerPos.y += _tileMap->getTileSize().height;
        } else {
            playerPos.y -= _tileMap->getTileSize().height;
        }
    }


    // safety check on the bounds of the map
    if (playerPos.x <= (_tileMap->getMapSize().width * _tileMap->getTileSize().width) &&
        playerPos.y <= (_tileMap->getMapSize().height * _tileMap->getTileSize().height) &&
        playerPos.y >= 0 &&
        playerPos.x >= 0 )
    {
        this->setPlayerPosition(playerPos);
    }

    this->setPlayerPosition(playerPos);
    this->setViewPointCenter(_player->getPosition());
}

CCPoint HelloWorld::tileCoordForPosition(CCPoint position)
{
int x = position.x / _tileMap->getTileSize().width;
int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - position.y) / _tileMap->getTileSize().height;
return ccp(x, y);
}