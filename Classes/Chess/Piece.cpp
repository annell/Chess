//
// Created by Stefan Annell on 2021-02-20.
//

#include "Chess/Piece.h"

namespace internal {

char getFenColor(char piece, Color color) {
    if (color == Color::Black) {
        return tolower(piece);
    }
    return toupper(piece);
}

char getPieceFen(Color color, Type type) {
    switch(type) {
        case Type::Pawn:
            return getFenColor('p', color);
        case Type::Knight:
            return getFenColor('n', color);
        case Type::Bishop:
            return getFenColor('b', color);
        case Type::Rook:
            return getFenColor('r', color);
        case Type::Queen:
            return getFenColor('q', color);
        case Type::King:
            return getFenColor('k', color);
        case Type::Nothing:
        default:
            return ' ';
    }
}

Type getTypeFromFen(const char& c) {
    switch (tolower(c)) {
        case 'p':
            return Type::Pawn;
        case 'n':
            return Type::Knight;
        case 'b':
            return Type::Bishop;
        case 'r':
            return Type::Rook;
        case 'q':
            return Type::Queen;
        case 'k':
            return Type::King;
        default:
            return Type::Nothing;
    }
}

MoveDirection::Move PawnForward = {0, 1};
MoveDirection::Move PawnBackward = {0 , -1};
MoveDirection::Move Forward = {0, 1};
MoveDirection::Move Backward = {0, -1};
MoveDirection::Move Left = {-1, 0};
MoveDirection::Move Right = {1, 0};
MoveDirection::Move DiagonalTopRight = {1, 1};
MoveDirection::Move DiagonalTopleft = {-1, 1};
MoveDirection::Move DiagonalBottomRight = {1, -1};
MoveDirection::Move DiagonalBottomleft = {-1, -1};
MoveDirection::Move KnightL1 = {-1, 2};
MoveDirection::Move KnightL2 = {1, 2};
MoveDirection::Move KnightL3 = {2, 1};
MoveDirection::Move KnightL4 = {2, -1};
MoveDirection::Move KnightL5 = {1, -2};
MoveDirection::Move KnightL6 = {-1, -2};
MoveDirection::Move KnightL7 = {-2, -1};
MoveDirection::Move KnightL8 = {-2, 1};

const std::map<Type, std::vector<MoveDirection::Move>> PieceMovement {
        std::make_pair<Type, std::vector<MoveDirection::Move>>(Type::Pawn , {
                PawnForward
        }),
        std::make_pair<Type, std::vector<MoveDirection::Move>>(Type::Knight , {
                KnightL1,
                KnightL2,
                KnightL3,
                KnightL4,
                KnightL5,
                KnightL6,
                KnightL7,
                KnightL8,
        }),
        std::make_pair<Type, std::vector<MoveDirection::Move>>(Type::Bishop , {
                DiagonalBottomRight,
                DiagonalBottomleft,
                DiagonalTopRight,
                DiagonalTopleft
        }),
        std::make_pair<Type, std::vector<MoveDirection::Move>>(Type::Rook , {
                Forward,
                Backward,
                Left,
                Right
        }),
        std::make_pair<Type, std::vector<MoveDirection::Move>>(Type::Queen , {
                Forward,
                Backward,
                Left,
                Right,
                DiagonalBottomRight,
                DiagonalBottomleft,
                DiagonalTopRight,
                DiagonalTopleft
        }),
        std::make_pair<Type, std::vector<MoveDirection::Move>>(Type::King , {
                Forward,
                Backward,
                Left,
                Right,
                DiagonalBottomRight,
                DiagonalBottomleft,
                DiagonalTopRight,
                DiagonalTopleft
        }),
};
}

char Piece::MakePiece(Color color, Type type) {
    return color | type;
}

Color Piece::getColor(const char& piece) {
    return ColorMask & piece ? Color::White : Color::Black;
}

Type Piece::getType(const char& piece) {
    return static_cast<Type>(TypeMask & piece);
}

const std::vector<MoveDirection::Move>& Piece::getMoveDirections(const char& piece) {
    if (getType(piece) == Type::Pawn && getColor(piece) == Color::White) {
        static std::vector<MoveDirection::Move> whitePawn = {
                internal::PawnBackward
        };
        return whitePawn;
    }
    return internal::PieceMovement.at(getType(piece));
}

char Piece::getPieceFromFen(const char& piece) {
    return MakePiece((isupper(piece) ? Color::White : Color::Black), internal::getTypeFromFen(piece));
}

bool Piece::isPieceFen(const char& c) {
    switch (tolower(c)) {
        case 'p':
            return true;
        case 'n':
            return true;
        case 'b':
            return true;
        case 'r':
            return true;
        case 'q':
            return true;
        case 'k':
            return true;
    }
    return false;
}

char Piece::toFen(const char& c) {
    return internal::getPieceFen(getColor(c), getType(c));
}