
#include "AnsiTerminal.h"
#include "spreadSheet.h"
#include "formulaParser.h"
#include "fileManager.h"
#include "cell.h"
#include "container.h"
#include <iostream>
#include <string>
#include <ncurses.h>

using namespace spreadsheet;
using namespace utils;

void handleInput(string& str, int row, int col, int firstR, SpreadSheet& table, AnsiTerminal& terminal, int);



int main() {

    AnsiTerminal terminal;
    terminal.clearScreen(); // Clear the screen at the beginning to start fresh

    cout << "\n"; // Extra newlines for spacing and layout
    cout << "\n"; // Extra newlines for spacing and layout
    
    int X=100;
    int Y=100;
    SpreadSheet table(X,Y); // Initialize a spreadsheet 

    string empty(CELL_SIZE,' ');
    int row = 4, col = 4; // Set initial cursor position to row 4, column 4
    const int firstR = row, firstC = col; // Store initial row and column to track grid starting position

    int checkIfNormal;
    terminal.printInvertedAt(row, col,empty); // Display the cursor in inverted mode at the initial position

    char key;
    string input = ""; // Initialize input string to collect characters typed by the user
    
    while (true) { // Infinite loop to keep processing input until the user quits
        checkIfNormal=1;
        // Display information about the selected cell

        table.infoCell(row - firstR, col / CELL_SIZE, firstR); 

        // Capture the special key pressed (like arrow keys, Enter, etc.)
        key = terminal.getSpecialKey(); 

        // Clear the previous content at the cursor position
        string printOnTerminal;
        table.printCell(printOnTerminal, row, col, firstR, table);
        terminal.printAt(row, col, printOnTerminal.substr(0, CELL_SIZE)); // Print the updated cell content

        // Handle case when input is empty and a digit or minus sign is pressed
        if (input.size() == 0 && (isdigit(key) || key == '-')) {
             checkIfNormal=0;
            char ch;
            string in = "";
            in += key; // Add the key to the input string
            
            handleInput(in, row, col, firstR, table, terminal, 1); // Handle input for number or negative sign
            table.setContent(row - firstR, col / CELL_SIZE,in); // Set the content of the cell
        } else {
            // Handle arrow key input to move the cursor
            switch (key) {
                case 'U': 
                    checkIfNormal=0;
                    // Move up, ensure not to go above first row
                    row = (row > firstR) ? row - 1 : row;
                    input.clear(); 
                    table.cleanFunc(row - firstR, col / CELL_SIZE, firstR); 
                    break; // Up

                case 'D': 
                    checkIfNormal=0;
                    // Move down, ensure not to go beyond last row
                    row = (row < table.getNumRows() + firstR - 1) ? row + 1 : row; 
                    input.clear();
                    table.cleanFunc(row - firstR, col / CELL_SIZE, firstR); 
                    break; // Down

                case 'R': 
                    checkIfNormal=0;
                    // Move right, ensure not to go beyond last column
                    col = (col < (table.getNumCols() - 1) * CELL_SIZE) ? col + CELL_SIZE : col; 
                    input.clear();
                    table.cleanFunc(row - firstR, col / CELL_SIZE, firstR); 
                    break; // Right

                case 'L':
                    checkIfNormal=0;
                    // Move left, ensure not to go before first column
                    col = (col > firstC) ? col - CELL_SIZE : col; 
                    input.clear();  
                    table.cleanFunc(row - firstR, col / CELL_SIZE, firstR); 
                    break; // Left

                case '>': { 
                    if(input.size()==0){
                        checkIfNormal=0;
                        // Jump to a specific cell based on a user-input reference
                        string newP = ">";
                        string temp;
                        int r, c,count=0,check=0,count2=0;
                        // Handle user input for cell reference
                        handleInput(newP, row, col, firstR, table, terminal, 3); 

                        // error checking
                        for(char c: newP){
                            if(isalpha(c))
                                count++;
                            if(islower(c))
                                check=1;
                            if(isdigit(c))
                                count2++;
                        }

                        try{

                            if(count<=2 && check==0 && count2!=0){
                                temp = newP.substr(1); // Remove the '>' symbol for processing
                                string str=table.getCell(row - firstR, col / CELL_SIZE)->getContent();
                                table.printCell(str, row, col, firstR, table);
                                terminal.printAt(row, col,str.substr(0, CELL_SIZE));

                                r = FormulaParser::getRows(temp); // Parse the row from the formula
                                c = FormulaParser::getCols(temp); // Parse the column from the formula
                            

                                // Update cursor position based on parsed row and column, ensuring they're within bounds
                            try {
                                    // Check: Is the input row (r) and column (c) within the valid range?
                                    if (r <= table.getNumRows() && r > 0 && c <= table.getNumCols() && c > 0) {
                                        row = (r + firstR) - 1; // Set the new row index based on input and offset.
                                        col = (c * CELL_SIZE + firstC) - CELL_SIZE; // Set the new column position based on input and offset.
                                    }
                                    else {
                                        throw out_of_range("Invalid Range"); // Throw an exception if the input is out of range.
                                    }
                                }
                                catch (exception& e) {
                                    // Handle the exception and pass the error message to the inputFunc method.
                                    table.inputFunc(row, col, 1, e.what());
                                }

                            }
                            else{
                                throw out_of_range("Invalid Cell"); // Throw an exception if the input is out of range.
                            }
                        
                        }
                        catch(exception& e){
                                table.inputFunc(row,col,1,e.what());
                         }
                    }

                } break;

               case '~': {
                    // Check if input is empty, indicating that a reset action is triggered
                    if(input.size() == 0) {
                        checkIfNormal = 0;  // Set flag indicating normal mode is off
        
                        string reset = "~";  
        
                        // Call handleInput function with reset string, passing other parameters
                        handleInput(reset, row, col, firstR, table, terminal, 3);

                        // If the reset string matches the expected command "~RESET"
                        if(reset == "~RESET") {
            
                            // Loop through all rows and columns of the spreadsheet
                            for(int i = 0; i < table.getNumRows(); i++) {
                                for(int j = 0; j < table.getNumCols(); j++) {
                                    // Reset the cell content in the table to an empty string
                                    table.getCell(i,j,1)=make_shared<EmptyValueCell>();
                    
                                    // Clear the cell display on the terminal by printing an empty string
                                    terminal.printAt(i + firstR, j * CELL_SIZE + firstC, empty);
                                }
                            }
                        }
                    }
                } break;


                case '&': { 
                    if(input.size()==0){
                        checkIfNormal=0;
                        // Handle saving the table to a file
                        int n;
                        string filename = "&";
                        string print;

                        handleInput(filename, row, col, firstR, table, terminal, 3); // Get file name from user input
                        try{
                            FileManager::fileHandle(table, filename); // Handle the file saving operation
                            // Iterate over all rows and columns
                            for (int i = 0; i < table.getNumRows(); i++) { 
                                for (int j = 0; j < table.getNumCols(); j++) {
                                    table.printCell(print, i + firstR, j * CELL_SIZE + firstC, firstR, table); // Print each cell
                                    terminal.printAt(i + firstR, j * CELL_SIZE + firstC, print.substr(0, CELL_SIZE)); // Update all grids
                                }
                            }
                        }
                        catch(exception& e){
                            table.inputFunc(row,col,1,e.what());
                        }

                    }
                } break;

                case '=':
                case '@': { 
                    if(input.size()==0){
                        checkIfNormal=0;
                        string formula;
                        formula = (key == '=') ? "=" : "@"; // Check whether it's an equation or a formula
                        handleInput(formula, row, col, firstR, table, terminal, 2); // Get user input for the formula
                        table.setContent(row - firstR, col / CELL_SIZE,formula); // Set the formula content in the cell
                       
                    }
                } break;

                case '<' :{
                    if(input.size()==0){
                        checkIfNormal=0;
                        // Handle formula entry
                        string formula="<";
                        FormulaCell temp;
                        handleInput(formula, row, col, firstR, table, terminal, 3); // Get user input for the formula
                       temp.setContent(formula, table); // Set the content of the cell with the given formula.
                        try {
                            // Attempt to parse and evaluate the formula using the FormulaParser.
                            FormulaParser::parserFormula(&temp, table);
                        }
                        catch (exception& e) {
                            // Handle any exceptions that occur during formula parsing.
                            // Pass the error message to the inputFunc method to display or log it.
                            table.inputFunc(row, col, 1, e.what());
                        }
                        
                    }
                } break;

                case '\n':
                case 'J':
                    checkIfNormal=0;
                    // Handle enter key or 'J' key press
                    input.clear();  
                    table.cleanFunc(row - firstR, col / CELL_SIZE, firstR);  // Clean the cell content
                    break; 

                case 'q':
                    // Quit the program if 'q' is pressed
                    if(input.size()==0)
                    return 0; 
      
            }
        }
        if(checkIfNormal==1){
            if (key == '\b' || key == 127) { // Backspace key detection
                input = table.getCell(row - firstR, col / CELL_SIZE)->getContent(); // Get the current content in the cell
                if (!input.empty()){
                    if(table.getCell(row - firstR, col / CELL_SIZE)->getType()==Type::formula){
                        input="";
                    }
                    else
                        input.pop_back(); // Remove the last character from the input
                } 
                else
                    table.setContent(row - firstR, col / CELL_SIZE,"");
            }
            else
                 input += key; // Add the character to the input string

            terminal.printAt(row, col, empty); // Clear previous content in the cell
            table.inputFunc(row - firstR, col / CELL_SIZE, firstR, input); // Process the input
            table.setContent(row - firstR, col / CELL_SIZE,input); // Set the content in the cell
        }
        // Display the updated cursor with the new content at the current position
        if(!(X<SPRERAD_ROW_SIZE && Y<SPRERAD_COL_SIZE))
        table.display(row - firstR, col / CELL_SIZE,terminal,table);
        else{
            table.printCell(printOnTerminal, row, col, firstR, table);
            terminal.printInvertedAt(row, col, printOnTerminal.substr(0, CELL_SIZE)); // Print the cursor in inverted mode
        }
     
        
        
    }

    terminal.clearScreen(); // Clear the screen on exit to reset the terminal state
    
    return 0; // Exit the program
}



