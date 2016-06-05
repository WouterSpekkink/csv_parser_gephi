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

// One issue I need to deal with at some point is that there may be additional
// separators within columns. 

// EDIT: I noticed that csv files store cell values with separators with quotes. Example:
// col_1;col_2
// 1;actor_piet
// 2;"actor_piet;actor_hans"

// So the trick is to identify the cells that have the quotes.

void InputTable::ReadFile(const std::string &inputFile, const char &del) 
{
  std::vector <std::vector <std::string> > dataVector;
  std::ifstream myFile (inputFile.c_str());

  while(myFile) {
    // First we get the lines of data.
    std::string buffer;
    if(!getline(myFile, buffer)) break;
    
    // Next, we cut the lines of data up in smaller pieces.
    std::istringstream stringStream(buffer);
    std::vector <std::string> record;
    
    while (stringStream) {
      std::string s;
      if(!getline(stringStream, s, del)) break;
      record.push_back(s);
    }
    dataVector.push_back(record);
  }
  
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
      rowNames.push_back(tempVector[0]); // This makes the vector of row names
      std::vector <std::string>::iterator it2;
      std::vector <short> numbers;
      
      for(it2 = tempVector.begin() + 1; it2 != tempVector.end(); it2++) {
	short tempNumber = 0;
	std::istringstream convert(*it2);
	convert >> tempNumber;
	numbers.push_back(tempNumber);
      }
      rowData.push_back(numbers);
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

const std::vector<std::vector <short> > InputTable::GetRowData() 
{
  return rowData;
}





