#include "Nonogram.h"
#include <fstream>

Nonogram::Nonogram(const std::initializer_list<std::initializer_list<int>>& horizontalHints, std::initializer_list<std::initializer_list<int>> verticalHints)
    : m_Width   { uint8_t(verticalHints.size()) }
    , m_Height  { uint8_t(horizontalHints.size()) }
{
    // Fill the horizontal hint vector
    for (auto hint : horizontalHints)
        m_HorizontalHints.push_back(hint);

    // Fill the vertical hint vector
    for (auto hint : verticalHints)
        m_VerticalHints.push_back(hint);

    // Generate empty Grid
    m_ImpossibleSquares = m_Grid = std::vector<bool>(m_Width * m_Height, false);
}

inline Nonogram::Nonogram(const std::vector<bool>& grid, int width, int height)
    : m_Width   { uint8_t(width) }
    , m_Height  { uint8_t(height) }
    , m_Grid    { grid }
{
    m_ImpossibleSquares = std::vector<bool>(m_Width * m_Height, false);
    GenerateHints();
}

Nonogram::Nonogram(int width, int height)
    : m_Width{ uint8_t(width) }
    , m_Height{ uint8_t(height) }
{
    m_ImpossibleSquares = m_Grid = std::vector<bool>(m_Width * m_Height, false);
    GenerateHints();
}

Nonogram::Nonogram(const std::wstring& filePath)
{
    std::ifstream ifStream;
    try {
        ifStream.open(filePath, std::ios::binary);

        ifStream >> m_Width;
        ifStream >> m_Height;

        m_Grid = std::vector<bool>(m_Width * m_Height, false);

        ifStream.read((char*)m_Grid._Myvec.data(), m_Grid._Myvec.size() * sizeof(m_Grid._Myvec.data()));

        m_ImpossibleSquares = std::vector<bool>(m_Width * m_Height, false);

        GenerateHints();
    }
    catch (std::exception e)
    {
        e.what();
    }
    ifStream.close();
}

void Nonogram::GenerateHints()
{
    if (m_IsLocked) return;

    m_HorizontalHints   = std::vector<std::vector<int>>(m_Height, std::vector<int>());
    m_VerticalHints     = std::vector<std::vector<int>>(m_Width, std::vector<int>());

    // generate horizontal hints
    for (size_t y = 0; y < m_Height; ++y)
    {
        UpdateHintRow(y);
    }

    // generate vertical hints
    for (size_t x = 0; x < m_Width; ++x)
    {
        UpdateHintColumn(x);
    }
}

void Nonogram::ClearGrid()
{
    if (m_IsLocked) return;

    for (size_t i{}; i < m_Grid.size(); ++i) {
        m_Grid[i] = false;
        m_ImpossibleSquares[i] = false;
    }
}

void Nonogram::SaveToFile(const std::wstring& filename) const
{
    if (m_IsLocked) return;

    std::ofstream ofStream;
    ofStream.open(filename, std::ios::binary);
    ofStream << m_Width;
    ofStream << m_Height;
    ofStream.write((char*)m_Grid._Myvec.data(), m_Grid._Myvec.size() * sizeof(m_Grid._Myvec.data()));

    ofStream.close();
}

bool Nonogram::SwitchSquare(int x, int y)
{
    if (m_IsLocked) return false;

    x = std::min(std::max(0, x), int(m_Width - 1));
    y = std::min(std::max(0, y), int(m_Height - 1));

    int index = x + y * m_Width;

    bool newVal = !m_Grid[index];
    m_Grid[index] = newVal;

    UpdateHintRow(y);
    UpdateHintColumn(x);

    return newVal;
}

void Nonogram::Unlock()
{
    m_IsLocked = false;
}

void Nonogram::UpdateHintColumn(int x)
{
    if (m_IsLocked) return;

    m_VerticalHints[x].clear();
    m_VerticalHints[x].push_back(0);

    int counter{}; // hint number
    bool previousValue{};
    for (size_t i = 0; i < m_Height; ++i)
    {
        bool gridVal{ m_Grid[i * m_Width + x] };
        if (gridVal)
        {
            ++m_VerticalHints[x][counter];
        }
        if (!gridVal && previousValue)
        {
            ++counter;
            m_VerticalHints[x].push_back(0);
        }
        previousValue = gridVal;
    }
    if (m_VerticalHints[x].back() == 0 && m_VerticalHints[x].size() > 1) m_VerticalHints[x].pop_back();
}