// Function to handle user input for a cell, with specific behavior for different keys
void handleInput(string& in, int row, int col, int firstR, SpreadSheet& table, AnsiTerminal& terminal, int n) {
    char ch;
    int i=2;
    string str=table.getCell(row - firstR, col / CELL_SIZE)->getContent();
    table.printCell(str, row, col, firstR, table);
    terminal.printInvertedAt(row, col,str.substr(0, CELL_SIZE)); // Display the cursor in inverted mode
    cout << "\033[?25h";
    
    do {
        // Update the input content in the spreadsheet based on the input
        if(in.empty()){
            table.inputFunc(row - firstR, col / CELL_SIZE, firstR, in);
            break;
        }
        table.inputFunc(row - firstR, col / CELL_SIZE, firstR, in);
        cout << "\033[" << 2 << ";" << i << "H" << flush;
        ch = terminal.getSpecialKey(); // Get the special key pressed by the user

        // Handle backspace (remove the last character from the input or reset cell content)
        if (ch == '\b' || ch == 127) {
            if (!in.empty()) {
                i-=1;
                in.pop_back(); // Remove the last character from the input
            } 
            else {   
                break;
            }
        } 
        // Handle other characters (excluding newline and enter)
        else if (!(ch == '\n' || ch == 'J')) {
            // If n == 1, allow only digits and decimal points as input
            if (n == 1) {
                if (isdigit(ch) || ch == '.') {
                    in += ch; // Append the character to the input string
                    i++;
                }
            } 
            // For other cases, simply add the character to the input string
            else {
                in += ch;
                i++;
            }
            
        }
        else 
             table.cleanFunc(row - firstR, col / CELL_SIZE, firstR);
         
    // Continue the loop until the Enter or newline key is pressed
    } while (!(ch == '\n' || ch == 'J'));
    cout << "\033[?25l";
    
}


