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

 TODO: 
 * I still have to come up with something to handle separators that are used within cells.
 * Tne below does not yet take into account the possibility that values in cells are not multiple.

*/

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "../include/InputTable.h"


InputTable::InputTable(QWidget *parent) 
{
}

void InputTable::readDataOne(const QString &fileName, const QString &sepOne, const QString &sepTwo) 
{
  const std::string file = fileName.toStdString();
  const std::string delimiterOne = sepOne.toStdString();
  const std::string delimiterTwo = sepTwo.toStdString();
  
  // Let's make sure that the data vector is emptied, before making a new one
  rowData.clear();
  header.clear();
   
  std::istringstream convertOne(delimiterOne.c_str());
  char delOne;
  convertOne >> delOne;
  std::istringstream convertTwo(delimiterTwo.c_str());
  char delTwo;
  convertTwo >> delTwo;
  
  ReadFileOne(file, delOne, delTwo);
}

void InputTable::readDataTwo(const QString &fileName, const QString &sepOne) 
{
  const std::string file = fileName.toStdString();
  const std::string delimiterOne = sepOne.toStdString();
  
  // Let's make sure that the data vector is emptied, before making a new one
  rowData.clear();
  header.clear();
   
  std::istringstream convertOne(delimiterOne.c_str());
  char delOne;
  convertOne >> delOne;
  
  ReadFileTwo(file, delOne);
}


/* One issue I need to deal with at some point is that there may be additional
   separators within columns. 

   EDIT: 
   I noticed that csv files store cell values with separators with quotes. Example:
   col_1;col_2
   1;actor_piet
   2;"actor_piet;actor_hans"

   EDIT 2:
   I actually wonder how much this matters. It is fine if the vectors that store the data
   from each line are of different lengths, as long as each line is in one vector.
   I might have to get rid of the quotation signs though.
   That could probably be done easily, by iterating to the resulting data vector.

   EDIT 3: 
   One thing I didn't keep in mind yet is that the separators in cells may be different
   from those used in the file in general. Perhaps I should develop a solution for both
   separately. 
*/

   //We implement the function that this class uses to read and process input files.
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
    if(!getline(myFile, buffer)) break;

    // Then we need to make sure that if there are seperators within columns, they are handled properly.
    // I will do this by replacing them by a separator that cannot be selected by the user.

    // So what this function does, is iterate to the most recently loaded string.
    std::string::iterator it;
    bool firstFound = false;
    for (it = buffer.begin(); it != buffer.end(); it++) {
      // If it finds a quotation sign, and it has not found one yet *firstFound == false, then it will
      // start replacing delimiters/separators by a "+" sign.
      char tempChar = *it;
      if (tempChar == '\"' && firstFound == false) {
	firstFound == true;
	// if it finds the second quotation sign, it will stop replacing separators by plus signs.
      } else if (tempChar == '\"' && firstFound == true) {
	firstFound == false;
      }
      if (firstFound && tempChar == delTwo) {
	*it = '+';
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
    // I think we should deal with these when writing the data with the CsvOutput class.
    dataVector.push_back(record);
  }
  
  // Some of this stuff is inherited from a program with a different purpose, but maybe still useful
  std::vector<std::vector <std::string> >::iterator it;
  for(it = dataVector.begin(); it != dataVector.end(); it++) {
    if(it == dataVector.begin()) {// The first line is always the header
      std::vector<std::string> tempVector = *it;
      std::vector<std::string>::iterator it2;
      for(it2 = tempVector.begin() + 1; it2 != tempVector.end(); it2++)
	header.push_back(*it2);
      ncol = header.size(); // - 1 for empty space;
    } else {
      std::vector<std::string> tempVector = *it;

      // REMOVED THE ROWNAMES STUFF HERE, BECAUSE WE DON'T NEED THAT ANYMORE.

      // Commenting out some stuff, because data were converted to numbers, and I think I don't need that.
      std::vector <std::string>::iterator it2;
      std::vector <std::string> dataLine;
      
      for(it2 = tempVector.begin(); it2 != tempVector.end(); it2++) {
	//short tempNumber = 0;
	//std::istringstream convert(*it2);
	//convert >> tempNumber;
	//numbers.push_back(tempNumber);
	std::string tempString = *it2;
	// What we do here is to remove all quotation signs that might still be left in the lines of data.
	if (tempString.substr(0, 1) == "\"") {
	  tempString  = tempString.substr(1, tempString.length());
	}
	if (tempString.substr(tempString.length()) == "\"") {
	  tempString = tempString.substr(0, tempString.length() - 1);
	  dataLine.push_back(tempString);
	}
      }
      rowData.push_back(dataLine);
    }
  }
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
  
  // Some of this stuff is inherited from a program with a different purpose, but maybe still useful
  std::vector<std::vector <std::string> >::iterator it;
  for(it = dataVector.begin(); it != dataVector.end(); it++) {
    if(it == dataVector.begin()) {// The first line is always the header
      std::vector<std::string> tempVector = *it;
      std::vector<std::string>::iterator it2;
      for(it2 = tempVector.begin() + 1; it2 != tempVector.end(); it2++)
	header.push_back(*it2);
      ncol = header.size(); 
    } else {
      std::vector<std::string> tempVector = *it;

      // REMOVED THE ROWNAMES STUFF HERE, BECAUSE WE DON'T NEED THAT ANYMORE.

      // Commenting out some stuff, because data were converted to numbers, and I think I don't need that.
      std::vector <std::string>::iterator it2;
      std::vector <std::string> dataLine;
      
      for(it2 = tempVector.begin(); it2 != tempVector.end(); it2++) {
	//short tempNumber = 0;
	//std::istringstream convert(*it2);
	//convert >> tempNumber;
	//numbers.push_back(tempNumber);
	std::string tempString = *it2;
      }
      rowData.push_back(dataLine);
    }
  }
  emit importFinished();
}



int InputTable::GetCols() 
{
  return ncol;
}

const std::vector<std::string> InputTable::GetHeader() 
{
  return header;
}

const std::vector<std::vector <std::string> > InputTable::GetRowData() 
{
  return rowData;
}





