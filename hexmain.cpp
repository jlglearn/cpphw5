#include <iostream>
#include <sstream>
#include <string>
#include "hexboard.h"
#include "hexmcplayer.hpp"
#include "hexmc2player.hpp"

static int readInt(const char *prompt, int minVal, int maxVal)
{
    int t;
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

int main(int argc, char *argv[])
{
    // first ask user to enter board size
    const char *sizePrompt = "Please enter the board size (3-15): ";
    int size = readInt(sizePrompt, HEXMINSIZE, HEXMAXSIZE);
    int redWins = 0;
    int blueWins = 0;
    
    HexMC2Player *pBlue = new HexMC2Player;
    HexMCPlayer *pRed  = new HexMCPlayer;    
  
    HexGame game(size);
    game.RegisterPlayer(pBlue, HEXBLUE);
    game.RegisterPlayer(pRed, HEXRED);
    HexColor winner = game.Play(HEXBLANK);

}