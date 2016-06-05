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

// I have to rewrite this class a bit, because it needs to read a different kind of file.

#ifndef INPUTTABLE_H
#define INPUTTABLE_H

#include <QObject>
#include <string>
#include <vector>

class InputTable : public QObject {

    Q_OBJECT

public:
    InputTable(QWidget *parent = 0); 
    void ReadFile(const std::string &file, const char &delim);
    int GetRows();
    int GetCols();
    const std::vector<std::string> GetHeader();
    const std::vector<std::string> GetRowNames();
    const std::vector<std::vector <short> > GetRowData();

    ~InputTable() {}

signals:
    void importFinished();

private slots:
void readData(const QString &fileName, const QString &sep);
    
private:
    std::vector<std::string> header; // This will hold the header of the file
    std::vector<std::string> rowNames; // This will hold the row names

    // I think I could keep this to hold the data, but probably need to change the variable
    // type into string.
    std::vector<std::vector <short> > rowData; // This will hold the numerical data row-wise
    int nrow;
    int ncol;
  };
#endif
