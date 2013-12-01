#ifndef _HEXBOARD_H_
#define _HEXBOARD_H_

#include "unionfind.h"
#include "mingraph.hpp"

typedef enum enumHexColor {
    HEXNULL, HEXBLANK, HEXBLUE, HEXRED
} HexColor;

typedef enum enumHexBoardError {
    HEXBOARD_ERR_INVALIDSIZE = 0x200,
    HEXBOARD_ERR_INVALIDCELL
} HexBoardError;

typedef enum enumHexGameError {
    HEXGAME_ERR_INVALIDTURN = 0x300,
    HEXGAME_ERR_INVALIDCOLOR,
    HEXGAME_ERR_INVALIDSIZE,
    HEXGAME_ERR_INVALIDPLAYER
} HexGameError;

typedef enum enumHexMoveResult {
    HEXMOVE_OK,
    HEXMOVE_INVALIDCELL,
    HEXMOVE_INVALIDCOLOR,
    HEXMOVE_OCCUPIED
} HexMoveResult;

const int HEXMINSIZE = 3;
const int HEXMAXSIZE = 15;

typedef struct structHexCell {
    unsigned int row;
    unsigned int col;
    HexColor     color;
} HexCell;

typedef std::vector<HexCell> HexCellSet;


// utility function to generate a random ordering of the numbers in the range:
// [0, n) 0 (inclusive) to n (exclusive)
void GenerateRandomOrdering(unsigned int n, std::vector<unsigned int> &vOrdering);


 

/* ============================================================================ *
 * HexBoard class                                                               *
 * ============================================================================ */
 
class HexBoard {
    public:
    HexBoard(void);
    HexBoard(unsigned int n);    
    HexMoveResult SetColor(unsigned int row, unsigned int col, HexColor color);
    HexColor GetColor(unsigned int row, unsigned int col);
    unsigned int Size(void);
    HexColor Winner(void);
    void GetCells(HexCellSet &hcs, HexColor color=HEXBLANK);
    void SetTrialMode(void);

    private:
    unsigned int size;    
    unsigned int BLUEHOME, BLUEGOAL, REDHOME, REDGOAL;
    unsigned int blueCells, redCells, freeCells;
    bool trialMode;
    
    MinGraph<HexColor> G;        
    
    inline unsigned int cellIndex(unsigned int row, unsigned int col) { return row * size + col; }
    inline unsigned int rowFromIndex(unsigned int index) { return index / size; }
    inline unsigned int colFromIndex(unsigned int index) { return index % size; }    
    void Reset(unsigned int n);
    
    friend class HexGame;

};


/* ============================================================================ *
 * HexGameIO class                                                              *
 * ============================================================================ */
class HexGameIO {
    public:
    char chip(HexColor c);
    const char *name(HexColor c);
    void Prompt(HexColor turn);
    void MoveFeedback(HexMoveResult result, HexColor turn, int row, int col);
    void AnnounceWinner(HexColor winner);
    void PrintBoard(HexBoard &board);
};

/* ============================================================================ *
 * HexPlayer class                                                              *
 * ============================================================================ */
 class HexPlayer {
    public:
    virtual void Move(HexBoard &board, HexColor turn, unsigned int &row, unsigned int &col);
};

 
/* ============================================================================ *
 * HexGame class
 * ============================================================================ */
typedef struct structHexGameOptions {
    bool mute;
} HexGameOptions;

class HexGame {
    public:
    HexGame(unsigned int n);
    HexColor RegisterPlayer(HexPlayer *p, HexColor color=HEXBLANK);
    HexColor Play(void);
    HexColor Play(HexColor movesFirst);
    
    bool SetOption(const char *optname, bool optval);
    
    private:
    HexBoard  board;
    HexGameIO gameIO;
    HexGameOptions options;
    
    HexPlayer *pBluePlayer;
    HexPlayer *pRedPlayer;
    
    void Reset(unsigned int n);
    
};

/* ============================================================================
   HexMoveGenerator class
   
   Computes the valid moves remaining on a board.
   ============================================================================ */
class HexMoveGenerator {
    public:
    HexMoveGenerator(HexBoard &board);
    bool Next(unsigned int &id, unsigned int &row, unsigned int &col);
    void Get(unsigned int id, unsigned int &row, unsigned int &col);
    void Shuffle(void);
    unsigned int Count(void);
    
    private:
    HexCellSet hcs;
    unsigned int cursor;
};

#endif