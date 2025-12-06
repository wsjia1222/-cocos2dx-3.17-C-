#pragma once
#include "views/GameView.h"
#include "models/GameModel.h"
#include "managers/UndoManager.h"
#include "models/UndoModel.h"
#include <functional>

/**
 * GameController
 * 游戏控制器，处理游戏逻辑和用户交互
 */
class GameController {
public:
    GameController(GameView* view);
    ~GameController();
    
    // 开始游戏（使用简单配置）
    void startGame();
    
    // 处理卡牌点击
    void onCardClicked(int cardId);
    
    // 处理回退点击
    void onUndoClicked();
    
    // 检查是否可以匹配（点数差1）
    bool canMatch(int card1Face, int card2Face) const;

private:
    GameView* _gameView;
    GameModel _gameModel;
    UndoManager _undoManager;
    
    // 更新视图
    void updateView();
    
    // 处理手牌区翻牌替换
    void handleStackCardReplace(int clickedCardId);
    
    // 处理桌面牌匹配
    void handlePlayfieldCardMatch(int playfieldCardId);
};

