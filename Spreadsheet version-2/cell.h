#ifndef CELLl_H
#define CELLl_H

#include <string>
#include <vector>
#include <memory>
#include "container.h"

using namespace std;
using namespace utils;

namespace spreadsheet {
    class SpreadSheet;

    // Enum to represent the type of content a cell can hold
    enum class Type {
        empty,   // Cell is empty
        formula, // Cell contains a formula
        string,  // Cell contains a string
        value    // Cell contains a numeric value
    };

    // Base class representing a generic Cell
    class Cell {
    public:
        Cell() = default; // Default constructor

        // Pure virtual function to get the content of the cell (used in derived classes)
        virtual string getContent() const = 0;

        // Pure virtual function to get the value of the cell (used in derived classes)
        virtual string getValue() const = 0;

        // Virtual function to return the type of the cell
        virtual Type getType() const = 0 ;

        // Pure virtual function to set content of the cell
        virtual void setContent(const string&, SpreadSheet&) = 0;

        // Pure virtual function to set value of the cell
        virtual void setValue(const string&) = 0;

        // Getter functions to retrieve the row and column position of the cell
        int getRow() const;

        int getCol() const;

        // Setter functions to set the row and column position of the cell
        void setPosition(int r, int c);

        // Function to add a dependent cell to the current cell's list of dependents
        void addDependents(Cell* dependent);

        // Equality operator to compare two cells
        bool operator==(const Cell& other) const;

        // Function to retrieve all dependent cells
        Container<Cell*> getDependents() const;

        // Setter function to update the list of dependent cells
        void setDependents(const Container<Cell*>& newDependents);

        // Notify all dependents when a change occurs
        void notifyDependents(SpreadSheet&);

        // Update the cell's value based on the content/formula
        void updateValue(SpreadSheet&);

        // Check if there is a cyclic dependency involving the current cell
        bool checkCyclicDependency(Cell* target);

        // Remove a dependent cell
        void remove(Cell* cell);

    protected:
        Container<Cell*> dependents; // Container to hold all dependent cells
        int row, col; // Row and column position of the cell
    };

    // Derived class representing a cell with a formula
    class FormulaCell : public Cell {
    public:
        string getContent() const override; // Return the formula as content
        string getValue() const override;   // Return the evaluated value of the formula
        Type getType() const override;      // Return the type as 'formula'

        void setContent(const string&, SpreadSheet&) override; // Set the formula content
        void setValue(const string&) override; // Set the evaluated value of the formula

    private:
        string formula; // The formula string
        string result;  // The evaluated result of the formula
    };

    // Abstract base class representing a value cell (numeric or string)
    class ValueCell : public Cell {
    public:
        virtual string getContent() const = 0; // Get the content as a string
        virtual string getValue() const = 0;   // Get the value as a string
        virtual Type getType() const = 0;      // Return the type of the value cell
        virtual void setContent(const string&, SpreadSheet&) = 0; // Set the content of the value cell
        virtual void setValue(const string&) = 0; // Set the value of the value cell
    };

    // Derived class for cells that store integer values
    class IntValueCell : public ValueCell {
    public:
        string getContent() const override; // Return the integer value as a string
        string getValue() const override;   // Return the integer value as a string
        Type getType() const override;      // Return the type as 'value'

        void setContent(const string&, SpreadSheet&) override; // Set the content as an integer value
        void setValue(const string&) override; // Set the integer value

    private:
        int value; // The integer value stored in the cell
    };

    // Derived class for cells that store string values
    class StringValueCell : public ValueCell {
    public:
        Type getType() const override;        // Return the type as 'value'
        string getValue() const override;     // Return the string value
        string getContent() const override;   // Return the string value as content
        void setContent(const string&, SpreadSheet&) override; // Set the content as a string
        void setValue(const string&) override; // Set the string value

    private:
        string value; // The string value stored in the cell
    };

    // Derived class for cells that store double (floating point) values
    class DoubleValueCell : public ValueCell {
    public:
        string getContent() const override; // Return the double value as a string
        string getValue() const override;   // Return the double value as a string
        Type getType() const override;      // Return the type as 'value'
        void setContent(const string&, SpreadSheet&) override; // Set the content as a double value
        void setValue(const string&) override; // Set the double value

    private:
        double value; // The double value stored in the cell
    };

    // Derived class for empty value cells
    class EmptyValueCell : public ValueCell {
    public:
        string getContent() const override; // Return an empty string as content
        string getValue() const override;   // Return an empty string as value
        Type getType() const override;      // Return the type as 'empty'
        void setContent(const string&, SpreadSheet&) override; // Set the content as empty
        void setValue(const string&) override; // Set the value as empty

    private:
        string value = ""; // The default empty value
    };

}

#endif // CELLl_H
