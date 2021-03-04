//
// Created by Stefan Annell on 2021-02-20.
//

#include "Chess/Board.h"
#include <algorithm>
#include "Chess/Piece.h"
#include <iostream>

namespace internal {
    int xyToBoard(const Position& pos) {
        return 8 * pos.row + pos.col;
    }

    int xyToBoard(const MoveDirection::Move& pos) {
        return 8 * pos.row + pos.col;
    }

    int moveDirectionToBoard(const MoveDirection::Move& pos) {
        return 8 * pos.row + pos.col;
    }

    MoveDirection::Move boardToXY(const int& index) {
        return {index % 8, int(index / 8)};
    }

    Position boardToXYPos(const int& index) {
        return {int(index / 8), index % 8};
    }

    void fillBoard(const char& c, int& row, int& col, Board::Data& board) {
        if(c == '/') {
            row++;
            col = 0;
        } else if (Piece::isPieceFen(c)) {
            board[internal::xyToBoard(Position{row, col})] = Piece::getPieceFromFen(c);
            col++;
        } else {
            col += c - '0';
        }
    }

    bool getNextMover(const char& c) {
        return c == 'w';
    }

    void getCastling(const char& c, Castling& castling) {
        switch (c) {
            case 'Q':
                castling.white.queen = true;
                break;
            case 'K':
                castling.white.king = true;
                break;
            case 'q':
                castling.black.queen = true;
                break;
            case 'k':
                castling.black.king = true;
                break;
        }
    }

}

Board::Board(const std::string& f)
    : fen(f)
    , board()
    , whitesMove(true)
    , castling(Castling{CastlingSide{true, true}, CastlingSide{true, true}})
    , enPersantIndex(-1)
    , checked(false)
    , isOver(false) {
    MakeBoard();
}

void Board::MakeBoard() {
    int row = 0;
    int col = 0;
    int state  = 0;
    // Builds board from FEN Notation
    for(const char& c : fen) {
        if (c == ' ') {
            state++;
        } else {
            switch (state) {
                case 0: //Board
                    internal::fillBoard(c, row, col, board);
                    break;
                case 1: //Next mover
                    whitesMove = internal::getNextMover(c);
                    break;
                case 2: //Castling availability
                    internal::getCastling(c, castling);
                    break;
                case 3: //En persant availability
                    enPersantIndex  = -1;
                    break;
                case 4: //Draw stuff
                    break;
                case 5: //Draw stuff
                    break;
            }
        }
    }
    for (int i = 0; i < board.size(); i++) {
        char piece = board[i];
        if (Piece::getType(piece) != 0) {
            Color color = Piece::getColor(piece);
            if (color == Color::White) {
                whitePieces.push_back({piece, i});
            } else {
                blackPieces.push_back({piece, i});
            }
        }
    }
}

void Board::MakeFen() {
    std::string newFen;
    for (int row = 0; row < 8; row++) {
        int spacingCounter = 0;
            for (int col = 0; col < 8; col++) {
            int index = internal::xyToBoard(Position{row, col});
            char piece = Piece::toFen(board[index]);
            if (Piece::isPieceFen(piece)) {
                newFen += (spacingCounter ? std::to_string(spacingCounter) : "")
                        + piece;
                spacingCounter = 0;
            } else {
                spacingCounter++;
            }
        }
        newFen += (spacingCounter ? std::to_string(spacingCounter) : "")
                + "/";
    }
    newFen += " ";
    newFen += (whitesMove ? "w" : "b");
    newFen += " ";
    newFen += castling.toFen();
    newFen += " ";
    newFen += (enPersantIndex != -1 ? "a4" : "-");
    newFen += " ";
    newFen += "0";
    newFen += " ";
    newFen += "0";
    fen = std::move(newFen);
}

