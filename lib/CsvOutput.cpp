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
bool CsvOutputEdges(InputTable *table, QString sourceSelection, QString targetSelection, bool directedRelationships, std::string filename, std::string sepOne) {
  // We point to the InputTable and we get all the stuff out that we need.
  InputTable *outputTable = table; // pointer to output table.
  std::vector <std::string> headers = outputTable->GetHeader();
  std::vector <std::vector <std::string> > data = outputTable->GetRowData();
  int sourceIndex = 0;
  int targetIndex = 0;
  std::string sourceString = sourceSelection.toUtf8().constData();
  std::string targetString = targetSelection.toUtf8().constData();

  std::string relationshipType = "Undirected";
  if (directedRelationships) {
    relationshipType = "Directed";
  }

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
	fileOut << currentSepSource << sepOne << currentSepTarget << sepOne << 1 << sepOne << relationshipType;
	fileOut << '\n';
      }
    }
  }
  // And we can close the file.
  fileOut.close();
  return true;
}

bool CsvOutputNodes(InputTable *table, QString sourceSelection, QString targetSelection, std::vector <std::string> sourceProperties, std::vector<std::string> targetProperties, bool excludeTargets, std::string filename, std::string sepOne) {
  // We point to the output table and get everything we need from it.
  InputTable *outputTable = table;
  std::vector <std::string> headers = outputTable->GetHeader();
  std::vector <std::vector <std::string> > data = outputTable->GetRowData();

  int sourceIndex = 0;
  int targetIndex = 0;

  std::vector <int> sourcePropertiesIndexes;
  std::vector <int> targetPropertiesIndexes;
  std::vector <int> sharedPropertiesIndexes;

  std::vector <std::string> sharedProperties;

  std::string sourceString = sourceSelection.toUtf8().constData();
  std::string targetString = targetSelection.toUtf8().constData();
  std::string sharedString = "";

  for (std::vector <std::string>::size_type i = 0; i != sourceProperties.size(); i++) {
    for (std::vector <std::string>::size_type j = 0; j != targetProperties.size(); j++) {
      if (sourceProperties[i] == targetProperties[j]) {
	sharedProperties.push_back(sourceProperties[i]);
	sourceProperties.erase(sourceProperties.begin() + i);
	targetProperties.erase(targetProperties.begin() + j);
      }
    }
  }

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
    std::vector <std::string>::iterator sharedPropertiesIt;
    for (sharedPropertiesIt = sharedProperties.begin(); sharedPropertiesIt != sharedProperties.end(); sharedPropertiesIt++) {
      std::string currentSharedProperty = *sharedPropertiesIt;
      if (currentString == currentSharedProperty) {
	sharedPropertiesIndexes.push_back(i);
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
    return false;
  }
  
  // First we write the header of the file.
  if (!excludeTargets) {
    fileOut << "Id" << sepOne << "Type" << sepOne << "Label" << sharedPropertiesHeader << sourcePropertiesHeader << targetPropertiesHeader;
    fileOut << '\n';
  } else {
    fileOut << "Id" << sepOne << "Type" << sepOne << "Label" << sourcePropertiesHeader;
    fileOut << '\n';
  }

  // And then we process all the data and eventually write it to the file.
  std::vector <std::vector <std::string> >::iterator itData;

  std::vector <std::string> sepSources;
  std::vector <std::string> sepTargets;
  
  for (itData = data.begin(); itData != data.end(); itData++) {
    std::vector <std::string> currentData = *itData;
    std::string currentSource = currentData[sourceIndex];
    std::string currentTarget = currentData[targetIndex];
    std::string fakeSourceString = "";
    std::string sourcePropsString = "";
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
    
    std::string sharedPropsString = "";
    std::vector <int>::iterator sharedPropsIt;
    for (sharedPropsIt = sharedPropertiesIndexes.begin(); sharedPropsIt != sharedPropertiesIndexes.end(); sharedPropsIt++) {
      int currentIndex = *sharedPropsIt;
      sharedPropsString = sharedPropsString + sepOne + currentData[currentIndex];
    }
    
    std::istringstream sourceStringStream(currentSource);
    while (sourceStringStream) {
      std::string s;
      if (!getline(sourceStringStream, s, '+')) break;
	s = s + sharedPropsString + sourcePropsString + fakeTargetString;
	sepSources.push_back(s);
    }

    std::istringstream targetStringStream(currentTarget);
    while (targetStringStream) {
      std::string s;
      if (!getline(targetStringStream, s, '+')) break;
      s = s + sharedPropsString + fakeSourceString + targetPropsString;
      sepTargets.push_back(s);
    }
  }
    
  // Removing all duplicates from sources and targets here.
  std::sort(sepSources.begin(), sepSources.end());
  sepSources.erase(std::unique(sepSources.begin(), sepSources.end()), sepSources.end());

  if (!excludeTargets) {
    std::sort(sepTargets.begin(), sepTargets.end());
    sepTargets.erase(std::unique(sepTargets.begin(), sepTargets.end()), sepTargets.end());
  }

  // Let's first make some vectors we need to make everything neat.
  std::vector <std::string> fileVector;

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
    std::string currentLine = first + sepOne + sourceString + sepOne + sepSource + '\n';
    fileVector.push_back(currentLine);
  }

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
  
  std::vector <std::string>::iterator fileIterator;
  for (fileIterator = fileVector.begin(); fileIterator != fileVector.end(); fileIterator++) {
    std::string currentLine = *fileIterator;
    fileOut << currentLine;
  }
  
  // And when we are finished, we can close the file.
  fileOut.close();
  return true;
}
