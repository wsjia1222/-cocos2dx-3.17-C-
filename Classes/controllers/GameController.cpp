#include "GameController.h"
#include "views/CardView.h"
#include "views/StackView.h"
#include "cocos2d.h"

USING_NS_CC;  // 使用cocos2d命名空间

/**
 * 构造函数：创建游戏控制器
 * 
 * 游戏控制器是MVC架构中的Controller层，负责：
 * 1. 处理用户输入（卡牌点击、回退按钮点击）
 * 2. 管理游戏逻辑（卡牌匹配、换底牌等）
 * 3. 更新游戏模型（GameModel）
 * 4. 更新游戏视图（GameView）
 * 
 * @param view 游戏视图指针，控制器通过它来更新UI
 */
GameController::GameController(GameView* view) : _gameView(view) {
    if (_gameView) {
        // 设置卡牌点击回调
        // 当玩家点击任何卡牌时，会调用onCardClicked方法
        // 使用lambda表达式捕获this指针，这样可以在回调中访问GameController的成员
        _gameView->setOnCardClickCallback([this](int cardId) {
            onCardClicked(cardId);
        });
        
        // 设置回退按钮点击回调
        // 当玩家点击回退按钮时，会调用onUndoClicked方法
        _gameView->setOnUndoClickCallback([this]() {
            onUndoClicked();
        });
    }
}

/**
 * 析构函数：销毁游戏控制器
 * 
 * 注意：视图（GameView）由场景（GameScene）管理，不需要手动释放
 * 控制器只需要清理自己的资源即可
 */
GameController::~GameController() {
    // 视图由场景管理，不需要手动释放
    // GameModel和UndoManager是栈对象，会自动释放
}

/**
 * 开始游戏
 * 
 * 这个方法负责初始化游戏：
 * 1. 清空游戏模型和回退管理器
 * 2. 创建初始卡牌（主牌区和底牌堆）
 * 3. 更新视图显示所有卡牌
 * 
 * 注意：这里硬编码了初始卡牌配置，实际项目中应该从配置文件读取
 */
void GameController::startGame() {
    // 清空游戏模型（移除所有卡牌，重置ID计数器）
    _gameModel.clear();
    
    // 清空回退管理器（移除所有历史记录）
    _undoManager.clear();
    
    // 初始化主牌区卡牌
    initializePlayfieldCards();
    
    // 初始化底牌堆卡牌
    initializeStackCards();
    
    // 创建视图：根据模型数据创建所有卡牌的UI显示
    updateView();
}

/**
 * 更新视图
 * 
 * 这个方法根据游戏模型（GameModel）的数据，更新游戏视图（GameView）
 * 当游戏状态改变时（比如添加/移除卡牌），需要调用这个方法来同步视图
 * 
 * 流程：
 * 1. 清空所有现有的卡牌视图
 * 2. 根据模型数据重新创建所有卡牌视图
 * 3. 更新回退按钮的显示状态
 */
