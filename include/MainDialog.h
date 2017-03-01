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
#include "DataInterface.h"
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
  void sendFile(const QString &file, const QString &sepOne);
  void sendDetails(const QVector<QString> &vector, const QString &source, const QString &target);;
  
private slots:
  void getFile();
  void setSepOne(const QString &selection);
  void switchSepTwo(const int &state);
  void setSepTwo(const QString &selection);
  //void fireFileSend();
  void readNewData();
  void enableVariables();
  void openPropertiesDialog();
  void setSourceSelection(const QString &selection);
  void setTargetSelection(const QString &selection);
  void setProperties(const QVector<QString> &sourceProps, const QVector<QString> &targetProps);
  void setRelationshipType();
  void setRelationshipLabel(const QString &label);
  void saveEdgesFile();
  void setExcludeSources(const int &state);
  void setExcludeTargets(const int &state);
  void saveNodesFile();
  void closing();
  void resetFileImport();
  
private:
  QPointer<QLabel> topLabel;
  QPointer<QLabel> lowerLabel;
  QPointer<QLabel> noteSeps;
  QPointer<QLabel> middleLabel;
  QPointer<QLabel> nodesEdgesLabel;
  QPointer<QComboBox> sourceSelector;
  QPointer<QComboBox> targetSelector;
  QPointer<QPushButton> setPropertiesButton; 
  QPointer<QDialog> propertiesDialog;
  QPointer<QCheckBox> excludeSourcesCheckBox;
  QPointer<QCheckBox> excludeTargetsCheckBox;
  QPointer<QLabel> relationsLabel;
  QPointer<QButtonGroup> relationsGroup;
  QPointer<QCheckBox> relationsDirectedCheckBox;
  QPointer<QCheckBox> relationsUndirectedCheckBox;
  QPointer<QLabel> warningSeps;
  QPointer<QLabel> relationsTypeLabel;
  QPointer<QLineEdit> relationsTypeText;
  QPointer<QPushButton> openFile;
  QPointer<QComboBox> sepSelector;
  QPointer<QComboBox> sepTwoSelector;
  QPointer<QCheckBox> sepTwoSwitcher;
  QPointer<QPushButton> importFile;
  QPointer<DataInterface> dataInterface;
  QPointer<QPushButton> exitButton;
  QPointer<QPushButton> saveNodes;
  QPointer<QPushButton> saveEdges;

  bool excludeSources;
  bool excludeTargets;
  bool directedRelationships;
  QString fileName;
  QString sepOne;
  QString sepTwo;
  QString sourceSelection;
  QString targetSelection;
  QString relationsType;
  std::vector <std::string> sourceProperties;
  std::vector <std::string> targetProperties;
};

#endif
