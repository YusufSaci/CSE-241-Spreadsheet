#ifndef FORMULA_H
#define FORMULA_H
#include <vector>
#include <string>
#include "cell.h"
#include "spreadSheet.h"

using namespace std;
using namespace spreadsheet;

namespace utils{

class FormulaParser {

 public:
   // Main function to parse and evaluate formulas in a given cell.
   // Takes a reference to a Cell and a SpreadSheet to resolve the formula.
   static void parserFormula(Cell* cell, SpreadSheet& table);
   
   // Helper function to extract the column number from a string representation of a cell (e.g., "A1" -> 1).
   static int getCols(const string& str);
   
   // Helper function to extract the row number from a string representation of a cell (e.g., "A1" -> 1).
   static int getRows(const string& str);

 private:
 
    // Helper function to check if a string represents a valid cell reference.
    static bool isCell(const string& str);
    
    
    // Validates individual elements of a formula, ensuring correct formatting.
    static void isValid(const SpreadSheet& table, const string& str); 
    
    static void isValid(const SpreadSheet& table ,const Cell& cell);  

    static void isValid(SpreadSheet table,const vector<string> &element);

    // Helper function to apply mathematical operations like +, -, *, / to two operands.
    static double applyOp(double a, double b, char op);
    
    // Converts all formula elements (cell references and constants) to double values.
    static void convertToDouble(SpreadSheet& table, const vector<string> &element, vector<double> &numbers);

  
};

}
#endif