void Nonogram::UpdateHintRow(int y)
{
    if (m_IsLocked) return;

    int counter{}; // hint number
    bool previousValue{};

    m_HorizontalHints[y].clear();
    m_HorizontalHints[y].push_back(0);

    for (size_t i = 0; i < m_Width; ++i)
    {
        bool gridVal{ m_Grid[y * m_Width + i] };
        if (gridVal)
        {
            ++m_HorizontalHints[y][counter];
        }
        if (!gridVal && previousValue)
        {
            ++counter;
            m_HorizontalHints[y].push_back(0);
        }
        previousValue = gridVal;
    }
    if (m_HorizontalHints[y].back() == 0 && m_HorizontalHints[y].size() > 1) m_HorizontalHints[y].pop_back();
}

void Nonogram::SolveRecursiveBacktracking()
{
    if (m_IsLocked) return;

    ClearGrid();

    m_IsLocked = true;

    if (!SetNextValueRecursion(0, true))
    {
        SetNextValueRecursion(0, false);
    }

    m_IsLocked = false;
}

void Nonogram::SolveImprovedRecursiveBacktracking()
{
    if (m_IsLocked) return;

    ClearGrid();

    m_IsLocked = true;

    FillFreeSquares();

    if (!SetNextValueRecursion(0, true))
    {
        SetNextValueRecursion(0, false);
    }

    m_IsLocked = false;
}


bool Nonogram::CheckIfValidSquare(int xPos, int yPos)
{
    int pos = yPos * m_Width + xPos;

    //check row
    {
        int rowChainCounter{ };
        int hintNumber{ -1 };
        bool previousSquare{ };

        for (int x = 0; x <= xPos; ++x)
        {
            bool currentSquare{ m_Grid[yPos * m_Width + x] };
            if (currentSquare)
            {
                ++rowChainCounter;

                if (!previousSquare) ++hintNumber;
            }

            if (previousSquare && !currentSquare)
            {
                if (m_HorizontalHints[yPos][hintNumber] != rowChainCounter)
                {
                    return false;
                }
                rowChainCounter = 0;
            }
            previousSquare = currentSquare;
        }

        if (hintNumber >= int(m_HorizontalHints[yPos].size()) || // if there are more hints than possible
            (hintNumber != -1 && m_HorizontalHints[yPos][hintNumber] < rowChainCounter)) // the chain is bigger than the current hint value
        {
            return false;
        }
        
        //check if final square
        if (xPos == m_Width - 1 && // if at final pos
                (hintNumber != int(m_HorizontalHints[yPos].size()) - 1 || // and we arent at the right amount of hints
                    previousSquare && m_HorizontalHints[yPos][hintNumber] != rowChainCounter)) // or the hint number doesnt correspond with the chain number
        {
            return false;
        }
    }

    //check column
    {
        int columnChainCounter{ };
        int hintNumber{ -1 };
        bool previousSquare{ };

        for (int y = 0; y <= yPos; ++y)
        {
            bool currentSquare{ m_Grid[y * m_Width + xPos] };
            if (currentSquare)
            {
                ++columnChainCounter;

                if (!previousSquare) ++hintNumber;
            }

            if (previousSquare && !currentSquare)
            {
                if (m_VerticalHints[xPos][hintNumber] != columnChainCounter)
                {
                    return false;
                }
                columnChainCounter = 0;
            }
            previousSquare = currentSquare;
        }

        //check final tile
        /*if (previousSquare)
        {
            if (hintNumber >= int(m_VerticalHints[xPos].size()) || m_VerticalHints[xPos][hintNumber] < columnChainCounter)
            {
                return false;
            }
        }*/

        if (hintNumber >= int(m_VerticalHints[xPos].size()) || // if there are more hints than possible
            (hintNumber != -1 && m_VerticalHints[xPos][hintNumber] < columnChainCounter)) // the chain is bigger than the current hint value
        {
            return false;
        }

        //check if final square
        if (yPos == m_Height - 1 && // if at final pos
            (hintNumber != int(m_VerticalHints[xPos].size()) - 1 || // and we arent at the right amount of hints
                previousSquare && m_VerticalHints[xPos][hintNumber] != columnChainCounter)) // or the hint number doesnt correspond with the chain number
        {
            return false;
        }
    }
    return true;
}

