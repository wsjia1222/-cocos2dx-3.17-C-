#include "UndoManager.h"

/**
 * 记录一次操作
 * 把操作记录压入栈中，就像把一张纸放到一摞纸的最上面
 */
void UndoManager::push(const UndoRecord& record) {
    _undoStack.push(record);
}

/**
 * 检查是否可以回退
 * 如果栈不为空，说明还有记录可以回退
 */
bool UndoManager::canUndo() const {
    return !_undoStack.empty();  // 栈不为空就可以回退
}

/**
 * 执行一次回退
 * 从栈顶取出（弹出）一条记录，这就是最近的一次操作
 * 返回这条记录，让调用者根据记录恢复状态
 */
UndoRecord UndoManager::undo() {
    if (_undoStack.empty()) {
        // 栈为空，没有记录可以回退，返回一个无效记录
        UndoRecord empty;
        empty.cardId = -1;
        return empty;
    }
    
    // 获取栈顶的记录（最近的操作）
    UndoRecord record = _undoStack.top();
    // 从栈中移除这条记录
    _undoStack.pop();
    return record;
}

/**
 * 清空所有历史记录
 * 把栈中的所有记录都弹出，直到栈为空
 */
void UndoManager::clear() {
    while (!_undoStack.empty()) {
        _undoStack.pop();  // 不断弹出，直到栈为空
    }
}

