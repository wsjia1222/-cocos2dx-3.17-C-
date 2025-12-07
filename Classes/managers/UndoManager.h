#pragma once
#include "models/UndoModel.h"
#include <stack>

/**
 * @brief UndoManager - 回退管理器类
 * 
 * 这个类管理所有的回退记录，实现游戏的撤销功能。
 * 它使用栈（stack）数据结构存储操作历史，支持后进先出的回退操作。
 * 
 * 职责：
 * - 记录每次游戏操作（压入栈）
 * - 提供回退功能（从栈顶弹出记录）
 * - 检查是否可以回退
 * - 清空所有历史记录
 * 
 * 使用场景：
 * - 玩家执行操作时记录操作信息
 * - 玩家点击回退按钮时恢复之前的状态
 * - 重新开始游戏时清空历史记录
 * 
 * 架构说明：
 * - 使用std::stack存储UndoRecord对象
 * - 栈是"后进先出"（LIFO）的数据结构，符合回退的需求
 * - 例如：操作A -> 操作B -> 操作C，回退时是 C -> B -> A
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

