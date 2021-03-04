#ifndef __CHESS_SCENE_H__
#define __CHESS_SCENE_H__

#include <Chess/Board.h>
#include "cocos2d.h"
#include "Square.h"

class Board;

class ChessScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(ChessScene);
private:
    void onTouchesBegan(const std::vector<cocos2d::Touch*>&, cocos2d::Event*);
    void onTouchesMoved(const std::vector<cocos2d::Touch*>&, cocos2d::Event*);
    void onTouchesEnded(const std::vector<cocos2d::Touch*>&, cocos2d::Event*);

    void highlightPossibleMoves();
    void resetHighlight();

    Square* findSquare(const cocos2d::Vec2& point);
    void DoMove(PieceState, Move);

    std::vector<Square> squares;
    Square* selectedSquare = nullptr;
    Board* board = nullptr;
};

#endif // __CHESS_SCENE_H__
