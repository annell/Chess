//
// Created by Stefan Annell on 2021-02-20.
//

#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <vector>
#include <array>
#include <memory>
#include <cstddef>
#include <string>
#include "Chess/Piece.h"

struct Position{
    int row;
    int col;

    std::string operator()() {
        return "(" + std::to_string(row) + ", " + std::to_string(col) + ")";
    }

    std::string toFen() {
        return "NotImplemented";
    }
};

enum MoveType {
    Normal,
    Capture,
    Castle,
    Enpersand,
};

struct Move {
   int index;
   MoveType type;
};

struct PieceState {
    char piece;
    int index;
    std::vector<Move> moves;

    bool operator==(const PieceState& rhs) const {
        return std::tie(piece, index) <
               std::tie(rhs.piece, rhs.index);
    }
};

struct CastlingSide {
    CastlingSide(bool k, bool q) : king(k), queen(q) {
    }

    bool king = false;
    bool queen = false;
};

struct Castling {
    CastlingSide white;
    CastlingSide black;

    bool canCastle(const PieceState& state) {
        Color color = Piece::getColor(state.piece);
        Type type = Piece::getType(state.piece);
        if (type == Type::King) {
            if (color == Color::White) {
                return white.queen || white.king;
            } else {
                return black.queen || black.king;
            }
        }
        return false;
    }

    std::string toFen() {
        std::string output;
        if (white.king) {
            output += "K";
        }
        if (white.queen) {
            output += "Q";
        }

        if (black.king) {
            output += "k";
        }
        if (black.queen) {
            output += "q";
        }
        return output;
    }
};

class Board {
public:
    typedef std::array<char, 64> Data;
    Board(const std::string&);
    std::string operator()();
    Board& operator=(Board other);
    bool MovePiece(const Position& from, const Position& to);
    std::vector<PieceState> GetActivePlayerMoves();
    void DoMove(const PieceState&, const Move&);
    std::string toFen();
    bool isWhiteActive();
    void UndoMove();

    const std::vector<PieceState>& getWhitePieces();
    const std::vector<PieceState>& getBlackPieces();

    bool isOver;
    Castling castling;
private:
    void MakeBoard();
    void MakeFen();
    void GenerateMoves(PieceState&);

    std::string fen;
    Data board;
    bool whitesMove;
    int enPersantIndex;
    bool checked;
    std::vector<PieceState> whitePieces;
    std::vector<PieceState> blackPieces;
};

#endif //CHESS_BOARD_H