void Board::GenerateMoves(PieceState& piece) {
    piece.moves.empty();
    Color color = Piece::getColor(piece.piece);
    auto stepsGetter = [&] (const PieceState& piece) {
        switch (Piece::getType(piece.piece)) {
            case Type::Pawn:
                if (color == Color::White) {
                    if (piece.index > 47 && piece.index < 56) {
                        return 2;
                    }
                } else {
                    if (piece.index > 7 && piece.index < 16) {
                        return 2;
                    }
                }
                return 1;
            case Type::Knight:
                return 1;
            case Type::Bishop:
                return -1;
            case Type::Rook:
                return -1;
            case Type::Queen:
                return -1;
            case Type::King:
                return 1;
            case Type::Nothing:
            default:
                return 0;
        }
    };
    auto getCastlingMoves = [&] (PieceState& piece) {
        if (castling.canCastle(piece) && !checked) {
            if (color == Color::White) {
                if (castling.white.king && board[61] == 0 && board[62] == 0) {
                    piece.moves.push_back({62, MoveType::Castle});
                }
                if (castling.white.queen && board[59] == 0 && board[58] == 0 && board[57] == 0) {
                    piece.moves.push_back({58, MoveType::Castle});
                }
            } else {
                if (castling.black.king && board[5] == 0 && board[6] == 0) {
                    piece.moves.push_back({6, MoveType::Castle});
                }
                if (castling.black.queen && board[1] == 0 && board[2] == 0 && board[3] == 0) {
                    piece.moves.push_back({2, MoveType::Castle});
                }
            }
        }
    };
    auto getMoveDirectionMoves = [&, steps = stepsGetter(piece)] (PieceState& piece) {
        for (const auto& move : Piece::getMoveDirections(piece.piece)) {
            int tempSteps = steps;
            auto tempPos = internal::boardToXY(piece.index) + move;

            if (Piece::getType(piece.piece) == Type::Pawn) {
                auto sameBoardSide = [] (int index1, int index2) {
                    return abs(internal::boardToXY(index1).col - internal::boardToXY(index2).col) == 1;
                };
                int index = internal::moveDirectionToBoard(tempPos);
                if (Piece::getType(board[index + 1]) != Type::Nothing
                    && Piece::getColor(board[index + 1]) != color
                    && sameBoardSide(index, index +1 )) {
                    piece.moves.push_back({index + 1, MoveType::Capture});
                }
                if (Piece::getType(board[index - 1]) != Type::Nothing
                    && Piece::getColor(board[index - 1]) != color
                    && sameBoardSide(index, index - 1)) {
                    piece.moves.push_back({index - 1, MoveType::Capture});
                }
            }

            while (tempSteps && tempPos.isValid()) {
                int index = internal::moveDirectionToBoard(tempPos);
                if (board[index] == EMPTY) {
                    piece.moves.push_back({index, MoveType::Normal});
                    tempSteps--;
                    tempPos = tempPos + move;
                } else if (Piece::getColor(board[index]) == color) {
                    tempSteps = 0;
                } else {
                    if (Piece::getType(piece.piece) != Type::Pawn) {
                        piece.moves.push_back({index, MoveType::Capture});
                    }
                    tempSteps = 0;
                }
            }
        }
    };
    auto getEnpersantMoves = [&] (PieceState& piece) {
        if (Piece::getType(piece.piece) == Type::Pawn
            && enPersantIndex != -1) {
            auto piecePos = internal::boardToXY(piece.index);
            auto enPersantPos = internal::boardToXY(enPersantIndex);
            if (piecePos.row != enPersantPos.row &&
                piecePos.col != enPersantPos.col &&
                abs(piecePos.row - enPersantPos.row) == 1 &&
                abs(piecePos.col - enPersantPos.col) == 1) {
                piece.moves.push_back({enPersantIndex, MoveType::Enpersand});
            }
        }
    };
    getEnpersantMoves(piece);
    getCastlingMoves(piece);
    getMoveDirectionMoves(piece);
}

std::vector<PieceState> Board::GetActivePlayerMoves() {
    auto pieces = whitesMove ? getWhitePieces() : getBlackPieces();
    for (auto& state : pieces) {
        GenerateMoves(state);
    }
    return pieces;
}

