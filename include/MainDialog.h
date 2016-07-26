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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with the Gephi CSV Parser.  If not, see <http://www.gnu.org/licenses/>.
  
*/

/*
  ===NOTES===
  See MainDialog.cpp for more details on what this class and its methods do.  

 */

#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QtGui>
#include <QApplication>
#include <QDialog>
#include "PropertiesDialog.h"
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
  void sendDetails(const QVector<QString> &vector, const QString &source, const QString &target);;
  
private slots:
  void getFile();
  void setSepOne(const QString &selection);
  void switchSepTwo(const int &state);
  void setSepTwo(const QString &selection);
  void fireFileSend();
  void enableVariables();
  void openPropertiesDialog();
  void setSourceSelection(const QString &selection);
  void setTargetSelection(const QString &selection);
  void enableSave();
  void setProperties(const QVector<QString> &sourceProps, const QVector<QString> &targetProps);
  void setRelationshipType();
  void saveEdgesFile();
  void setExcludeTargets(const int &state);
  void saveNodesFile();
  void closing();
  void resetFileImport();
  
private:
  QLabel *topLabel;
  QLabel *lowerLabel;
  QLabel *noteSeps;

  // Adding some new widgets
  QLabel *middleLabel;
  QLabel *nodesEdgesLabel;
  QComboBox *sourceSelector;
  QComboBox *targetSelector;
  QPushButton *setPropertiesButton; // Should open a new dialog that allows the user to set the properties of the source and target with checkboxes.
  QString sourceSelection;
  QString targetSelection;
  std::vector <std::string> sourceProperties;
  std::vector <std::string> targetProperties;
  QDialog *propertiesDialog;
  QCheckBox *excludeTargetsCheckBox;
  bool excludeTargets;
  QLabel *relationsLabel;
  QButtonGroup *relationsGroup;
  QCheckBox *relationsDirectedCheckBox;
  QCheckBox *relationsUndirectedCheckBox;
  bool directedRelationships;
  // 

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
  QPushButton *saveNodes;
  QPushButton *saveEdges;
};

#endif