void GameController::updateView() {
    if (!_gameView) return;  // 如果视图为空，直接返回
    
    // 获取主牌区视图和底牌堆视图
    auto playfieldView = _gameView->getPlayfieldView();
    auto stackView = _gameView->getStackView();
    
    // 步骤1：清空视图 - 移除所有现有卡牌视图
    // 先移除主牌区的所有卡牌
    for (auto* card : playfieldView->getCards()) {
        playfieldView->removeCard(card);
    }
    // 再移除底牌堆的所有卡牌
    for (auto* card : stackView->getCards()) {
        stackView->removeCard(card);
    }
    
    // 步骤2：创建主牌区卡牌视图
    // 遍历模型中的每张主牌区卡牌，创建对应的视图
    for (const auto& cardModel : _gameModel.playfieldCards) {
        // 创建卡牌视图（传入点数、花色、是否正面朝上）
        auto* cardView = CardView::create(cardModel.face, cardModel.suit, cardModel.isFaceUp);
        cardView->setCardId(cardModel.id);  // 设置卡牌ID（用于识别卡牌）
        cardView->setPosition(Vec2(cardModel.posX, cardModel.posY));  // 设置卡牌位置
        playfieldView->addCard(cardView);  // 添加到主牌区视图
    }
    
    // 步骤3：创建底牌堆卡牌视图
    // 遍历模型中的每张底牌堆卡牌，创建对应的视图
    for (const auto& cardModel : _gameModel.stackCards) {
        // 创建卡牌视图
        auto* cardView = CardView::create(cardModel.face, cardModel.suit, cardModel.isFaceUp);
        cardView->setCardId(cardModel.id);  // 设置卡牌ID
        cardView->setPosition(Vec2(cardModel.posX, cardModel.posY));  // 设置卡牌位置
        stackView->addCard(cardView);  // 添加到底牌堆视图（会自动布局）
    }
    
    // 步骤4：更新回退按钮的显示状态
    // 如果可以回退，显示按钮；如果不能回退，隐藏按钮
    _gameView->showUndoButton(_undoManager.canUndo());
}

/**
 * 处理卡牌点击事件
 * 
 * 当玩家点击任何卡牌时，会调用这个方法
 * 这个方法会判断：
 * 1. 卡牌来自哪里（主牌区还是底牌堆）
 * 2. 执行什么操作（匹配还是换底牌）
 * 
 * @param cardId 被点击的卡牌ID
 */
void GameController::onCardClicked(int cardId) {
    // 检查视图是否有效
    if (!_gameView) {
        CCLOG("GameView为空");
        return;
    }
    
    CCLOG("========== 卡牌点击: cardId=%d ==========", cardId);
    
    // 获取主牌区视图和底牌堆视图
    auto playfieldView = _gameView->getPlayfieldView();
    auto stackView = _gameView->getStackView();
    
    // 检查视图是否有效
    if (!playfieldView || !stackView) {
        CCLOG("视图为空");
        return;
    }
    
    // 检查卡牌来源并执行相应操作
    CardView* clickedCard = playfieldView->findCardById(cardId);
    if (clickedCard) {
        // 主牌区卡牌点击：尝试与底牌堆顶部卡牌匹配
        CCLOG("卡牌来自主牌区，处理主牌区卡牌匹配");
        handlePlayfieldCardMatch(cardId);
    } else {
        // 在底牌堆查找
        clickedCard = stackView->findCardById(cardId);
        if (clickedCard) {
            CCLOG("卡牌来自底牌堆，处理底牌堆卡牌替换");
            handleStackCardReplace(cardId);
        } else {
            CCLOG("未找到卡牌: cardId=%d", cardId);
        }
    }
}

