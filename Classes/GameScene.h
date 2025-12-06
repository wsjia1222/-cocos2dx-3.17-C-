#pragma once
#include "cocos2d.h"
#include "views/GameView.h"
#include "controllers/GameController.h"

/**
 * GameScene
 * 游戏场景，初始化游戏视图和控制器
 */
class GameScene : public cocos2d::Scene {
public:
    static GameScene* create();
    virtual bool init();
    virtual ~GameScene();

private:
    GameView* _gameView;
    GameController* _gameController;
};

