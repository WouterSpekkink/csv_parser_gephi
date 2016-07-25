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
bool CsvOutputEdges(InputTable *table, QString sourceSelection, QString targetSelection, std::string filename, std::string sepOne) {
  // We point to the InputTable and we get all the stuff out that we need.
  InputTable *outputTable = table; // pointer to output table.
  std::vector <std::string> headers = outputTable->GetHeader();
  std::vector <std::vector <std::string> > data = outputTable->GetRowData();
  int sourceIndex = 0;
  int targetIndex = 0;
  std::string sourceString = sourceSelection.toUtf8().constData();
  std::string targetString = targetSelection.toUtf8().constData();

  for (std::vector<std::string>::size_type i = 0; i != headers.size(); i++) {
    std::string currentString = headers[i];
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
    return false;
  }
  
  // First we write the header of the file.
  fileOut << "Source" << sepOne << "Target" << sepOne << "Weight" << sepOne << "Type";
  fileOut << '\n';

  std::vector <std::vector <std::string> >::iterator itData;
  
  for (itData = data.begin(); itData != data.end(); itData++) {
    std::vector <std::string> currentData = *itData;
    std::string currentSource = currentData[sourceIndex];
    std::string currentTarget = currentData[targetIndex];
    std::vector <std::string> sepSources;
    std::vector <std::string> sepTargets;
    std::istringstream sourceStream(currentSource);
    while (sourceStream) {
      std::string s;
      if (!getline(sourceStream, s, '+')) break;
      sepSources.push_back(s);
    }
    std::istringstream targetStream(currentTarget);
    while (targetStream) {
      std::string s;
      if (!getline(targetStream, s, '+')) break;
      sepTargets.push_back(s);
    }
    std::vector <std::string>::iterator itSepSources;
    std::vector <std::string>::iterator itSepTargets;
    for (itSepSources = sepSources.begin(); itSepSources != sepSources.end(); itSepSources++) {
      std::string currentSepSource = *itSepSources;
      for (itSepTargets = sepTargets.begin(); itSepTargets != sepTargets.end(); itSepTargets++) {
	std::string currentSepTarget = *itSepTargets;
	fileOut << currentSepSource << sepOne << currentSepTarget << sepOne << 1 << sepOne << "Undirected";
	fileOut << '\n';
      }
    }
  }
  // And we can close the file.
  fileOut.close();
  return true;
}

