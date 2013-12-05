#include <stdint.h>
#include <vector>

typedef enum enumHexBoardError {
    HEXBOARD_ERR_INVALIDSIZE = 0x111
} HexBoardError;

typedef enum enumHexColor {
    HEXBLANK = 0,
    HEXBLUE,
    HEXRED,
    HEXNULL
} HexColor;

class HexBitBoard {
    public:
        HexBitBoard(unsigned int n);
        void SetColor(unsigned int row, unsigned int col, HexColor color);
        HexColor GetColor(unsigned int row, unsigned int col);
        HexColor Winner(void);
        inline unsigned int Size(void) { return size; }
        
    private:
        std::vector<uint32_t> blueBits;
        std::vector<uint32_t> redBits;
        uint32_t initMask;
        uint32_t blueMask;
        uint32_t redMask;
        unsigned int size;
};



HexBitBoard::HexBitBoard(unsigned int n)
{
    if ((n < 3) || (n > 15))
        throw HEXBOARD_ERR_INVALIDSIZE;
        
    blueBits.resize(n, 0);
    redBits.resize(n, 0);
    
    initMask = (((uint32_t) -1) << (2 * n));
    blueMask = 0;
    redMask  = 0;
    
    for (unsigned int i = 0; i < n; i++)
    {
        blueBits[i] = initMask;
        redBits[i] = initMask;
        blueMask |= (((uint32_t) HEXBLUE) << (2 * n));
        redMask  |= (((uint32_t) HEXRED) << (2 * n));
    }
        
    size = n;
}

void HexBitBoard::SetColor(unsigned int row, unsigned int col, HexColor color)
{
    blueBits[row] &= ~(((uint32_t) 0x03)  << (2 * col));
    blueBits[row] |=  (((uint32_t) color) << (2 * col));
    
    // red bits are inverted (row, col) => (col, row)
    redBits[col]  &= ~(((uint32_t) 0x03)  << (2 * row));
    redBits[col]  |=  (((uint32_t) color) << (2 * row));
}

HexColor HexBitBoard::GetColor(unsigned int row, unsigned int col)
{
    uint32_t v = blueBits[row] & (0x03 << (2 * col));   // isolate the row, col we need    
    return (HexColor) (v >> (2 * col));                 // and return the bits as a low order value
}

HexColor HexBitBoard::Winner()
{
    uint32_t bmask = blueMask;
    uint32_t rmask = redMask;
    
    for (unsigned int irow = 0; irow < size; irow++)
    {
        bmask = ((blueBits[irow] & bmask) | ((blueBits[irow] >> 2) & bmask));
        rmask = ((redBits[irow]  & rmask) | ((redBits[irow]  >> 2) & rmask));
    }
        
    if (bmask != 0)
        return HEXBLUE;
    if (rmask != 0)
        return HEXRED;
    return HEXBLANK;
        
}

int main(int argc, char *argv[])
{
}