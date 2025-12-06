
#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "GameScene.h"

// 音频引擎配置（目前都注释掉了，表示不使用音频）
// #define USE_AUDIO_ENGINE 1
// #define USE_SIMPLE_AUDIO_ENGINE 1

// 如果同时启用了两个音频引擎，编译时会报错
#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

// 根据配置选择使用哪个音频引擎（目前都不使用）
#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

USING_NS_CC;  // 使用cocos2d命名空间，这样写代码时不用写cocos2d::前缀

// 设计分辨率：这是游戏设计时使用的屏幕尺寸（1080x2080，类似手机竖屏）
static cocos2d::Size designResolutionSize = cocos2d::Size(1080, 2080);
// 小分辨率：适配小屏幕设备（如老款手机）
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
// 中等分辨率：适配中等屏幕设备（如平板）
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
// 大分辨率：适配大屏幕设备（如iPad）
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

/**
 * 构造函数：创建AppDelegate对象
 * 目前是空的，不需要初始化任何东西
 */
AppDelegate::AppDelegate()
{
}

/**
 * 析构函数：销毁AppDelegate对象
 * 在程序退出时调用，清理音频引擎资源
 */
AppDelegate::~AppDelegate() 
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();  // 结束音频引擎
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::end();  // 结束简单音频引擎
#endif
}

/**
 * 初始化OpenGL上下文属性
 * 
 * OpenGL是图形渲染的底层接口，这里设置：
 * - 颜色通道位数（红、绿、蓝、透明度各8位，共32位颜色）
 * - 深度缓冲区位数（24位，用于3D渲染的深度测试）
 * - 模板缓冲区位数（8位，用于特殊效果）
 * - 多重采样数（0，不使用抗锯齿）
 * 
 * 如果你想修改这些参数，可以改glContextAttrs的值
 * 注意：修改会影响所有平台（Windows、Android、iOS等）
 */
void AppDelegate::initGLContextAttrs()
{
    // 设置OpenGL上下文属性：红、绿、蓝、透明度、深度、模板、多重采样数
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

    GLView::setGLContextAttrs(glContextAttrs);  // 应用这些设置
}

/**
 * 注册所有包（用于包管理器）
 * 如果你想使用包管理器安装更多包，不要修改或删除这个函数
 * 目前返回0，表示没有包需要注册
 */
static int register_all_packages()
{
    return 0; // 包管理器的标志位
}

/**
 * 应用程序启动完成时调用
 * 这是游戏初始化的核心方法，按顺序执行以下步骤：
 */
bool AppDelegate::applicationDidFinishLaunching() {
    // 步骤1：初始化Director（导演）
    // Director是cocos2d-x的核心，管理所有场景（Scene）的切换和运行
    auto director = Director::getInstance();  // 获取Director的单例（全局唯一实例）
    
    // 获取OpenGL视图（游戏窗口）
    auto glview = director->getOpenGLView();
    if(!glview) {
        // 如果还没有创建窗口，就创建一个
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        // Windows/Mac/Linux平台：创建指定大小的窗口
        // 参数：窗口标题、窗口大小(0,0,1080,2080)、缩放比例0.5
        glview = GLViewImpl::createWithRect("wsj", cocos2d::Rect(0, 0, 1080, 2080), 0.5f);
#else
        // 其他平台（Android/iOS）：使用默认方式创建
        glview = GLViewImpl::create("wsj");
#endif
        director->setOpenGLView(glview);  // 设置窗口
    }

    // 步骤2：显示FPS（帧率）
    // FPS = Frames Per Second，每秒渲染的帧数，用于监控游戏性能
    director->setDisplayStats(true);  // true表示在屏幕左下角显示FPS

    // 步骤3：设置游戏帧率
    // 1.0f / 60 表示每秒60帧，即每帧约16.67毫秒
    // 如果不调用这个方法，默认也是60帧
    director->setAnimationInterval(1.0f / 60);

    // 步骤4：设置设计分辨率
    // 设计分辨率是1080x2080，使用FIXED_WIDTH策略（固定宽度，高度自适应）
    // 这样在不同屏幕比例的设备上，宽度都是1080，高度会自动调整
    glview->setDesignResolutionSize(1080, 2080, ResolutionPolicy::FIXED_WIDTH);
    
    // 获取实际窗口大小
    auto frameSize = glview->getFrameSize();
    
    // 步骤5：根据实际窗口大小设置内容缩放因子
    // 缩放因子决定了游戏内容如何适配不同分辨率的屏幕
    // 如果实际屏幕高度大于中等分辨率
    if (frameSize.height > mediumResolutionSize.height)
    {        
        // 使用大分辨率的缩放因子
        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
    }
    // 如果实际屏幕高度大于小分辨率
    else if (frameSize.height > smallResolutionSize.height)
    {        
        // 使用中等分辨率的缩放因子
        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    }
    // 如果实际屏幕高度小于中等分辨率
    else
    {        
        // 使用小分辨率的缩放因子
        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
    }

    // 步骤6：注册所有包（目前没有包需要注册）
    register_all_packages();

    // 步骤7：添加资源搜索路径
    // 告诉游戏引擎在哪里找图片、声音等资源文件
    FileUtils::getInstance()->addSearchPath("Resources");      // 基础资源路径
    FileUtils::getInstance()->addSearchPath("Resources/res1"); // 游戏资源路径

    // 步骤8：创建第一个游戏场景
    // GameScene是游戏的主场景，包含游戏的所有内容
    auto scene = GameScene::create();  // create()方法会创建场景并自动管理内存

    // 步骤9：运行场景
    // 让Director运行这个场景，游戏正式开始
    director->runWithScene(scene);

    return true;  // 返回true表示初始化成功
}

/**
 * 应用程序进入后台时调用
 * 
 * 当用户切换到其他应用、锁屏或接电话时，游戏会进入后台
 * 这时需要暂停游戏，节省电量和系统资源
 * 
 * 注意：在手机上接电话时也会调用这个方法
 */
void AppDelegate::applicationDidEnterBackground() {
    // 停止游戏动画
    // 这样游戏就不会继续渲染，节省CPU和电量
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    // 如果使用AudioEngine，暂停所有音频
    AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    // 如果使用SimpleAudioEngine，暂停背景音乐和所有音效
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
}

/**
 * 应用程序重新进入前台时调用
 * 
 * 当用户切回游戏时，游戏会重新进入前台
 * 这时需要恢复游戏，让玩家继续游戏
 */
void AppDelegate::applicationWillEnterForeground() {
    // 恢复游戏动画
    // 让游戏重新开始渲染
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    // 如果使用AudioEngine，恢复所有音频
    AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    // 如果使用SimpleAudioEngine，恢复背景音乐和所有音效
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
}
