
#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"

/**
 * @brief AppDelegate - 应用程序委托类
 * 
 * 这是整个游戏的入口类，负责：
 * 1. 初始化游戏引擎（cocos2d-x）
 * 2. 设置游戏窗口大小和分辨率
 * 3. 创建第一个游戏场景（GameScene）
 * 4. 处理应用进入后台/前台的事件
 * 
 * 为什么用private继承？
 * - 使用private继承可以隐藏Application的部分接口
 * - 这样外部代码不能直接访问Application的方法，更安全
 */
class  AppDelegate : private cocos2d::Application
{
public:
    /**
     * 构造函数：创建AppDelegate对象
     * 在程序启动时自动调用
     */
    AppDelegate();
    
    /**
     * 析构函数：销毁AppDelegate对象
     * 在程序退出时自动调用，用于清理资源
     */
    virtual ~AppDelegate();

    /**
     * 初始化OpenGL上下文属性
     * OpenGL是图形渲染的底层接口，这里设置颜色、深度等参数
     * 这个方法在applicationDidFinishLaunching之前被调用
     */
    virtual void initGLContextAttrs();

    /**
     * @brief 应用程序启动完成时调用
     * 
     * 这是游戏初始化的核心方法，在这里：
     * 1. 初始化Director（导演，管理所有场景）
     * 2. 设置游戏窗口大小和分辨率适配
     * 3. 创建第一个游戏场景并运行
     * 
     * @return true  初始化成功，游戏继续运行
     * @return false 初始化失败，程序终止
     */
    virtual bool applicationDidFinishLaunching();

    /**
     * @brief 应用程序进入后台时调用
     * 
     * 当用户切换到其他应用或锁屏时，游戏会进入后台
     * 这时需要：
     * 1. 停止游戏动画（节省电量）
     * 2. 暂停背景音乐和音效
     * 
     * @param  the pointer of the application 应用程序指针（未使用）
     */
    virtual void applicationDidEnterBackground();

    /**
     * @brief 应用程序重新进入前台时调用
     * 
     * 当用户切回游戏时，游戏会重新进入前台
     * 这时需要：
     * 1. 恢复游戏动画
     * 2. 恢复背景音乐和音效
     * 
     * @param  the pointer of the application 应用程序指针（未使用）
     */
    virtual void applicationWillEnterForeground();
};

#endif // _APP_DELEGATE_H_

