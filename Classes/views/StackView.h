#pragma once
#include "cocos2d.h"
#include "CardView.h"
#include <vector>
#include <functional>

/**
 * @brief StackView - 底牌堆视图类
 * 
 * 这个类负责显示底牌堆（手牌区）的所有卡牌。
 * 底牌堆包括备用底牌和当前使用的顶部底牌，玩家可以点击备用底牌替换顶部底牌。
 * 
 * 职责：
 * - 管理底牌堆的所有卡牌视图
 * - 处理卡牌的添加和移除
 * - 自动布局卡牌（备用底牌在左边，主底牌在右边）
 * - 处理卡牌点击事件并传递给外部
 * - 提供获取顶部卡牌的功能
 * 
 * 使用场景：
 * - 显示游戏开始时底牌堆的卡牌
 * - 显示玩家可以点击替换的备用底牌
 * - 显示当前用于匹配的顶部底牌
 * 
 * 架构说明：
 * - 继承自cocos2d::Node，可以添加到场景中
 * - 通过回调函数与GameController通信
 * - 使用vector存储所有卡牌视图的指针，最后一张是顶部卡牌
 * - 自动管理卡牌的布局和层级
 */
class StackView : public cocos2d::Node {
public:
    static StackView* create();
    virtual bool init();
    
    // 添加卡牌
    void addCard(CardView* cardView);
    
    // 移除卡牌
    void removeCard(CardView* cardView);
    
    // 设置点击回调
    void setOnCardClickCallback(const std::function<void(int)>& callback);
    
    // 获取顶部卡牌
    CardView* getTopCard() const;
    
    // 获取所有卡牌
    const std::vector<CardView*>& getCards() const { return _cards; }
    std::vector<CardView*>& getCards() { return _cards; }
    
    // 根据ID查找卡牌
    CardView* findCardById(int cardId) const;
    
    // 布局卡牌
    void layoutCards();
    
    // 根据点击位置找到被点击的卡牌（从最上层开始检查）
    CardView* findCardAtPosition(const cocos2d::Vec2& worldPos) const;
    
    static const int kStackWidth = 1080;   // 底牌堆宽度
    static const int kStackHeight = 580;   // 底牌堆高度

private:
    std::vector<CardView*> _cards;
    std::function<void(int)> _onCardClickCallback;
    
    // 顶部卡牌位置
    cocos2d::Vec2 getTopCardPosition() const;
};

