#pragma once
#include "cocos2d.h"
#include <functional>

/**
 * @brief CardView - 单张卡牌视图类
 * 
 * 这个类负责显示一张卡牌的UI界面，是游戏中最基础的UI组件。
 * 它封装了卡牌的显示逻辑和交互逻辑，提供统一的卡牌视图接口。
 * 
 * 职责：
 * - 显示卡牌的视觉元素（底图、数字、花色）
 * - 处理卡牌的点击事件
 * - 播放卡牌移动动画
 * - 管理卡牌的正反面状态
 * 
 * 使用场景：
 * - 在主牌区显示桌面上的卡牌
 * - 在底牌堆显示手牌区的卡牌
 * - 需要显示任何卡牌时使用
 * 
 * 架构说明：
 * - 继承自cocos2d::Node，可以添加到场景中显示
 * - 通过回调函数通知外部卡牌被点击
 * - 支持正面和背面两种显示状态
 * 
 * 使用示例：
 * CardView* card = CardView::create(1, 2, true);  // 创建一张红桃A
 * card->setCardId(100);  // 设置ID
 * card->setPosition(Vec2(100, 200));  // 设置位置
 * parent->addChild(card);  // 添加到父节点
 */
class CardView : public cocos2d::Node {
public:
    /**
     * 创建卡牌视图（静态工厂方法）
     * @param cardFace 卡牌点数：1=A, 2-10=数字, 11=J, 12=Q, 13=K
     * @param cardSuit 卡牌花色：0=梅花, 1=方块, 2=红桃, 3=黑桃
     * @param isFaceUp 是否正面朝上，默认true
     * @return 创建成功的CardView指针，失败返回nullptr
     */
    static CardView* create(int cardFace, int cardSuit, bool isFaceUp = true);
    
    /**
     * 初始化卡牌视图
     * @param cardFace 卡牌点数
     * @param cardSuit 卡牌花色
     * @param isFaceUp 是否正面朝上
     * @return true=成功, false=失败
     */
    bool init(int cardFace, int cardSuit, bool isFaceUp);
    
    /**
     * 设置卡牌的唯一ID
     * @param cardId 卡牌ID，用于区分不同的卡牌
     */
    void setCardId(int cardId);
    
    /**
     * 获取卡牌的ID
     * @return 卡牌ID
     */
    int getCardId() const;
    
    /**
     * 设置卡牌被点击时的回调函数
     * @param callback 回调函数，参数是卡牌的ID
     * 当卡牌被点击时，会调用这个回调函数，并传入卡牌的ID
     */
    void setOnClickCallback(const std::function<void(int)>& callback);
    
    /**
     * 设置卡牌是否正面朝上
     * @param isFaceUp true=正面（显示数字和花色）, false=背面（不显示）
     */
    void setFaceUp(bool isFaceUp);
    
    /**
     * 播放卡牌移动动画
     * @param targetPos 目标位置，卡牌会平滑移动到这里
     * @param callback 动画完成后的回调函数（可选）
     * 动画时长固定为0.3秒
     */
    void playMoveAnimation(const cocos2d::Vec2& targetPos, std::function<void()> callback = nullptr);

private:
    int _cardFace;      // 卡牌点数（1-13）
    int _cardSuit;      // 卡牌花色（0-3）
    int _cardId;        // 卡牌唯一ID，用于标识这张卡牌
    bool _isFaceUp;     // 是否正面朝上
    
    // 卡牌的UI元素（都是Sprite精灵）
    cocos2d::Sprite* _bgSprite;          // 卡牌底图（白色卡牌背景）
    cocos2d::Sprite* _suitSprite;        // 花色精灵（右上角，显示♣♦♥♠）
    cocos2d::Sprite* _bigNumberSprite;    // 大数字精灵（中间，显示A/2-10/J/Q/K）
    cocos2d::Sprite* _smallNumberSprite;  // 小数字精灵（左上角，显示A/2-10/J/Q/K）
    
    std::function<void(int)> _onClickCallback;  // 点击回调函数
    
    /**
     * 卡牌被点击时调用
     * 会触发_onClickCallback回调，通知外部有卡牌被点击了
     */
    void onCardClicked();
    
    /**
     * 获取大数字图片路径
     * @param cardFace 卡牌点数
     * @param cardSuit 卡牌花色（用于判断是红色还是黑色）
     * @return 图片路径，如"res1/number/big_red_A.png"
     */
    std::string getBigNumberImagePath(int cardFace, int cardSuit);
    
    /**
     * 获取小数字图片路径
     * @param cardFace 卡牌点数
     * @param cardSuit 卡牌花色（用于判断是红色还是黑色）
     * @return 图片路径，如"res1/number/small_red_A.png"
     */
    std::string getSmallNumberImagePath(int cardFace, int cardSuit);
    
    /**
     * 获取花色图片路径
     * @param cardSuit 卡牌花色
     * @return 图片路径，如"res1/suits/heart.png"
     */
    std::string getSuitImagePath(int cardSuit);
};

