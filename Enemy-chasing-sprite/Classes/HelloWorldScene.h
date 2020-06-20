

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
//#include <CCVector.h>
#include <vector>
using namespace std;
//using NS_CC_BEGIN;

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    cocos2d::CCTMXTiledMap *_tileMap;
    cocos2d::CCTMXLayer *_background;
    cocos2d::CCTMXLayer *_meta;
    cocos2d::CCTMXLayer *_foreground;
    cocos2d::CCTMXLayer *_obstacles;

    cocos2d::Sprite *_player;
    void setViewPointCenter(cocos2d::CCPoint position);
    void animateEnemy(cocos2d::Sprite *enemy);
    void addEnemyAtPos(cocos2d::Point pos);
    void enemyMoveFinished(cocos2d::Object *pSender);

    vector<cocos2d::Sprite*> _enemies;
    //cocos2d::Vector _enemies;
    void setPlayerPosition(cocos2d::CCPoint position);
    void moveSoldier(cocos2d::Touch *touch, cocos2d::Event *event);
    void initTouch();
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
};

#endif // __HELLOWORLD_SCENE_H__