void GameController::handleStackCardReplace(int clickedCardId) {
    auto stackView = _gameView->getStackView();
    auto topCard = stackView->getTopCard();
    
    if (!topCard) {
        CCLOG("没有顶部底牌");
        return;
    }
    
    CCLOG("处理底牌点击: clickedCardId=%d, topCardId=%d", clickedCardId, topCard->getCardId());
    
    // 如果点击的是顶部牌，不允许操作（顶部牌只能用于匹配）
    if (topCard->getCardId() == clickedCardId) {
        CCLOG("点击的是顶部底牌，不能换底牌");
        return;
    }
    
    // 检查顶部底牌是否可以与主牌区匹配
    CardModel topCardModel = _gameModel.getCardById(topCard->getCardId());
    if (checkTopCardCanMatch(topCardModel)) {
        CCLOG("顶部底牌可以匹配，不允许换底牌");
        return;
    }
    
    CCLOG("顶部底牌无法匹配，允许换底牌");
    
    // 找到被点击的卡牌
    CardView* clickedCard = stackView->findCardById(clickedCardId);
    if (!clickedCard) return;
    
    // 获取卡牌信息
    CardModel clickedCardModel = _gameModel.getCardById(clickedCardId);
    
    // 找到被点击卡牌在底牌堆中的索引
    int clickedIndex = -1;
    for (size_t i = 0; i < _gameModel.stackCards.size(); i++) {
        if (_gameModel.stackCards[i].id == clickedCardId) {
            clickedIndex = (int)i;
            break;
        }
    }
    
    // 记录回退信息
    UndoRecord record = createStackReplaceRecord(clickedCardId, clickedCardModel, clickedIndex, topCard->getCardId());
    
    // 更新底牌堆中的卡牌顺序
    moveCardToStackTop(clickedCardId, clickedIndex);
    
    // 记录回退
    _undoManager.push(record);
    
    // 获取顶部卡牌位置（主底牌在右边）
    Vec2 topPos = Vec2(StackView::kStackWidth - 200, StackView::kStackHeight / 2);
    
    // 播放移动动画
    clickedCard->playMoveAnimation(topPos, [this, clickedCard, stackView, clickedCardId]() {
        // 动画完成后，重新布局
        stackView->layoutCards();
        _gameView->showUndoButton(true);
        
        // 验证顶部卡牌是否正确更新
        auto newTopCard = stackView->getTopCard();
        if (newTopCard) {
            CCLOG("换底牌完成，新的顶部卡牌ID: %d, 点数: %d", 
                  newTopCard->getCardId(), 
                  _gameModel.getCardById(newTopCard->getCardId()).face);
        } else {
            CCLOG("换底牌完成，但没有顶部卡牌");
        }
    });
}

void GameController::handlePlayfieldCardMatch(int playfieldCardId) {
    auto playfieldView = _gameView->getPlayfieldView();
    auto stackView = _gameView->getStackView();
    auto topCard = stackView->getTopCard();
    
    if (!topCard) return;
    
    CardModel playfieldCard = _gameModel.getCardById(playfieldCardId);
    CardModel stackCard = _gameModel.getCardById(topCard->getCardId());
    
    if (playfieldCard.id == -1 || stackCard.id == -1) return;
    
    // 检查是否可以匹配（点数差1）
    if (!canMatch(playfieldCard.face, stackCard.face)) {
        CCLOG("卡牌不匹配: %d 和 %d", playfieldCard.face, stackCard.face);
        return;
    }
    
    // 记录回退信息
    UndoRecord record = createPlayfieldMatchRecord(playfieldCardId, playfieldCard, stackCard);
    
    // 找到卡牌视图
    CardView* cardView = playfieldView->findCardById(playfieldCardId);
    if (!cardView) return;
    
    // 获取顶部卡牌位置
    Vec2 topPos = topCard->getPosition();
    
    // 更新模型：
    // 1. 将主牌区的卡牌移到底牌堆（成为新的顶部）
    // 2. 将原来的顶部卡牌从底牌堆移除（直接消失）
    _gameModel.removeCardFromPlayfield(playfieldCardId);
    
    // 原顶部卡牌ID，用于从视图中移除
    int oldTopCardId = stackCard.id;
    
    // 从底牌堆中移除原顶部卡牌（直接消失）
    _gameModel.removeCardFromStack(oldTopCardId);
    
    // 将主牌区的卡牌添加到底牌堆（成为新的顶部）
    CardModel newTopCard = playfieldCard;
    newTopCard.posX = 800;  // 主底牌位置
    newTopCard.posY = 290;
    _gameModel.addCardToStack(newTopCard);
    
    // 记录回退
    _undoManager.push(record);
    
    // 播放移动动画
    cardView->playMoveAnimation(topPos, [this, cardView, playfieldView, stackView, oldTopCardId]() {
        // 动画完成后：
        // 1. 从主牌区移除
        playfieldView->removeCard(cardView);
        
        // 2. 从底牌堆移除原顶部卡牌（直接消失）
        CardView* oldTopCardView = stackView->findCardById(oldTopCardId);
        if (oldTopCardView) {
            stackView->removeCard(oldTopCardView);
        }
        
        // 3. 将新卡牌添加到底牌堆（成为顶部）
        stackView->addCard(cardView);
        stackView->layoutCards();
        
        _gameView->showUndoButton(true);
        CCLOG("卡牌匹配完成，主牌区卡牌已移到底牌区顶部，原顶部卡牌已消失");
    });
}

