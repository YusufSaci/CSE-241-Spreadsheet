#ifndef CELL_H
#define CELL_H

#include <string>
#include <vector>

using namespace std;

// Forward declaration of the SpreadSheet class to avoid circular dependency
class SpreadSheet;

// Enum to represent the type of content in the cell (empty, formula, string, or value)
enum class Type{
    empty,   // Cell is empty
    formula, // Cell contains a formula
    string,  // Cell contains a string
    value    // Cell contains a numeric value
};

// Cell class definition
class Cell{
 
 public:
    // Default constructor 
    Cell();

    // Getter function to retrieve the content of the cell as a string
    string getContent() const;

    // Getter function to retrieve the evaluated value of the cell
    string getValue() const;

    // Getter function to retrieve the type of content stored in the cell
    Type getType() const;

    // Getter functions to retrieve the row and column position of the cell
    int getRow() const;
    int getCol() const;
    
    // Adds a dependent cell to the current cell's list of dependents
    void addDependents(Cell&);
    
    // Sets the row and column position of the cell
    void setPosition(int, int);

    // Sets the type of content the cell holds (empty, formula, string, or value)
    void setType(Type);

    // Sets the content of the cell
    void setContent(const string&, SpreadSheet&);

    // Sets the evaluated value of the cell
    void setValue(const string&);

    // Updates the value of the cell based on any formula it may contain
    void updateValue(SpreadSheet&);

    // Notifies all dependent cells of a change
    void notifyDependents(SpreadSheet&);

    // Comparison operator to check if two cells are equal 
    bool operator==(const Cell& other) const;
    
    // Initializes the type of the cell 
    void initType();

    // Function to check if there is a cyclic dependency involving the target cell.
    bool checkCyclicDependency(Cell* target);

 private:
    vector<Cell*> dependents;  // List of dependent cells that rely on the current cell
    Type contentType;          // The type of content stored in the cell
    string content;            // The content of the cell 
    int row, col;              // Row and column positions of the cell in the spreadsheet
    string value;              // Evaluated value of the cell (used for formulas or values)
};

#endif
