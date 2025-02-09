#include "spreadSheet.h"
#include <iostream>
#include <string>
#include <iomanip>

// Constructor to initialize a spreadsheet with given columns and rows
SpreadSheet::SpreadSheet(int cols, int rows) : grid(rows, vector<Cell>(cols)), colsLabel(cols, ""), rowsLabel(rows) {
    initLabels();  // Initialize column and row labels
    // Set positions for each cell in the grid
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j].setPosition(i + 4, j * CELL_SIZE + 4);  // Position cells with an offset for visual representation
        }
    }
}

// Default constructor to initialize an empty spreadsheet
SpreadSheet::SpreadSheet() : SpreadSheet(0, 0) {}

// Function to initialize labels for rows and columns
void SpreadSheet::initLabels() {
    initCols();  // Initialize column labels
    initRows();  // Initialize row labels

    // Print column labels in a formatted way with a highlight
    cout << "\033[7m" << "   " << "\033[0m" << std::flush;
    for (const string& i : colsLabel) {
        if (i.size() == 1)
            cout << "\033[7m" << "   " << i << "   " << "\033[0m" << std::flush;
        else if (i.size() == 2)
            cout << "\033[7m" << "   " << i << "  " << "\033[0m" << std::flush;
    }
    cout << endl;

    // Print row labels with appropriate formatting
    for (int i : rowsLabel) {
        cout << "\033[7m" << i << " " << "\033[0m" << std::flush;
        if (i < 10)
            cout << "\033[7m" << " " << "\033[0m" << std::flush;
        cout << endl;
    }
}

// Function to initialize row labels from 1 upwards
void SpreadSheet::initRows() {
    for (int i = 0; i < rowsLabel.size(); i++) {
        rowsLabel[i] = i + 1;  // Assign row numbers starting from 1
    }
}

// Function to initialize column labels (e.g., A, B, C, ... Z, AA, AB, etc.)
void SpreadSheet::initCols() {
    char ch;
    for (int i = 0, ch = 'A'; i < colsLabel.size(); i++, ch++) {
        // Handle cases where column index exceeds 26 (e.g., AA, AB, etc.)
        if ((i / 26) != 0)
            colsLabel[i] += (i / 26) + 'A' - 1;

        if (i % 26 == 0)
            ch = 'A';
        
        colsLabel[i] += ch;  // Add the character label for the column
    }
}

// Function to display information about a specific cell (row, col) in a formatted way
void SpreadSheet::infoCell(int row, int col, int firstR) const {
    string empty(50, ' ');  // Create an empty string for clearing previous outputs
    string temp = "";

    // Calculate the column label (e.g., A, B, C, ..., Z, AA, AB, etc.)
    if ((col / 26) != 0)
        temp += (col / 26 - 1) + 'A';
    temp += (col % 26) + 'A';

    // Print row and column info with formatting and clearing old data
    cout << "\033[" << 1 << ";" << 1 << "H" << empty<< std::flush;;
    cout << "\033[" << 1 << ";" << 6 << "H" << empty<< std::flush;;
    cout << "\033[" << 1 << ";" << 1 << "H" << temp<< std::flush;
    cout << "\033[" << 1 << ";" << 2 + col / 26 << "H" << row + 1<< std::flush;

    // Check if the cell contains a formula or a regular value, and print accordingly
    if (grid[row][col].getType() == Type::formula)
        cout << "\033[" << 1 << ";" << 6 << "H" << grid[row][col].getContent() << "    "
             << fixed << setprecision(2) << stod(grid[row][col].getValue())<< std::flush;
    else
        cout << "\033[" << 1 << ";" << 6 << "H" << grid[row][col].getContent()<< std::flush;

    // Move the cursor back to the cell position
    cout << "\033[" << row + firstR << ";" << col *  CELL_SIZE + 4 << "H" << std::flush;
}

// Function to handle input for a cell (used for backspacing or updating input)
void SpreadSheet::inputFunc(int row, int col, int firstR, const string& input) const {
    string empty(50, ' ');  // Create an empty string for clearing old input
    cout << "\033[" << 2 << ";" << 0 << "H" << empty;  // Clear previous input
    cout << "\033[" << 2 << ";" << 0 << "H" << input;  // Print the new input
}

// Function to clear the content of input function.
void SpreadSheet::cleanFunc(int row, int col, int firstR) const {
    string empty(50, ' ');  // Create an empty string to clear the input area
    cout << "\033[" << 2 << ";" << 0 << "H" << empty;  // Clear the input area
    cout << "\033[" << row + firstR << ";" << col *  CELL_SIZE + 4 << "H" << std::flush;  // Move the cursor back to the original position
}

// Getter function to return the number of columns in the spreadsheet
int SpreadSheet::getNumCols() const {
    return colsLabel.size();  // Return the number of columns based on the label count
}

// Getter function to return the number of rows in the spreadsheet
int SpreadSheet::getNumRows() const {
    return rowsLabel.size();  // Return the number of rows based on the label count
}

// Getter function to return a reference to a specific cell in the grid
Cell& SpreadSheet::getCell(int row, int col) {
    return grid[row][col];  // Return the cell at the specified position
}

// Function to set the content of a specific cell in the grid
void SpreadSheet::setCell(int row, int col, const Cell& cell) {
    grid[row][col] = cell;  // Set the cell at the specified position
}

// Function to set the content of a cell using a string value
void SpreadSheet::setCell(int row, int col, const string& newContent) {
    grid[row][col].setContent(newContent, *this);  // Set the content for the specified cell
}
