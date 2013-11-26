#include <iostream>
#include "hexboard.h"
   
/* ---- char chip(HexColor c); ------------------------------------------------
        Returns the ASCII representation for the given cell color
   ---------------------------------------------------------------------------- */
char HexGameIO::chip(HexColor c)
{
    if (c == HEXBLUE) return 'O';
    if (c == HEXRED) return 'X';
    return '.';
}

/* ---- const char * name(HexColor c); ----------------------------------------
        Returns the name (string representation) of a player's color
   ---------------------------------------------------------------------------- */
const char * HexGameIO::name(HexColor c)
{
    static const char * bluename = "BLUE";
    static const char * redname = "RED";

    if (c == HEXBLUE)
        return bluename;

    if (c == HEXRED)
        return redname;
        
    throw HEXGAME_ERR_INVALIDTURN;
}

/* ---- void HexGameIO::Prompt(HexColor turn); --------------------------------
        Prompt player for move
   ---------------------------------------------------------------------------- */
void HexGameIO::Prompt(HexColor turn)
{
    if ((turn != HEXBLUE) && (turn != HEXRED))
        throw HEXGAME_ERR_INVALIDTURN;
     
    std::string sDirection;
    
    if (turn == HEXBLUE)
        sDirection = "BOTTOM -> TOP";
    else
        sDirection = "LEFT -> RIGHT";
    
    std::cout << "\n" << name(turn) << " PLAYER (" << chip(turn) << "), your turn!\n"
              << "You move " << sDirection << "\n"
              << "Please enter your move ROW COL: ";
    
}   

/* ---- void HexGameIO::MoveFeedback(HexMoveResult result, HexColor turn, int row, int col)
        Provide feedback about entered move.
   ---------------------------------------------------------------------------- */
void HexGameIO::MoveFeedback(HexMoveResult result, HexColor turn, int row, int col)
{
    if ((turn != HEXBLUE) && (turn != HEXRED))
        throw HEXGAME_ERR_INVALIDTURN;
    
    switch (result)
    {
        case HEXMOVE_OK:
            std::cout << "Move accepted... (" << row << ", " << col << ")\n";
            break;            
            
        case HEXMOVE_INVALIDCELL:
            std::cout << name(turn) << " PLAYER, " << row << ", " << col 
                      << " is not a valid move.  Please try again.\n";
            break;
            
        case HEXMOVE_OCCUPIED:
            std::cout << name(turn) << " PLAYER, " << row << ", " << col 
                      << " is already occupied.  Please try again.\n";
            break;
    }
}

/* ---- void HexGameIO::AnnounceWinner(HexColor winner) -----------------------
        Gives feedback about which player won the game.
   ---------------------------------------------------------------------------- */
void HexGameIO::AnnounceWinner(HexColor winner)
{
    if ((winner != HEXBLUE) && (winner != HEXRED))
        throw HEXGAME_ERR_INVALIDCOLOR;
        
    std::cout << name(winner) << " PLAYER you have won the game!!!\n";
}

/* ---- void HexGameIO::PrintBoard(HexBoard &board); --------------------------
        Prints board
   ---------------------------------------------------------------------------- */
void HexGameIO::PrintBoard(HexBoard &board)
{
    int size = board.Size();
    char sMargin[] = "    ";
    
    // ----- print column labels ----------------------------------------------
    std::cout << sMargin;
    
    for (int i = 0; i < (3 + 1); i++)
        std::cout << " ";    
        
    int padding = (size * 3 - 9) / 2;
    
    for (int i = 0; i < padding; i++)
        std::cout << " ";
        
    std::cout << name(HEXBLUE) << " GOAL" << "\n";
    
    
    std::cout << sMargin;
    
    for (int i = 0; i < (3 + 1); i++)
        std::cout << " ";    
        
    for (int i = 0; i < size; i++)
        std::cout << " " << i << " ";
        
    std::cout << "\n";
    
    
    // ----- print top border -------------------------------------------------
    std::cout << sMargin;
    
    for (int i = 0; i < (3 + 1); i++)
        std::cout << " ";        
        
    for (int i = 0; i < (2 + 3 * size + 1); i++)
        std::cout << "-";        
        
    std::cout << "\n";
    
    // ----- row labels, lateral borders and cells ----------------------------
    for (int row = 0; row < size; row++)
    {
        if (row == (size/2 - 1))
            std::cout << " " << name(HEXRED);
        else if (row == (size/2))
            std::cout << "HOME";
        else if (row == (size/2 + 1))
            std::cout << " (" << chip(HEXRED) << ")";
        else
            std::cout << sMargin;
            
        for (int s = 0; s < row; s++)
            std::cout << " ";
            
        std::cout << ((row < 10) ? "  " : " ") << row << " " << "\\ ";
        
        for (int col = 0; col < size; col++)
            std::cout << " " << chip(board.GetColor(row, col)) << " ";
            
        std::cout << " \\ " << row << " ";
        
        if (row == (size/2 - 1))
            std::cout << name(HEXRED);
        else if (row == (size/2))
            std::cout << "GOAL";
        
        std::cout << "\n";
    }
    
    // ----- print bottom border ----------------------------------------------
    std::cout << sMargin;
    
    for (int i = 0; i < (size + 3 + 1); i++)
        std::cout << " ";
        
    for (int i = 0; i < (2 + 3 * size + 1); i++)
        std::cout << "-";
        
    std::cout << "\n";   

    // ----- print column labels ----------------------------------------------
    std::cout << sMargin;
    
    for (int i = 0; i < (size + 3 + 1 + 2); i++)
        std::cout << " ";
        
    for (int i = 0; i < size; i++)
        std::cout << " " << i << " ";
        
    std::cout << "\n";
    
    for (int i = 0; i < (size + 3 + 1 + 2); i++)
        std::cout << " ";
        
    for (int i = 0; i < padding; i++)
        std::cout << " ";
        
    std::cout << name(HEXBLUE) << " HOME (" << chip(HEXBLUE) <<")\n\n";
}