void GameController::onUndoClicked() {
    if (!_undoManager.canUndo()) {
        return;
    }
    
    UndoRecord record = _undoManager.undo();
    
    // 根据操作类型执行相应的回退操作
    if (record.moveType == MoveType::STACK_REPLACE) {
        undoStackReplace(record);
    } else if (record.moveType == MoveType::PLAYFIELD_MATCH) {
        undoPlayfieldMatch(record);
    }
}

/**
 * 检查两张卡牌是否可以匹配
 * 
 * 匹配规则：两张卡牌的点数差1即可匹配（无花色要求）
 * 例如：A(1)和2可以匹配，2和3可以匹配，Q(12)和K(13)可以匹配
 * 
 * @param card1Face 第一张卡牌的点数（1-13）
 * @param card2Face 第二张卡牌的点数（1-13）
 * @return true=可以匹配, false=不能匹配
 */
bool GameController::canMatch(int card1Face, int card2Face) const {
    // 点数差1即可匹配（无花色要求）
    // std::abs()计算绝对值，确保无论哪个大哪个小都能匹配
    return std::abs(card1Face - card2Face) == 1;
}

/**
 * 初始化主牌区卡牌
 * 根据游戏配置创建主牌区的所有卡牌
 */
void GameController::initializePlayfieldCards() {
    // 主牌区左侧（3张卡牌）
    _gameModel.addCardToPlayfield(createCard(12, 0, 250, 1000));  // 梅花Q
    _gameModel.addCardToPlayfield(createCard(2, 0, 300, 800));   // 梅花2
    _gameModel.addCardToPlayfield(createCard(2, 1, 350, 600));   // 方块2
    
    // 主牌区右侧（3张卡牌）
    _gameModel.addCardToPlayfield(createCard(2, 0, 850, 1000));  // 梅花2
    _gameModel.addCardToPlayfield(createCard(2, 0, 800, 800));   // 梅花2
    _gameModel.addCardToPlayfield(createCard(1, 3, 750, 600));   // 黑桃A
}

/**
 * 初始化底牌堆卡牌
 * 根据游戏配置创建底牌堆的所有卡牌
 * 注意：底牌堆的卡牌顺序很重要，最后添加的卡牌是顶部卡牌（当前使用的）
 */
void GameController::initializeStackCards() {
    // 第一张备用底牌（最左边，最底层）
    _gameModel.addCardToStack(createCard(3, 0, 200, 290));  // 梅花3
    
    // 第二张备用底牌（中间）
    _gameModel.addCardToStack(createCard(1, 2, 200, 290));  // 红桃A
    
    // 主底牌（最右边，当前使用的顶部卡牌）
    // 注意：这是最后添加的，所以是顶部卡牌（vector的最后一个元素）
    _gameModel.addCardToStack(createCard(4, 0, 800, 290));  // 梅花4
}

/**
 * 创建卡牌模型
 * @param face 卡牌点数（1-13）
 * @param suit 卡牌花色（0-3）
 * @param posX X坐标
 * @param posY Y坐标
 * @return 创建的卡牌模型
 */
CardModel GameController::createCard(int face, int suit, float posX, float posY) {
    CardModel card;
    card.id = _gameModel.getNextCardId();
    card.face = face;
    card.suit = suit;
    card.isFaceUp = true;
    card.posX = posX;
    card.posY = posY;
    return card;
}

/**
 * 检查顶部底牌是否可以与主牌区匹配
 * @param topCardModel 顶部底牌模型
 * @return true=可以匹配, false=不能匹配
 */
