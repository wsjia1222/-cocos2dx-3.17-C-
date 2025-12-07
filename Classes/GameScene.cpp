#include "GameScene.h"
#include "cocos2d.h"

USING_NS_CC;  // 使用cocos2d命名空间

/**
 * 创建GameScene对象（静态工厂方法）
 * 
 * 这是cocos2d-x的标准创建模式：
 * 1. 用new创建对象（nothrow表示失败返回nullptr而不是抛异常）
 * 2. 调用init()初始化
 * 3. 如果成功，调用autorelease()（交给cocos2d管理内存，自动释放）
 * 4. 如果失败，用CC_SAFE_DELETE安全删除对象
 * 
 * @return 创建成功的GameScene指针，失败返回nullptr
 */
GameScene* GameScene::create() {
    GameScene* ret = new (std::nothrow) GameScene();  // 创建对象
    if (ret && ret->init()) {  // 如果创建成功且初始化成功
        ret->autorelease();  // 交给cocos2d管理内存，不需要手动delete
        return ret;
    }
    CC_SAFE_DELETE(ret);  // 如果失败，安全删除对象
    return nullptr;
}

/**
 * 初始化游戏场景
 * 
 * 这个方法在场景创建后立即调用，负责：
 * 1. 初始化父类Scene
 * 2. 创建游戏视图（GameView）- 负责显示游戏界面
 * 3. 创建游戏控制器（GameController）- 负责处理游戏逻辑
 * 4. 启动游戏
 * 
 * @return true=初始化成功, false=初始化失败
 */
bool GameScene::init() {
    // 先初始化父类Scene（必须的，否则场景无法正常工作）
    if (!Scene::init()) return false;
    
    // 创建游戏视图（GameView）
    // GameView负责显示游戏的所有UI元素：卡牌、按钮等
    _gameView = GameView::create();
    if (!_gameView) {
        // 如果创建失败，返回false
        return false;
    }
    // 将视图添加到场景中，这样视图才能显示出来
    this->addChild(_gameView);
    
    // 创建游戏控制器（GameController）
    // 注意：GameController不是cocos2d对象，所以用new创建，需要手动管理内存
    // GameController负责处理游戏逻辑：卡牌点击、匹配、回退等
    _gameController = new GameController(_gameView);
    if (_gameController) {
        // 启动游戏：初始化卡牌、设置游戏状态等
        _gameController->startGame();
    }
    
    return true;  // 初始化成功
}

/**
 * 析构函数：销毁GameScene对象
 * 
 * 当场景被销毁时（比如切换到其他场景），会调用这个方法
 * 需要手动删除GameController，因为它不是cocos2d对象
 * GameView不需要手动删除，因为它由cocos2d自动管理
 */
GameScene::~GameScene() {
    if (_gameController) {
        delete _gameController;  // 手动删除控制器
        _gameController = nullptr;  // 将指针设为nullptr，防止重复删除
    }
}

