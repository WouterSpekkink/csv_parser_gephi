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
 

#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QtGui>
#include <QApplication>
#include <QDialog>
#include "InputTable.h"
#include "CsvOutput.h"
#include <vector>
#include <string>

class QPushButton;
class QComboBox;
class QLabel;
class QSpinBox;
class QCheckBox;

class MainDialog : public QDialog {

  Q_OBJECT

public:
  MainDialog(QWidget *parent=0);

signals:
  void sendFileOne(const QString &file, const QString &sepOne, const QString &sepTwo);
  void sendFileTwo(const QString &file, const QString &sepOne);

private slots:
  void getFile();
  void setSep(const QString &selection);
  void switchSepTwo(const int &state);
  void setSepTwo(const QString &selection);
  void fireFileSend();
  void enableSave();
  void getDetails();
  void resetFileImport();
  void closing();
  
private:
  QLabel *topLabel;
  QLabel *middleLabel;
  QLabel *lowerLabel;
  QString fileName;
  QString sepOne;
  QString sepTwo;
  QPushButton *openFile;
  QComboBox *sepSelector;
  QComboBox *sepTwoSelector;
  QCheckBox *sepTwoSwitcher;
  QPushButton *importFile;
  InputTable *inputTable;
  QPushButton *exitButton;
  std::vector<std::string> tableHeader;
  std::vector<std::string> nameList;
  QPushButton *saveNodes;
  QPushButton *saveEdges;
};

#endif
