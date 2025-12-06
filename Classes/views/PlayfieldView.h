#pragma once
#include "cocos2d.h"
#include "CardView.h"
#include <vector>
#include <functional>

/**
 * PlayfieldView
 * 主牌区视图，显示桌面上的卡牌
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
    
    static const int PLAYFIELD_WIDTH = 1080;
    static const int PLAYFIELD_HEIGHT = 1500;

private:
    std::vector<CardView*> _cards;
    std::function<void(int)> _onCardClickCallback;
};

