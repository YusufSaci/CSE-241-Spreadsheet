
#include "formulaParser.h"
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

using namespace std;

void FormulaParser::parserFormula(Cell& cell, SpreadSheet& table) {
    // Static vectors to store parsed formula elements:
    // 'op' stores operators (+, -, *, /), 'element' stores individual formula components (e.g., cell references), and 
    // 'numbers' stores numeric values derived from the formula elements.
    vector<char> op;  
    vector<string> element;
    vector<double> numbers;
    string token = "";
    char c = cell.getContent()[0];  // Get the first character of the cell content

    switch (c) {
        case '=': {  // Case where the formula starts with '='
            // Parse the formula by separating operators and numbers/cell references
            int checkmin=1;
            for (char ch : cell.getContent()) {
                if (checkmin==0 && (ch == '+' || ch == '-' || ch == '*' || ch == '/')) {
                    if (!token.empty()) {
                        element.push_back(token);  // Add the current token (number or cell reference) to element
                        op.push_back(ch);  // Add the operator to op
                        token.clear();  // Reset the token for the next number or reference
                        checkmin=1;
                    }
                } else if (ch != '=') {
                    token += ch;  // Accumulate the current character into the token
                    checkmin=0;
                }
            }

            // Add the final token (after the last operator)
            if (!token.empty()) {
                element.push_back(token);
            }

            vector<int> index;
            for (int i = 0; i < element.size(); i++) {
                // Check if the first character of the element is a minus sign ('-').
                if (element[i][0] == '-') {
                    index.push_back(i);  // Add the index of the element to the 'index' vector.
                    element[i].erase(0, 1);  // Remove the minus sign ('-') from the beginning of the element.
                }
            }


            // Validate if the formula is valid with respect to the table
            if (!isValid(table,element)) {
                cell.setContent("", table);  // Clear the content if invalid
                cout << "\033[" << cell.getRow() << ";" << cell.getCol() << "H" << "       " << std::flush; 
                return;
            }

            // Convert all the element strings to numeric values (either from cells or direct numbers)
            convertToDouble(table,element,numbers);

            //Multiply previously saved values ​​by minus.
             for(int i=0;i<index.size();i++){
                numbers[index[i]]=-numbers[index[i]];
            }

            // Process multiplication and division first
            for (int i = 0; i < op.size(); i++) {
                if (op[i] == '*' || op[i] == '/') {
                    numbers[i] = applyOp(numbers[i], numbers[i + 1], op[i]);  // Apply operation
                    numbers.erase(numbers.begin() + i + 1);  // Remove the processed number
                    op.erase(op.begin() + i);  // Remove the operator
                    i--;  // Adjust the index after removal
                }
            }

            // Process remaining addition and subtraction operations
            while (!op.empty()) {
                double result = applyOp(numbers[0], numbers[1], op[0]);  // Apply the operation
                numbers[0] = result;  // Store the result back
                numbers.erase(numbers.begin() + 1);  // Remove the second operand
                op.erase(op.begin());  // Remove the operator
            }

            double result = numbers[0];  // Final result of the formula

            // Set the result value to the cell
            cell.setValue(to_string(result));

            // Update dependent cells
            for (const string& ref : element) {
                if (isCell(ref)) {
                    int r = getRows(ref);
                    int c = getCols(ref);
                    table.getCell(r - 1, c - 1).addDependents(cell);  // Add this cell as a dependent to others
                }
            }

        } break;

        case '<':// for copy;
        case '@': {  // Case where the formula starts with '@' (range-based functions like SUM, AVER, etc.)
            string str = "";
            int k,l, flag = 0;
            string firstCell = "", lastCell = "",position="";
            int fr, fc, lr, lc, pr, pc;
            double result = 0.0;
            char ch;
            k=1;

            // Validate the range of cells
            if (c=='@' && !isValid(table, cell.getContent()) ) {
                cell.setContent("", table);  // Invalid range, clear the cell content
                return;
            }
            if(c=='<' && !isValid(table,cell)){
                return;
            }

            l=1;
            // Extract the cell to be copied
            if(c=='<'){
                while (cell.getContent()[l]!='-') {
                    position += cell.getContent()[l];
                    l++;
                }
                k=l+1;
            }

            // Extract the function name (like SUM, AVER, etc.)
            while (isalpha(cell.getContent()[k])) {
                str += cell.getContent()[k];
                k++;
            }

            // Extract the cell range
            while ((ch = cell.getContent()[k]) != ')') {
                if (ch != '.' && ch != '(' && flag == 0) {
                    firstCell += ch;  // First cell in the range (e.g., A1)
                } else if (ch == '.') {
                    flag = 1;
                } else if (flag == 1) {
                    lastCell += ch;  // Last cell in the range (e.g., A10)
                }
                k++;
            }

            // Convert first and last cell references to row and column numbers
            fr = getRows(firstCell);
            fc = getCols(firstCell);
            lr = getRows(lastCell);
            lc = getCols(lastCell);

            if(c=='<'){ 
                pr = getRows(position); //row of the cell to be copied
                pc = getCols(position); //col of the cell to be copied
            }

            // Check if the row and column numbers are within valid bounds of the spreadsheet
            if(fr > table.getNumRows() || fr < 1 || fc > table.getNumCols() || fc < 1){
                cell.setContent("", table);  // Invalid range, clear the cell content
                return;
            }
            if(lr > table.getNumRows() || lr < 1 || lc > table.getNumCols() || lc < 1){
                cell.setContent("", table);  // Invalid range, clear the cell content
                return;
            }
           
            if(c=='<' && (pr > table.getNumRows() || pr < 1 || pc > table.getNumCols() || pc < 1)){
                return; // Invalid range
            }

            int findex=(fc==lc) ? fr : fc;
            int lindex=(fc==lc) ? lr : lc;

           if(c=='<'){
                for (int i = findex; i <= lindex; i++) {
                    int r=i-1;
                    int c=(fc==lc) ? fc-1 : fr-1;
                    if(fc!=lc){ int temp= r; r=c;c=temp;}
                    // Add current cell as a dependent for the range
                    table.getCell(pr - 1, pc - 1).addDependents(table.getCell(r,c));  
                }
            }
            
            // Mark dependent cells in the specified range
            else{
                for (int i = findex; i <= lindex; i++) {
                    int r=i-1;
                    int c=(fc==lc) ? fc-1 : fr-1;
                    if(fc!=lc){ int temp= r; r=c;c=temp;}
                    // Add current cell as a dependent for the range
                    table.getCell(r, c).addDependents(cell);  
                }
            }

            // Calculate the result based on the specified function
            if (str == "SUM") {
                // Calculates the sum of the values in the specified range of cells for the "SUM" function.
                for (int i = findex; i <= lindex; i++) {
                    int r=i-1;
                    int c=(fc==lc) ? fc-1 : fr-1;
                    if(fc!=lc){ int temp= r; r=c;c=temp;}
                    // If the cell contains a formula or a value, add its value to the result.
                    if(table.getCell(r, c).getType() == Type::formula || table.getCell(r ,c).getType() == Type::value)
                        result += stod(table.getCell(r, c).getValue());  // Convert the cell's value to double and add it.
                }
            } 
            else if (str == "AVER") {
                // Calculates the average of the values in the specified range of cells for the "AVER" function.
                int count = 0;  // Counter for invalid cells (not formula or value).
                for (int i = findex; i <= lindex; i++) {
                    int r=i-1;
                    int c=(fc==lc) ? fc-1 : fr-1;
                    if(fc!=lc){ int temp= r; r=c;c=temp;}

                    // If the cell contains a formula or a value, add its value to the result.
                    if(table.getCell(r, c).getType() == Type::formula || table.getCell(r ,c).getType() == Type::value)
                        result += stod(table.getCell(r, c).getValue());
                    else
                        count++;  // Increment the counter for invalid cells.
                }
                result /= (lindex - findex + 1 - count);  // Divide the sum by the number of valid cells to calculate the average.
            } 
            
            else if (str == "MAX") {
                // Finds the maximum value in the specified range of cells for the "MAX" function.
                double max;
                // Initialize 'max' with the first valid cell value in the range.
                for (int i = findex; i <= lindex; i++) {
                    int r=i-1;
                    int c=(fc==lc) ? fc-1 : fr-1;
                    if(fc!=lc){ int temp= r; r=c;c=temp;}

                    if(table.getCell(r, c).getType() == Type::formula || table.getCell(r,c).getType() == Type::value){
                        max = stod(table.getCell(r, c).getValue());
                        break;
                    }
                }
                // Iterate through the rest of the range to find the maximum value.
                for (int i = findex + 1; i <= lindex; i++) {
                    int r=i-1;
                    int c=(fc==lc) ? fc-1 : fr-1;
                    if(fc!=lc){ int temp= r; r=c;c=temp;}

                    if(table.getCell(r, c).getType() == Type::formula || table.getCell(r, c).getType() == Type::value){
                        if (stod(table.getCell(r, c).getValue()) > max) {
                            max = stod(table.getCell(r, c).getValue());  // Update 'max' if a larger value is found.
                        }
                    }
                }
                result = max;  // Store the maximum value in 'result'.
            }

            else if (str == "MIN") {
                // Finds the minimum value in the specified range of cells for the "MIN" function.
                double min;
                // Initialize 'min' with the first valid cell value in the range.
                for (int i = findex; i <= lindex; i++) {
                    int r=i-1;
                    int c=(fc==lc) ? fc-1 : fr-1;
                    if(fc!=lc){ int temp= r; r=c;c=temp;}

                    if(table.getCell(r, c).getType() == Type::formula || table.getCell(r,c).getType() == Type::value){
                        min = stod(table.getCell(r, c).getValue());
                        break;
                    }
                }
                // Iterate through the rest of the range to find the minimum value.
                for (int i = findex + 1; i <= lindex; i++) {
                    int r=i-1;
                    int c=(fc==lc) ? fc-1 : fr-1;
                    if(fc!=lc){ int temp= r; r=c;c=temp;}

                    if(table.getCell(r, c).getType() == Type::formula || table.getCell(r, c).getType() == Type::value){
                        if (stod(table.getCell(r, c).getValue()) < min) {
                            min = stod(table.getCell(r, c).getValue());  // Update 'min' if a smaller value is found.
                        }
                    }
                }
                result = min;  // Store the minimum value in 'result'.
            } 

            else if (str == "STDDEV") {
                // Calculates the standard deviation of the values in the specified range of cells for the "STDDEV" function.
                double sum = 0.0;  // To store the sum of all valid cell values.
                int count = 0;  // Counter for invalid cells (not formula or value).
                for (int i = findex; i <= lindex; i++) {
                    int r=i-1;
                    int c=(fc==lc) ? fc-1 : fr-1;
                    if(fc!=lc){ int temp= r; r=c;c=temp;}

                    // If the cell contains a formula or a value, add its value to the sum.
                    if(table.getCell(r, c).getType() == Type::formula || table.getCell(r, c).getType() == Type::value)
                        sum += stod(table.getCell(r, c).getValue());
                    else
                        count++;  // Increment the counter for invalid cells.
                }

                sum /= (lindex - findex + 1-count);  // Calculate mean
                for (int i = findex; i <= lindex; i++) {
                    int r=i-1;
                    int c=(fc==lc) ? fc-1 : fr-1;
                    if(fc!=lc){ int temp= r; r=c;c=temp;}
                    if(table.getCell(r, c).getType()==Type::formula || table.getCell(r, c).getType()==Type::value)
                        result += pow(stod(table.getCell(r, c).getValue()) - sum, 2);  // Sum squared differences
                }
                result = sqrt(result / (lindex - findex + 1-count));  // Standard deviation calculation
               
            }

            else if(str=="CPY"){
                string str= table.getCell(pr-1,pc-1).getContent();
                for (int i = findex; i <= lindex; i++) {
                    int r=i-1;
                    int c=(fc==lc) ? fc-1 : fr-1;
                    if(fc!=lc){ int temp= r; r=c;c=temp;}

                    // Set the content of the target cell (i-1, fc-1) to the source cell's content
                    table.getCell(r, c).setContent(str,table);

                    // If the source cell contains a string or is empty
                    if(table.getCell(pr-1,pc-1).getType()==Type::string ||table.getCell(pr-1,pc-1).getType()==Type::empty){

                        // Move cursor to the target cell's position and display its content
                         cout << "\033[" <<  table.getCell(r, c).getRow() << ";" << 
                         table.getCell(r, c).getCol() << "H"
                         << table.getCell(pr-1,pc-1).getContent().substr(0, CELL_SIZE) << std::flush;
                    }
                    else{
                        string str;
                        // Set the value of the target cell to the source cell's value
                        table.getCell(r, c).setValue(table.getCell(pr-1,pc-1).getValue());
                        str=table.getCell(r, c).getValue().substr(0,CELL_SIZE-1);
                        while (str.size() < CELL_SIZE) {
                            str = " " + str; // Add spaces to the left if the content is smaller than the cell size
                         }
                        // Move cursor to the target cell's position and display its value
                        cout << "\033[" <<  table.getCell(r, c).getRow() << ";" << 
                         table.getCell(r, c).getCol() << "H"
                         << str.substr(0, CELL_SIZE) << std::flush;
                    }
                }
            }

            // Set the calculated result to the cell
            if(c=='@')
            cell.setValue(to_string(result));
           
        } break;

       
    }
        
    
}


