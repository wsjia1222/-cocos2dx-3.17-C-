#pragma once
#include "views/GameView.h"
#include "models/GameModel.h"
#include "managers/UndoManager.h"
#include "models/UndoModel.h"
#include <functional>

/**
 * @brief GameController - 游戏控制器类
 * 
 * 这是MVC架构中的Controller层，负责处理游戏的核心逻辑和用户交互。
 * 它连接了视图层（GameView）和模型层（GameModel），协调两者之间的交互。
 * 
 * 职责：
 * - 处理用户输入（卡牌点击、回退按钮点击）
 * - 管理游戏逻辑（卡牌匹配规则、换底牌规则）
 * - 更新游戏模型（GameModel）的状态
 * - 更新游戏视图（GameView）的显示
 * - 管理回退功能（UndoManager）
 * 
 * 使用场景：
 * - 游戏运行时处理所有用户交互
 * - 执行游戏规则和逻辑判断
 * - 协调视图和模型的同步
 * 
 * 架构说明：
 * - 持有GameView指针，用于更新UI
 * - 持有GameModel对象，用于存储游戏数据
 * - 持有UndoManager对象，用于管理回退功能
 */
class GameController {
public:
    GameController(GameView* view);
    ~GameController();
    
    /**
     * 开始游戏
     * 初始化游戏模型、创建初始卡牌并更新视图
     */
    void startGame();
    
    /**
     * 处理卡牌点击事件
     * @param cardId 被点击的卡牌ID
     */
    void onCardClicked(int cardId);
    
    /**
     * 处理回退按钮点击事件
     * 撤销上一步操作，恢复游戏状态
     */
    void onUndoClicked();
    
    /**
     * 检查两张卡牌是否可以匹配
     * 匹配规则：点数差1即可匹配
     * @param card1Face 第一张卡牌的点数（1-13）
     * @param card2Face 第二张卡牌的点数（1-13）
     * @return true=可以匹配, false=不能匹配
     */
    bool canMatch(int card1Face, int card2Face) const;

private:
    GameView* _gameView;
    GameModel _gameModel;
    UndoManager _undoManager;
    
    /**
     * 更新视图
     * 根据游戏模型数据同步更新游戏视图显示
     */
    void updateView();
    
    /**
     * 处理底牌堆卡牌替换
     * 当玩家点击备用底牌时，将其替换为顶部底牌
     * @param clickedCardId 被点击的卡牌ID
     */
    void handleStackCardReplace(int clickedCardId);
    
    /**
     * 处理主牌区卡牌匹配
     * 当玩家点击主牌区卡牌时，尝试与顶部底牌匹配
     * @param playfieldCardId 主牌区卡牌ID
     */
    void handlePlayfieldCardMatch(int playfieldCardId);
    
    /**
     * 初始化主牌区卡牌
     * 根据游戏配置创建主牌区的所有卡牌
     */
    void initializePlayfieldCards();
    
    /**
     * 初始化底牌堆卡牌
     * 根据游戏配置创建底牌堆的所有卡牌
     */
    void initializeStackCards();
    
    /**
     * 创建卡牌模型
     * @param face 卡牌点数（1-13）
     * @param suit 卡牌花色（0-3）
     * @param posX X坐标
     * @param posY Y坐标
     * @return 创建的卡牌模型
     */
    CardModel createCard(int face, int suit, float posX, float posY);
    
    /**
     * 检查顶部底牌是否可以与主牌区匹配
     * @param topCardModel 顶部底牌模型
     * @return true=可以匹配, false=不能匹配
     */
    bool checkTopCardCanMatch(const CardModel& topCardModel) const;
    
    /**
     * 更新底牌堆中的卡牌顺序（将指定卡牌移到底部）
     * @param clickedCardId 被点击的卡牌ID
     * @param clickedIndex 卡牌在底牌堆中的索引
     */
    void moveCardToStackTop(int clickedCardId, int clickedIndex);
    
    /**
     * 记录底牌替换的回退信息
     * @param clickedCardId 被点击的卡牌ID
     * @param clickedCardModel 被点击的卡牌模型
     * @param clickedIndex 卡牌在底牌堆中的索引
     * @param topCardId 顶部卡牌ID
     * @return 回退记录
     */
    UndoRecord createStackReplaceRecord(int clickedCardId, const CardModel& clickedCardModel, 
                                         int clickedIndex, int topCardId);
    
    /**
     * 记录主牌区匹配的回退信息
     * @param playfieldCardId 主牌区卡牌ID
     * @param playfieldCard 主牌区卡牌模型
     * @param stackCard 顶部底牌模型
     * @return 回退记录
     */
    UndoRecord createPlayfieldMatchRecord(int playfieldCardId, const CardModel& playfieldCard, 
                                          const CardModel& stackCard);
    
    /**
     * 执行底牌替换的回退操作
     * @param record 回退记录
     */
    void undoStackReplace(const UndoRecord& record);
    
    /**
     * 执行主牌区匹配的回退操作
     * @param record 回退记录
     */
    void undoPlayfieldMatch(const UndoRecord& record);
};

