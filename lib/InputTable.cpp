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


#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "../include/InputTable.h"


InputTable::InputTable(QWidget *parent) 
  : nrow(0), ncol(0)
{
}

void InputTable::readData(const QString &fileName, const QString &sep) 
{
  const std::string file = fileName.toStdString();
  const std::string delimiter = sep.toStdString();
  
  // Let's make suer that the data vector is emptied, before making a new one
  rowData.clear();
  header.clear();
  rowNames.clear();
  nrow = 0;
  ncol = 0;
  
  std::istringstream convert(delimiter.c_str());
  char del;
  convert >> del;
  
  ReadFile(file, del);
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
void InputTable::ReadFile(const std::string &inputFile, const char &del) 
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

    // Next, we cut the lines of data up in smaller pieces.
    std::istringstream stringStream(buffer);
    // We make a vector of strings to hold the pieces of this line of data.
    std::vector <std::string> record;
    // We then go through the line of data, and cut it up in pieces, using the delimiter.
    while (stringStream) {
      std::string s;
      if(!getline(stringStream, s, del)) break;
      // We put the results in the record vector.
      record.push_back(s);
    }
    // And then we push this line of data in the larger data vector.
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

      // Getting the first columns separately as row names is still useful, because it represents the events/incidents
      // that the other data needs to be linked to.
      rowNames.push_back(tempVector[0]); // This makes the vector of row names

      // Commenting out some stuff, because data were converted to numbers, and I think I don't need that.
      std::vector <std::string>::iterator it2;
      //std::vector <short> numbers;
      std::vector <std::string> dataLine;
      
      for(it2 = tempVector.begin() + 1; it2 != tempVector.end(); it2++) {
	//short tempNumber = 0;
	//std::istringstream convert(*it2);
	//convert >> tempNumber;
	//numbers.push_back(tempNumber);
	std::string tempString = *it2;
	if (tempString.substr(0, 1) == "\"") {
	  tempString  = tempString.substr(1, tempString.length());
	} else if (tempString.substr(tempString.length()) == "\"") {
	  tempString = tempString.substr(0, tempString.length() - 1);
	  dataLine.push_back(tempString);
	}
      }
      rowData.push_back(dataLine);
    }
  }
  nrow = rowNames.size();
  emit importFinished();
}

int InputTable::GetRows() 
{
  return nrow;
}

int InputTable::GetCols() 
{
  return ncol;
}

const std::vector<std::string> InputTable::GetHeader() 
{
  return header;
}

const std::vector<std::string> InputTable::GetRowNames() 
{
  return rowNames;
}

const std::vector<std::vector <std::string> > InputTable::GetRowData() 
{
  return rowData;
}





