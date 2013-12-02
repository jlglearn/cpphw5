#include <iostream>
#include <cstdlib>
#include <time.h>
#include <algorithm>
#include "hexboard.h"


// evaluate a proposed move and return its score
static int EvaluateMove(HexBoard &b, HexColor turn, unsigned int row, unsigned int col, unsigned int nTrials, int curMax)
{
    // make a local working copy of the board
    HexBoard board(b);
    
    // put the board in trial mode so that we can run repeated trials on the same board
    board.SetTrialMode();
    
    // play the proposed move
    board.SetColor(row, col, turn);

    // order of turns, beginning with opponent (we already placed our first move)
    HexColor turns[2] = {((turn == HEXBLUE) ? HEXRED : HEXBLUE), turn};

    // obtain remaining unoccupied cells
    HexMoveGenerator mg(board);
            
    int score = nTrials;
    for (unsigned int iTrial = 0; iTrial < nTrials; iTrial++)
    {   
        unsigned int idMove, moveRow, moveCol;    
        
        // reshuffle sequence
        mg.Shuffle();
        
        // play all moves sequentially until board full
        for (unsigned int i = 0; mg.Next(idMove, moveRow, moveCol); i++)
            board.SetColor(moveRow, moveCol, turns[i % 2]);
                        
        // if we lost, decrease counter
        if (board.Winner() != turn)
            score--;
            
        // if score is already suboptimal, stop evaluation
        if (score < curMax)
            break;
    }
    
    return score;
}

/* ============================================================================ *
   HexMC2Player class
   
   Implements an automatic Hex player that uses MonteCarlo simulation to 
   determine next move.
   
   At each move, it picks the cell with larger win count.
   ============================================================================ */
class HexMCPlayer : public HexPlayer {
    virtual void Move(HexBoard &board, HexColor turn, unsigned int &row, unsigned int &col);
};

void HexMCPlayer::Move(HexBoard &board, HexColor turn, unsigned int &row, unsigned int &col)
{        
    unsigned int nTrials = 1000;
    unsigned int idPlay, bestPlay, trow, tcol;
    int score, bestScore = -1;
    
    // obtain the sequence of available moves, so that we can evaluate them one at a time
    HexMoveGenerator mg(board);
    
    // iterate over all possible moves
    while (mg.Next(idPlay, trow, tcol))
    {
        // evaluate this move
        score = EvaluateMove(board, turn, trow, tcol, nTrials, bestScore);
        
        // keep track of best score so far
        if (score > bestScore)
        {
            bestScore = score;
            bestPlay = idPlay;
            
            if (score == nTrials)
            {
                // sure play, won every trial, just play it.  No need to evaluate further
                break;
            }
        }
    }
    
    // retrieve best play
    mg.Get(bestPlay, row, col);
}
