#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include "hexboard.h"
#include "hexmcplayer.hpp"

static int readUInt(const char *prompt, int minVal, int maxVal)
{
    unsigned int t;
    while (true)
    {
        std::cout << prompt;
        
        std::string s;
        getline(std::cin, s);
        
        std::istringstream ss(s);
        ss >> t;
        
        if (ss.fail())
        {
            std::cout << s << "is not a valid input.  Please try again.\n";
            continue;
        }
        
        if ((t < minVal) || (t > maxVal))
        {
            std::cout << t << "is not valid.  Please enter a value in the range (" << minVal << "-" << maxVal <<")\n";
            continue;
        }        
        return t;
    }
}

unsigned int readPlayerColor(unsigned int player, unsigned int options[])
{
    const char *p1 = "Player 1";
    const char *p2 = "Player 2";
    const char *prompt1 = ", please select your color:\n";
    const char *b = "\t[1] Blue (moves first)\n";
    const char *r = "\t[2] Red\n";
    const char *c = "\t[3] Computer (automatic play)\n";
    const char *prompt2 = "\nPlease make your selection by entering the corresponding number: ";
    const char *error = " is not a valid selection, please try again.\n";
    unsigned int t;
    
    while (true)
    {
        std::cout << ((player == 1) ? p1 : p2) << prompt1
                  << ((options[0] == 0) ? "\n" : b)
                  << ((options[1] == 0) ? "\n" : r)
                  << c
                  << prompt2;
                  
        std::string s;
        getline(std::cin, s);
        
        std::istringstream ss(s);
        ss >> t;
        
        if (ss.fail())
        {
            std::cout << s << error;
            continue;
        }
        
        if ((t < 1) || (t > 3))
        {
            std::cout << s << error;
            continue;
        }
        
        if (((t == 1) && (options[0] == 0)) || ((t == 2) && (options[1] == 0)))
        {
            std::cout << s << error;
            continue;
        }
        
        return t;        
    }
}


void readParameters(unsigned int &size, unsigned int &p1, unsigned int &p2)
{    
    // first ask user to enter board size
    const char *sizePrompt = "Please enter the board size (3-15): ";
    size = readUInt(sizePrompt, HEXMINSIZE, HEXMAXSIZE);
    
    // ask player 1 to choose color
    unsigned int options[] = {1, 2, 3};
    p1 = readPlayerColor(1, options);
    
    options[0] = ((p1 == 1) ? 0 : 1);      // if player 1 selected 1 (blue), it is no longer available for player 2
    options[1] = ((p1 == 2) ? 0 : 2);      // if player 2 selected 2 (red), it is no longer available for player 2
    p2 = readPlayerColor(2, options);    
}

// ----------------------------------------------------------------------------
// void registerPlayers(HexGame &game, unsigned int p1, unsigned int p2)
//
// Register players according to their color selections.
// Both p1 and p2 are one of the following:
//    1. Blue (moves first)
//    2. Red
//    3. Computer (automatic play)
//
// Inputs are assumed to be nonconflicting (checked at the time user entered input)
// ----------------------------------------------------------------------------
void registerPlayers(HexGame &game, unsigned int p1, unsigned int p2)
{

    // First register human (non-automatic play) players, to give each the
    // colors they requested
    
    // if player 1 selected blue or red, register player
    if (p1 != 3)
    {
        HexPlayer *p = new HexPlayer;
        std::cout << "Registering player 1: requesting: " << ((p1 == 1) ? "BLUE" : "RED") << "\n";
        game.RegisterPlayer(p, ((p1 == 1) ? HEXBLUE : HEXRED));
    }
    
    // if player 2 selected blue or red, register player
    if (p2 != 3)
    {
        HexPlayer *p = new HexPlayer;
        std::cout << "Registering player 2: requesting " << ((p2 == 1) ? "BLUE" : "RED") << "\n";
        game.RegisterPlayer(p, ((p2 == 1) ? HEXBLUE : HEXRED));
    }
    
    // then process automatic-play selections, will be assigned an available
    // color
    
    // if player 1 selected computer (automatic play), register player
    if (p1 == 3)
    {
        HexMCPlayer *p = new HexMCPlayer;
        std::cout << "Registering player 1: requesting first available.\n";
        game.RegisterPlayer(p, HEXBLANK);
    }
    
    // if player 2 selected computer (automatic play), register player
    if (p2 == 3)
    {
        HexMCPlayer *p = new HexMCPlayer;
        std::cout << "Registering player 2: requesting first available.\n";
        game.RegisterPlayer(p, HEXBLANK);
    }
}

int main(int argc, char *argv[])
{
    unsigned int size, p1, p2;
    
    // obtain user inputs
    readParameters(size, p1, p2);

    // create board of selected size
    HexGame game(size);
    
    // register players
    registerPlayers(game, p1, p2);
    
    // seed random generator and start play
    srand(time(0));
    game.Play(HEXBLUE);

}