void Board::DoMove(const PieceState& pieceState, const Move& move) {
    auto destroyCastling = [&] (const Move& move) {
        Type type = Piece::getType(pieceState.piece);
        Color color = Piece::getColor(pieceState.piece);
        if (type == Type::Rook) {
            switch (pieceState.index) {
                case 63:
                    castling.white.king = false;
                    break;
                case 56:
                    castling.white.queen = false;
                    break;
                case 7:
                    castling.black.king = false;
                    break;
                case 0:
                    castling.black.queen = false;
                    break;
            }

        } else if (type == Type::King) {
            if (color == Color::White) {
                castling.white.queen = false;
                castling.white.king = false;
            } else {
                castling.black.queen = false;
                castling.black.king = false;
            }

        }
    };
    auto normalMove = [&] (const Move& move) {
        auto& enemyPieces = whitesMove ? blackPieces : whitePieces;
        for (auto it = enemyPieces.begin(); it != enemyPieces.end(); ++it) {
            if (it->index == move.index) {
                enemyPieces.erase(it);
                break;
            }
        }
        auto& myPieces = whitesMove ? whitePieces : blackPieces;
        for (auto it = myPieces.begin(); it != myPieces.end(); ++it) {
            if (it->index == pieceState.index) {
                it->index = move.index;
                break;
            }
        }
        board[move.index] = pieceState.piece;
        board[pieceState.index] = EMPTY;
        destroyCastling(move);
        whitesMove = !whitesMove;
    };
    auto captureMove = [&] (const Move& move) {
        if (Piece::getType(board[move.index]) != Type::Nothing) {
            if (Piece::getType(board[move.index]) == Type::King) {
                isOver = true;
            }
        }
    };
    auto castleMove = [&] (const Move& move) {
        switch (move.index) {
            case 62:
                board[61] = board[63];
                board[62] = pieceState.piece;
                board[60] = EMPTY;
                board[63] = EMPTY;
                break;
            case 58:
                board[59] = board[56];
                board[58] = pieceState.piece;
                board[56] = EMPTY;
                board[60] = EMPTY;
                break;
            case 6:
                board[5] = board[7];
                board[6] = pieceState.piece;
                board[7] = EMPTY;
                board[4] = EMPTY;
                break;
            case 2:
                board[3] = board[0];
                board[2] = pieceState.piece;
                board[0] = EMPTY;
                board[4] = EMPTY;
                break;
        }
        destroyCastling(move);
        whitesMove = !whitesMove;
    };
    auto enpersandMove = [&] (const Move& move) {
        Color color = Piece::getColor(pieceState.piece);
        int direction = (color == Color::White ? 1 : -1);
        auto takenPawn = internal::boardToXY(move.index);
        takenPawn.row += direction;
        int takenPawnIndex = internal::xyToBoard(takenPawn);

        board[move.index] = pieceState.piece;
        board[pieceState.index] = EMPTY;
        board[takenPawnIndex] = EMPTY;
        destroyCastling(move);
        whitesMove = !whitesMove;
    };
    auto doubleJumpPawn = [&] () {
        bool results =  Piece::getType(board[move.index]) == Type::Pawn &&
                           abs(internal::boardToXY(move.index).row - internal::boardToXY(pieceState.index).row) == 2;
        if (results) {
            enPersantIndex = pieceState.index + 8 * (pieceState.index - move.index < 0 ? 1 : -1);
        } else {
            enPersantIndex = -1;
        }
    };
    auto pawnUpgrade = [&] () {
        if (Piece::getType(pieceState.piece) == Type::Pawn) {
            auto pos = internal::boardToXYPos(move.index);
            if (pos.row == 0 || pos.row == 7) {
                board[move.index] = Piece::MakePiece(Piece::getColor(pieceState.piece), Type::Queen);
            }
        }
    };
    auto checkCheck = [&] () {
        return false;
    };
    switch (move.type) {
        case MoveType::Capture:
            captureMove(move);
        case MoveType::Normal:
            normalMove(move);
            doubleJumpPawn();
            pawnUpgrade();
            break;
        case MoveType::Castle:
            castleMove(move);
            doubleJumpPawn();
            break;
        case MoveType::Enpersand:
            enpersandMove(move);
            doubleJumpPawn();
            break;
        default:
            break;
    }
    //checked = checkCheck();
}

std::string Board::toFen() {
    MakeFen();
    return fen;
}

std::string Board::operator()() {
    std::string boardState = "-----------";
    for (int row = 0; row < 8; row++) {
        boardState += "\n|";
        for (int col = 0; col < 8; col++) {
            auto val = internal::xyToBoard(Position{row, col});
            boardState += Piece::toFen(board[val]);
        }
        boardState += "|";
    }
    boardState += "\n-----------\n" + toFen();
    return boardState;
}

bool Board::isWhiteActive() {
    return whitesMove;
}

const std::vector<PieceState>& Board::getBlackPieces() {
    return blackPieces;
}

const std::vector<PieceState>& Board::getWhitePieces() {
    return whitePieces;
}
