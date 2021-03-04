//
// Created by Stefan Annell on 2021-02-21.
//

#ifndef CHESS_BOT_H
#define CHESS_BOT_H
#include "Chess/Board.h"

namespace Bot {
struct Results {
    Results() {}
    Results(const PieceState state, const Move move, int i)
        : state(state), move(move), score(i) {
    }

    PieceState state;
    Move move;
    int score = -1000000000;

    bool operator<(const Results& rhs) {
        return std::tie(score) < std::tie(rhs.score);
    }
};
Results GetBestMove(Board&, int depth, int& counter);
}


#endif //CHESS_BOT_H
