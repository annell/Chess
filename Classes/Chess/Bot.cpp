//
// Created by Stefan Annell on 2021-02-21.
//

#include <iostream>
#include "Chess/Bot.h"
#include "Chess/Board.h"
#include  <random>
#include  <iterator>

#include <thread>
#include <future>

namespace internal {
int Evaluation(Board& board, const std::vector<PieceState>& mine, const std::vector<PieceState>& theirs) {
    auto evalPieces = [] (const std::vector<PieceState>& pieces) {
        int val = 0;
        for (const auto& piece : pieces) {
            switch (Piece::getType(piece.piece)) {
                case Type::Pawn:
                    val += 100;
                    break;
                case Type::Knight:
                    val += 300;
                    break;
                case Type::Bishop:
                    val += 300;
                    break;
                case Type::Rook:
                    val += 500;
                    break;
                case Type::Queen:
                    val += 900;
                    break;
                case Type::King:
                    val += 1000000;
                    break;
                case Type::Nothing:
                default:
                    break;
            }
        }
        return val;
    };
    auto castling = [&] () {
        return 0;
        int balance = 0;
        balance += board.castling.white.queen ? 10 : -10;
        balance += board.castling.white.king ? 10 : -10;
        balance += board.castling.black.queen ? -10 : 10;
        balance += board.castling.black.king ? -10 : 10;
        return balance;
    };
    return castling() + evalPieces(mine) - evalPieces(theirs);
}

int MinMax(Board& board, bool isMaxTurn, int depth, int& counter) {
    if (board.isOver) {
        return isMaxTurn ? INT32_MIN : INT32_MAX;
    }
    if (--depth <= 0) {
        counter++;
        return Evaluation(board, board.getWhitePieces(), board.getBlackPieces());
    }
    int minMaxScore = isMaxTurn ? INT32_MIN : INT32_MAX;

    for (auto& state : board.GetActivePlayerMoves()) {
        for (const auto& move : state.moves) {
            auto temp = board;
            temp.DoMove(state, move);
            if (isMaxTurn) {
                minMaxScore = std::max(-MinMax(temp, false, depth, counter), minMaxScore);
            } else {
                minMaxScore = std::min(MinMax(temp, true, depth, counter), minMaxScore);
            }
        }
    }
    return minMaxScore;
}

int AlphaBetaPruning(Board board, bool isMaxTurn, int depth, int& counter, int alpha, int beta) {
    if (board.isOver) {
        return isMaxTurn ? INT32_MIN : INT32_MAX;
    }
    if (--depth <= 0) {
        counter++;
        return Evaluation(board, board.getWhitePieces(), board.getBlackPieces());
    }
    int minMaxScore = isMaxTurn ? INT32_MIN : INT32_MAX;

    for (auto& state : board.GetActivePlayerMoves()) {
        for (const auto& move : state.moves) {
            auto t = board;
            t.DoMove(state, move);
            if (isMaxTurn) {
                minMaxScore = std::max(-AlphaBetaPruning(t, false, depth, counter, alpha, beta), minMaxScore);
                alpha = std::max(alpha, minMaxScore);
            } else {
                minMaxScore = std::min(AlphaBetaPruning(t, true, depth, counter, alpha, beta), minMaxScore);
                beta = std::min(beta, minMaxScore);
            }
            if (beta <= alpha) {
                return minMaxScore;
            }
        }
    }
    return minMaxScore;
}

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

}

namespace Bot {
Results GetBestMove(Board& board, int depth, int& counter) {
    Results best;
    int bestScore = INT32_MIN;
    struct data {
        std::future<int> call;
        Bot::Results results;
    };

    std::vector<data> scores;
    for (auto& state : board.GetActivePlayerMoves()) {
        for (const auto& move : state.moves) {
            auto t = board;
            t.DoMove(state, move);
            //auto score = internal::MinMax(t, false, depth, counter);
            //auto score = internal::AlphaBetaPruning(t, false, depth, counter, INT32_MIN, INT32_MAX);
            std::future<int> asyncCall = std::async(std::launch::async, [=, &counter] () {
                return internal::AlphaBetaPruning(t, false, depth, counter, INT32_MIN, INT32_MAX);
            });
            scores.push_back({std::move(asyncCall), Bot::Results{state, move, 0}});
        }
    }
    std::vector<Results> filtered;
    for (auto& pair : scores) {
        auto score = pair.call.get();
        if (score > bestScore) {
            bestScore = score;
            best = pair.results;
            best.score = score;
            filtered.empty();
            filtered.push_back(pair.results);
        } else if (score == bestScore) {
            filtered.push_back(pair.results);
        }
    }

    //std::cout << "selected:" << best.state.index << " -> " << best.move.index << " = " << best.score << std::endl;
    //return *internal::select_randomly(filtered.begin(), filtered.end());
    return best;
}

}