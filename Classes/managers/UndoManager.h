#pragma once
#include "models/UndoModel.h"
#include <stack>

/**
 * UndoManager - 回退管理器
 * 
 * 这个类管理所有的回退记录，就像一个历史记录本
 * 每次操作时，把操作记录"push"（压入）栈中
 * 回退时，从栈顶"pop"（弹出）一条记录，按照记录恢复状态
 * 
 * 使用栈（stack）的原因：
 * - 栈是"后进先出"（LIFO）的数据结构
 * - 最后操作的要最先回退，正好符合栈的特性
 * - 比如：操作A -> 操作B -> 操作C，回退时是 C -> B -> A
 */
class UndoManager {
public:
    /**
     * 记录一次操作
     * @param record 操作记录，包含所有回退需要的信息
     */
    void push(const UndoRecord& record);
    
    /**
     * 检查是否可以回退
     * @return true=有记录可以回退, false=没有记录了
     */
    bool canUndo() const;
    
    /**
     * 执行一次回退
     * @return 返回要回退的操作记录
     * 注意：调用这个方法会从栈中移除一条记录
     */
    UndoRecord undo();
    
    /**
     * 清空所有历史记录
     * 重新开始游戏时调用
     */
    void clear();

private:
    std::stack<UndoRecord> _undoStack;  // 回退记录栈，存储所有的操作记录
};

