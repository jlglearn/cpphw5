#include <iostream>
#include <cstdlib>
#include <time.h>
#include <algorithm>
#include "hexboard.h"

/* ============================================================================ *
   HexMCPlayer class
   
   Implements an automatic Hex player that uses MonteCarlo simulation to 
   determine next move.
   
   At each move, it picks the cell with larger win count.
   ============================================================================ */
class HexMCPlayer : public HexPlayer {
    virtual void Move(HexBoard &board, HexColor turn, unsigned int &row, unsigned int &col);
};


void HexMCPlayer::Move(HexBoard &board, HexColor turn, unsigned int &row, unsigned int &col)
{    
    clock_t tStart = clock();
    
    const unsigned int nTrials = 1000;
    
    // keep track of best play and best score
    int bestScore = -1;
    unsigned int bestPlay;
        
    // obtain the currently open cells
    HexCellSet hcs;
    board.GetCells(hcs, HEXBLANK);
    unsigned int nCells = hcs.size();
    
    // shuffle cells to examine them in random order
    std::random_shuffle(hcs.begin(), hcs.end());
        
    // precompute the order of moves during simulation
    HexColor turns[2];
    turns[0] = turn;
    turns[1] = ((turn == HEXBLUE) ? HEXRED : HEXBLUE);    
    
    // now evaluate each cell in turn
    for (unsigned int iCell = 0; iCell < nCells; iCell++)
    {        
        // make a working copy of the cell set
        HexCellSet hcsCopy(hcs);
        
        // store away the current cell by swapping with last cell
        std::swap(hcsCopy[iCell], hcsCopy[nCells - 1]);
                
        // run MC simulation
        int nWins = 0;
        for (unsigned int iTrial = 0; iTrial < nTrials; iTrial++)
        {        
            // shuffle the first n-1 entries (keeping the current cell safely at the end)
            // to randomize the order in which cells are played in simulation
            std::random_shuffle(hcsCopy.begin(), hcsCopy.begin() + (nCells-1));
            
            // make a copy of the board to run simulated game
            HexBoard boardCopy(board);
            
            // alternate turns until board is full (until no more cells to play)
            for (unsigned int iTurn = 0; iTurn < nCells; iTurn++)
            {
                // for the first move, play the cell currently under evaluation,
                // which is stored at the end of the vector.
                // for subsequent moves, play each cell in succession, starting from 0th cell
                
                unsigned int thisMove = ((iTurn == 0) ? (nCells - 1) : (iTurn - 1));
                HexColor thisColor = turns[iTurn % 2];
                
                boardCopy.SetColor(hcsCopy[thisMove].row, hcsCopy[thisMove].col, thisColor);                
            }
            
            // find out who won this simulated game
            HexColor winner = boardCopy.Winner();
            
            // if we won, update stats
            if (winner == turn)
            {
                nWins++;
            }
        }
        
        if (nWins > bestScore)
        {
            bestScore = nWins;
            bestPlay = iCell;
            
            if (nWins == nTrials)
            {
                // if this cell won all its trials, then play it
                break;
            }            
        }        
    }
    
    // return the best play we found
    clock_t tEnd = clock();
    clock_t tElapsed = (tEnd - tStart) / CLOCKS_PER_SEC;
    std::cout << "Time elapsed: " << tElapsed << "(secs) \n";
    row = hcs[bestPlay].row;
    col = hcs[bestPlay].col;
}