#include "GameView.h"
#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;  // 使用cocos2d命名空间
using namespace ui;  // 使用UI命名空间（按钮等UI控件）

/**
 * 创建GameView对象（静态工厂方法）
 * 
 * 这是cocos2d-x的标准创建模式
 * 
 * @return 创建成功的GameView指针，失败返回nullptr
 */
GameView* GameView::create() {
    GameView* ret = new (std::nothrow) GameView();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

/**
 * 初始化游戏视图
 * 
 * 这个方法负责创建游戏的所有UI元素：
 * 1. 背景图片
 * 2. 主牌区视图（显示桌面上的卡牌）
 * 3. 底牌堆视图（显示手牌区的卡牌）
 * 4. 回退按钮
 * 
 * @return true=初始化成功, false=初始化失败
 */
bool GameView::init() {
    // 先初始化父类Scene
    if (!Scene::init()) return false;
    
    // 创建背景图片
    createBackground();
    
    // 创建主牌区视图
    // 主牌区显示桌面上的卡牌，玩家可以点击这些卡牌与底牌匹配
    _playfieldView = PlayfieldView::create();
    _playfieldView->setPosition(Vec2(0, 580)); // 主牌区在上方（y=580，屏幕总高度2080）
    this->addChild(_playfieldView);
    
    // 创建底牌堆视图
    // 底牌堆显示手牌区的卡牌，包括备用底牌和当前使用的顶部底牌
    _stackView = StackView::create();
    _stackView->setPosition(Vec2(0, 0)); // 底牌堆在下方（y=0）
    this->addChild(_stackView);
    
    // 创建回退按钮
    // 玩家点击这个按钮可以撤销上一步操作
    createUndoButton();
    
    return true;
}

/**
 * 创建背景图片
 * 
 * 加载背景图片并设置到屏幕中心，缩放以填满整个屏幕
 */
void GameView::createBackground() {
    // 创建背景精灵（Sprite）
    auto bg = Sprite::create("res1/back_ground.jpg");
    if (bg) {
        // 设置背景位置为屏幕中心（1080x2080的中心点是540,1040）
        bg->setPosition(Vec2(540, 1040));
        
        // 缩放背景以填满整个屏幕
        // 计算缩放比例：目标尺寸 / 原始尺寸
        bg->setScale(1080.0f / bg->getContentSize().width, 2080.0f / bg->getContentSize().height);
        
        // 添加到场景，z-order=-1（在最底层，其他元素在上面）
        this->addChild(bg, -1);
    }
}

/**
 * 创建回退按钮
 * 
 * 创建一个按钮，点击后可以撤销上一步操作
 * 按钮默认是隐藏的，只有当有操作可以回退时才显示
 */
void GameView::createUndoButton() {
    // 创建按钮（使用cocos2d的UI系统）
    _undoButton = Button::create();
    
    // 设置按钮文字和字体大小
    _undoButton->setTitleText("回退");
    _undoButton->setTitleFontSize(30);
    
    // 设置按钮位置：左上角附近
    _undoButton->setPosition(Vec2(100, 2000));
    
    // 默认隐藏按钮（只有当有操作可以回退时才显示）
    _undoButton->setVisible(false);
    
    // 添加触摸事件监听器
    // 当用户点击按钮时，会触发回调函数
    _undoButton->addTouchEventListener([this](Ref* sender, Widget::TouchEventType type) {
        // 只有当触摸事件类型是"结束"（即用户抬起手指）时才执行
        if (type == Widget::TouchEventType::ENDED) {
            // 如果设置了回调函数，就调用它
            if (_onUndoClickCallback) {
                _onUndoClickCallback();
            }
        }
    });
    
    // 将按钮添加到场景中
    this->addChild(_undoButton);
}

/**
 * 设置卡牌点击回调函数
 * 
 * 当玩家点击任何卡牌时，会调用这个回调函数
 * 这个函数会被传递给主牌区和底牌堆，让它们都能响应卡牌点击
 * 
 * @param callback 回调函数，参数是卡牌的ID
 */
void GameView::setOnCardClickCallback(const std::function<void(int)>& callback) {
    _onCardClickCallback = callback;
    
    // 将回调函数传递给主牌区
    if (_playfieldView) {
        _playfieldView->setOnCardClickCallback(callback);
    }
    
    // 将回调函数传递给底牌堆
    if (_stackView) {
        _stackView->setOnCardClickCallback(callback);
    }
}

/**
 * 设置回退按钮点击回调函数
 * 
 * 当玩家点击回退按钮时，会调用这个回调函数
 * 
 * @param callback 回调函数，无参数
 */
void GameView::setOnUndoClickCallback(const std::function<void()>& callback) {
    _onUndoClickCallback = callback;
}

/**
 * 显示或隐藏回退按钮
 * 
 * 当有操作可以回退时显示按钮，没有操作可以回退时隐藏按钮
 * 
 * @param visible true=显示按钮, false=隐藏按钮
 */
void GameView::showUndoButton(bool visible) {
    if (_undoButton) {
        _undoButton->setVisible(visible);
    }
}

