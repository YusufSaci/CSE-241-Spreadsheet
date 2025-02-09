#include "spreadSheet.h"


#include <iostream>
#include <string>
#include <iomanip>

using namespace utils;

namespace spreadsheet{

// Constructor to initialize a spreadsheet with given columns and rows
SpreadSheet::SpreadSheet(int cols, int rows) : grid(rows, Container<shared_ptr<Cell>>(cols)), colsLabel(cols, ""), rowsLabel(rows) {
    // Set positions for each cell in the grid
    initCols();  // Initialize column labels
    initRows();  // Initialize row labels
    initLabels(0,0);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
         
           grid[i][j]=make_shared<EmptyValueCell>();
        }
    }
}

// Default constructor to initialize an empty spreadsheet
SpreadSheet::SpreadSheet() : SpreadSheet(0, 0) {}

// Function to initialize labels for rows and columns
void SpreadSheet::initLabels(int x,int y) {
    
    // Print column labels in a formatted way with a highlight
   std::cout << "\033[" << 3 << ";" << 0 << "H" << std::flush;
    cout << "\033[7m" << "   " << "\033[0m" << std::flush;
    int size= (colsLabel.size()>SPRERAD_COL_SIZE) ? SPRERAD_COL_SIZE : colsLabel.size();
    for (int i=x;i<size+x;i++) {
        if (colsLabel[i].size() == 1)
            cout << "\033[7m" << "   " << colsLabel[i] << "   " << "\033[0m" << std::flush;
        else if (colsLabel[i].size() == 2)
            cout << "\033[7m" << "   " << colsLabel[i] << "  " << "\033[0m" << std::flush;
    }
    cout << endl;
    int size2 =(rowsLabel.size()>SPRERAD_ROW_SIZE) ? SPRERAD_ROW_SIZE : rowsLabel.size();
    // Print row labels with appropriate formatting
    for (int i=y;i<size2+y;i++) {
        cout << "\033[7m" << rowsLabel[i] << " " << "\033[0m" << std::flush;
        if (rowsLabel[i] < 10)
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
    if (grid[row][col]->getType() == Type::formula)
        cout << "\033[" << 1 << ";" << 6 << "H" << grid[row][col]->getContent() << "    "
             << fixed << setprecision(2) << stod(grid[row][col]->getValue())<< std::flush;
    else
        cout << "\033[" << 1 << ";" << 6 << "H" << grid[row][col]->getContent()<< std::flush;

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
Cell* SpreadSheet::getCell(int row, int col) {
    return grid[row][col].get();  // Return the cell at the specified position
}

shared_ptr<Cell>& SpreadSheet::getCell(int row, int col,int a) {
    return grid[row][col];  // Return the cell at the specified position
    
}

void SpreadSheet::setContent(int row, int col, const string& str) {

    // If the current cell is of type formula, remove its dependencies from all other cells.
    if (grid[row][col]->getType() == Type::formula) {
        for (int i = 0; i < getNumRows(); i++) {
            for (int j = 0; j < getNumCols(); j++) {
                grid[i][j]->remove(grid[row][col].get()); // Remove dependency.
            }
        }
    }

    // Check if the new content is empty.
    if (str.empty()) {
        // Create an EmptyValueCell and transfer the dependencies of the old cell.
        shared_ptr<Cell> ptr = make_shared<EmptyValueCell>();
        ptr->setDependents(grid[row][col]->getDependents()); // Set dependents from the previous cell.
        grid[row][col] = ptr; // Assign the new cell to the grid.
        ptr->setPosition(row + 4, col * CELL_SIZE + 4); // Update its position.
        ptr->setContent(str, *this); // Set its content as an empty string.
    }
    // Check if the new content is a formula (starts with '=' or '@').
    else if (str[0] == '=' || str[0] == '@') {
        // Create a FormulaCell and transfer dependencies.
        shared_ptr<Cell> ptr = make_shared<FormulaCell>();
        ptr->setDependents(grid[row][col]->getDependents());
        grid[row][col] = ptr;
        ptr->setPosition(row + 4, col * CELL_SIZE + 4);
        ptr->setContent(str, *this);
    }
    // Check if the new content is a double value (contains digits or '-' and has a decimal point).
    else if ((isdigit(str[0]) || str[0] == '-') && (str.find('.') != std::string::npos)) {
        // Create a DoubleValueCell and transfer dependencies.
        shared_ptr<Cell> ptr = make_shared<DoubleValueCell>();
        ptr->setDependents(grid[row][col]->getDependents());
        grid[row][col] = ptr;
        ptr->setPosition(row + 4, col * CELL_SIZE + 4);
        ptr->setContent(str, *this);
    }
    // Check if the new content is an integer value (contains digits or '-').
    else if ((isdigit(str[0]) || str[0] == '-')) {
        // Create an IntValueCell and transfer dependencies.
        shared_ptr<Cell> ptr = make_shared<IntValueCell>();
        ptr->setDependents(grid[row][col]->getDependents());
        grid[row][col] = ptr;
        ptr->setPosition(row + 4, col * CELL_SIZE + 4);
        ptr->setContent(str, *this);
    }
    // Otherwise, treat the new content as a string value.
    else if (!str.empty()) {
        // Create a StringValueCell and transfer dependencies.
        shared_ptr<Cell> ptr = make_shared<StringValueCell>();
        ptr->setDependents(grid[row][col]->getDependents());
        grid[row][col] = ptr;
        ptr->setPosition(row + 4, col * CELL_SIZE + 4);
        ptr->setContent(str, *this);
    }
}

// Function to set the content of a specific cell in the grid


// Function to set the content of a cell using a string value
void SpreadSheet::setCell(int row, int col, const string& newContent) {
    grid[row][col]->setContent(newContent, *this);  // Set the content for the specified cell
}

void SpreadSheet::printCell(string& printOnTerminal, int row, int col, int firstR, SpreadSheet& table) {

    // If the cell contains a value (not formula), print the value with padding
    if (table.getCell(row - firstR, col / CELL_SIZE)->getType() == Type::value) {
        printOnTerminal = table.getCell(row - firstR, col / CELL_SIZE)->getContent().substr(0,CELL_SIZE-1); // Get cell content
        while (printOnTerminal.size() < CELL_SIZE) {
            printOnTerminal = " " + printOnTerminal; // Add spaces to the left if the content is smaller than the cell size
        }
    }
    // If the cell does not contain a formula, print the content with spaces padded to the right
    else if (table.getCell(row - firstR, col / CELL_SIZE)->getType() != Type::formula) {
        printOnTerminal = table.getCell(row - firstR, col / CELL_SIZE)->getContent(); // Get cell content
        while (printOnTerminal.size() < CELL_SIZE) {
            printOnTerminal += " "; // Add spaces to the right if the content is smaller than the cell size
        }
    }
    // If the cell contains a formula, print the evaluated result with padding
    else {
        printOnTerminal = table.getCell(row - firstR, col / CELL_SIZE)->getValue().substr(0,CELL_SIZE-1); // Get the evaluated value of the formula
        printOnTerminal = " "+printOnTerminal; // Add spaces to the right to ensure the content fits the cell size
        
    }
}

void SpreadSheet::display(int row, int col, AnsiTerminal& terminal, SpreadSheet& table) {

    string print; // Temporary string to hold the content of the cell to be printed.

    // Case 1: Both row and column exceed the visible grid size.
    if (row > SPRERAD_ROW_SIZE - 1 && col > SPRERAD_COL_SIZE - 1) {
        initLabels(col - SPRERAD_COL_SIZE + 1, row - SPRERAD_ROW_SIZE + 1); // Adjust the row and column labels for the visible grid.

        // Loop through the visible rows and columns within the adjusted offsets.
        for (int i = row - SPRERAD_ROW_SIZE; i < row; i++) {
            for (int j = col - SPRERAD_COL_SIZE; j < col; j++) {
                table.printCell(print, i + 5, (j + 1) * CELL_SIZE + 4, 4, *this); // Fetch the content of the current cell.
                terminal.printAt(i + SPRERAD_ROW_SIZE - row + 4, (j + SPRERAD_COL_SIZE - col) * CELL_SIZE + 4, 
                                 print.substr(0, CELL_SIZE)); // Print the cell content within the terminal grid.
            }
        }
    }
    // Case 2: Only the row exceeds the visible grid size.
    else if (row > SPRERAD_ROW_SIZE - 1) {
        initLabels(0, row - SPRERAD_ROW_SIZE + 1); // Adjust only the row labels for the visible grid.

        // Loop through the visible rows and all columns.
        for (int i = row - SPRERAD_ROW_SIZE; i < row; i++) {
            for (int j = 0; j < SPRERAD_COL_SIZE; j++) {
                table.printCell(print, i + 5, j * CELL_SIZE + 4, 4, *this); // Fetch the content of the current cell.
                terminal.printAt(i + SPRERAD_ROW_SIZE - row + 4, j * CELL_SIZE + 4, 
                                 print.substr(0, CELL_SIZE)); // Print the cell content within the terminal grid.
            }
        }
    }
    // Case 3: Only the column exceeds the visible grid size.
    else if (col > SPRERAD_COL_SIZE - 1) {
        initLabels(col - SPRERAD_COL_SIZE + 1, 0); // Adjust only the column labels for the visible grid.

        // Loop through all rows and the visible columns.
        for (int i = 0; i < SPRERAD_ROW_SIZE; i++) {
            for (int j = col - SPRERAD_COL_SIZE; j < col; j++) {
                table.printCell(print, i + 4, (j + 1) * CELL_SIZE + 4, 4, *this); // Fetch the content of the current cell.
                terminal.printAt(i + 4, (j + SPRERAD_COL_SIZE - col) * CELL_SIZE + 4, 
                                 print.substr(0, CELL_SIZE)); // Print the cell content within the terminal grid.
            }
        }
    }
    // Case 4: Both row and column are within the visible grid size.
    else {
        initLabels(0, 0); // No adjustments needed for row or column labels.

        // Loop through all visible rows and columns.
        for (int i = 0; i < SPRERAD_ROW_SIZE; i++) {
            for (int j = 0; j < SPRERAD_COL_SIZE; j++) {
                table.printCell(print, i + 4, j * CELL_SIZE + 4, 4, *this); // Fetch the content of the current cell.
                terminal.printAt(i + 4, j * CELL_SIZE + 4, 
                                 print.substr(0, CELL_SIZE)); // Print the cell content within the terminal grid.
            }
        }
    }

    // Highlight the active cell in the terminal.
    table.printCell(print, row + 4, col * CELL_SIZE + 4, 4, *this); // Fetch the content of the active cell.
    terminal.printInvertedAt(row + 4, col * CELL_SIZE + 4, 
                             print.substr(0, CELL_SIZE)); // Print the active cell in inverted style for emphasis.
}


}