// Function to apply arithmetic operation on two numbers (a and b) based on the operator 'op'
double FormulaParser::applyOp(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;   // Add a and b
        case '-': return a - b;   // Subtract b from a
        case '*': return a * b;   // Multiply a and b
        case '/': b = (b==0) ? 1 : b;
            return a / b;   // Divide a by b
        default: return 0;        // If an unknown operator, return 0
    }
}

// Function to convert the elements (cell references or numbers) to double values
void FormulaParser::convertToDouble(SpreadSheet& table, const vector<string> &element, vector<double> &numbers) {
    for(int i = 0; i < element.size(); i++) {
        if(isalpha(element[i][0])) { // If it's a cell reference (e.g., A1)
            int c = getCols(element[i]);  // Get column index
            int r = getRows(element[i]);  // Get row index
            if(table.getCell(r - 1, c - 1).getType()==Type::string || table.getCell(r - 1, c - 1).getType()==Type::empty){
                numbers.push_back(0);
            }
            else 
            numbers.push_back(stod(table.getCell(r - 1, c - 1).getValue())); // Convert cell value to double
        } else {
            double num = stod(element[i]);  // Convert the string number to double
            numbers.push_back(num);
        }
    }
}

// Check if a given string represents a cell reference
bool FormulaParser::isCell(const string& str) {
    return isalpha(str[0]);  // Return true if the string starts with a letter (i.e., it's a cell reference)
}

