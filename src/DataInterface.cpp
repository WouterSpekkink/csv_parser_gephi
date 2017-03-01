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
  of the program. The data are imported as well as stored by the DataInterface class.

*/

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <QMessageBox>
#include <QPointer>
#include <QSpacerItem>
#include <QGridLayout>
#include "../include/DataInterface.h"

// Constructor for this class.
DataInterface::DataInterface()
{
}

void DataInterface::readFile(const QString &fileName, const QString &sepOne) 
{
  // Here any existing data are removed. 
  rowData.clear();
  header.clear();

  // Translating the method arguments to something that this class can use.
  // We sometimes need the separator as a char, and sometimes we need it as a string.
  const std::string inputFile = fileName.toStdString();
  std::string sepOneString = sepOne.toStdString();
  std::istringstream convertOne(sepOneString.c_str());
  char sepOneChar;
  convertOne >> sepOneChar;
  
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
	previousPos++;
	stringLength--;
      } else if (inTextField == true && buffer[i] == '"') {
	inTextField = false;
	stringLength--;
      }
      if (inTextField == false && buffer[i] == sepOneChar) {
	while (buffer[previousPos] == ' ') {
	  previousPos++;
	  stringLength--;
	}
	std::string tempString = buffer.substr(previousPos, stringLength);
	currentLineProcessed.push_back(tempString);
	previousPos = i + 1;
	stringLength = 0;
      } else {
	stringLength++;
      }
    }
    while (buffer[previousPos] == ' ') {
      previousPos++;
      stringLength--;
    }
    std::string tempString = buffer.substr(previousPos, stringLength);
    currentLineProcessed.push_back(tempString);
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

