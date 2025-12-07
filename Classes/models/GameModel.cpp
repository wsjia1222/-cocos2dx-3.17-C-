#include "GameModel.h"
#include <algorithm>

/**
 * 获取下一个卡牌ID
 * 每次调用都会返回一个新的唯一ID，然后自动+1
 * 比如第一次调用返回0，第二次返回1，以此类推
 */
int GameModel::getNextCardId() {
    return nextCardId++;  // 先返回当前值，然后+1
}

/**
 * 添加卡牌到主牌区
 * 使用vector的push_back方法，把卡牌添加到数组末尾
 */
void GameModel::addCardToPlayfield(const CardModel& card) {
    playfieldCards.push_back(card);
}

/**
 * 添加卡牌到底牌堆
 * 新添加的卡牌会放在最后，成为顶部牌（因为顶部牌是vector的最后一个元素）
 */
void GameModel::addCardToStack(const CardModel& card) {
    stackCards.push_back(card);
}

/**
 * 根据ID查找卡牌
 * 先在主牌区找，找不到再在底牌堆找
 * 如果都找不到，返回一个id=-1的空卡牌表示没找到
 */
CardModel GameModel::getCardById(int cardId) const {
    // 在主牌区查找：遍历主牌区的所有卡牌
    for (const auto& card : playfieldCards) {
        if (card.id == cardId) {
            return card;  // 找到了，返回这张卡牌
        }
    }
    // 在底牌堆查找：遍历底牌堆的所有卡牌
    for (const auto& card : stackCards) {
        if (card.id == cardId) {
            return card;  // 找到了，返回这张卡牌
        }
    }
    // 未找到，返回空卡牌（id=-1表示无效）
    CardModel empty;
    empty.id = -1;
    return empty;
}

/**
 * 从主牌区移除卡牌
 * 使用STL的remove_if算法找到要删除的卡牌，然后用erase删除
 */
void GameModel::removeCardFromPlayfield(int cardId) {
    playfieldCards.erase(
        std::remove_if(playfieldCards.begin(), playfieldCards.end(),
            [cardId](const CardModel& card) { return card.id == cardId; }),  // lambda表达式：找到id匹配的卡牌
        playfieldCards.end()
    );
}

/**
 * 从底牌堆移除卡牌
 * 和上面的方法一样，只是操作的是stackCards数组
 */
void GameModel::removeCardFromStack(int cardId) {
    stackCards.erase(
        std::remove_if(stackCards.begin(), stackCards.end(),
            [cardId](const CardModel& card) { return card.id == cardId; }),
        stackCards.end()
    );
}

/**
 * 获取底牌堆的顶部卡牌
 * 顶部卡牌就是vector的最后一个元素（back()）
 * 如果底牌堆为空，返回id=-1的空卡牌
 */
CardModel GameModel::getStackTopCard() const {
    if (stackCards.empty()) {
        CardModel empty;
        empty.id = -1;
        return empty;
    }
    return stackCards.back();  // back()返回vector的最后一个元素
}

/**
 * 清空所有数据
 * 清空两个数组，并把ID计数器重置为0
 */
void GameModel::clear() {
    playfieldCards.clear();  // 清空主牌区
    stackCards.clear();      // 清空底牌堆
    nextCardId = 0;          // 重置ID计数器
}

