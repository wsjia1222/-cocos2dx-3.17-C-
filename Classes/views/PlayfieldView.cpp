#include "PlayfieldView.h"
#include "cocos2d.h"

USING_NS_CC;  // 使用cocos2d命名空间

/**
 * 创建PlayfieldView对象（静态工厂方法）
 * 
 * @return 创建成功的PlayfieldView指针，失败返回nullptr
 */
PlayfieldView* PlayfieldView::create() {
    PlayfieldView* ret = new (std::nothrow) PlayfieldView();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

/**
 * 初始化主牌区视图
 * 
 * 设置主牌区的大小（宽度和高度）
 * 
 * @return true=初始化成功, false=初始化失败
 */
bool PlayfieldView::init() {
    // 先初始化父类Node
    if (!Node::init()) return false;
    
    // 设置主牌区的大小
    // kPlayfieldWidth = 1080（屏幕宽度）
    // kPlayfieldHeight = 1500（主牌区高度）
    this->setContentSize(Size(kPlayfieldWidth, kPlayfieldHeight));
    return true;
}

/**
 * 添加卡牌到主牌区
 * 
 * 当需要显示一张新卡牌时，调用这个方法
 * 会将卡牌添加到主牌区的卡牌列表中，并添加到场景中显示
 * 
 * @param cardView 要添加的卡牌视图
 */
void PlayfieldView::addCard(CardView* cardView) {
    if (!cardView) return;  // 如果卡牌为空，直接返回
    
    // 将卡牌添加到卡牌列表（vector）
    _cards.push_back(cardView);
    
    // 将卡牌添加到场景中，这样卡牌才能显示出来
    this->addChild(cardView);
    
    // 设置卡牌点击回调
    // 当玩家点击这张卡牌时，会调用_onCardClickCallback回调函数
    cardView->setOnClickCallback([this](int cardId) {
        if (_onCardClickCallback) {
            _onCardClickCallback(cardId);  // 调用回调，传入卡牌ID
        }
    });
}

/**
 * 从主牌区移除卡牌
 * 
 * 当卡牌被匹配或需要隐藏时，调用这个方法
 * 会从卡牌列表中移除，并从场景中删除
 * 
 * @param cardView 要移除的卡牌视图
 */
void PlayfieldView::removeCard(CardView* cardView) {
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
 * 当玩家点击主牌区的任何卡牌时，会调用这个回调函数
 * 
 * @param callback 回调函数，参数是卡牌的ID
 */
void PlayfieldView::setOnCardClickCallback(const std::function<void(int)>& callback) {
    _onCardClickCallback = callback;
}

/**
 * 根据ID查找卡牌
 * 
 * 在主牌区的所有卡牌中查找指定ID的卡牌
 * 
 * @param cardId 要查找的卡牌ID
 * @return 找到的卡牌视图，如果没找到返回nullptr
 */
CardView* PlayfieldView::findCardById(int cardId) const {
    // 遍历所有卡牌
    for (auto* card : _cards) {
        // 如果卡牌的ID匹配，返回这张卡牌
        if (card->getCardId() == cardId) {
            return card;
        }
    }
    return nullptr;  // 没找到，返回nullptr
}