void DataInterface::writeEdgeList(
		   const QString sourceSelection, const QString targetSelection, const bool directedRelationships,
		   const QString relationsType, const std::string filename, const std::string sepOne, const std::string sepTwo
		   ) {
  // We point to the InputTable and we get all the stuff out that we need from the input table and the function arguments.
  std::string sourceString = sourceSelection.toStdString();
  std::string targetString = targetSelection.toStdString();
  std::string relationshipType = "Undirected";
  if (directedRelationships) {
    relationshipType = "Directed";
  }
  std::string relationshipLabel = relationsType.toStdString();
  std::istringstream convertSepOne(sepOne.c_str());
  char sepOneChar;
  convertSepOne >> sepOneChar;
  std::istringstream convertSepTwo(sepTwo.c_str());
  char sepTwoChar;
  convertSepTwo >> sepTwoChar;
    
  // We need to know in which part of the data vector the source and target node reside.
  // We create and set an index to achieve that.
  int sourceIndex = 0;
  int targetIndex = 0;
  for (std::vector<std::string>::size_type i = 0; i != header.size(); i++) {
    std::string currentString = header[i];
    if (currentString == sourceString) {
      sourceIndex = i;
    }
    if (currentString == targetString) {
      targetIndex = i;
    }
  }

  // Then we prepare the file for writing.
  std::ofstream fileOut(filename.c_str());
  if (!fileOut.is_open()) {
    QPointer<QMessageBox> errorBox = new QMessageBox;
    errorBox->setText(tr("<b>ERROR: Could not write file</b>"));
    errorBox->setInformativeText("The program had problems trying to open the file in which to write data");
    errorBox->exec();
    return;
  }
  
  // First we write the header of the file.
  if (relationshipLabel.length() > 0) {
    fileOut << "Source" << sepOne << "Target" << sepOne << "Weight" << sepOne << "Type" << sepOne << "Type_User_Defined";
  } else {
    fileOut << "Source" << sepOne << "Target" << sepOne << "Weight" << sepOne << "Type";
  }
  fileOut << '\n';

  // We make a vector to store our lines for the output file.
  std::vector <std::string> fileVector;
  
  // Then we iterate through the data vector, find the appropriate entries, and write them to our file vector.
  std::vector <std::vector <std::string> >::iterator itData;
  for (itData = rowData.begin(); itData != rowData.end(); itData++) {
    std::vector <std::string> currentData = *itData;
    std::string currentSource = currentData[sourceIndex];
    std::string currentTarget = currentData[targetIndex];
    std::vector <std::string> sepSources;
    std::vector <std::string> sepTargets;

    // Some of the entries may have multiple values in one string. We need to separate these now.
    std::istringstream sourceStream(currentSource);
    while (sourceStream) {
      std::string s;
      if (!getline(sourceStream, s, sepTwoChar)) break;
      while (s[0] == ' ') {
	s = s.substr(1, s.length());
      }
      sepSources.push_back(s);
    }
    std::istringstream targetStream(currentTarget);
    while (targetStream) {
      std::string s;
      if (!getline(targetStream, s, sepTwoChar)) break;
      while (s[0] == ' ') {
	s = s.substr(1, s.length());
      }
      sepTargets.push_back(s);
    }
    std::vector <std::string>::iterator itSepSources;
    std::vector <std::string>::iterator itSepTargets;

    // After making sures that the values have been separated where necessary, we can write the bulk of our file.
    for (itSepSources = sepSources.begin(); itSepSources != sepSources.end(); itSepSources++) {
      std::string currentSepSource = *itSepSources;
      for (itSepTargets = sepTargets.begin(); itSepTargets != sepTargets.end(); itSepTargets++) {
	std::string currentSepTarget = *itSepTargets;
	if (currentSepSource != currentSepTarget) {
	  std::vector <std::string> tempVector;
	  tempVector.push_back(currentSepSource);
	  tempVector.push_back(currentSepTarget);
	  if (sourceSelection == targetSelection) {
	    std::sort(tempVector.begin(), tempVector.end());
	  }
	  std::string sourceTarget = tempVector[0] + sepOne + tempVector[1];
	  if (relationshipLabel.size() > 0) {
	    fileVector.push_back(sourceTarget + sepOne + "1" + sepOne + relationshipType + sepOne +  relationshipLabel + '\n');
	  } else {
	    fileVector.push_back(sourceTarget + sepOne + "1" + sepOne + relationshipType + '\n');
	  }
	}
      }
    }
  }
  if (fileVector.empty()) {
    QPointer<QMessageBox> errorBox =  new QMessageBox;
    errorBox->setText("<b>WARNING</b>");
    errorBox->setInformativeText("The Edge list is empty. Check your settings.");
    errorBox->exec();
  }
  // We remove all doubles from the edge list here.
  std::sort(fileVector.begin(), fileVector.end());
  fileVector.erase(std::unique(fileVector.begin(), fileVector.end()), fileVector.end());

  // Then we iterate through our file vector and write everything to our file.
  std::vector <std::string>::iterator lineReader;
  for (lineReader = fileVector.begin(); lineReader != fileVector.end(); lineReader++) {
    fileOut << *lineReader;
  }
  
  // And after that we can close the file and end the function.
  fileOut.close();
  return;
}

