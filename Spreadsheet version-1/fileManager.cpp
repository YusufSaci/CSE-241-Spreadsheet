#include "fileManager.h"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

// Saves the spreadsheet content to a CSV file.
void FileManager::saveAsCSV(SpreadSheet& table, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        // If the file cannot be opened, return early.
        return;
    }

    // Iterate through all rows of the spreadsheet.
    for (int row = 0; row < table.getNumRows(); ++row) {
        for (int col = 0; col < table.getNumCols(); ++col) {
            if (col != 0) 
                file << ","; // Add a comma to separate columns.

            string cellContent = table.getCell(row, col).getContent();

            // Check if the cell content is a formula (starts with '@').
            if (!cellContent.empty() && cellContent[0] == '@') {
                cellContent = convertToExcelFormula(cellContent);
            }

            file << cellContent;
        }
        file << "\n"; // Add a newline at the end of the row.
    }

    file.close(); // Close the file after writing.
}

// Loads spreadsheet content from a CSV file.
void FileManager::loadFromCSV(SpreadSheet& table, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        // If the file cannot be opened, return early.
        return;
    }

    string line;
    int row = 0;

    // Read the file line by line.
    while (getline(file, line)) {
        istringstream ss(line);
        string cellContent;
        int col = 0;

        // Split the line by commas to get cell contents.
        while (getline(ss, cellContent, ',')) {
            // Convert Excel formula to internal representation if necessary.
            if (!cellContent.empty() && cellContent[0] == '=') {
                cellContent = convertToInternalFormula(cellContent);
            }

            table.getCell(row, col).setContent(cellContent, table);
            ++col;
        }

        // Set remaining cells in the row to empty if there are fewer columns.
        while (col < table.getNumCols()) {
            table.getCell(row, col).setContent("", table);
            ++col;
        }

        ++row;
    }

    file.close(); // Close the file after reading.
}

 void FileManager::createNewFile(SpreadSheet& table, const std::string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        // If the file cannot be opened, return early.
        return;

    }

    // Iterate through all rows of the spreadsheet.
    for (int row = 0; row < table.getNumRows(); ++row) {
        // Iterate through all columns of the current row.
        for (int col = 0; col < table.getNumCols(); ++col) {
            if (col != 0) 
                file << ","; // Add a comma to separate columns.
        }
        file << "\n"; // Add a newline at the end of the row.
    }

    file.close(); // Close the file after writing.
}

// Handles file operations based on the command (&SAVE or &LOAD).
void FileManager::fileHandle(SpreadSheet& table, const string& filename) {
    string str = "", name;
    int i = 0;

    // Extract the command (&SAVE or &LOAD) from the filename string.
    while (filename[i] != ' ') {
        str += filename[i++];
    }
    i++; // Move past the space.

    // If the command is &SAVE, extract the file name and save the content.
    if (str == "&SAVE") {
        name = filename.substr(i);
        saveAsCSV(table, name);
    }
    // If the command is &LOAD, extract the file name and load the content.
    else if (str == "&LOAD") {
        name = filename.substr(i);
        loadFromCSV(table, name);
    }
    // If the command is &NEW, extract the file name creat new file.
    else if (str=="&NEW"){
        name = filename.substr(i);
        createNewFile(table, name);
    }
}

string FileManager::convertToExcelFormula(const string& formula) {
    vector<string> originalFunctions = {"@SUM", "@MAX", "@MIN", "@AVER", "@STDDEV"};
    vector<string> excelFunctions = {"=SUM", "=MAX", "=MIN", "=AVERAGE", "=STDEV"};

    string result = formula;

    // Replace the function name.
    for (int i = 0; i < originalFunctions.size(); ++i) {
        int pos = result.find(originalFunctions[i]);
        if (pos != string::npos) {
            result.replace(pos, originalFunctions[i].length(), excelFunctions[i]);
            break;
        }
    }

    // Replace ".." with ":" for range compatibility in Excel.
    int rangePos = result.find("..");
    if (rangePos != string::npos) {
        result.replace(rangePos, 2, ":");
    }

    return result;
}

string FileManager::convertToInternalFormula(const string& formula) {
    
    vector<string> excelFunctions = {"=SUM", "=MAX", "=MIN", "=AVERAGE", "=STDEV"};
    vector<string> internalFunctions = {"@SUM", "@MAX", "@MIN", "@AVER", "@STDDEV"};

    string result = formula;

    // Replace Excel function names with internal function names.
    for (int i = 0; i < excelFunctions.size(); ++i) {
        int pos = result.find(excelFunctions[i]);
        if (pos != string::npos) {
            result.replace(pos, excelFunctions[i].length(), internalFunctions[i]);
            break;
        }
    }

    // Replace ":" with ".." for range compatibility in the internal format.
    int rangePos = result.find(":");
    if (rangePos != string::npos) {
        result.replace(rangePos, 1, "..");
    }

    return result;
}
