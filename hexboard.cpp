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
    
    if (G.GetVertexValue(iCell) != HEXBLANK)
        return HEXMOVE_OCCUPIED;
        
    G.SetVertexValue(iCell, color);
    
    if ((color == HEXBLUE) && (row == 0))
        G.SetVertexValue(BLUEGOAL, HEXBLUE);
    
    if ((color == HEXRED) && (col == (size - 1)))
        G.SetVertexValue(REDGOAL, HEXRED);
    
    VertexIDSet vs;
    G.Neighbors(iCell, vs, true);                           // retrieve neighbors that have same color
    
    for (unsigned int i = 0; i < vs.size(); i++)
        UF.Join(iCell, vs[i]);                              // and connect to them        
        
    return HEXMOVE_OK;
}   

/* ----------------------------------------------------------------------------
   HexColor Winner(void);
   
   Returns the color (HEXBLUE, HEXRED) of the player who has won the game, 
   or HEXBLANK if no player has yet won the game.
   ---------------------------------------------------------------------------- */
HexColor HexBoard::Winner(void)
{        
    if (UF.Find(BLUEHOME) == UF.Find(BLUEGOAL))
        return HEXBLUE;
    if (UF.Find(REDHOME) == UF.Find(REDGOAL))
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
    UF.Reset(n2+4);                                                     // allocate space in UF structure
}

