#include "hexboard.h"

/* ============================================================================
   class HexBoard
   
   Implements a Hex Game board; 
   * Manages cell connectivity and keeps track of cell colors.
   * Provides a method to determine whether a player has won the game.
   ============================================================================ */

/* ----------------------------------------------------------------------------
   constructors
    HexBoard(void)            -- creates an empty 0 x 0 board
    HexBoard(unsigned int n)  -- creates an empty n x n board
   ---------------------------------------------------------------------------- */
HexBoard::HexBoard(void)
{}

HexBoard::HexBoard(unsigned int n)
{   Reset(n);   }

/* ----------------------------------------------------------------------------
   unsigned int Size(void);
   
   Returns the size of the board (in number of cells per side)
   ---------------------------------------------------------------------------- */
unsigned int HexBoard::Size(void)
{   return size;    }

/* ----------------------------------------------------------------------------
   HexMoveResult HexBoard::SetColor(unsigned int row, unsigned int col, HexColor color);
   
   Sets the given cell (row, col) to the given color
   
   Returns:
    HEXMOVE_OK:             if move accepted
    HEXMOVE_INVALIDCELL:    if (row, col) is an invalid cell
    HEXMOVE_INVALIDCOLOR:   if given color is invalid
    HEXMOVE_OCCUPIED:       if (row, col) is already occupied
   ---------------------------------------------------------------------------- */
HexMoveResult HexBoard::SetColor(unsigned int row, unsigned int col, HexColor color)
{    
    if ((row >= size) || (col >= size))
        return HEXMOVE_INVALIDCELL;
        
    if ((color != HEXBLUE) && (color != HEXRED))
        return HEXMOVE_INVALIDCOLOR;
        
    unsigned int iCell = cellIndex(row, col);
    
    if ((G.GetVertexValue(iCell) != HEXBLANK) && !trialMode)
        return HEXMOVE_OCCUPIED;
        
    HexColor oldColor = G.SetVertexValue(iCell, color);
    
    if ((color == HEXBLUE) && (row == 0))
        G.SetVertexValue(BLUEGOAL, HEXBLUE);
    
    if ((color == HEXRED) && (col == (size - 1)))
        G.SetVertexValue(REDGOAL, HEXRED);
        
        
    if (trialMode)
    {
        // if in trial mode, whenever we overwrite a blue cell on the blue goal row, or
        // a red cell on the red goal column, we need to check whether we also need to
        // reset the goal virtual cells
        
        if ((row == 0) && (oldColor == HEXBLUE) && (color != HEXBLUE))
        {
            VertexIDSet vs;
            G.Neighbors(BLUEGOAL, vs, true);
            if (vs.size() == 0)
            {
                // no more blue cells connected to blue virtual goal, reset blue virtual goal color
                G.SetVertexValue(BLUEGOAL, HEXBLANK);
            }
        }
        
        if ((col == (size - 1)) && (oldColor == HEXRED) && (color != HEXRED))
        {
            VertexIDSet vs;
            G.Neighbors(REDGOAL, vs, true);
            if (vs.size() == 0)
            {
                // no more red cells connected to blue virtual goal, reset red virtual goal color
                G.SetVertexValue(REDGOAL, HEXBLANK);
            }
        }
    }
    
    return HEXMOVE_OK;
}   

/* ----------------------------------------------------------------------------
   HexColor Winner(void);
   
   Returns the color (HEXBLUE, HEXRED) of the player who has won the game, 
   or HEXBLANK if no player has yet won the game.
   ---------------------------------------------------------------------------- */
