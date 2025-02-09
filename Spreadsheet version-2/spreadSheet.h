#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "cell.h"  // Include the header file for the Cell class
#include <string>  // Include the string library
#include <vector>  // Include the vector library
#include <memory>
#include "container.h"
#include"container.cpp"
#include "AnsiTerminal.h"

#define CELL_SIZE 7  // Define the default size for cells 
#define SPRERAD_ROW_SIZE 40
#define SPRERAD_COL_SIZE 30

using namespace std;
using namespace utils;

// Definition of the SpreadSheet class

namespace spreadsheet {

class SpreadSheet {
public:
    // Default constructor: initializes the spreadsheet with default values
    SpreadSheet();

    // Constructor that initializes the spreadsheet with a specified number of rows and columns
    SpreadSheet(int rows, int cols);

    // Sets the content of a specific cell by row and column (using string content)
    void setCell(int row, int col, const string& str);

    // Returns a reference to the Cell object at the specified row and column
    Cell* getCell(int row, int col);
    
    shared_ptr<Cell>& getCell(int row, int col,int a);

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

    // Sets the content of a specific cell in the spreadsheet.
    void setContent(int row,int col,const string& str);

    // Displays the spreadsheet grid
    void display(int row,int col,AnsiTerminal& terminal,SpreadSheet& table);

    // Converts the cell to a printable format
    void printCell(string& printOnTerminal, int row, int col, int firstR, SpreadSheet& table);

private:
    // Stores labels for the columns (e.g., A, B, C, ...)
    Container<string> colsLabel;

    // Stores labels for the rows (e.g., 1, 2, 3, ...)
    Container<int> rowsLabel;

    // The main grid of the spreadsheet, represented as a 2D CONTAINER of Cell objects
    Container<Container<shared_ptr<Cell>>> grid;

    // Initializes the column labels (for example, A, B, C...)
    void initCols();

    // Initializes the row labels (for example, 1, 2, 3...)
    void initRows();

    // Initializes both column and row labels in the spreadsheet
    void initLabels(int,int);
};

}


#endif