bool CsvOutputNodes(InputTable *table, QString sourceSelection, QString targetSelection, std::vector <std::string> sourceProperties, std::vector<std::string> targetProperties, std::string filename, std::string sepOne) {
  // We point to the output table and get everything we need from it.
  InputTable *outputTable = table;
  std::vector <std::string> headers = outputTable->GetHeader();
  std::vector <std::vector <std::string> > data = outputTable->GetRowData();

  int sourceIndex = 0;
  int targetIndex = 0;

  std::vector <int> sourcePropertiesIndexes;
  std::vector <int> targetPropertiesIndexes;

  std::string sourceString = sourceSelection.toUtf8().constData();
  std::string targetString = targetSelection.toUtf8().constData();

  for (std::vector<std::string>::size_type i = 0; i != headers.size(); i++) {
    std::string currentString = headers[i];
    if (currentString == sourceString) {
      sourceIndex = i;
    }
    if (currentString == targetString) {
      targetIndex = i;
    }

    // Here we make indexes for our source properties, so we know from which columns they originate.
    std::vector <std::string>::iterator sourcePropertiesIt;
    for (sourcePropertiesIt = sourceProperties.begin(); sourcePropertiesIt != sourceProperties.end(); sourcePropertiesIt++) {
      std::string currentSourceProperty = *sourcePropertiesIt;
      if (currentString == currentSourceProperty) {
	sourcePropertiesIndexes.push_back(i);
      }
    }

    // Here we make indexes for our target properties, so we know from which columns they originate.
    std::vector <std::string>::iterator targetPropertiesIt;
    for (targetPropertiesIt = targetProperties.begin(); targetPropertiesIt != targetProperties.end(); targetPropertiesIt++) {
      std::string currentTargetProperty = *targetPropertiesIt;
      if (currentString == currentTargetProperty) {
	targetPropertiesIndexes.push_back(i);
      }
    }
  }


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
      
  // Then we prepare the file for writing.
  std::ofstream fileOut(filename.c_str());
  if (!fileOut.is_open()) {
    return false;
  }
  
  // First we write the header of the file.
  fileOut << "Id" << sepOne << "Type" << sepOne << "Label" << sourcePropertiesHeader << targetPropertiesHeader;
  fileOut << '\n';
  
  // Writing the rest of the data is easier in this case.
  // Just iterate through the different vectors and write their contents to the file.

  std::vector <std::vector <std::string> >::iterator itData;

  std::vector <std::string> sepSources;
  std::vector <std::string> sepTargets;

  for (itData = data.begin(); itData != data.end(); itData++) {
    std::vector <std::string> currentData = *itData;
    std::string currentSource = currentData[sourceIndex];
    std::string currentTarget = currentData[targetIndex];

    std::string sourcePropsString = "";
    std::string fakeSourceString = "";
    std::vector <int>::iterator sourcePropsIt;
    for (sourcePropsIt = sourcePropertiesIndexes.begin(); sourcePropsIt != sourcePropertiesIndexes.end(); sourcePropsIt++) {
      int currentIndex = *sourcePropsIt;
      sourcePropsString = sourcePropsString + sepOne + currentData[currentIndex];
      fakeSourceString = fakeSourceString + sepOne + "";
    }
    
    std::string targetPropsString = "";
    std::string fakeTargetString = "";
    std::vector <int>::iterator targetPropsIt;
    for (targetPropsIt = targetPropertiesIndexes.begin(); targetPropsIt != targetPropertiesIndexes.end(); targetPropsIt++) {
      int currentIndex = *targetPropsIt;
      targetPropsString = targetPropsString + sepOne + currentData[currentIndex];
      fakeTargetString = fakeTargetString + sepOne + "";
    }
             
    std::istringstream sourceStringStream(currentSource);
    while (sourceStringStream) {
      std::string s;
      if (!getline(sourceStringStream, s, '+')) break;
      s = s + sourcePropsString + fakeTargetString;
      sepSources.push_back(s);
    }

    std::istringstream targetStringStream(currentTarget);
    while (targetStringStream) {
      std::string s;
      if (!getline(targetStringStream, s, '+')) break;
      s = s + fakeSourceString + targetPropsString;
      sepTargets.push_back(s);
    }
  }

  std::sort(sepSources.begin(), sepSources.end());
  sepSources.erase(std::unique(sepSources.begin(), sepSources.end()), sepSources.end());
  std::vector <std::string>::iterator ssI;
  std::vector <std::string> firstSources;
  for (ssI = sepSources.begin(); ssI != sepSources.end(); ssI++) {
    std::string currentSource = *ssI;
    std::size_t firstStop = currentSource.find_first_of(sepOne);
    currentSource = currentSource.substr(0, firstStop);
    firstSources.push_back(currentSource);
  }
  
  std::vector <std::string>::iterator itSepSource;
  std::vector <std::string>::iterator itSepSourceFirst = firstSources.begin();
  for (itSepSource = sepSources.begin(); itSepSource != sepSources.end(); itSepSource++, itSepSourceFirst++) {
    std::string sepSource = *itSepSource;
    std::string first = *itSepSourceFirst;
    fileOut << first << sepOne << sourceString << sepOne << sepSource;
    fileOut << '\n';
  }

  std::sort(sepTargets.begin(), sepTargets.end());
  sepTargets.erase(std::unique(sepTargets.begin(), sepTargets.end()), sepTargets.end());
  std::vector <std::string>::iterator stI;
  std::vector <std::string> firstTargets;
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
    fileOut << first << sepOne << targetString << sepOne << sepTarget;
    fileOut << '\n';
  }
  
  // And when we are finished, we can close the file.
  fileOut.close();
  return true;
}
