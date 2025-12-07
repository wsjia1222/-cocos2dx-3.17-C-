#pragma once
#include "cocos2d.h"
#include "views/GameView.h"
#include "controllers/GameController.h"

/**
 * @brief GameScene - 游戏主场景类
 * 
 * 这是游戏的主场景，负责初始化和协调游戏的核心组件。
 * 采用MVC架构模式，将视图（View）和控制器（Controller）分离。
 * 
 * 职责：
 * - 创建和管理游戏视图（GameView）
 * - 创建和管理游戏控制器（GameController）
 * - 协调视图和控制器之间的交互
 * 
 * 使用场景：
 * - 游戏启动时作为第一个场景
 * - 负责初始化整个游戏系统
 * 
 * 架构说明：
 * - GameView：负责显示游戏界面（卡牌、按钮等）
 * - GameController：负责处理游戏逻辑（卡牌匹配、回退等）
 * - GameModel：负责存储游戏数据（由GameController管理）
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

