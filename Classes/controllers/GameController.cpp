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
    
    // ========== 根据设计文档创建主牌区卡牌 ==========
    // 主牌区左侧（3张卡牌）
    
    // 第一张卡牌：梅花Q（Queen，皇后）
    CardModel card1;
    card1.id = _gameModel.getNextCardId();  // 获取新的唯一ID
    card1.face = 12;  // Q（Queen，皇后）
    card1.suit = 0;   // 梅花（Club，♣）
    card1.isFaceUp = true;  // 正面朝上（可以看到点数）
    card1.posX = 250;  // X坐标（屏幕左侧）
    card1.posY = 1000; // Y坐标（屏幕中上部）
    _gameModel.addCardToPlayfield(card1);  // 添加到主牌区
    
    // 第二张卡牌：梅花2
    CardModel card2;
    card2.id = _gameModel.getNextCardId();
    card2.face = 2;   // 2（数字2）
    card2.suit = 0;   // 梅花（Club，♣）
    card2.isFaceUp = true;
    card2.posX = 300;  // 稍微靠右一点
    card2.posY = 800;   // 稍微靠下一点
    _gameModel.addCardToPlayfield(card2);
    
    // 第三张卡牌：方块2
    CardModel card3;
    card3.id = _gameModel.getNextCardId();
    card3.face = 2;   // 2（数字2）
    card3.suit = 1;   // 方块（Diamond，♦）
    card3.isFaceUp = true;
    card3.posX = 350;  // 再靠右一点
    card3.posY = 600;   // 再靠下一点（形成阶梯状）
    _gameModel.addCardToPlayfield(card3);
    
    // 主牌区右侧（3张卡牌）
    
    // 第四张卡牌：梅花2
    CardModel card4;
    card4.id = _gameModel.getNextCardId();
    card4.face = 2;   // 2（数字2）
    card4.suit = 0;   // 梅花（Club，♣）
    card4.isFaceUp = true;
    card4.posX = 850;  // 屏幕右侧
    card4.posY = 1000; // 屏幕中上部
    _gameModel.addCardToPlayfield(card4);
    
    // 第五张卡牌：梅花2
    CardModel card5;
    card5.id = _gameModel.getNextCardId();
    card5.face = 2;   // 2（数字2）
    card5.suit = 0;   // 梅花（Club，♣）
    card5.isFaceUp = true;
    card5.posX = 800;  // 稍微靠左一点
    card5.posY = 800;  // 稍微靠下一点
    _gameModel.addCardToPlayfield(card5);
    
    // 第六张卡牌：黑桃A（Ace，尖）
    CardModel card6;
    card6.id = _gameModel.getNextCardId();
    card6.face = 1;   // A（Ace，尖）
    card6.suit = 3;   // 黑桃（Spade，♠）
    card6.isFaceUp = true;
    card6.posX = 750;  // 再靠左一点
    card6.posY = 600;  // 再靠下一点（形成阶梯状）
    _gameModel.addCardToPlayfield(card6);
    
    // ========== 底牌堆：共3张卡牌 ==========
    // 注意：底牌堆的卡牌顺序很重要，最后添加的卡牌是顶部卡牌（当前使用的）
    
    // 第一张备用底牌（最左边，最底层）
    CardModel stackCard1;
    stackCard1.id = _gameModel.getNextCardId();
    stackCard1.face = 3;   // 3（数字3）
    stackCard1.suit = 0;   // 梅花（Club，♣）
    stackCard1.isFaceUp = true;
    stackCard1.posX = 200;  // 左边位置（备用底牌在左边）
    stackCard1.posY = 290;  // 底牌区Y坐标
    _gameModel.addCardToStack(stackCard1);  // 添加到底牌堆（第一个）
    
    // 第二张备用底牌（中间）
    CardModel stackCard2;
    stackCard2.id = _gameModel.getNextCardId();
    stackCard2.face = 1;   // A（Ace，尖）
    stackCard2.suit = 2;   // 红桃（Heart，♥）
    stackCard2.isFaceUp = true;
    stackCard2.posX = 200;  // 左边位置（备用底牌在左边，会垂直排列）
    stackCard2.posY = 290;
    _gameModel.addCardToStack(stackCard2);  // 添加到底牌堆（第二个）
    
    // 主底牌（最右边，当前使用的顶部卡牌）
    // 注意：这是最后添加的，所以是顶部卡牌（vector的最后一个元素）
    CardModel stackCard3;
    stackCard3.id = _gameModel.getNextCardId();
    stackCard3.face = 4;   // 4（数字4）
    stackCard3.suit = 0;   // 梅花（Club，♣）
    stackCard3.isFaceUp = true;
    stackCard3.posX = 800;  // 右边位置（主底牌在右边）
    stackCard3.posY = 290;  // 底牌区Y坐标
    _gameModel.addCardToStack(stackCard3);  // 添加到底牌堆（第三个，成为顶部卡牌）
    
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
    
    // 步骤1：检查卡牌来源
    // 先在主牌区查找这张卡牌
    CardView* clickedCard = playfieldView->findCardById(cardId);
    bool fromPlayfield = (clickedCard != nullptr);  // 如果找到了，说明来自主牌区
    
    if (!fromPlayfield) {
        // 如果主牌区没找到，再在底牌堆查找
        clickedCard = stackView->findCardById(cardId);
        if (clickedCard) {
            CCLOG("卡牌来自底牌堆");
        } else {
            // 两个地方都没找到，说明卡牌不存在
            CCLOG("未找到卡牌: cardId=%d", cardId);
            return;
        }
    } else {
        CCLOG("卡牌来自主牌区");
    }
    
    // 检查卡牌视图是否有效
    if (!clickedCard) {
        CCLOG("卡牌视图为空");
        return;
    }
    
    // 步骤2：根据卡牌来源判断操作类型
    if (fromPlayfield) {
        // 主牌区卡牌点击：尝试与底牌堆顶部卡牌匹配
        CCLOG("处理主牌区卡牌匹配");
        handlePlayfieldCardMatch(cardId);
    } else {
        // 底牌堆卡牌点击：尝试替换顶部底牌
        CCLOG("处理底牌堆卡牌替换");
        handleStackCardReplace(cardId);
    }
}

