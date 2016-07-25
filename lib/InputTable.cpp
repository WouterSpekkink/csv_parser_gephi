/*
 Copyright 2016 Wouter Spekkink Authors : Wouter Spekkink
 <wouter.spekkink@gmail.com> Website : http://www.wouterspekkink.org

 This file is part of the Gephi CSV Parser.

 DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.

 Copyright 2016 Wouter Spekkink. All rights reserved.

 The Gephi CSV Parser is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 The Gephi CSV Parser is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with the Gephi CSV Parser.  If not, see <http://www.gnu.org/licenses/>.

*/ 

/*
  ==NOTES==
  This class imports data from a csv-file, based on instructions received from the main dialog
  of the program. The data are imported as well as stored by the InputTable class.

*/

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "../include/InputTable.h"

// Constructor for this class.
InputTable::InputTable(QWidget *parent) 
{
}

// We first have two methods that process the instructions received by the main dialog.
// The methods also make sure that data that was already stored is removed first.
// The first method is called if the user told the program that multi-value columns exist.
void InputTable::readDataOne(const QString &fileName, const QString &sepOne, const QString &sepTwo) 
{
  // Here any existing data are removed. 
  rowData.clear();
  header.clear();
  nEvents = 0;

  // Translating the method arguments to something that this class can use.
  const std::string file = fileName.toStdString();
  const std::string delimiterOne = sepOne.toStdString();
  const std::string delimiterTwo = sepTwo.toStdString();
  std::istringstream convertOne(delimiterOne.c_str());
  char delOne;
  convertOne >> delOne;
  std::istringstream convertTwo(delimiterTwo.c_str());
  char delTwo;
  convertTwo >> delTwo;

  // And then we call another method that will do the actual importing of data.
  ReadFileOne(file, delOne, delTwo);
}

// This method is called if the user told the program that no multi-value columns exist.
void InputTable::readDataTwo(const QString &fileName, const QString &sepOne) 
{
  // Here any existing data are removed. 
  rowData.clear();
  header.clear();
  nEvents = 0;

  // Translating the method arguments to something that this class can use.
  const std::string file = fileName.toStdString();
  const std::string delimiterOne = sepOne.toStdString();
  std::istringstream convertOne(delimiterOne.c_str());
  char delOne;
  convertOne >> delOne;

  // And we call another method that will do the actual importing of data.
  ReadFileTwo(file, delOne);
}

// The next two methods read the files and import data.
// They are triggered by one of the two methods above.
// One deals with multi-value columns, and the other doesn't.
void InputTable::ReadFileOne(const std::string &inputFile, const char &delOne, const char &delTwo) 
{
  // Let's first make a vector of vectors of strings. Currently, this will make a matrix of strings.
  std::vector <std::vector <std::string> > dataVector;

  // Set up an file instream for the input file.
  std::ifstream myFile (inputFile.c_str());

  // While we have not gotten to the end of the file yet.
  while(myFile) {
    
    // First we get the lines of data, one by one.
    std::string buffer;

    // I also decided to set the second separator to a standard symbol that cannot be used by the user.
    // Then the CsvOutput functions can simply use that symbol to separate the values again.
    if(!getline(myFile, buffer)) break;
    for (std::string::size_type i = 0; i != buffer.length(); i++) {
      if (buffer[i] == delTwo) {
	buffer[i] = '+';
      }
    }
  
    // Next, we load the string into an istringstream and cut it up in smaller pieces.
    std::istringstream stringStream(buffer);
    // We make a vector of strings to hold the pieces of this line of data.
    std::vector <std::string> record;
    // We then go through the string, and cut it up in pieces, using the delimiter.
    while (stringStream) {
      std::string s;
      
      if(!getline(stringStream, s, delOne)) break;
      // We put the results in the record vector.
      record.push_back(s);
    }
    // And then we push this line of data in the larger data vector.
    // Cells with muliple values will still have the "+" signs.
    dataVector.push_back(record);
  }
  
  // This will disect the data into the header row, the event column, and the data rows. 
  std::vector<std::vector <std::string> >::iterator it;
  for(it = dataVector.begin(); it != dataVector.end(); it++) {
    // The first line is always the header
    if(it == dataVector.begin()) {
      std::vector<std::string> tempVector = *it;
      std::vector<std::string>::iterator it2;
      for(it2 = tempVector.begin(); it2 != tempVector.end(); it2++)
	header.push_back(*it2);
    } else {
      // Then we handle the other 'rows' in the data vector.
      std::vector<std::string> tempVector = *it;
      std::vector <std::string>::iterator it2;
      std::vector <std::string> dataLine;

      // What we do here is to remove all quotation signs that might still be left in the lines of data.
      // I think this is only necessary when handling some csv-files (maybe the windows-based ones).
      // Perhaps this is not necessary at all. I can always remove it.
      for(it2 = tempVector.begin(); it2 != tempVector.end(); it2++) {
	std::string tempString = *it2;
	
	if (tempString.substr(0, 1) == "\"") {
	  tempString  = tempString.substr(1, tempString.length());
	}
	if (tempString.substr(tempString.length()) == "\"") {
	  tempString = tempString.substr(0, tempString.length() - 1);
	}
	// After processing the data point, we push it to a data-line vector
	// And then we push the result into the rowData vector.
	dataLine.push_back(tempString);
      }
      rowData.push_back(dataLine);
    }
  }
  // We also want to know how many events we actually have.
  nEvents = dataVector.size() - 1;
  // This signal is sent to the main dialog to let it know we have finished importing the file.
  emit importFinished();
}