bool GameController::checkTopCardCanMatch(const CardModel& topCardModel) const {
    auto playfieldView = _gameView->getPlayfieldView();
    if (!playfieldView) return false;
    
    for (auto* playfieldCard : playfieldView->getCards()) {
        CardModel playfieldCardModel = _gameModel.getCardById(playfieldCard->getCardId());
        if (canMatch(playfieldCardModel.face, topCardModel.face)) {
            return true;
        }
    }
    return false;
}

/**
 * 更新底牌堆中的卡牌顺序（将指定卡牌移到底部）
 * @param clickedCardId 被点击的卡牌ID
 * @param clickedIndex 卡牌在底牌堆中的索引
 */
void GameController::moveCardToStackTop(int clickedCardId, int clickedIndex) {
    auto stackView = _gameView->getStackView();
    if (!stackView) return;
    
    // 更新模型：将点击的卡牌移到底牌堆末尾（成为顶部）
    if (clickedIndex >= 0 && clickedIndex < (int)_gameModel.stackCards.size() - 1) {
        // 将卡牌移到最后
        CardModel temp = _gameModel.stackCards[clickedIndex];
        _gameModel.stackCards.erase(_gameModel.stackCards.begin() + clickedIndex);
        _gameModel.stackCards.push_back(temp);
        
        // 同步更新视图中的卡牌顺序
        CardView* clickedCard = stackView->findCardById(clickedCardId);
        if (clickedCard) {
            auto& cards = stackView->getCards();
            auto it = std::find(cards.begin(), cards.end(), clickedCard);
            if (it != cards.end() && it != cards.end() - 1) {
                // 将卡牌移到视图的最后（成为顶部）
                CardView* tempCard = *it;
                cards.erase(it);
                cards.push_back(tempCard);
            }
        }
    }
}

/**
 * 记录底牌替换的回退信息
 * @param clickedCardId 被点击的卡牌ID
 * @param clickedCardModel 被点击的卡牌模型
 * @param clickedIndex 卡牌在底牌堆中的索引
 * @param topCardId 顶部卡牌ID
 * @return 回退记录
 */
UndoRecord GameController::createStackReplaceRecord(int clickedCardId, const CardModel& clickedCardModel, 
                                                     int clickedIndex, int topCardId) {
    auto stackView = _gameView->getStackView();
    CardView* clickedCard = stackView ? stackView->findCardById(clickedCardId) : nullptr;
    
    UndoRecord record;
    record.cardId = clickedCardId;
    record.moveType = MoveType::STACK_REPLACE;
    record.originalPos = clickedCard ? clickedCard->getPosition() : Vec2::ZERO;
    record.originalParent = 1; // 底牌堆
    record.targetCardId = topCardId;
    record.cardFace = clickedCardModel.face;
    record.cardSuit = clickedCardModel.suit;
    record.originalStackIndex = clickedIndex;
    return record;
}

/**
 * 记录主牌区匹配的回退信息
 * @param playfieldCardId 主牌区卡牌ID
 * @param playfieldCard 主牌区卡牌模型
 * @param stackCard 顶部底牌模型
 * @return 回退记录
 */
UndoRecord GameController::createPlayfieldMatchRecord(int playfieldCardId, const CardModel& playfieldCard, 
                                                       const CardModel& stackCard) {
    UndoRecord record;
    record.cardId = playfieldCardId;
    record.moveType = MoveType::PLAYFIELD_MATCH;
    record.originalPos = Vec2(playfieldCard.posX, playfieldCard.posY);
    record.originalParent = 0; // 主牌区
    record.targetCardId = stackCard.id;
    record.cardFace = playfieldCard.face;
    record.cardSuit = playfieldCard.suit;
    record.oldTopCardFace = stackCard.face;
    record.oldTopCardSuit = stackCard.suit;
    return record;
}

/**
 * 执行底牌替换的回退操作
 * @param record 回退记录
 */
