#include "StackView.h"
#include "cocos2d.h"
#include <algorithm>

USING_NS_CC;  // 使用cocos2d命名空间

/**
 * 创建StackView对象（静态工厂方法）
 * 
 * @return 创建成功的StackView指针，失败返回nullptr
 */
StackView* StackView::create() {
    StackView* ret = new (std::nothrow) StackView();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

/**
 * 初始化底牌堆视图
 * 
 * 设置底牌堆的大小（宽度和高度）
 * 
 * @return true=初始化成功, false=初始化失败
 */
bool StackView::init() {
    // 先初始化父类Node
    if (!Node::init()) return false;
    
    // 设置底牌堆的大小
    // kStackWidth = 1080（屏幕宽度）
    // kStackHeight = 580（底牌堆高度）
    this->setContentSize(Size(kStackWidth, kStackHeight));
    return true;
}

/**
 * 添加卡牌到底牌堆
 * 
 * 当需要显示一张新卡牌时，调用这个方法
 * 会将卡牌添加到底牌堆的卡牌列表中，并添加到场景中显示
 * 添加后会自动重新布局所有卡牌
 * 
 * @param cardView 要添加的卡牌视图
 */
void StackView::addCard(CardView* cardView) {
    if (!cardView) return;  // 如果卡牌为空，直接返回
    
    // 将卡牌添加到卡牌列表（vector）
    // 注意：最后添加的卡牌会成为顶部卡牌（vector的最后一个元素）
    _cards.push_back(cardView);
    
    // 将卡牌添加到场景中，这样卡牌才能显示出来
    this->addChild(cardView);
    
    // 设置卡牌点击回调
    // 当玩家点击这张卡牌时，会调用_onCardClickCallback回调函数
    // 注意：这里使用统一的点击处理，实际点击事件在CardView中处理
    cardView->setOnClickCallback([this](int cardId) {
        if (_onCardClickCallback) {
            _onCardClickCallback(cardId);  // 调用回调，传入卡牌ID
        }
    });
    
    // 重新布局所有卡牌
    // 因为添加了新卡牌，需要重新计算每张卡牌的位置
    layoutCards();
}

/**
 * 从底牌堆移除卡牌
 * 
 * 当卡牌被移除或需要隐藏时，调用这个方法
 * 会从卡牌列表中移除，并从场景中删除
 * 
 * @param cardView 要移除的卡牌视图
 */
void StackView::removeCard(CardView* cardView) {
    if (!cardView) return;  // 如果卡牌为空，直接返回
    
    // 在卡牌列表中找到这张卡牌
    auto it = std::find(_cards.begin(), _cards.end(), cardView);
    if (it != _cards.end()) {
        // 从列表中移除
        _cards.erase(it);
        // 从场景中移除（这样卡牌就不会显示了）
        this->removeChild(cardView);
    }
}

/**
 * 设置卡牌点击回调函数
 * 
 * 当玩家点击底牌堆的任何卡牌时，会调用这个回调函数
 * 
 * @param callback 回调函数，参数是卡牌的ID
 */
void StackView::setOnCardClickCallback(const std::function<void(int)>& callback) {
    _onCardClickCallback = callback;
}

/**
 * 获取顶部卡牌（当前使用的底牌）
 * 
 * 顶部卡牌是底牌堆中最后一张卡牌（vector的最后一个元素）
 * 这是玩家当前可以使用的底牌，用于与主牌区的卡牌匹配
 * 
 * @return 顶部卡牌视图，如果底牌堆为空返回nullptr
 */
CardView* StackView::getTopCard() const {
    if (_cards.empty()) return nullptr;  // 如果底牌堆为空，返回nullptr
    return _cards.back();  // 返回最后一张卡牌（顶部卡牌）
}

/**
 * 根据ID查找卡牌
 * 
 * 在底牌堆的所有卡牌中查找指定ID的卡牌
 * 
 * @param cardId 要查找的卡牌ID
 * @return 找到的卡牌视图，如果没找到返回nullptr
 */
CardView* StackView::findCardById(int cardId) const {
    // 遍历所有卡牌
    for (auto* card : _cards) {
        // 如果卡牌的ID匹配，返回这张卡牌
        if (card->getCardId() == cardId) {
            return card;
        }
    }
    return nullptr;  // 没找到，返回nullptr
}

/**
 * 布局底牌堆中的所有卡牌
 * 
 * 这个方法会计算每张卡牌的位置并设置：
 * - 主底牌（最后一张，顶部卡牌）：放在右边
 * - 备用底牌（前面的卡牌）：放在左边，垂直排列，不重叠以便点击
 * 
 * 布局规则：
 * 1. 最后一张卡牌是主底牌，放在右边固定位置
 * 2. 前面的卡牌是备用底牌，放在左边，垂直排列
 * 3. 卡牌之间不重叠，方便玩家点击
 */
void StackView::layoutCards() {
    if (_cards.empty()) return;  // 如果没有卡牌，直接返回
    
    // 主底牌在右边，备用底牌在左边
    // 最后一张卡牌是主底牌（顶部），放在右边
    // 前面的卡牌是备用底牌，放在左边，不重叠以便点击
    
    float centerY = kStackHeight / 2;  // 底牌区垂直中心
    float cardWidth = 120.0f;  // 卡牌宽度（缩放后）
    float cardSpacing = 130.0f;  // 卡牌间距（未使用，但保留以备后用）
    
    // 主底牌位置（右边，距离右边缘200像素）
    Vec2 mainCardPos = Vec2(kStackWidth - 200, centerY);
    
    // 备用底牌位置（左边），垂直排列
    float leftStartX = 200.0f;  // 左边起始位置（距离左边缘200像素）
    float cardHeight = 170.0f;  // 卡牌高度（缩放后）
    float verticalSpacing = 120.0f;  // 垂直间距，确保卡牌不重叠但紧凑
    
    // 计算备用底牌的总高度
    // 备用底牌数量 = 总卡牌数 - 1（减去主底牌）
    int reserveCardCount = (int)_cards.size() - 1;
    // 总高度 = (数量-1) * 间距 + 一张卡牌的高度
    float totalHeight = reserveCardCount > 0 ? (float)(reserveCardCount - 1) * verticalSpacing + cardHeight : 0;
    // 起始Y坐标：从中心向上偏移，让卡牌垂直居中
    float startY = centerY - totalHeight / 2 + cardHeight / 2;
    
    // 遍历所有卡牌，设置每张卡牌的位置
    for (size_t i = 0; i < _cards.size(); i++) {
        Vec2 pos;
        
        if (i == _cards.size() - 1) {
            // 最后一张是主底牌，放在右边固定位置
            pos = mainCardPos;
        } else {
            // 备用底牌放在左边，垂直排列
            // 每张卡牌的Y坐标 = 起始Y + 索引 * 垂直间距
            float offsetY = verticalSpacing * (float)i;
            pos = Vec2(leftStartX, startY + offsetY);
        }
        
        // 设置卡牌位置
        _cards[i]->setPosition(pos);
        
        // 设置层级（z-order），确保主底牌在最上层（显示在最前面）
        // 索引越大，层级越高，显示越靠前
        _cards[i]->setLocalZOrder((int)i);
    }
}

/**
 * 获取顶部卡牌的位置
 * 
 * 返回顶部卡牌应该显示的位置（底牌区中心）
 * 
 * @return 顶部卡牌的位置坐标
 */
cocos2d::Vec2 StackView::getTopCardPosition() const {
    // 顶部卡牌位置在底牌区中心
    return cocos2d::Vec2(kStackWidth / 2, kStackHeight / 2);
}

/**
 * 根据世界坐标查找被点击的卡牌
 * 
 * 当玩家点击屏幕时，传入世界坐标（屏幕坐标）
 * 这个方法会检查哪张卡牌被点击了
 * 从最上层（最后添加的卡牌）开始检查，这样点击重叠的卡牌时会选中最上层的
 * 
 * @param worldPos 世界坐标（屏幕坐标）
 * @return 被点击的卡牌视图，如果没点击到任何卡牌返回nullptr
 */
CardView* StackView::findCardAtPosition(const cocos2d::Vec2& worldPos) const {
    // 从最上层（最后添加的卡牌）开始检查
    // rbegin()和rend()是反向迭代器，从后往前遍历
    for (auto it = _cards.rbegin(); it != _cards.rend(); ++it) {
        CardView* card = *it;
        // 如果卡牌不存在或不可见，跳过
        if (!card || !card->isVisible()) continue;
        
        // 将世界坐标转换为卡牌的本地坐标
        // 因为卡牌的位置是相对于父节点的，需要转换坐标系统
        Vec2 locationInCard = card->convertToNodeSpace(worldPos);
        
        // 获取卡牌的实际大小（考虑缩放）
        // 注意：这里简化处理，假设卡牌大小固定
        // 实际应该从CardView获取真实大小
        float cardWidth = 120.0f;  // 卡牌宽度（缩放后）
        float cardHeight = 170.0f;  // 卡牌高度（缩放后）
        // 创建卡牌的矩形区域（以卡牌中心为原点）
        Rect cardRect(-cardWidth / 2, -cardHeight / 2, cardWidth, cardHeight);
        
        // 检查点击位置是否在卡牌矩形内
        if (cardRect.containsPoint(locationInCard)) {
            return card;  // 找到了，返回这张卡牌
        }
    }
    return nullptr;  // 没找到，返回nullptr
}

