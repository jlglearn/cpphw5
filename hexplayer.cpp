#include <iostream>
#include <sstream>
#include <string>
#include "hexboard.h"

/* ----------------------------------------------------------------------------
   static void errorMsg(std::string const &sInput, const char *msg);
   outputs an error message when user input is not valid.
   ---------------------------------------------------------------------------- */
static void errorMsg(std::string const &sInput, const char *msg)
{
    std::cout << sInput << msg;
}

/* ----------------------------------------------------------------------------
   static void readMove(unsigned int &row, unsigned int &col);
   
   Utility function that reads a move (row, col) from stdin.
   ---------------------------------------------------------------------------- */
static void readMove(unsigned int &row, unsigned int &col)
{
    const char *sPrompt = " is not a valid move, please try again.\nPlease enter your move (ROW COL): ";
    
    while (true)
    {
        std::string s;
        getline(std::cin, s);
        
        std::istringstream ss(s);
        unsigned int tRow, tCol;
        
        ss >> tRow;
        if (ss.fail() || ss.eof() || (tRow >= HEXMAXSIZE))
        {
            errorMsg(s, sPrompt);
            continue;
        }
        
        ss >> tCol;
        if (ss.fail() || (tCol >= HEXMAXSIZE))
        {
            errorMsg(s, sPrompt);
            continue;
        }
        
        row = tRow;
        col = tCol;
        break;
    }
}

/* ============================================================================ *
 * HexPlayer class                                                              *
 *                                                                              *
 * Implements a default player that accepts input from stdin                    *
 * ============================================================================ */
void HexPlayer::Move(HexBoard &board, HexColor turn, unsigned int &row, unsigned int &col)
{
    readMove(row, col);
}
 