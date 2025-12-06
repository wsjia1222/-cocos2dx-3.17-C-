#pragma once
#include "cocos2d.h"

/**
 * UndoModel - 回退数据模型
 * 
 * 这个文件定义了回退功能需要的数据结构
 * 每次玩家操作时，都会记录一条UndoRecord，用于回退时恢复状态
 */

/**
 * MoveType - 操作类型枚举
 * 用来区分不同的操作类型，因为不同类型的操作回退方式不同
 */
enum class MoveType {
    STACK_REPLACE,      // 手牌区翻牌替换：点击备用底牌替换顶部底牌
    PLAYFIELD_MATCH     // 桌面牌匹配：点击主牌区的牌与底牌区顶部牌匹配
};

/**
 * UndoRecord - 回退记录结构体
 * 
 * 每次操作时，都会创建一个UndoRecord记录这次操作的所有信息
 * 回退时，根据这些信息恢复到操作前的状态
 */
struct UndoRecord {
    int cardId;                    // 被操作的卡牌ID（比如被点击的卡牌）
    MoveType moveType;              // 操作类型（是换底牌还是匹配）
    cocos2d::Vec2 originalPos;     // 卡牌的原始位置（回退时要移回这里）
    int originalParent;             // 卡牌原来在哪个区域：0=主牌区, 1=底牌堆
    int targetCardId;               // 目标卡牌ID（比如匹配时目标底牌，换底牌时原来的顶部牌）
    int cardFace;                   // 卡牌点数（回退时需要重新创建卡牌，所以要保存点数）
    int cardSuit;                   // 卡牌花色（回退时需要重新创建卡牌，所以要保存花色）
    int originalStackIndex;         // 在底牌堆中的原始索引位置（换底牌时用，知道原来在第几个位置）
    
    // 用于匹配操作回退：保存原顶部卡牌信息
    // 因为匹配时原顶部卡牌会被移除，回退时需要恢复它
    int oldTopCardFace;             // 原顶部卡牌的点数
    int oldTopCardSuit;             // 原顶部卡牌的花色
};

