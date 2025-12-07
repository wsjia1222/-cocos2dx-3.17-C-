#pragma once
#include "cocos2d.h"
#include "CardView.h"
#include <vector>
#include <functional>

/**
 * @brief PlayfieldView - 主牌区视图类
 * 
 * 这个类负责显示主牌区（桌面上的卡牌区域）的所有卡牌。
 * 它是游戏界面的重要组成部分，玩家可以点击主牌区的卡牌与底牌匹配。
 * 
 * 职责：
 * - 管理主牌区的所有卡牌视图
 * - 处理卡牌的添加和移除
 * - 处理卡牌点击事件并传递给外部
 * - 提供根据ID查找卡牌的功能
 * 
 * 使用场景：
 * - 显示游戏开始时桌面上的卡牌
 * - 显示玩家可以点击匹配的卡牌
 * - 在卡牌匹配后移除对应的卡牌
 * 
 * 架构说明：
 * - 继承自cocos2d::Node，可以添加到场景中
 * - 通过回调函数与GameController通信
 * - 使用vector存储所有卡牌视图的指针
 */
class PlayfieldView : public cocos2d::Node {
public:
    static PlayfieldView* create();
    virtual bool init();
    
    // 添加卡牌
    void addCard(CardView* cardView);
    
    // 移除卡牌
    void removeCard(CardView* cardView);
    
    // 设置点击回调
    void setOnCardClickCallback(const std::function<void(int)>& callback);
    
    // 获取所有卡牌
    const std::vector<CardView*>& getCards() const { return _cards; }
    
    // 根据ID查找卡牌
    CardView* findCardById(int cardId) const;
    
    static const int kPlayfieldWidth = 1080;   // 主牌区宽度
    static const int kPlayfieldHeight = 1500;   // 主牌区高度

private:
    std::vector<CardView*> _cards;
    std::function<void(int)> _onCardClickCallback;
};

