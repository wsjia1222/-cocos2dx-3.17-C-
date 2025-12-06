
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;  // 使用cocos2d命名空间

/**
 * 创建HelloWorld场景（静态工厂方法）
 * 
 * 这是cocos2d-x的模板场景，目前没有被使用
 * 实际游戏使用的是GameScene
 * 
 * @return 创建成功的Scene指针
 */
Scene* HelloWorld::createScene()
{
    return HelloWorld::create();  // 调用create()方法创建场景
}

/**
 * 打印资源加载错误信息
 * 
 * 当图片等资源文件加载失败时，调用这个函数打印错误信息
 * 这样可以帮助调试，知道是哪个文件找不到
 * 
 * @param filename 加载失败的文件名
 */
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

/**
 * 初始化HelloWorld场景
 * 
 * 这是cocos2d-x的模板代码，创建了一个简单的"Hello World"场景
 * 包含一个关闭按钮和一个"Hello World"标签
 * 目前这个场景没有被使用，实际游戏使用的是GameScene
 * 
 * @return true=初始化成功, false=初始化失败
 */
bool HelloWorld::init()
{
    //////////////////////////////
    // 步骤1：先初始化父类Scene（必须的）
    if ( !Scene::init() )
    {
        return false;  // 如果父类初始化失败，返回false
    }

    // 获取可见区域大小和原点位置
    // visibleSize：屏幕上可见区域的大小（宽度和高度）
    // origin：屏幕原点的位置（通常是(0,0)，但有些情况下可能不是）
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 步骤2：添加一个关闭按钮（"X"图标），点击后退出程序
    //    你可以根据需要修改这个按钮

    // 创建一个关闭按钮（使用图片菜单项）
    // 参数1：正常状态的图片（"CloseNormal.png"）
    // 参数2：按下状态的图片（"CloseSelected.png"）
    // 参数3：点击回调函数（menuCloseCallback）
    // 注意：这是autorelease对象，由cocos2d自动管理内存
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    // 检查按钮是否创建成功
    // 如果图片文件不存在，按钮会创建失败
    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        // 打印错误信息，帮助调试
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        // 设置按钮位置：右上角
        // x = 原点x + 屏幕宽度 - 按钮宽度的一半（让按钮右边缘对齐屏幕右边缘）
        // y = 原点y + 按钮高度的一半（让按钮上边缘对齐屏幕顶部）
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // 创建菜单（包含关闭按钮）
    // Menu是菜单容器，可以包含多个菜单项
    // create()的第一个参数是菜单项，可以有多个，最后一个参数必须是NULL
    // 注意：这是autorelease对象，由cocos2d自动管理内存
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);  // 菜单位置设为(0,0)，菜单项的位置是相对于菜单的
    this->addChild(menu, 1);  // 添加到场景，z-order=1（在背景之上）

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");
    if (sprite == nullptr)
    {
        problemLoading("'HelloWorld.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
