#include "CardView.h"
#include "cocos2d.h"

USING_NS_CC;

/**
 * 创建卡牌视图
 * 这是cocos2d-x的标准创建模式：
 * 1. 用new创建对象
 * 2. 调用init初始化
 * 3. 如果成功，调用autorelease（自动释放，由cocos2d管理内存）
 * 4. 如果失败，手动删除
 */
CardView* CardView::create(int cardFace, int cardSuit, bool isFaceUp) {
    CardView* ret = new (std::nothrow) CardView();  // new创建对象，nothrow表示失败返回nullptr而不是抛异常
    if (ret && ret->init(cardFace, cardSuit, isFaceUp)) {  // 如果创建成功且初始化成功
        ret->autorelease();  // 交给cocos2d管理内存，不需要手动delete
        return ret;
    }
    CC_SAFE_DELETE(ret);  // 如果失败，安全删除对象
    return nullptr;
}

bool CardView::init(int cardFace, int cardSuit, bool isFaceUp) {
    if (!Node::init()) return false;
    
    _cardFace = cardFace;
    _cardSuit = cardSuit;
    _isFaceUp = isFaceUp;
    _cardId = -1;
    
    // 卡牌底图
    _bgSprite = Sprite::create("res1/card_general.png");
    if (!_bgSprite) {
        CCLOG("无法加载 card_general.png");
        return false;
    }
    this->addChild(_bgSprite);
    
    // 获取卡牌尺寸（未缩放前）
    Size cardSize = _bgSprite->getContentSize();
    
    // 左上角：小数字
    std::string smallNumberPath = getSmallNumberImagePath(cardFace, cardSuit);
    _smallNumberSprite = Sprite::create(smallNumberPath);
    if (_smallNumberSprite) {
        // 左上角位置：距离左边和顶部一定距离
        _smallNumberSprite->setPosition(Vec2(50, cardSize.height - 60));
        _bgSprite->addChild(_smallNumberSprite);
    }
    
    // 右上角：花色
    std::string suitPath = getSuitImagePath(_cardSuit);
    _suitSprite = Sprite::create(suitPath);
    if (_suitSprite) {
        // 右上角位置：距离右边和顶部一定距离
        _suitSprite->setPosition(Vec2(cardSize.width - 50, cardSize.height - 60));
        _bgSprite->addChild(_suitSprite);
    }
    
    // 中间：大数字
    std::string bigNumberPath = getBigNumberImagePath(cardFace, cardSuit);
    _bigNumberSprite = Sprite::create(bigNumberPath);
    if (_bigNumberSprite) {
        // 中间位置：卡牌中心
        _bigNumberSprite->setPosition(Vec2(cardSize.width / 2, cardSize.height / 2));
        _bgSprite->addChild(_bigNumberSprite);
    }
    
    // 缩放适配 - 调整卡牌大小使其更合理
    float targetCardWidth = 120;  // 减小卡牌宽度，使其更合理
    float scale = targetCardWidth / _bgSprite->getContentSize().width;
    _bgSprite->setScale(scale);
    
    // 点击事件
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);  // 拦截触摸，确保点击的是正确的卡牌
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        if (!this->isVisible()) return false;
        Vec2 worldPos = touch->getLocation();
        Vec2 locationInNode = this->convertToNodeSpace(worldPos);
        
        // 获取卡牌的实际大小（考虑缩放）
        Size s = _bgSprite->getContentSize() * _bgSprite->getScale();
        Rect rect(-s.width / 2, -s.height / 2, s.width, s.height);
        
        if (rect.containsPoint(locationInNode)) {
            onCardClicked();
            return true;
        }
        return false;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    setFaceUp(isFaceUp);
    return true;
}

void CardView::setCardId(int cardId) {
    _cardId = cardId;
}

int CardView::getCardId() const {
    return _cardId;
}

void CardView::setOnClickCallback(const std::function<void(int)>& callback) {
    _onClickCallback = callback;
}

void CardView::setFaceUp(bool isFaceUp) {
    _isFaceUp = isFaceUp;
    if (_suitSprite) {
        _suitSprite->setVisible(isFaceUp);
    }
    if (_bigNumberSprite) {
        _bigNumberSprite->setVisible(isFaceUp);
    }
    if (_smallNumberSprite) {
        _smallNumberSprite->setVisible(isFaceUp);
    }
}

void CardView::onCardClicked() {
    if (_onClickCallback) {
        _onClickCallback(_cardId);
    }
}

void CardView::playMoveAnimation(const cocos2d::Vec2& targetPos, std::function<void()> callback) {
    auto moveAction = MoveTo::create(0.3f, targetPos);
    if (callback) {
        auto callbackAction = CallFunc::create(callback);
        this->runAction(Sequence::create(moveAction, callbackAction, nullptr));
    } else {
        this->runAction(moveAction);
    }
}

std::string CardView::getBigNumberImagePath(int cardFace, int cardSuit) {
    std::string color = (cardSuit == 1 || cardSuit == 2) ? "red" : "black";
    std::string faceStr;
    switch (cardFace) {
        case 1: faceStr = "A"; break;
        case 2: faceStr = "2"; break;
        case 3: faceStr = "3"; break;
        case 4: faceStr = "4"; break;
        case 5: faceStr = "5"; break;
        case 6: faceStr = "6"; break;
        case 7: faceStr = "7"; break;
        case 8: faceStr = "8"; break;
        case 9: faceStr = "9"; break;
        case 10: faceStr = "10"; break;
        case 11: faceStr = "J"; break;
        case 12: faceStr = "Q"; break;
        case 13: faceStr = "K"; break;
        default: faceStr = "A"; break;
    }
    return "res1/number/big_" + color + "_" + faceStr + ".png";
}

std::string CardView::getSmallNumberImagePath(int cardFace, int cardSuit) {
    std::string color = (cardSuit == 1 || cardSuit == 2) ? "red" : "black";
    std::string faceStr;
    switch (cardFace) {
        case 1: faceStr = "A"; break;
        case 2: faceStr = "2"; break;
        case 3: faceStr = "3"; break;
        case 4: faceStr = "4"; break;
        case 5: faceStr = "5"; break;
        case 6: faceStr = "6"; break;
        case 7: faceStr = "7"; break;
        case 8: faceStr = "8"; break;
        case 9: faceStr = "9"; break;
        case 10: faceStr = "10"; break;
        case 11: faceStr = "J"; break;
        case 12: faceStr = "Q"; break;
        case 13: faceStr = "K"; break;
        default: faceStr = "A"; break;
    }
    return "res1/number/small_" + color + "_" + faceStr + ".png";
}

std::string CardView::getSuitImagePath(int cardSuit) {
    switch (cardSuit) {
        case 0: return "res1/suits/club.png";
        case 1: return "res1/suits/diamond.png";
        case 2: return "res1/suits/heart.png";
        case 3: return "res1/suits/spade.png";
        default: return "res1/suits/club.png";
    }
}

