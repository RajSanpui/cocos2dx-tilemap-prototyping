

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

//class HelloWorld : public cocos2d::Scene
class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    cocos2d::Sprite *_player, *_enemy;
    cocos2d::CCTMXTiledMap *_tileMap;


    void setViewPointCenter(cocos2d::CCPoint position);
    void setPlayerPosition(cocos2d::CCPoint position);
    cocos2d::CCPoint tileCoordForPosition(cocos2d::CCPoint position);
    void update(float dt);
    void initalizePhysics();
    void jumpSprite(cocos2d::Sprite *mysprite);
    void addEnemy(float);
    //void initPhysics(void);
    void projectileMoveFinished(cocos2d::Object *pSender);

    cocos2d::CCTMXLayer *_background;
    cocos2d::CCTMXLayer *_meta;
    cocos2d::CCTMXLayer *_foreground;
    cocos2d::CCTMXLayer *_obstacles;

    //bool onContactBegan(cocos2d::PhysicsContact &contact);
    void initTouch();
    void moveSoldier(cocos2d::Touch* toque, cocos2d::Event* event);
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
};

#endif // __HELLOWORLD_SCENE_H__