bool Nonogram::SetNextValueRecursion(int position, bool value)
{
    if (!m_IsLocked) return false;

    //if final position + 1: return true
    if (position == m_Width * m_Height)
    {
        return true;
    }

    int xPos = position % m_Width;
    int yPos = position / m_Width;

    //if there is already a value here instead of none:
    //dont modify the value but ask the next tile
    if (m_Grid[position] || m_ImpossibleSquares[position])
    {
        if (CheckIfValidSquare(xPos, yPos)) {

            //if next position is already calculated, dont make him do the steps
            if (position + 1 != m_Width * m_Height && (m_Grid[position + 1] || m_ImpossibleSquares[position + 1]))
            {
                //if they return true also return true
                return SetNextValueRecursion(position + 1, true);
            }
            //ask the next tile to be filled
            if (SetNextValueRecursion(position + 1, true))
            {
                //if they said it was valid, also return valid
                return true;
            }
            //if filled didn't work, ask if it will work as empty
            else if (SetNextValueRecursion(position + 1, false))
            {
                //if the said it is valid return true
                return true;
            }
            else
            {
                //if neither is valid:
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    //place the value
    m_Grid[position] = value;

    //check if value doesnt invalidate the board
    if (CheckIfValidSquare(xPos, yPos))
    {
        //if next position is already calculated, dont make him do the steps
        if (position + 1 != m_Width * m_Height)
        {
            if (m_Grid[position + 1] || m_ImpossibleSquares[position + 1])
            {
                //if they return true also return true
                if (SetNextValueRecursion(position + 1, true))
                {
                    return true;
                }
                else
                {
                    m_Grid[position] = false;
                    return false;
                }
            }
        }
        //if it is valid ask the next cell to place a value
        if (SetNextValueRecursion(position + 1, true))
        {
            //if the next cell is also valid: return true
            return true;
        }
        //if not valid try with other value
        else if (SetNextValueRecursion(position + 1, false))
        {
            //if its valid now return true;
            return true;
        }
        else
        {
            //if both are invalid that means that our value is also invalid. so return false;
            //also change back the value
            m_Grid[position] = false;
            return false;
        }
    }
    else
    {
        //return false if tile placement isn't valid
        //also change back the value
        m_Grid[position] = false;
        return false;
    }
}

void Nonogram::FillFreeSquares()
{
    // Complete rows as much as possible.
    // Use the overlap of all possible combinations
    // Example:
    // ╔═══╦───────────────────┐
    // ║4 3║▒│▒│▒│▒│ │▒│▒│▒│ │ │
    // ╚═══╩───────────────────┘
    // Tt can also be written as
    // ╔═══╦───────────────────┐
    // ║4 3║ │▒│▒│▒│▒│ │▒│▒│▒│ │
    // ╚═══╩───────────────────┘
    // Or
    // ╔═══╦───────────────────┐
    // ║4 3║ │ │▒│▒│▒│▒│ │▒│▒│▒│
    // ╚═══╩───────────────────┘
    // There are also some other ones that have the final 3 blocks in different positions,
    // But if you look at each chain individually, they are in all of the possible locations.
    // Now we just take the overlapping squares (Intersection):
    // ╔═══╦───────────────────┐
    // ║4 3║ │ │▓│▓│ │ │ │▓│ │ │
    // ╚═══╩───────────────────┘
    // These are the squares that will always be filled in this specific example.
    //
    // Instead of checking every combination there is fast way to calculate it:

    // Go over every row
    for (int RowIdx = 0; RowIdx < m_Height; RowIdx++)
    {
        // Sum the hint values and add 1 every time there is a space in between
        int hintSum{};

        for (int hintValue : m_HorizontalHints[RowIdx])
            hintSum += 1 + hintValue;
        --hintSum; // Remove one because we dont care about the space after the last cluster of squares

        // The amount of empty spaces that that come after all the squares if we were to place them next to each other
        int totalDifference{ m_Width - hintSum };

        // go over the row again, this time filling in the squares
        int rowPosition{};
        for (int hintValue : m_HorizontalHints[RowIdx])
        {
            int hintDifference{ hintValue - totalDifference };
            rowPosition += hintValue - std::max(0, hintDifference);
            if (hintDifference > 0)
            {
                for (int i = 0; i < hintDifference; ++i)
                {
                    m_Grid[RowIdx * m_Width + rowPosition] = true;
                    ++rowPosition;
                }
            }
            ++rowPosition;
        }
    }

    // Repeat for columns
    for (int ColumnIdx = 0; ColumnIdx < m_Width; ColumnIdx++)
    {
        int hintSum{};

        for (int hintValue : m_VerticalHints[ColumnIdx])
            hintSum += 1 + hintValue;
        --hintSum;

        int totalDifference{ m_Height - hintSum };

        int columnPosition{};
        for (int hintValue : m_VerticalHints[ColumnIdx])
        {
            int hintDifference{ hintValue - totalDifference };
            columnPosition += hintValue - std::max(0, hintDifference);
            if (hintDifference > 0)
            {
                for (int i = 0; i < hintDifference; ++i)
                {
                    m_Grid[columnPosition * m_Width + ColumnIdx] = true;
                    ++columnPosition;
                }
            }
            ++columnPosition;
        }
    }

    // if there are empty rows mark them as empty
    for (int rowIdx = 0; rowIdx < m_Height; rowIdx++)               // for every row
        if (m_HorizontalHints[rowIdx].front() == 0)                 // if the first hint is 0
            for (int i = 0; i < m_Width; ++i)                       // go over every square
                m_ImpossibleSquares[rowIdx * m_Width + i] = true;   // mark it impossible

    // same with columns
    for (int columnIdx = 0; columnIdx < m_Width; columnIdx++)
        if (m_VerticalHints[columnIdx].front() == 0)
            for (int i = 0; i < m_Width; ++i)
                m_ImpossibleSquares[i * m_Width + columnIdx] = true;

    // continue existing chains.
    // example:
    // ╔═╦───────────────────┐
    // ║4║ │▓│ │ │ │ │ │ │ │ │
    // ╚═╩───────────────────┘
    // in this case you can extend the first filled square to this:
    // ╔═╦───────────────────┐
    // ║4║ │▓│▓│▓│ │░│░│░│░│░│
    // ╚═╩───────────────────┘
    // (░ means there cant be anything there)

    // Starting from left to right
    for (int y = 0; y < m_Height; y++)
    {
        if (m_HorizontalHints[y].front() == 0) continue; // Skip if no hints
        
        // go over squares until we find a filled in one or until we succeed the first hints value
        bool encounteredFilledSquare{};
        int filledSquareCounter{};
        for (int x{}; x < m_HorizontalHints[y].front(); ++x)
        {
            encounteredFilledSquare |= m_Grid[y * m_Width + x]; // set and keep true once we found a filled square

            if (encounteredFilledSquare) {
                m_Grid[y * m_Width + x] = true; // if we've encountered a filled square before, fill this one
                ++filledSquareCounter; // track the amount of filled squares we've seen incase there is only 1 hint in the row
            }
        }
        if (m_HorizontalHints[y].size() == 1 && encounteredFilledSquare) // If there is only 1 hint in the row, fill the remaining as imposible
            for (int x{ 2 * m_HorizontalHints[y].front() - filledSquareCounter }; x < m_Width; ++x)
                m_ImpossibleSquares[y * m_Width + x] = true;
    }

    // same but from right to left
    for (int y = 0; y < m_Height; y++)
    {
        if (m_HorizontalHints[y].front() == 0) continue;

        bool encounteredFilledSquare{};
        int filledSquareCounter{};
        for (int x{}; x < m_HorizontalHints[y].back(); ++x)
        {
            encounteredFilledSquare |= m_Grid[y * m_Width + (m_Width - x - 1)];

            if (encounteredFilledSquare) {
                m_Grid[y * m_Width + (m_Width - x - 1)] = true;
                ++filledSquareCounter;
            }
        }
        if (m_HorizontalHints[y].size() == 1 && encounteredFilledSquare)
            for (int x{ 2 * m_HorizontalHints[y].front() - filledSquareCounter }; x < m_Width; ++x)
                m_ImpossibleSquares[y * m_Width + (m_Width - x - 1)] = true;
    }

    for (int x = 0; x < m_Width; x++)
    {
        if (m_VerticalHints[x].front() == 0) continue; // Skip if no hints

        bool encounteredFilledSquare{};
        int filledSquareCounter{};
        for (int y{}; y < m_VerticalHints[x].front(); ++y)
        {
            encounteredFilledSquare |= m_Grid[y * m_Width + x];

            if (encounteredFilledSquare) {
                m_Grid[y * m_Width + x] = true;
                ++filledSquareCounter;
            }
        }
        if (m_VerticalHints[x].size() == 1 && encounteredFilledSquare) 
            for (int y{ 2 * m_VerticalHints[x].front() - filledSquareCounter }; y < m_Height; ++y)
                m_ImpossibleSquares[y * m_Width + x] = true;
    }

    for (int x = 0; x < m_Width; x++)
    {
        if (m_VerticalHints[x].front() == 0) continue; // Skip if no hints

        bool encounteredFilledSquare{};
        int filledSquareCounter{};
        for (int y{}; y < m_VerticalHints[x].back(); ++y)
        {
            encounteredFilledSquare |= m_Grid[(m_Height - y - 1) * m_Width + x];

            if (encounteredFilledSquare) {
                m_Grid[(m_Height - y - 1) * m_Width + x] = true;
                ++filledSquareCounter;
            }
        }
        if (m_VerticalHints[x].size() == 1 && encounteredFilledSquare) 
            for (int y{ 2 * m_VerticalHints[x].front() - filledSquareCounter }; y < m_Height; ++y)
                m_ImpossibleSquares[(m_Height - y - 1) * m_Width + x] = true;
    }
}