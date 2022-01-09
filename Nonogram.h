#pragma once
#include <vector>
#include <string>

class Nonogram
{
public:

    Nonogram(const std::initializer_list<std::initializer_list<int>>& horizontalHints, std::initializer_list<std::initializer_list<int>> verticalHints);
    Nonogram(const std::vector<bool>& grid, int width, int height);
    Nonogram(int width, int height);
    Nonogram(const std::wstring& filePath);
    ~Nonogram() = default;

    Nonogram(const Nonogram& other) = default;
    Nonogram& operator=(const Nonogram& other) = default;
    Nonogram(Nonogram&& other) = default;
    Nonogram& operator=(Nonogram&& other) = default;
    
    // Generate the hints from the current grid
    void GenerateHints();

    // Clear the grid without clearing the hints
    // used for the solvers to start solving
    void ClearGrid();

    // Saves the nonogram to the given file path
    void SaveToFile(const std::wstring& filename) const;

    // change the value of the square and return the new value of the square
    bool SwitchSquare(int x, int y);

    // stop the solver and unlock the grid
    void Unlock();

    const std::vector<bool>& getGrid() const { return m_Grid; }
    const std::vector<bool>& getImpossibleGrid() const { return m_ImpossibleSquares; }
    const std::vector<std::vector<int>>& GetHorizontalHints() const { return m_HorizontalHints; }
    const std::vector<std::vector<int>>& GetVerticalHints() const { return m_VerticalHints; }
    int GetWidth() const { return int(m_Width); }
    int GetHeight() const { return int(m_Height); }

private:

    void UpdateHintColumn(int x);
    void UpdateHintRow(int y);

    std::vector<bool> m_Grid;
    std::vector<std::vector<int>> m_HorizontalHints;
    std::vector<std::vector<int>> m_VerticalHints;

    uint8_t m_Width;
    uint8_t m_Height;
    bool m_IsLocked = false; // no changes can be made when locked

public: // Solvers

    // Reset and solve the nonogram using Recursive Backtracking
    void SolveRecursiveBacktracking();

    // Reset and solve the nonogram by first filling in guarantied squares and then applying recursive backtracking
    void SolveImprovedRecursiveBacktracking();

private: // Solver Helpers

    // Squares that have to be empty
    // Usually marked with a cross in normal playing
    std::vector<bool> m_ImpossibleSquares;

    bool CheckIfValidSquare(int xPos, int yPos);

    bool SetNextValueRecursion(int position, bool value);

    // Fill all the squares that have to be filled
    // See explanation in function code
    void FillFreeSquares();
};