void DataInterface::writeNodeList(
		    QString sourceSelection, QString targetSelection, std::vector <std::string> sourceProperties,
		    std::vector<std::string> targetProperties, bool excludeSources, bool excludeTargets,
		    std::string filename, std::string sepOne, std::string sepTwo
		    ) {
  // We point to the output table and get everything we need from the table and the function arguments.

  std::istringstream convertSepOne(sepOne.c_str());
  char sepOneChar;
  convertSepOne >> sepOneChar;
  std::istringstream convertSepTwo(sepTwo.c_str());
  char sepTwoChar;
  convertSepTwo >> sepTwoChar;
  std::string sourceString = sourceSelection.toStdString();
  std::string targetString = targetSelection.toStdString();
  /* 
     sharedProperties will be a new vector that holds the properties that the source and target node have in common (if any).
     These will be deleted from the original vectors for source and target properties. This is done to make sure that they
     are not include twice when writing the file. 
  */
  std::vector <std::string> sharedProperties;
  for (std::vector <std::string>::size_type i = 0; i != sourceProperties.size(); i++) {
    for (std::vector <std::string>::size_type j = 0; j != targetProperties.size(); j++) {
      if (sourceProperties[i] == targetProperties[j]) {
	sharedProperties.push_back(sourceProperties[i]);
	sourceProperties.erase(sourceProperties.begin() + i);
	targetProperties.erase(targetProperties.begin() + j);
      }
    }
  }

  /* 
     We need to know in which part of the data vector the source and target node reside.
     We create and set an index to achieve that.
     We need to do something similar for the properties of the nodes and the edges.
     Because there can be multiple properties per node, we make a vector of index numbers for each of these.
  */
  int sourceIndex = 0;
  int targetIndex = 0;
  std::vector <int> sourcePropertiesIndexes;
  std::vector <int> targetPropertiesIndexes;
  std::vector <int> sharedPropertiesIndexes;
  for (std::vector<std::string>::size_type i = 0; i != header.size(); i++) {
    // First we set the indexes for our source and target nodes.
    std::string currentString = header[i];
    if (currentString == sourceString) {
      sourceIndex = i;
    }
    if (currentString == targetString) {
      targetIndex = i;
    }
    // Here we make indexes for our source properties.
    std::vector <std::string>::iterator sourcePropertiesIt;
    for (sourcePropertiesIt = sourceProperties.begin(); sourcePropertiesIt != sourceProperties.end(); sourcePropertiesIt++) {
      std::string currentSourceProperty = *sourcePropertiesIt;
      if (currentString == currentSourceProperty) {
	sourcePropertiesIndexes.push_back(i);
      }
    }
    // Here we make indexes for our target properties.
    std::vector <std::string>::iterator targetPropertiesIt;
    for (targetPropertiesIt = targetProperties.begin(); targetPropertiesIt != targetProperties.end(); targetPropertiesIt++) {
      std::string currentTargetProperty = *targetPropertiesIt;
      if (currentString == currentTargetProperty) {
	targetPropertiesIndexes.push_back(i);
      }
    }
    // And here we set the indexes for the shared properties.
    std::vector <std::string>::iterator sharedPropertiesIt;
    for (sharedPropertiesIt = sharedProperties.begin(); sharedPropertiesIt != sharedProperties.end(); sharedPropertiesIt++) {
      std::string currentSharedProperty = *sharedPropertiesIt;
      if (currentString == currentSharedProperty) {
	sharedPropertiesIndexes.push_back(i);
      }
    }
  }

  // We need to make sure that our output file header also takes into account any properties that are to be included.
  // We create the different parts of the header here.
  std::string sourcePropertiesHeader = "";
  if (sourceProperties.size() > 0) {
    std::vector <std::string>::iterator sourcePropertiesIt;
    for (sourcePropertiesIt = sourceProperties.begin(); sourcePropertiesIt != sourceProperties.end(); sourcePropertiesIt++) {
      std::string currentSourceHeader = *sourcePropertiesIt;
      sourcePropertiesHeader = sourcePropertiesHeader + sepOne + currentSourceHeader;
    }
  }
  std::string targetPropertiesHeader = "";    
  if (targetProperties.size() > 0) {
    std::vector <std::string>::iterator targetPropertiesIt;
    for (targetPropertiesIt = targetProperties.begin(); targetPropertiesIt != targetProperties.end(); targetPropertiesIt++) {
      std::string currentTargetHeader = *targetPropertiesIt;
      targetPropertiesHeader = targetPropertiesHeader + sepOne + currentTargetHeader;
    }
  }
  std::string sharedPropertiesHeader = "";
  if (sharedProperties.size() >0) {
    std::vector <std::string>::iterator sharedPropertiesIt;
    for (sharedPropertiesIt = sharedProperties.begin(); sharedPropertiesIt != sharedProperties.end(); sharedPropertiesIt++) {
      std::string currentSharedHeader = *sharedPropertiesIt;
      sharedPropertiesHeader = sharedPropertiesHeader + sepOne + currentSharedHeader;
    }
  }

  // Then we prepare the file for writing.
  std::ofstream fileOut(filename.c_str());
  if (!fileOut.is_open()) {
    QPointer<QMessageBox> errorBox = new QMessageBox;
    errorBox->setText(tr("<b>ERROR: Could not write file</b>"));
    errorBox->setInformativeText("The program had problems trying to open the file in which to write data");
    errorBox->exec();
    return;
  }
  
  // First we write the header of the file. We add conditionals to handle the possibility that source or target nodes are excluded.
  if (excludeSources) {
    fileOut << "Id" << sepOne << "Type" << sepOne << "Label" << targetPropertiesHeader;
    fileOut << '\n';
  } else if (excludeTargets) {
    fileOut << "Id" << sepOne << "Type" << sepOne << "Label" << sourcePropertiesHeader;
    fileOut << '\n';
  } else {
    fileOut << "Id" << sepOne << "Type" << sepOne << "Label" << sharedPropertiesHeader << sourcePropertiesHeader << targetPropertiesHeader;
    fileOut << '\n';
  }

  // And then we iterate through our dat and process them before we write the bulk of the file.
  std::vector <std::vector <std::string> >::iterator itData;

  // We still need to separate entries with multiple values. We make two vectors that will store these
  // separated entries.
  std::vector <std::string> sepSources;
  std::vector <std::string> sepTargets;
  for (itData = rowData.begin(); itData != rowData.end(); itData++) {
    std::vector <std::string> currentData = *itData;
    std::string currentSource = currentData[sourceIndex];
    std::string currentTarget = currentData[targetIndex];
    // The so-called fake string will act as dummies that make sure that the file lines of the source and target nodes
    // are of the same length and have the same columns. Basically, they represent empty cells in the output file.
    std::string fakeSourceString = "";
    std::string sourcePropsString = "";
    std::vector <int>::iterator sourcePropsIt;
    for (sourcePropsIt = sourcePropertiesIndexes.begin(); sourcePropsIt != sourcePropertiesIndexes.end(); sourcePropsIt++) {
      int currentIndex = *sourcePropsIt;
      std::string tempData = currentData[currentIndex];
      sourcePropsString = sourcePropsString + sepOne + tempData;
      fakeSourceString = fakeSourceString + sepOne + "";
    }
    std::string targetPropsString = "";
    std::string fakeTargetString = "";
    std::vector <int>::iterator targetPropsIt;
    for (targetPropsIt = targetPropertiesIndexes.begin(); targetPropsIt != targetPropertiesIndexes.end(); targetPropsIt++) {
      int currentIndex = *targetPropsIt;
      std::string tempData = currentData[currentIndex];
      targetPropsString = targetPropsString + sepOne + tempData;
      fakeTargetString = fakeTargetString + sepOne + "";
    }
    std::string sharedPropsString = "";
    std::vector <int>::iterator sharedPropsIt;
    for (sharedPropsIt = sharedPropertiesIndexes.begin(); sharedPropsIt != sharedPropertiesIndexes.end(); sharedPropsIt++) {
      int currentIndex = *sharedPropsIt;
      std::string tempData = currentData[currentIndex];
      sharedPropsString = sharedPropsString + sepOne + tempData;
    }
    // And here the entries with multiple values are separated out. We don't do that for properties.
    std::istringstream sourceStringStream(currentSource);
    while (sourceStringStream) {
      std::string s;
      if (!getline(sourceStringStream, s, sepTwoChar)) break;
      while (s[0] == ' ') {
	s = s.substr(1, s.length());
      }
      s = s + sharedPropsString + sourcePropsString + fakeTargetString;
      sepSources.push_back(s);
    }
    std::istringstream targetStringStream(currentTarget);
    while (targetStringStream) {
      std::string s;
      if (!getline(targetStringStream, s, sepTwoChar)) break;
      while (s[0] == ' ') {
	s = s.substr(1, s.length());
      }
      s = s + sharedPropsString + fakeSourceString + targetPropsString;
      sepTargets.push_back(s);
    }
  }
    
  // There will be duplicates in the list of nodes. These are removed here.
  if (!excludeSources) {
    std::sort(sepSources.begin(), sepSources.end());
    sepSources.erase(std::unique(sepSources.begin(), sepSources.end()), sepSources.end());
  }
  if (!excludeTargets) {
    std::sort(sepTargets.begin(), sepTargets.end());
    sepTargets.erase(std::unique(sepTargets.begin(), sepTargets.end()), sepTargets.end());
  }

  /*
    In the above we have basically prepared our data for writing. The actual writing happens below.
    Instead of writing immediately to a file, the lines for the file are first put into a vector,
    which is then written to a file incrementaly. This is mostly an artefact of a function I decided
    no longer to include in the program, but I don't dislike this procedure, so I kept it.
  */
  std::vector <std::string> fileVector;
  if (!excludeSources) {
    std::vector <std::string>::iterator ssI;
    std::vector <std::string> firstSources;
    // Below we separate out the name of the current node. We need to have it separate because we will
    // include the name in the file twice.
    for (ssI = sepSources.begin(); ssI != sepSources.end(); ssI++) {
      std::string currentSource = *ssI;
      std::size_t firstStop = currentSource.find_first_of(sepOne);
      currentSource = currentSource.substr(0, firstStop);
      firstSources.push_back(currentSource);
    }
    // Here we create the actual line of data to be used as output.
    std::vector <std::string>::iterator itSepSource;
    std::vector <std::string>::iterator itSepSourceFirst = firstSources.begin();
    for (itSepSource = sepSources.begin(); itSepSource != sepSources.end(); itSepSource++, itSepSourceFirst++) {
      std::string sepSource = *itSepSource;
      std::string first = *itSepSourceFirst;
      std::string currentLine = first + sepOne + sourceString + sepOne + sepSource + '\n';
      fileVector.push_back(currentLine);
    }
  }

  if (sourceSelection != targetSelection) {
    // And here we basically do the same for the target nodes.
    if (!excludeTargets) {
      std::vector <std::string> firstTargets;
      std::vector <std::string>::iterator stI;
      for (stI = sepTargets.begin(); stI != sepTargets.end(); stI++) {
	std::string currentTarget = *stI;
	std::size_t firstStop = currentTarget.find_first_of(sepOne);
	currentTarget = currentTarget.substr(0, firstStop);
	firstTargets.push_back(currentTarget);
      }
      std::vector <std::string>::iterator itSepTarget;
      std::vector <std::string>::iterator itSepTargetFirst = firstTargets.begin();
      for (itSepTarget = sepTargets.begin(); itSepTarget != sepTargets.end(); itSepTarget++, itSepTargetFirst++) {
	std::string sepTarget = *itSepTarget;
	std::string first = *itSepTargetFirst;
	std::string currentLine = first + sepOne + targetString + sepOne + sepTarget + '\n';
	fileVector.push_back(currentLine);
      }
    }
  }
  
  // Now that we have put all the data we want to write in the file vector, we iterate through the vector
  // and actually write the file.
  std::vector <std::string>::iterator fileIterator;
  for (fileIterator = fileVector.begin(); fileIterator != fileVector.end(); fileIterator++) {
    std::string currentLine = *fileIterator;
    fileOut << currentLine;
  }
  
  // And when we are finished, we can close the file.
  fileOut.close();
  return;
}

// Returns a vector with the headers.
const std::vector<std::string> DataInterface::GetHeader() {
  return header;
}

// Returns a vector with the data.
const std::vector<std::vector <std::string> > DataInterface::GetRowData() {
  return rowData;
}





