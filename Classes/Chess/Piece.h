//
// Created by Stefan Annell on 2021-02-20.
//

#ifndef CHESS_PIECE_H
#define CHESS_PIECE_H

#include <string>
#include <map>
#include <vector>

enum Type {
    Nothing = 0,
    Pawn = 1,
    Knight = 2,
    Bishop = 3,
    Rook = 4,
    Queen = 5,
    King = 6,
};
constexpr char TypeMask = 0x0f;

enum Color {
    Black = 0,
    White = 0x80,
};
constexpr char ColorMask = 0x80;


namespace MoveDirection {
    struct Move {
        int col;
        int row;
        Move& operator+(const Move& rhs){
            col += rhs.col;
            row += rhs.row;
            return *this;
        }
        bool isValid() {
            return (col < 8 && col >= 0) && (row < 8 && row >= 0);
        }
    };
}


class Piece {
public:
    static char MakePiece(Color, Type);
    static Color getColor(const char&);
    static Type getType(const char&);
    static const std::vector<MoveDirection::Move>& getMoveDirections(const char&);

    static char getPieceFromFen(const char&);
    static bool isPieceFen(const char&);
    static char toFen(const char&);

private:
    Color color;
    Type type;
};

static const char EMPTY = Piece::MakePiece(Color::Black, Type::Nothing);


#endif //CHESS_PIECE_H
