#include <iostream>
#include "Board.h"
#include "Bot.h"
#include "Piece.h"


int main() {
    std::cout << "Starting game..." << std::endl;
    //Board board("8/8/1q1nnp1n/5b2/8/3r4/1k6/8 b KQkq - 1 2");
    //Board board("rnbqkbnr/pppppppp/8/8/8/1NBQ1BN1/PPPPPPPP/R3K2R w KQkq - 1 2");
    Board board("rnbqk1nr/ppp2ppp/8/2bpp3/3PP3/4BN2/PPP2PPP/RN1QKB1R b KQkq - 0 1");
    //Board board("rnbqkbnr/pppppppp/7P/8/8/8/PPPPPPP1/RNBQKBNR b KQkq - 0 1");
    //Board board("rnbqkbnr/ppppppp1/8/8/8/7p/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    for (int i = 0; i < 10; i++) {
        int counter = 0;
        auto start = std::chrono::high_resolution_clock::now();
        auto results = Bot::GetBestMove(board, 10, counter);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = duration_cast<std::chrono::milliseconds>(stop - start);

        //for (auto results : Bot::GetBestMoveMinMax(board, i, counter)) {
        //board.DoMove(results.state, results.move);
        board.DoMove(results.state, results.move);
        std::cout << board() << std::endl;
        std::cout << "At depth: " << 6 << " Processed moves: " << counter << " ";
        std::cout << "Time spent: " << duration.count() << " ms ";
        std::cout << "Time per move: " << (int)(duration.count()*1000 / counter) << " microseconds ";
        std::cout << "Score: " << results.score << std::endl;
    }
    return 0;
}
