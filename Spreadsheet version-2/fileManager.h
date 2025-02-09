#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "spreadSheet.h"
#include <string>

using namespace std;
using namespace spreadsheet;
class FileManager{

 public:
   // Public method to handle file operations for the spreadsheet.
   // Depending on the use case, it may load from or save to a file.
   static void fileHandle(SpreadSheet& table,const string & filename);
  
  private:
   // Loads data into the spreadsheet from a CSV file.
   static void loadFromCSV(SpreadSheet& table, const string & filename);

   // Saves the spreadsheet data to a CSV file.
   static void saveAsCSV(SpreadSheet& table, const string & filename);

   // Create new file
   static void createNewFile(SpreadSheet& table, const string & filename);

   // Converts a formula to an Excel-compatible formula format.
   static string convertToExcelFormula(const string& formula);

   // Converts an Excel-compatible formula to an internal formula format.
   static string convertToInternalFormula(const string& formula);


};

#endif
