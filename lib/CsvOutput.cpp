/*
 Copyright 2016 Wouter Spekkink
 Authors : Wouter Spekkink <wouter.spekkink@gmail.com>
 Website : http://www.wouterspekkink.org

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
  Here we only find two functions that write an edges file and a nodes file (respectively)
  to the disk, based on instructions received from the main dialog.

  If the user indicated that his/her input file uses multi-value columns, 
  than these values are separated here. See the InputTable implementation to understand
  why we use the '+' char as a separator for multiple values here.

 */
 
#include "../include/CsvOutput.h"
#include <fstream>
#include <QFileDialog>
#include <sstream>

// This function writes an edges file to the disk.
bool CsvOutputEdges(InputTable *table, std::string filename, std::string sepOne) {
  // We point to the InputTable and we get all the stuff out that we need.
  InputTable *outputTable = table; // pointer to output table.
  std::vector <std::string> events = outputTable->GetEvents();
  std::vector <std::vector <std::string> > data = outputTable->GetRowData();
  int numberEvents = outputTable->GetEventNumber();

  // Then we prepare the file for writing.
  std::ofstream fileOut(filename.c_str());
  if (!fileOut.is_open()) {
    return false;
  }
  
  // First we write the header of the file.
  fileOut << "Source" << sepOne << "Target" << sepOne << "Weight" << sepOne << "Type";
  fileOut << '\n';

  // Before we start writing the rest of the file, we set up two iterators that we
  // need in the for-loop below. We increment them for each unique event.
  std::vector<std::string>::iterator it = events.begin();
  std::vector<std::vector <std::string> >::iterator it2 = data.begin();

  // Then we write the rest of the data into the file.
  // We take the number of events as a first indication of how many iterations we need.
  for (unsigned int i = 0; i != numberEvents; i++, it++, it2++) {
    std::string currentEvent = *it;
    // We have now selected the first event, but we need to iterate through all the data points
    // that have a relationship to this event, before moving on to the next event.
    // currentData holds all the data points that are related to this particular event.
    std::vector<std::string> currentData = *it2;
    // We set up an iterator to go through the currentData vector, and we start the next for-loop.
    std::vector<std::string>::iterator it3;
    for (it3 = currentData.begin(); it3 != currentData.end(); it3++) {
      // currentElement is always a data point, but some of these may still have multiple values,
      // that are concatenated by the '+' char. We walk through currentElement to separate these
      // multiple values where necessary.
      std::string currentElement = *it3;
      std::vector <std::string> sepStrings;
      std::istringstream stringStream(currentElement);
      while (stringStream) {
	std::string s;
	if (!getline(stringStream, s, '+')) break;
	sepStrings.push_back(s);
      }
      // Then we go through the result vector (which may be only one string if there was only
      // 1 value in the cell), and we start writing stuff to the file.
      std::vector <std::string>::iterator it4;
      for (it4 = sepStrings.begin(); it4 != sepStrings.end(); it4++) {
	std::string splitElement = *it4;
	fileOut << currentEvent << sepOne << splitElement << sepOne << 1 << sepOne << "Undirected";
	fileOut << '\n';
      }
    }
  }
  // And we can close the file.
  fileOut.close();
  return true;
}

bool CsvOutputNodes(InputTable *table, std::string filename, std::string sepOne) {
  // We point to the output table and get everything we need from it.
  InputTable *outputTable = table;
  std::vector <std::string> events = outputTable->GetEvents();
  std::vector <std::string> headers = outputTable->GetHeader();
  std::vector <std::vector <std::string> > data = outputTable->GetRowData();

  // Then we prepare the file for writing.
  std::ofstream fileOut(filename.c_str());
  if (!fileOut.is_open()) {
    return false;
  }
  
  // First we write the header of the file.
  fileOut << "Id" << sepOne << "Label" << sepOne << "Type";
  fileOut << '\n';
  
  // Writing the rest of the data is easier in this case.
  // Just iterate through the different vectors and write their contents to the file.
  std::vector <std::string>::iterator it;
  for (it = events.begin(); it != events.end(); it++) {
    std::string currentEvent = *it;
    // The line below is used to retrieve the information that we use to indicate
    // what type of node this is. See the comment further below for more information on this. 
    std::string currentHeader = *headers.begin();
    fileOut << currentEvent << sepOne << currentEvent << sepOne << currentHeader;
    fileOut << '\n';
  }
  std::vector <std::vector <std::string> >::iterator it2;
  for (it2 = data.begin(); it2 != data.end(); it2++) {
    std::vector <std::string> currentData = *it2;
    std::vector <std::string>::iterator it3;
    /* We want to add a variable "Type" to the nodes list, which indicates
     What kind of node it is (what column it originated from in the original file).
     We can retrieve that information from the headers vector.
     We set up an iterator that we can use to go through this vector,
     and we let it increment together with the it3 iterator.*/
    std::vector <std::string>::iterator headerIterator = headers.begin() + 1; 
    for (it3 = currentData.begin(); it3 !=  currentData.end(); it3++, headerIterator++) {
      std::string currentElement = *it3;
      std::string currentHeader = *headerIterator;
      // We still need to separate the values within columns if there are multiple.
      std::vector <std::string> sepStrings;
      std::istringstream stringStream(currentElement);
      while (stringStream) {
	std::string s;
	if (!getline(stringStream, s, '+')) break;
	sepStrings.push_back(s);
      }
      std::vector <std::string>::iterator it4;
      for (it4 = sepStrings.begin(); it4 != sepStrings.end(); it4++) {
	std::string splitElement = *it4;
	fileOut << splitElement << sepOne << splitElement << sepOne << currentHeader;
	fileOut << '\n';
      }
    }
  }
  // And when we are finished, we can close the file.
  fileOut.close();
  return true;
}
