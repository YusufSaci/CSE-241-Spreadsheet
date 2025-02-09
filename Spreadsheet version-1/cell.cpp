#include "cell.h"
#include "spreadSheet.h"
#include "formulaParser.h"
#include "AnsiTerminal.h"
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// Default constructor initializes content to an empty string and type to 'empty'
Cell::Cell() : content(""), contentType(Type::empty){}

// Getter function to retrieve the content of the cell
string Cell::getContent() const { 
    return content;
}

// Getter function to retrieve the type of content stored in the cell
Type Cell::getType() const { 
    return contentType; 
}

// Setter function to change the type of the content in the cell
void Cell::setType(Type newType) { 
    contentType = newType; 
}

// Getter function to retrieve the evaluated value of the cell
string Cell::getValue() const {
    return value;
}

// Getter functions to retrieve the row and column positions of the cell
int Cell::getRow() const { 
    return row; 
}
int Cell::getCol() const { 
    return col; 
}

// Setter function to set the value of the cell (for formula or values)
void Cell::setValue(const string& str) {
    value = str;
}

// Setter function to set the row and column positions of the cell
void Cell::setPosition(int r, int c) {
    row = r;
    col = c;
}

// Function to set the content of the cell and update its type and value based on content
void Cell::setContent(const string& newContent, SpreadSheet& table) {
    content = newContent; // Assign new content to the cell

    initType();  // Initialize the type of content based on the new content
    // If the content is a value, set the value and notify dependents
    if (contentType == Type::value) {
        value = content;   
    }
    else if(contentType == Type::formula){
        value="0";
        FormulaParser::parserFormula(*this, table); 
    }
    notifyDependents(table);  // Notify dependent cells of change
}

// Function to add a dependent cell to the current cell's list of dependents
void Cell::addDependents(Cell& add) {
    int flag = 1;  // Flag to check if the dependent cell is already added
    for (Cell* c : dependents) {
        // If the dependent cell is already in the list, set the flag to 0
        if (*c == add) {
            flag = 0;
        }
    }
    // If not already added and not the same cell, add it to the dependents list
    if (flag == 1 && this != &add && !add.checkCyclicDependency(this)) {
        dependents.push_back(&add);
    }
}


bool Cell::checkCyclicDependency(Cell* target) {
    // Check if the current cell is the same as the target cell.
    // If so, it means a cyclic dependency is detected (cell depends on itself).
    if (this == target) {
        return true; // A cyclic dependency exists.
    }

    // Iterate through all dependent cells of the current cell.
    for (Cell* dependent : dependents) {
        // Recursively check each dependent cell for a cyclic dependency.
        if (dependent->checkCyclicDependency(target)) {
            return true; // A cyclic dependency has been found.
        }
    }

    return false; // No cyclic dependency found.
}



// Function to notify all dependent cells that the value of the current cell has changed
void Cell::notifyDependents(SpreadSheet& spreadsheet) {
    if (dependents.size() != 0) {
        for (Cell* dep : dependents) {
            if(dep->getType()==Type::formula){
                dep->updateValue(spreadsheet); // Update the value of each dependent cell
            }   
            else{
                dep->dependents.clear();
            }
        }
    }
}

// Function to update the value of the cell (e.g., after evaluating a formula)
void Cell::updateValue(SpreadSheet& table) {
    // Parse the formula to evaluate its value
    FormulaParser::parserFormula(*this, table);  
     // Print the updated value in the cell
    cout << "\033[" << row << ";" << col << "H" <<" "<<getValue().substr(0, CELL_SIZE-1) << std::flush; 
    notifyDependents(table);  // Notify dependent cells of the update
}
//Function to initialize the type of the content based on the cell's content
void Cell::initType() {
    if (content.empty()) {
        value="";
        contentType = Type::empty;  // Empty content
    }
    else if (content[0] == '=' || content[0] == '@' ) {
        contentType = Type::formula;  // Formula type if starts with '=' or '@'
    }
    else if (isdigit(content[0]) || content[0] == '-') {
        contentType = Type::value;  // Numeric value if it's a digit (or negative)
    } 
    else {
        value="";
        contentType = Type::string;  // Otherwise, treat as a string
    }
}


// Comparison operator to check if two cells are equal based on their row and column positions
bool Cell::operator==(const Cell& o) const {
    return (row == o.row && col == o.col);  // Cells are equal if their row and column match
}