// Function to extract the row number from a cell reference
int FormulaParser::getRows(const string& str) {
    int r = 0;
    for(char c : str) {  // Loop through each character in the string
        if(isdigit(c)) {  // If the character is a digit
            r += c - '0';  // Convert the character to its numeric value
            r *= 10;  // Shift the number
        }
    }
    r = r / 10;  // Correct the row number by dividing by 10 (as it was multiplied in the loop)
    return r;
}

// Function to extract the column number from a cell reference
int FormulaParser::getCols(const string& str) {
    if(!isalpha(str[1])) {
        return str[0] - 'A' + 1;  // If it's a single letter (A, B, C, etc.), return its position (1, 2, 3, ...)
    } else {
        int sum = 0;
        sum += (str[0] - 'A' + 1) * 26;  // Account for two-letter columns like AA, AB, etc.
        sum += str[1] - 'A' + 1;  // Add the second letter's value
        return sum;
    }
}

// This function validates the formula string to ensure it uses a valid function and proper syntax.

bool FormulaParser::isValid(const SpreadSheet& table, const string& str) {
    int c = 0; // Counter for special characters: '(', ')', and '.'

    // Check if the function name is one of the supported ones: @SUM, @MIN, @MAX, @AVER, or @STDDEV
    if(str.substr(0, 4) != "@SUM" && str.substr(0, 4) != "@MIN" && str.substr(0, 4) != "@MAX"
       && str.substr(0, 5) != "@AVER" && str.substr(0, 7) != "@STDDEV") {
        return false;  // Invalid function name
    }

    // Iterate through each character in the formula string
    for(char ch : str) {
        // Count the occurrences of special characters: '(', ')', and '.'
        if(ch == '(' || ch == ')' || ch == '.')
            c++;
    }

    // Check if there are exactly 4 occurrences of the special characters
    if(c != 4)
        return false;  // Invalid syntax (wrong number of parentheses or periods)

    return true;  // Formula is valid
}


