#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "PlayfieldView.h"
#include "StackView.h"
#include <functional>

/**
 * GameView
 * 游戏主视图，包含主牌区和底牌区
 */
class GameView : public cocos2d::Scene {
public:
    static GameView* create();
    virtual bool init();
    
    // 设置卡牌点击回调
    void setOnCardClickCallback(const std::function<void(int)>& callback);
    
    // 设置回退按钮点击回调
    void setOnUndoClickCallback(const std::function<void()>& callback);
    
    // 显示/隐藏回退按钮
    void showUndoButton(bool visible);
    
    // 获取视图
    PlayfieldView* getPlayfieldView() { return _playfieldView; }
    StackView* getStackView() { return _stackView; }

private:
    PlayfieldView* _playfieldView;  // 主牌区
    StackView* _stackView;          // 底牌堆
    cocos2d::ui::Button* _undoButton; // 回退按钮
    std::function<void(int)> _onCardClickCallback;
    std::function<void()> _onUndoClickCallback;
    
    void createBackground();
    void createUndoButton();
};

