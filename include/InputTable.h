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
  ===NOTES===
  See InputTable.cpp for more details on what this class and its methods do.  

 */
   
#ifndef INPUTTABLE_H
#define INPUTTABLE_H

#include <QObject>
#include <string>
#include <vector>

class InputTable : public QObject {

  Q_OBJECT

public:
  InputTable(QWidget *parent = 0); 
  void ReadFileOne(const std::string &file, const char &delimOne, const char &delimTwo);
  void ReadFileTwo(const std::string &file, const char &delimOne);
  int GetEventNumber();
  const std::vector<std::string> GetHeader();
  const std::vector<std::vector <std::string> > GetRowData();
  
  ~InputTable() {}
  
signals:
  void importFinished();
		       
private slots:
  void readDataOne(const QString &fileName, const QString &sepOne, const QString &sepTwo);
  void readDataTwo(const QString &fileName, const QString &sepOne);
  
private:
  // We assume that the first column of the file that the user submits has the events/incidents.
  // We also assume that the file has a header.
  // The rest of the contents of the file are treated as data.
  std::vector<std::string> header; 
  std::vector<std::vector <std::string> > rowData;
  int nEvents;
};
#endif
