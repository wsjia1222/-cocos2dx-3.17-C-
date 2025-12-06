#pragma once
#include "cocos2d.h"
#include "CardView.h"
#include <vector>
#include <functional>

/**
 * StackView
 * 底牌堆视图（手牌区），显示底牌堆的卡牌
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
    
    static const int STACK_WIDTH = 1080;
    static const int STACK_HEIGHT = 580;

private:
    std::vector<CardView*> _cards;
    std::function<void(int)> _onCardClickCallback;
    
    // 顶部卡牌位置
    cocos2d::Vec2 getTopCardPosition() const;
};

