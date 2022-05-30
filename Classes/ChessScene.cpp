#include "ChessScene.h"
#include "Chess/Piece.h"
#include "Chess/Board.h"
#include "Chess/Bot.h"
#include <iostream>

USING_NS_CC;

Scene* ChessScene::createScene()
{
    return ChessScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

namespace internal
{
#define PATH std::string("/Users/stan/dev/C++/ChessGUI/Resources/")
#define END std::string(".png")
std::string getNameFromColor(Color color) {
    if (color == Color::Black) {
        return "b";
    }
    return "w";
}
std::string getSprite(Type type, Color color) {
    switch(type) {
        case Pawn:
            return PATH + "p" + getNameFromColor(color) + END;
        case Knight:
            return PATH + "n" + getNameFromColor(color) + END;
        case Bishop:
            return PATH + "b" + getNameFromColor(color) + END;
        case Rook:
            return PATH + "r" + getNameFromColor(color) + END;
        case Queen:
            return PATH + "q" + getNameFromColor(color) + END;
        case King:
            return PATH + "k" + getNameFromColor(color) + END;
        case Nothing:
        default:
            return "";
    }

}

}

// on "init" you need to initialize your instance
bool ChessScene::init()
{
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    board = new Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    //Board board("rnbqkbnr/pppppppp/7P/8/8/8/PPPPPPP1/RNBQKBNR b KQkq - 0 1");

    // listen for touch events
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = CC_CALLBACK_2(ChessScene::onTouchesBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(ChessScene::onTouchesMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(ChessScene::onTouchesEnded, this);
    listener->onTouchesCancelled = CC_CALLBACK_2(ChessScene::onTouchesEnded, this);
    this->getEventDispatcher()->
            addEventListenerWithSceneGraphPriority(listener, this);

    Color4F white(1, 1, 1, 1);
    Color4F black(0.4, 0.133, 0, 1);
    bool whiteSquare = true;
    int stepWidth = visibleSize.width / 8;
    int stepHeight = visibleSize.height / 8;
    int i = 0;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            squares.push_back(Square(stepWidth, stepHeight, col * stepWidth, row * stepHeight, i++, whiteSquare ? white : black));
            auto& square = squares.back();
            this->addChild(square.CreateNode());
            whiteSquare = !whiteSquare;
        }
        whiteSquare = !whiteSquare;
    }
    for (const auto& state : board->getWhitePieces()) {
        auto piece = Piece::getType(state.piece);
        this->addChild(squares.at(state.index).CreateSprite(internal::getSprite(piece, Color::White)), 2, state.index);
    }
    for (const auto& state : board->getBlackPieces()) {
        auto piece = Piece::getType(state.piece);
        this->addChild(squares.at(state.index).CreateSprite(internal::getSprite(piece, Color::Black)), 2, state.index);
    }
    return true;
}

void ChessScene::highlightPossibleMoves() {
    for (auto state : board->GetActivePlayerMoves()) {
        if (selectedSquare->index == state.index) {
            for (auto move : state.moves) {
                squares.at(move.index).Select();
            }
        }
    }
}

void ChessScene::resetHighlight() {
    for (auto square : squares) {
        square.DeSelect();
    }
}

Square* ChessScene::findSquare(const cocos2d::Vec2& point) {
    for (auto& square : squares) {
        if (square.ContainsPoint(point)) {
            return &square;
        }
    }
    return nullptr;
}

void ChessScene::DoMove(PieceState pieceState, Move move) {
    board->DoMove(pieceState, move);
    switch (move.type) {
        case MoveType::Castle:
        {
            auto type = Piece::getType(pieceState.piece);
            auto color = Piece::getColor(pieceState.piece);
            auto rookPlacement = [] (int index) {
                switch (index) {
                    case 62:
                        return 61;
                    case 58:
                        return 59;
                    case 6:
                        return 5;
                    case 2:
                        return 3;
                }
            }(move.index);
            auto oldRookPlacement = [] (int index) {
                switch (index) {
                    case 62:
                        return 63;
                    case 58:
                        return 56;
                    case 6:
                        return 7;
                    case 2:
                        return 0;
                }
            }(move.index);
            if (auto child = this->getChildByTag(oldRookPlacement)){
                // Remove old rook
                this->removeChild(child);
            }
            // Add new rook
            this->addChild(squares.at(rookPlacement).CreateSprite(internal::getSprite(Type::Rook, color)), 2, rookPlacement);
            // Remove old king
            this->removeChildByTag(pieceState.index);
            // Add new king
            this->addChild(squares.at(move.index).CreateSprite(internal::getSprite(type, color)), 2, move.index);
            break;
        }
        case MoveType::Enpersand: {
            auto oldPawnPlacement = [&] () {
                Color color = Piece::getColor(pieceState.piece);
                return move.index + (color == Color::White ? 8 : -8);
            }();
            if (auto child = this->getChildByTag(oldPawnPlacement)){
                // Remove old pawn
                this->removeChild(child);
            }
        }
        case MoveType::Capture:
            if (auto child = this->getChildByTag(move.index)){
                this->removeChild(child);
            }
        case MoveType::Normal:
        default:
            auto type = Piece::getType(pieceState.piece);
            auto color = Piece::getColor(pieceState.piece);
            auto pawnUpgrade = [&] () {
                return type == Type::Pawn && (
                        move.index >= 0 && move.index < 8 ||
                        move.index >= 56 && move.index < 64);
            }();
            if (pawnUpgrade) {
                type = Type::Queen;
            }
            this->removeChildByTag(pieceState.index);
            this->addChild(squares.at(move.index).CreateSprite(internal::getSprite(type, color)), 2, move.index);
    }
}

void ChessScene::onTouchesBegan(const std::vector<cocos2d::Touch*>& touch, cocos2d::Event* event) {
    auto point = touch.front()->getLocation();
    if (auto square = findSquare(point)) {
        for (auto state : board->GetActivePlayerMoves()) {
            if (square->index == state.index) {
                auto* sprite = this->getChildByTag<cocos2d::Sprite*>(square->index);
                if (square->TrySetSpritePosition(point, sprite)) {
                    selectedSquare = square;
                    highlightPossibleMoves();
                    break;
                }
            }
        }
    }
}

void ChessScene::onTouchesMoved(const std::vector<cocos2d::Touch*>& touch, cocos2d::Event* event) {
    auto point = touch.front()->getLocation();
    if (selectedSquare) {
        auto* sprite = this->getChildByTag<cocos2d::Sprite*>(selectedSquare->index);
        selectedSquare->TrySetSpritePosition(point, sprite);
    }
}

void ChessScene::onTouchesEnded(const std::vector<cocos2d::Touch*>& touch, cocos2d::Event* event) {
    auto point = touch.front()->getLocation();
    if (selectedSquare) {
        PieceState pieceState;
        for (auto state : board->GetActivePlayerMoves()) {
            if (selectedSquare->index == state.index) {
                pieceState = state;
            }
        }
        //Square* target = nullptr;

        bool found = false;
        if (auto targetSquare = findSquare(point)) {
            for (auto move : pieceState.moves) {
                if (targetSquare->index == move.index) {
                    // My move
                    DoMove(pieceState, move);

                    // Bot move
                    Bot::Results results;
                    std::cout << "===== Calulating best move ====" << std::endl;
                    for (int i = 1; i < 5; i++) {
                        int counter = 1;
                        auto start = std::chrono::high_resolution_clock::now();
                        results = Bot::GetBestMove(*board, i, counter);
                        auto stop = std::chrono::high_resolution_clock::now();
                        auto duration = duration_cast<std::chrono::milliseconds>(stop - start);
                        std::cout << "At depth: " << i << " Processed moves: " << counter << " ";
                        std::cout << "Time spent: " << duration.count() << " ms ";
                        std::cout << "Time per move: " << (int)(duration.count() * 1000 / counter) << " microseconds ";
                        std::cout << "Score: " << results.score << std::endl;
                        found = true;
                    }
                    DoMove(results.state, results.move);
                    break;
                }
            }
        }

        if (!found) {
            auto* sprite = this->getChildByTag<cocos2d::Sprite*>(selectedSquare->index);
            selectedSquare->SetSpriteOrigin(sprite);
        }
        resetHighlight();
        selectedSquare = nullptr;
    }
}