void GameController::undoStackReplace(const UndoRecord& record) {
    auto stackView = _gameView->getStackView();
    if (!stackView) return;
    
    CardView* cardView = stackView->findCardById(record.cardId);
    if (!cardView) return;
    
    // 恢复底牌堆中的顺序：将卡牌移回原来的索引位置
    if (record.originalStackIndex >= 0) {
        int currentIndex = -1;
        for (size_t i = 0; i < _gameModel.stackCards.size(); i++) {
            if (_gameModel.stackCards[i].id == record.cardId) {
                currentIndex = (int)i;
                break;
            }
        }
        
        if (currentIndex >= 0 && record.originalStackIndex < (int)_gameModel.stackCards.size()) {
            CardModel temp = _gameModel.stackCards[currentIndex];
            _gameModel.stackCards.erase(_gameModel.stackCards.begin() + currentIndex);
            if (record.originalStackIndex <= (int)_gameModel.stackCards.size()) {
                _gameModel.stackCards.insert(_gameModel.stackCards.begin() + record.originalStackIndex, temp);
            } else {
                _gameModel.stackCards.push_back(temp);
            }
        }
    }
    
    // 播放动画回到原位置
    Vec2 originalPos = record.originalPos;
    cardView->playMoveAnimation(originalPos, [this, cardView, stackView]() {
        stackView->layoutCards();
        _gameView->showUndoButton(_undoManager.canUndo());
    });
}

/**
 * 执行主牌区匹配的回退操作
 * @param record 回退记录
 */
void GameController::undoPlayfieldMatch(const UndoRecord& record) {
    auto playfieldView = _gameView->getPlayfieldView();
    auto stackView = _gameView->getStackView();
    if (!playfieldView || !stackView) return;
    
    CardView* cardView = stackView->findCardById(record.cardId);
    if (!cardView) return;
    
    // 恢复主牌区的卡牌模型
    CardModel originalCard;
    originalCard.id = record.cardId;
    originalCard.face = record.cardFace;
    originalCard.suit = record.cardSuit;
    originalCard.posX = record.originalPos.x;
    originalCard.posY = record.originalPos.y;
    originalCard.isFaceUp = true;
    
    // 恢复原顶部卡牌模型（被移除的卡牌）
    CardModel oldTopCard;
    oldTopCard.id = record.targetCardId;
    oldTopCard.face = record.oldTopCardFace;
    oldTopCard.suit = record.oldTopCardSuit;
    oldTopCard.posX = 800;  // 主底牌位置
    oldTopCard.posY = 290;
    oldTopCard.isFaceUp = true;
    
    // 从底牌堆移除当前顶部卡牌（主牌区的卡牌）
    _gameModel.removeCardFromStack(record.cardId);
    
    // 恢复原顶部卡牌到底牌堆
    _gameModel.addCardToStack(oldTopCard);
    
    // 恢复主牌区的卡牌
    _gameModel.addCardToPlayfield(originalCard);
    
    // 将主牌区的卡牌移回主牌区
    Vec2 originalPos = record.originalPos;
    cardView->playMoveAnimation(originalPos, [this, cardView, playfieldView, stackView, oldTopCard]() {
        // 从底牌堆移除主牌区的卡牌
        stackView->removeCard(cardView);
        
        // 创建并恢复原顶部卡牌到底牌堆
        CardView* oldTopCardView = CardView::create(oldTopCard.face, oldTopCard.suit, oldTopCard.isFaceUp);
        if (oldTopCardView) {
            oldTopCardView->setCardId(oldTopCard.id);
            oldTopCardView->setPosition(Vec2(oldTopCard.posX, oldTopCard.posY));
            stackView->addCard(oldTopCardView);
        }
        
        // 将主牌区的卡牌添加回主牌区
        playfieldView->addCard(cardView);
        
        stackView->layoutCards();
        _gameView->showUndoButton(_undoManager.canUndo());
    });
}

