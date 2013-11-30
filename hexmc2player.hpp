#include <iostream>
#include <cstdlib>
#include <time.h>
#include <algorithm>
#include "hexboard.h"

class MoveGenerator {
    public:
    MoveGenerator(HexBoard &board);
    bool Next(unsigned int &id, unsigned int &row, unsigned int &col);
    void Get(unsigned int id, unsigned int &row, unsigned int &col);
    unsigned int Count(void);
    
    private:
    HexCellSet hcs;
    unsigned int cursor;
};

MoveGenerator::MoveGenerator(HexBoard &board)
{
    // retrieve all unoccupied cells on the board
    board.GetCells(hcs, HEXBLANK);
    // shuffle them to randomize them
    std::random_shuffle(hcs.begin(), hcs.end());
    // set cursor to beginning of sequence
    cursor = 0;
}

bool MoveGenerator::Next(unsigned int &id, unsigned int &row, unsigned int &col)
{
    if (cursor >= hcs.size()) return false;
    row = hcs[cursor].row;
    col = hcs[cursor].col;
    id = cursor++;
    return true;
}

void MoveGenerator::Get(unsigned int id, unsigned int &row, unsigned int &col)
{
    if (id >= hcs.size()) throw HEXBOARD_ERR_INVALIDCELL;
    row = hcs[id].row;
    col = hcs[id].col;
}

unsigned int MoveGenerator::Count(void)
{   return hcs.size();  }

// evaluate a proposed move and return its score
static int EvaluateMove(HexBoard &b, HexColor turn, unsigned int row, unsigned int col)
{
    // make a local working copy of the board
    HexBoard board(b);
    
    const int nTrials = 1000;
    int score = 0;
    
    // play the proposed move
    board.SetColor(row, col, turn);
    
    // obtain remaining blank cells
    HexCellSet hcs;
    board.GetCells(hcs, HEXBLANK);
    
    // order of turns, beginning with opponent (we already placed our first move)
    HexColor turns[2] = {((turn == HEXBLUE) ? HEXRED : HEXBLUE), turn};
    
    for (unsigned int iTrial = 0; iTrial < nTrials; iTrial++)
    {
        // make a working copy of the board to trash in this trial
        HexBoard boardCopy(board);
        
        // randomize blank cells to play them in different random order each trial
        std::random_shuffle(hcs.begin(), hcs.end());
        
        for (unsigned int i = 0; i < hcs.size(); i++)
            boardCopy.SetColor(hcs[i].row, hcs[i].col, turns[i % 2]);
            
        // check whether we (turn) won, and update stats
        if (boardCopy.Winner() == turn)
            score++;
    }
    
    return score;
}

/* ============================================================================ *
   HexMC2Player class
   
   Implements an automatic Hex player that uses MonteCarlo simulation to 
   determine next move.
   
   At each move, it picks the cell with larger win count.
   ============================================================================ */
class HexMC2Player : public HexPlayer {
    virtual void Move(HexBoard &board, HexColor turn, unsigned int &row, unsigned int &col);
};

void HexMC2Player::Move(HexBoard &board, HexColor turn, unsigned int &row, unsigned int &col)
{        
    unsigned int idPlay, bestPlay, trow, tcol;
    int score, bestScore = -1;
    
    MoveGenerator mg(board);
    while (mg.Next(idPlay, trow, tcol))
    {
        score = EvaluateMove(board, turn, trow, tcol);
        if (score > bestScore)
        {
            bestScore = score;
            bestPlay = idPlay;
        }
    }
    mg.Get(bestPlay, row, col);
}