void GameController::handleStackCardReplace(int clickedCardId) {
    auto stackView = _gameView->getStackView();
    auto playfieldView = _gameView->getPlayfieldView();
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
    // 需要重新获取topCard，因为可能已经换过底牌了
    topCard = stackView->getTopCard();
    if (!topCard) {
        CCLOG("重新获取顶部底牌失败");
        return;
    }
    
    bool topCanMatch = false;
    CardModel topCardModel = _gameModel.getCardById(topCard->getCardId());
    CCLOG("检查顶部底牌(点数=%d)是否可以匹配", topCardModel.face);
    for (auto* playfieldCard : playfieldView->getCards()) {
        CardModel playfieldCardModel = _gameModel.getCardById(playfieldCard->getCardId());
        CCLOG("  检查主牌区卡牌(点数=%d)", playfieldCardModel.face);
        if (canMatch(playfieldCardModel.face, topCardModel.face)) {
            topCanMatch = true;
            CCLOG("  可以匹配！");
            break;
        }
    }
    
    // 如果顶部底牌可以匹配，不允许换底牌
    if (topCanMatch) {
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
    UndoRecord record;
    record.cardId = clickedCardId;
    record.moveType = MoveType::STACK_REPLACE;
    record.originalPos = clickedCard->getPosition();
    record.originalParent = 1; // 底牌堆
    record.targetCardId = topCard->getCardId();
    record.cardFace = clickedCardModel.face;
    record.cardSuit = clickedCardModel.suit;
    record.originalStackIndex = clickedIndex;
    
    // 先更新模型：将点击的卡牌移到底牌堆末尾（成为顶部）
    if (clickedIndex >= 0 && clickedIndex < (int)_gameModel.stackCards.size() - 1) {
        // 将卡牌移到最后
        CardModel temp = _gameModel.stackCards[clickedIndex];
        _gameModel.stackCards.erase(_gameModel.stackCards.begin() + clickedIndex);
        _gameModel.stackCards.push_back(temp);
        
        // 同步更新视图中的卡牌顺序
        // 找到被点击的卡牌在视图中的位置
        auto& cards = stackView->getCards();
        auto it = std::find(cards.begin(), cards.end(), clickedCard);
        if (it != cards.end() && it != cards.end() - 1) {
            // 将卡牌移到视图的最后（成为顶部）
            CardView* tempCard = *it;
            cards.erase(it);
            cards.push_back(tempCard);
            CCLOG("更新视图顺序：将卡牌ID=%d移到底牌堆顶部", clickedCardId);
        }
    }
    
    // 记录回退
    _undoManager.push(record);
    
    // 获取顶部卡牌位置（主底牌在右边）
    Vec2 topPos = Vec2(StackView::STACK_WIDTH - 200, StackView::STACK_HEIGHT / 2);
    
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
    UndoRecord record;
    record.cardId = playfieldCardId;
    record.moveType = MoveType::PLAYFIELD_MATCH;
    record.originalPos = Vec2(playfieldCard.posX, playfieldCard.posY);
    record.originalParent = 0; // 主牌区
    record.targetCardId = stackCard.id;
    record.cardFace = playfieldCard.face;
    record.cardSuit = playfieldCard.suit;
    // 保存原顶部卡牌信息，用于回退时恢复
    record.oldTopCardFace = stackCard.face;
    record.oldTopCardSuit = stackCard.suit;
    
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
    
    auto playfieldView = _gameView->getPlayfieldView();
    auto stackView = _gameView->getStackView();
    
    CardView* cardView = nullptr;
    
    if (record.moveType == MoveType::STACK_REPLACE) {
        // 回退手牌区翻牌替换
        cardView = stackView->findCardById(record.cardId);
        if (cardView) {
            // 恢复底牌堆中的顺序：将卡牌移回原来的索引位置
            if (record.originalStackIndex >= 0) {
                // 找到当前顶部卡牌（目标卡牌）
                CardModel targetCard = _gameModel.getCardById(record.targetCardId);
                
                // 将当前顶部卡牌（被点击的卡牌）移回原来的位置
                // 找到它在vector中的当前位置（应该在最后）
                int currentIndex = -1;
                for (size_t i = 0; i < _gameModel.stackCards.size(); i++) {
                    if (_gameModel.stackCards[i].id == record.cardId) {
                        currentIndex = (int)i;
                        break;
                    }
                }
                
                if (currentIndex >= 0 && record.originalStackIndex < (int)_gameModel.stackCards.size()) {
                    // 将卡牌移回原来的索引位置
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
    } else if (record.moveType == MoveType::PLAYFIELD_MATCH) {
        // 回退桌面牌匹配
        cardView = stackView->findCardById(record.cardId);
        if (cardView) {
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

