#include "cell.h"
#include "spreadSheet.h"
#include "formulaParser.h"
#include "AnsiTerminal.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <memory>
#include "container.h"

using namespace std;
using namespace utils;

namespace spreadsheet{

// Getter for the row of the cell
int Cell::getRow() const {
    return row;
}

// Getter for the column of the cell
int Cell::getCol() const {
    return col;
}

// Setter for the position (row and column) of the cell
void Cell::setPosition(int r, int c) {
    row = r;
    col = c;
}


// Adds a dependent cell to the current cell's dependents list
void Cell::addDependents(Cell* add) {
    int flag = 1;  // Flag to check if the dependent cell is already added
    for (Cell * c : dependents) {
        // If the dependent cell is already in the list, set the flag to 0
        if (*c == *add) {
            flag = 0;
        }
    }
    // If not already added and not the same cell, add it to the dependents list
    if (flag == 1 && this != add && !add->checkCyclicDependency(this)) {
        dependents.push_back(add);
    }
}

// Getter for all dependent cells of the current cell
Container<Cell*> Cell::getDependents() const {
    return dependents;
}

// Setter to update the list of dependent cells
void Cell::setDependents(const Container<Cell*>& newDependents) {
    dependents = newDependents;
}

// Checks if there's a cyclic dependency involving the current cell
bool Cell::checkCyclicDependency(Cell* target) {
    // Check if the current cell is the same as the target cell (cyclic dependency)
    if (this == target) {
        return true; // A cyclic dependency exists
    }

    // Recursively check through all dependent cells for a cyclic dependency
    for (auto& dependent : dependents) {
        if (dependent->checkCyclicDependency(target)) {
            return true; // A cyclic dependency has been found
        }
    }

    return false; // No cyclic dependency found
}

// Notifies all dependent cells that the value of the current cell has changed
void Cell::notifyDependents(SpreadSheet& spreadsheet) {
    if (dependents.size() != 0) {
        for (auto& dep : dependents) {
            // If the dependent cell is a formula, update its value
            if (dep->getType() == Type::formula) {
                dep->updateValue(spreadsheet); // Update the value of each dependent cell
            }   
        }
    }
}

// Removes a dependent cell from the dependents list
void Cell::remove(Cell* cell) {
    auto it = std::remove(dependents.begin(), dependents.end(), cell);
    if (it != dependents.end()) {
        dependents.erase(it); // Remove the cell from the list
    }
}

// Updates the value of the current cell based on its formula/content
void Cell::updateValue(SpreadSheet& table) {
    try {
        // Parse and evaluate the formula for this cell
        FormulaParser::parserFormula(this, table);
        
        // Print the updated value in the terminal at the given position (row, col)
        if(row < SPRERAD_ROW_SIZE + 4 && col < 7 * SPRERAD_COL_SIZE - 2)
            cout << "\033[" << row << ";" << col << "H" << " " << getValue().substr(0, CELL_SIZE - 1) << std::flush;
        
        // Notify dependent cells of the update
        notifyDependents(table);
    }
    catch(exception& e) {
        // If an error occurs during formula parsing, notify the user
        table.inputFunc(row, col, 1, e.what());
    }
}

// Equality operator to compare two cells based on their row and column
bool Cell::operator==(const Cell& o) const {
    return (row == o.row && col == o.col);  // Cells are equal if their row and column match
}

// FormulaCell class methods

// Returns the formula content as a string
string FormulaCell::getContent() const {
    return formula;
}

// Returns the evaluated value of the formula
string FormulaCell::getValue() const {
    return result;
}

// Returns the type as 'formula'
Type FormulaCell::getType() const {
    return Type::formula;
}

// Sets the formula content and re-evaluates the formula
void FormulaCell::setContent(const string& str, SpreadSheet& table) {
    formula = str;
    result = "0";  // Default value before formula evaluation
    try {
        FormulaParser::parserFormula(this, table);
        notifyDependents(table);  // Notify dependents of the update
    }
    catch(exception& e) {
        table.inputFunc(row, col, 1, e.what());
    }
}

// Sets the evaluated result of the formula
void FormulaCell::setValue(const string& str) {
    result = str;
}

// IntValueCell class methods

// Returns the integer value as a string
string IntValueCell::getContent() const {
    return to_string(value);
}

// Returns the integer value as a string
string IntValueCell::getValue() const {
    return to_string(value);
}

// Sets the content of the cell as an integer value
void IntValueCell::setContent(const string& str, SpreadSheet& table) {
    value = stoi(str);
    notifyDependents(table);  // Notify dependents of the update
}

// Sets the integer value directly
void IntValueCell::setValue(const string& str) {
    value = stoi(str);
}

// Returns the type as 'value' for integer value cells
Type IntValueCell::getType() const {
    return Type::value;
}

// StringValueCell class methods

// Returns the string value
string StringValueCell::getContent() const {
    return value;
}

// Sets the content of the cell as a string
void StringValueCell::setContent(const string& str, SpreadSheet& table) {
    value = str;
    notifyDependents(table);  // Notify dependents of the update
}

// Returns the type as 'string' for string value cells
Type StringValueCell::getType() const {
    return Type::string;
}

// Returns the string value
string StringValueCell::getValue() const {
    return value;
}

// Sets the string value directly
void StringValueCell::setValue(const string& str) {
    value = str;
}

// DoubleValueCell class methods

// Returns the double value as a string
string DoubleValueCell::getContent() const {
    return to_string(value);
}

// Returns the double value as a string
string DoubleValueCell::getValue() const {
    return to_string(value);
}

// Sets the content of the cell as a double value
void DoubleValueCell::setContent(const string& str, SpreadSheet& table) {
    value = stod(str);
    notifyDependents(table);  // Notify dependents of the update
}

// Sets the double value directly
void DoubleValueCell::setValue(const string& str) {
    value = stod(str);
}

// Returns the type as 'value' for double value cells
Type DoubleValueCell::getType() const {
    return Type::value;
}

// EmptyValueCell class methods

// Returns an empty string for empty cells
string EmptyValueCell::getContent() const {
    return value;
}

// Returns an empty string for empty cells
string EmptyValueCell::getValue() const {
    return value;
}

// Sets the content of the empty cell (empty string)
void EmptyValueCell::setContent(const string& str, SpreadSheet& table) {
    value = str;
    notifyDependents(table);  // Notify dependents of the update
}

// Sets the value of the empty cell (empty string)
void EmptyValueCell::setValue(const string& str) {
    value = str;
}

// Returns the type as 'empty' for empty value cells
Type EmptyValueCell::getType() const {
    return Type::empty;
}

}  // End of spreadsheet namespace
