#pragma once
#include "CardModel.h"
#include <vector>

/**
 * GameModel - 游戏数据模型
 * 
 * 这个结构体管理整个游戏的所有数据状态
 * 它存储了主牌区和底牌堆的所有卡牌数据
 * 
 * 职责：
 * - 存储所有卡牌的数据（主牌区和底牌堆）
 * - 提供添加、移除、查找卡牌的方法
 * - 管理卡牌ID的分配
 * 
 * 注意：这个类只管理数据，不负责显示，显示由View层负责
 */
struct GameModel {
    std::vector<CardModel> playfieldCards;  // 主牌区的所有卡牌，用vector数组存储
    std::vector<CardModel> stackCards;     // 底牌堆的所有卡牌（手牌区），最后一张是当前使用的顶部牌
    int nextCardId = 0;                    // 卡牌ID计数器，每创建一张新卡牌就+1，确保每张卡牌ID唯一

    /**
     * 获取下一个卡牌ID
     * @return 返回一个新的唯一ID，然后计数器自动+1
     */
    int getNextCardId();
    
    /**
     * 添加卡牌到主牌区
     * @param card 要添加的卡牌数据
     */
    void addCardToPlayfield(const CardModel& card);
    
    /**
     * 添加卡牌到底牌堆
     * @param card 要添加的卡牌数据
     * 注意：新添加的卡牌会放在最后，成为顶部牌
     */
    void addCardToStack(const CardModel& card);
    
    /**
     * 根据ID查找卡牌
     * @param cardId 卡牌的ID
     * @return 找到的卡牌，如果没找到返回id=-1的卡牌
     */
    CardModel getCardById(int cardId) const;
    
    /**
     * 从主牌区移除卡牌
     * @param cardId 要移除的卡牌ID
     */
    void removeCardFromPlayfield(int cardId);
    
    /**
     * 从底牌堆移除卡牌
     * @param cardId 要移除的卡牌ID
     */
    void removeCardFromStack(int cardId);
    
    /**
     * 获取底牌堆的顶部卡牌（当前使用的底牌）
     * @return 顶部卡牌，如果底牌堆为空返回id=-1的卡牌
     */
    CardModel getStackTopCard() const;
    
    /**
     * 清空所有数据
     * 用于重新开始游戏时清空之前的数据
     */
    void clear();
};