// This function validates a formula contained within a cell in a spreadsheet.
// It checks the format and structure of the formula string.
bool FormulaParser::isValid(const SpreadSheet& table, const Cell& cell) {
    int c = 0;    
    int flag = 0;  
    int c2 = 0;      

    // Iterate through each character in the cell's content
    for(char ch : cell.getContent()) {
        if(ch == '(' || ch == ')' || ch == '.')
            c++;    // Increment counter for each special character
        if(ch == '-')
            flag = 1; // Set flag if '-' character is found
        if(isalpha(ch))
            c2++;   // Increment counter for each alphabetic character
    }
    // Validate the formula:
    // - There must be exactly 4 special characters ('(', ')', '.')
    // - A '-' character must be present
    // - The total number of alphabetic characters should be 6 or 9
    if(c != 4 || flag == 0 || c2 < 6 || c2 > 9)
        return false; // Formula is invalid

    return true; // Formula is valid
}




// Validate if the formula elements (cell references or numbers) are valid
bool FormulaParser::isValid( SpreadSheet table,const vector<string> &element) {
    if(element.size() == 0) {
        return false;  // No elements in the formula
    }
    
    for(int i = 0; i < element.size(); i++) {
        int ct = 0;
        int count = 0;
        for(char c : element[i]) {  // Count the number of digits in the element
            if(isdigit(c)|| c=='-') {
                count++;
            }
        }
        if(count == element[i].size()) 
            return true;  // If the element is a number, it's valid
        
        if(element[i].size() > 4) 
            return false;  // If the element is longer than 4 characters, it's invalid

        for(char c : element[i]) {  // Check if the element has valid cell reference format
            if(isalpha(c) && islower(c)) {
                return false;  // If there's a lowercase letter, it's invalid
            }
            if(isalpha(c)) {
                ct++;
            }
        }

        if(ct > 2)  // Cell references can only be 1 or 2 characters long
            return false;

        int c = getCols(element[i]);  // Get the column of the cell reference
        int r = getRows(element[i]);  // Get the row of the cell reference

        if(r > table.getNumRows() || r < 1 || c > table.getNumCols() || c < 1)
            return false;  // If the cell is out of bounds, it's invalid
        

    }
    return true;
}



