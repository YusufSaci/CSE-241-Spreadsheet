#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "cell.h"  // Include the header file for the Cell class
#include <string>  // Include the string library
#include <vector>  // Include the vector library

#define CELL_SIZE 7  // Define the default size for cells 

using namespace std;

// Definition of the SpreadSheet class
class SpreadSheet {

public:
    // Default constructor: initializes the spreadsheet with default values
    SpreadSheet();

    // Constructor that initializes the spreadsheet with a specified number of rows and columns
    SpreadSheet(int rows, int cols);

    // Sets the content of a specific cell by row and column (using string content)
    void setCell(int row, int col, const string& str);

    // Returns a reference to the Cell object at the specified row and column
    Cell& getCell(int row, int col);

    // Sets a specific cell to the given Cell object (overload with Cell object)
    void setCell(int row, int col, const Cell& cell);

    // Returns the number of columns in the spreadsheet
    int getNumCols() const;

    // Returns the number of rows in the spreadsheet
    int getNumRows() const;

    // Displays information about a specific cell (e.g., for user interaction)
    void infoCell(int row, int col, int firstR) const;

    // Handles input function related to a specific cell (may involve formula or content)
    void inputFunc(int row, int col, int firstR, const string& input) const;

    // Cleans up (resets) the content or functionality of a input function
    void cleanFunc(int row, int col, int firstR) const;

private:
    // Stores labels for the columns (e.g., A, B, C, ...)
    vector<string> colsLabel;

    // Stores labels for the rows (e.g., 1, 2, 3, ...)
    vector<int> rowsLabel;

    // The main grid of the spreadsheet, represented as a 2D vector of Cell objects
    vector<vector<Cell>> grid;

    // Initializes the column labels (for example, A, B, C...)
    void initCols();

    // Initializes the row labels (for example, 1, 2, 3...)
    void initRows();

    // Initializes both column and row labels in the spreadsheet
    void initLabels();
};

#endif