HexColor HexBoard::Winner(void)
{        
    if (trialMode)
    {
        // if in trial mode, the value of the goal virtual cells is not reliable, as
        // it might be out of sync with the board if the connected cells were overwritten
        
        // first compute the right color for the goal cells
        bool setColor = false;
        for (unsigned int i = 0; i < size; i++)
        {
            if (GetColor(0, i) == HEXBLUE)
            {
                setColor = true;
                break;
            }
        }
        
        G.SetVertexValue(BLUEGOAL, setColor ? HEXBLUE : HEXBLANK);
        
        setColor = false;
        for (unsigned int i = 0; i < size; i++)
        {
            if (GetColor(i, 0) == HEXRED)
            {
                setColor = true;
                break;
            }
        }
        
        G.SetVertexValue(REDGOAL, setColor ? HEXRED : HEXBLANK);
    }
    
    if ((G.GetVertexValue(BLUEGOAL) == HEXBLUE) && G.HasPath(BLUEGOAL, BLUEHOME, true))
        return HEXBLUE;
    if ((G.GetVertexValue(REDGOAL) == HEXRED) && G.HasPath(REDGOAL, REDHOME, true))
        return HEXRED;
        
    return HEXBLANK;
}

   
/* ----------------------------------------------------------------------------
   HexColor GetColor(unsigned int row, unsigned int col)
   
   Return the color of the given cell (HEXBLANK, HEXBLUE, HEXRED)
   ---------------------------------------------------------------------------- */
HexColor HexBoard::GetColor(unsigned int row, unsigned int col)
{
    if ((row >= size) || (col >= size))
        throw HEXBOARD_ERR_INVALIDCELL;

    return G.GetVertexValue(cellIndex(row, col));
}

/* ----------------------------------------------------------------------------
   void HexBoard::GetCells(HexCellSet &hcs, HexColor color=HEXBLANK);
   
   Return all the cells of the given color.
   If no color given, then return the set of all open cells.
   
   Client must provide a reference to a valid HexCellSet object where cells
   will be returned.
   ---------------------------------------------------------------------------- */
void HexBoard::GetCells(HexCellSet &hcs, HexColor color)
{
    unsigned int iMax = size * size;        // max number of cells
    hcs.clear();                            // clear set of cells
    hcs.reserve(iMax);                      // reserve enough space
    
    // iterate through all board cells (excluding virtual cells) and return
    // those that are of the requested color
    for (unsigned int row = 0, iCell = 0; row < size; row++)
    {
        for (unsigned int col = 0; col < size; col++, iCell++)
        {
            HexColor thisCell = G.GetVertexValue(iCell);
            
            // if color matches, copy cell info
            if (thisCell == color)
            {
                HexCell cell;
                cell.row = row;
                cell.col = col;
                cell.color = thisCell;
                hcs.push_back(cell);
            }
        }
    }
}

/* ----------------------------------------------------------------------------
   void HexBoard::SetTrialMode(void);
   
   Puts the board in trial mode.  In trial mode we allow overwriting cells, so
   that repeated trials can be played on the same board.
   ---------------------------------------------------------------------------- */
void HexBoard::SetTrialMode(void)
{   trialMode = true;   }

   
/* ----------------------------------------------------------------------------
   void HexBoard::Reset(unsigned int n)
   
   Clears all internal board states and creates an empty board of n x n cells.
   ---------------------------------------------------------------------------- */
