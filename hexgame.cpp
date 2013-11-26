#include <iostream>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include "hexboard.h"

/* ============================================================================
   HexGame class
   
   Implements game flow (alternating turns), accepts player registrations,
   initiates a game and determines when a player has won the game.
   ============================================================================ */

/* ----------------------------------------------------------------------------
   HexGame::HexGame(unsigned int n)
   
   Builds a game with a board of size n x n cells.
   n must be in the range 3-15
   ---------------------------------------------------------------------------- */
HexGame::HexGame(unsigned int n)
{   Reset(n);   }

/* ----------------------------------------------------------------------------
   HexColor HexGame::RegisterPlayer(HexPlayer *p, HexColor color=HEXBLANK);
   
   Accepts a player registration.
   The optional parameter color indicates which color the player is requesting.
   
   Returns the color assigned to the player, or HEXNULL in case that:
       * Two players have already registered for the game
       * The requested color has already been assigned
   ---------------------------------------------------------------------------- */
HexColor HexGame::RegisterPlayer(HexPlayer *p, HexColor color)
{
    if (p == (HexPlayer *)0)
        throw HEXGAME_ERR_INVALIDPLAYER;
    
    if ((color != HEXBLANK) && (color != HEXBLUE) && (color != HEXRED))
        throw HEXGAME_ERR_INVALIDCOLOR;
        
    if ((color == HEXBLANK) || ((color == HEXBLUE) && (pBluePlayer == (HexPlayer *)0)))
    {
        // register as blue player
        pBluePlayer = p;
        return HEXBLUE;
    }
    
    if ((color == HEXBLANK) || ((color == HEXRED) && (pRedPlayer == (HexPlayer *)0)))
    {
        // register as red player
        pRedPlayer = p;
        return HEXRED;
    }
    
    // either two players have already registered or else the requested color is not available
    return HEXNULL;
}

/* ----------------------------------------------------------------------------
   HexColor HexGame::Play(void);
   HexColor HexGame::Play(HexColor movesFirst);
   
   Initiates a game.
   The optional second parameter indicates which player moves first.
   If not specified, blue player will move first.
   If HEXBLANK is specified as movesFirst color, then first mover will be chosen at random.
   
   Assigns a default player to any unregistered players.
   
   Returns the color of the player who wins the game.
   ---------------------------------------------------------------------------- */
HexColor HexGame::Play(void)
{   Play(HEXBLUE); }

HexColor HexGame::Play(HexColor movesFirst)
{
    if ((movesFirst != HEXBLUE) && (movesFirst != HEXRED) && (movesFirst != HEXBLANK))
        throw HEXGAME_ERR_INVALIDCOLOR;
        
    if (movesFirst == HEXBLANK)
    {
        // chose firstMover at random
        movesFirst = (((((double)rand()) / RAND_MAX) < 0.50) ? HEXBLUE : HEXRED);
    }
        
    HexPlayer *players[2];
    HexColor  turns[2];
    HexColor  winner = HEXBLANK;

    // assign a default player to any unregistered player
    if (pBluePlayer == (HexPlayer *)0)
        pBluePlayer = new HexPlayer;
        
    if (pRedPlayer == (HexPlayer *)0)
        pRedPlayer = new HexPlayer;
    
    // set correct turn order    
    if (movesFirst == HEXBLUE)
    {
        turns[0] = HEXBLUE;
        turns[1] = HEXRED;
        players[0] = pBluePlayer;
        players[1] = pRedPlayer;
    }
    else
    {
        turns[0] = HEXRED;
        turns[1] = HEXBLUE;
        players[0] = pRedPlayer;
        players[1] = pBluePlayer;
    }
        
    // start play, alternate turns until there is a winner
    for (unsigned int iTurn = 0; winner == HEXBLANK; iTurn++)
    {       
        if (!options.mute) gameIO.PrintBoard(board);
        
        // make a copy of the playing board to hand down to player code
        HexBoard   boardCopy(board);
        HexColor   thisTurn = turns[iTurn % 2];
        HexPlayer *thisPlayer = players[iTurn % 2];
        
        unsigned int row, col;
        HexMoveResult result;
        
        // obtain move from player
        while (true)
        {
            if (!options.mute) gameIO.Prompt(thisTurn);
            
            thisPlayer->Move(boardCopy, thisTurn, row, col);
            
            // pass player's move to board manager
            result = board.SetColor(row, col, thisTurn);
            
            // provide feedback
            if (!options.mute) gameIO.MoveFeedback(result, thisTurn, row, col);
            
            // if move is not accepted, prompt again
            if (result != HEXMOVE_OK) continue;
            
            // check for a winner
            winner = board.Winner();
            
            if ((winner != HEXBLANK) && (!options.mute))
            {
                // indeed, have a winner, print the final board and announce winner
                gameIO.PrintBoard(board);
                gameIO.AnnounceWinner(winner);
            }
            
            break;  // break out of prompt loop into turns loop
        }
    }
    
    return winner;
}   

/* ----------------------------------------------------------------------------
   bool HexGame::SetOption(const char *optname, bool optval)
   Sets the value of a boolean option, returns true if successful
   ---------------------------------------------------------------------------- */
bool HexGame::SetOption(const char *optname, bool optval)
{
    if (!strcmp(optname, "mute"))
    {
        options.mute = optval;
        std::cout << "Setting game options: mute=" << (optval ? "T" : "F") << "\n";
        return true;
    }
    
    return false;
}

   
/* ----------------------------------------------------------------------------
   void HexGame::Reset(unsigned int n)
   
   Clears all internal state and initializes an empty game on a board of size
   n x n.
   ---------------------------------------------------------------------------- */
void HexGame::Reset(unsigned int n)
{
    if ((n < HEXMINSIZE) || (n > HEXMAXSIZE))
        throw HEXGAME_ERR_INVALIDSIZE;
    
    board.Reset(n);
    
    pBluePlayer = (HexPlayer *)0;
    pRedPlayer  = (HexPlayer *)0;
    
    options.mute = false;
}