void InputTable::ReadFileTwo(const std::string &inputFile, const char &delOne) 
{
  // Let's first make a vector of vectors of strings. Currently, this will make a matrix of strings.
  std::vector <std::vector <std::string> > dataVector;

  // Set up an file instream for the input file.
  std::ifstream myFile (inputFile.c_str());

  // While we have not gotten to the end of the file yet.
  while(myFile) {
    
    // First we get the lines of data, one by one.
    std::string buffer;
    if(!getline(myFile, buffer)) break;
 
    // Next, we load the string into an istringstream and cut it up in smaller pieces.
    std::istringstream stringStream(buffer);
    // We make a vector of strings to hold the pieces of this line of data.
    std::vector <std::string> record;
    // We then go through the string, and cut it up in pieces, using the delimiter.
    while (stringStream) {
      std::string s;
      
      if(!getline(stringStream, s, delOne)) break;
      // We put the results in the record vector.
      record.push_back(s);
    }
    // And then we push this line of data in the larger data vector.
    // Cells with muliple values will still have the "+" signs.
    // I think we should deal with these when writing the data with the CsvOutput class.
    dataVector.push_back(record);
  }
  
  // This will disect the data into the header row, the event column, and the data rows. 
  std::vector<std::vector <std::string> >::iterator it;
  for(it = dataVector.begin(); it != dataVector.end(); it++) {
    // The first line is always the header
    if(it == dataVector.begin()) {
      std::vector<std::string> tempVector = *it;
      std::vector<std::string>::iterator it2;
      for(it2 = tempVector.begin(); it2 != tempVector.end(); it2++)
	header.push_back(*it2);
    } else {
      // Then we process the other 'rows' of the data vector.
      std::vector<std::string> tempVector = *it;
      std::vector<std::string>::iterator it2;
      std::vector <std::string> dataLine;
      for (it2 = tempVector.begin(); it2 != tempVector.end(); it2++) {
	std::string tempString = *it2;
	dataLine.push_back(tempString);
      }
      rowData.push_back(dataLine);
    }
  }
  // We also want to know the number of events that we imported.
  nEvents = dataVector.size() - 1;
  // We finish by sending a signal to the main dialog to tell it we have finished importing the file.
  emit importFinished();
}

// Returns the number of events currently stored.
int InputTable::GetEventNumber() {
  return nEvents;
}

// Returns a vector with the headers.
const std::vector<std::string> InputTable::GetHeader() {
  return header;
}

// Returns a vector with the data.
const std::vector<std::vector <std::string> > InputTable::GetRowData() {
  return rowData;
}