void HexBoard::Reset(unsigned int n)
{
    if ((n < HEXMINSIZE) || (n > HEXMAXSIZE))
        throw HEXBOARD_ERR_INVALIDSIZE;
        
    unsigned int n2 = n * n;
    
    size = n;                       // remember board dimension
    
    G.Reset(n2 + 4, HEXBLANK);      // reset graph to appropriate size
    
    BLUEHOME = n2 + 0;              // set values of 4 virtual cells
    BLUEGOAL = n2 + 1;
    REDHOME  = n2 + 2;
    REDGOAL  = n2 + 3;    
    
    // now build graph connectivity
    for (unsigned int r = 0, iCell = 0; r < n ; r++)
    {
        for (unsigned int c = 0; c < n; c++, iCell++)
        {   
            if (r < (n - 1))
            {
                G.AddEdge(iCell, cellIndex(r + 1, c));                  // connect down                
                if (c > 0) G.AddEdge(iCell, cellIndex(r + 1, c - 1));   // unless left-most column, connect down-left
                if (r == 0) G.AddEdge(iCell, BLUEGOAL);                 // if top row, connect to BLUEGOAL                
            }
            else
            {
                G.AddEdge(iCell, BLUEHOME);                             // if bottom row, connect to BLUEHOME
            }            
            
            if (c < (n - 1)) 
            {
                G.AddEdge(iCell, cellIndex(r, c + 1));                  // unless right-most column, connect right
                if (c == 0) G.AddEdge(iCell, REDHOME);                  // if left-most column, connect to REDHOME                            
            }
            else
            {
                G.AddEdge(iCell, REDGOAL);                              // if right-most column, connect to REDGOAL
            }
        }
    }
    
    // initialize HOME virtual cells to their respective colors
    G.SetVertexValue(BLUEHOME, HEXBLUE);
    G.SetVertexValue(REDHOME, HEXRED);  

    trialMode = false;
}

/* ============================================================================
   HexMoveGenerator class
   
   Computes the set of valid moves remaining on a board, and provides
   a convenient access to a randomized sequence of those moves.
   ============================================================================ */

/* ----------------------------------------------------------------------------
   HexMoveGenerator::HexMoveGenerator(HexBoard &board);
   
   constructor - computes the set of valid moves (unoccupied cells) remaining
   on the board according to the state of the board at the time the constructor
   is called.
   ---------------------------------------------------------------------------- */
HexMoveGenerator::HexMoveGenerator(HexBoard &board)
{
    // retrieve all unoccupied cells on the board
    board.GetCells(hcs, HEXBLANK);
    // shuffle them to randomize them
    std::random_shuffle(hcs.begin(), hcs.end());
    // set cursor to beginning of sequence
    cursor = 0;
}

/* ----------------------------------------------------------------------------
   bool HexMoveGenerator::Next(unsigned int &id, unsigned int &row, unsigned int &col);
   
   Returns one of the valid moves remaining on the sequence.
   
   Returns true on success, false if there were no more valid moves remaining.
   
   If successful, the following are returned using the by-reference parameters:
        id: a move id, can be used to later retrieve the move using Get()
        row: the row index
        col: the col index
   ----------------------------------------------------------------------------- */
bool HexMoveGenerator::Next(unsigned int &id, unsigned int &row, unsigned int &col)
{
    if (cursor >= hcs.size()) return false;
    row = hcs[cursor].row;
    col = hcs[cursor].col;
    id = cursor++;
    return true;
}

/* -----------------------------------------------------------------------------
   void HexMoveGenerator::Get(unsigned int id, unsigned int &row, unsigned int &col);
   
   Retrieve the move identified by the given id.  The move (row, col) is returned
   using the by-reference parameters
   ----------------------------------------------------------------------------- */
void HexMoveGenerator::Get(unsigned int id, unsigned int &row, unsigned int &col)
{
    if (id >= hcs.size()) throw HEXBOARD_ERR_INVALIDCELL;
    row = hcs[id].row;
    col = hcs[id].col;
}

/* -----------------------------------------------------------------------------
   unsigned int HexMoveGenerator::Count(void);
   
   Returns the number of valid moves remaining (at the time the constructor was
   called).
   ----------------------------------------------------------------------------- */
unsigned int HexMoveGenerator::Count(void)
{   return hcs.size();  }

/* -----------------------------------------------------------------------------
   void HexMoveGenerator::Shuffle();
   
   Reshuffles the sequence of remaining moves and resets the cursor to the first
   move of the (newly shuffled) sequence.
   ----------------------------------------------------------------------------- */
void HexMoveGenerator::Shuffle(void)
{
    std::random_shuffle(hcs.begin(), hcs.end());
    cursor = 0;
}