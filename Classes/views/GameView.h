#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "PlayfieldView.h"
#include "StackView.h"
#include <functional>

/**
 * @brief GameView - 游戏主视图类
 * 
 * 这是游戏的主视图，负责显示整个游戏界面。
 * 它包含主牌区视图（PlayfieldView）和底牌堆视图（StackView），
 * 以及回退按钮等UI元素。
 * 
 * 职责：
 * - 创建和管理游戏的所有UI元素
 * - 协调主牌区和底牌堆的显示
 * - 处理用户交互事件（卡牌点击、回退按钮点击）
 * - 管理回退按钮的显示/隐藏状态
 * 
 * 使用场景：
 * - 作为游戏的主界面显示
 * - 接收用户输入并传递给控制器
 * - 根据游戏状态更新UI显示
 * 
 * 架构说明：
 * - 继承自cocos2d::Scene，可以作为场景使用
 * - 包含PlayfieldView（主牌区）和StackView（底牌堆）
 * - 通过回调函数与GameController通信
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
    PlayfieldView* _playfieldView;              // 主牌区视图
    StackView* _stackView;                      // 底牌堆视图
    cocos2d::ui::Button* _undoButton;          // 回退按钮
    std::function<void(int)> _onCardClickCallback;   // 卡牌点击回调函数
    std::function<void()> _onUndoClickCallback;      // 回退按钮点击回调函数
    
    void createBackground();
    void createUndoButton();
};

