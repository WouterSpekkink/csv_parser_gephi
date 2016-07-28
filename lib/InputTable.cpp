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
#include <QMessageBox>
#include <QPointer>
#include <QSpacerItem>
#include <QGridLayout>
#include "../include/InputTable.h"

// Constructor for this class.
InputTable::InputTable(QWidget *parent) 
{
}

// We first have one method that processes the instructions received by the main dialog.
void InputTable::readData(const QString &fileName, const QString &sepOne) 
{
  // Here any existing data are removed. 
  rowData.clear();
  header.clear();
  
  // Translating the method arguments to something that this class can use.
  // We sometimes need the separator as a char, and sometimes we need it as a string.
  const std::string file = fileName.toStdString();
  std::string sepOneString = sepOne.toStdString();
  std::istringstream convertOne(sepOneString.c_str());
  char sepOneChar;
  convertOne >> sepOneChar;

  // And then we call another method that will do the actual importing of data.
  ReadFile(file, sepOneChar);
}

void InputTable::ReadFile(const std::string &inputFile, const char &sepOneChar) 
{
  // Let's first make a vector of vectors of strings. Currently, this will make a matrix of strings.
  std::vector <std::vector <std::string> > dataVector;

  // Set up an file instream for the input file.
  std::ifstream myFile (inputFile.c_str());

  // Then we read all the lines of data in the input file.
  while(myFile) {
    // The buffer will hold one line of raw data temporarily, and we will do some processing on it.
    std::string buffer;
    if (!getline(myFile, buffer)) break;

    /* I had some problems reading files with embedded line breaks. 
       It should be easy to spot these linebreaks by checking for unmatched quotes.  
       However, I am not sure whether unmatched quotes are necessarily caused by embedded linebreaks.
       Therefore, the program shows an error box, cancels the import process, and leaves it to the 
       user to deal with the problem outside the program. 
    */
    bool quoteFound = false;
    for (std::string::size_type i = 0; i != buffer.length(); i++) {
      if (quoteFound == false && buffer[i] == '"') {
	quoteFound = true;
      } else if (quoteFound == true && buffer[i] == '"') {
	quoteFound = false;
      }
    }
    if (quoteFound == true) {
      QPointer<QMessageBox> errorBox = new QMessageBox;
      errorBox->setText(tr("<b>ERROR: Import cancelled</b>"));
      errorBox->setInformativeText("Unmatched quotes (\") were found in one of the lines of the file.");
      errorBox->exec();
      return;
    }
    // This boolean will indicate whether or not we find ourselves in a text field. These may hold
    // delimiter symbols that should be ignored. The code below is customized to do just that.
    bool inTextField = false;
    std::vector <std::string> currentLineProcessed;
    std::string::size_type stringLength = 0;
    std::string::size_type previousPos = 0;
    for (std::string::size_type i = 0; i != buffer.length(); i++) {
      if (inTextField == false && buffer[i] == '"') {
	inTextField = true;
      } else if (inTextField == true && buffer[i] == '"') {
	inTextField = false;
      }
      if (inTextField == false && buffer[i] == sepOneChar) {
	std::string tempString = buffer.substr(previousPos, stringLength);
	currentLineProcessed.push_back(tempString);
	previousPos = i + 1;
	stringLength = 0;
      } else {
	stringLength++;
      }
    }
    // And then we push this line of data in the larger data vector.
    dataVector.push_back(currentLineProcessed);
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
      rowData.push_back(*it);
    }
  }
  if (header.empty()) {
    QPointer<QMessageBox> errorBox =  new QMessageBox;
    errorBox->setText(tr("<b>ERROR: Import cancelled</b>"));
    errorBox->setInformativeText("Something strange happened during the import. Did you set the delimiters correctly?");
    errorBox->exec();
    return;
  }
  // This signal is sent to the main dialog to let it know we have finished importing the file.
  emit importFinished();
}

// Returns a vector with the headers.
const std::vector<std::string> InputTable::GetHeader() {
  return header;
}

// Returns a vector with the data.
const std::vector<std::vector <std::string> > InputTable::GetRowData() {
  return rowData;
}





