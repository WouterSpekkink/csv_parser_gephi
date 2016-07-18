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

TODO:
 * I have to add something that allows you the columns to be exported as Nodes and Edges.
   This also needs to be passed as arguments to the CsvOutput method responsible for writing
   the files.

 */

#include <QtGui>

#include "../include/MainDialog.h"
#include <QFileDialog>
#include <QMessageBox>

// Constructor
MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
{
  //
  inputTable = new InputTable();
  exitButton = new QPushButton(tr("Exit Program"));
  topLabel = new QLabel(tr("<h3>File import</h3>"));
  openFile = new QPushButton(tr("Open File"));
  sepSelector = new QComboBox(this);
  sepSelector->addItem("-Select a delimiter-");
  sepSelector->addItem(",");
  sepSelector->addItem(";");
  sepSelector->addItem("|");
  sepSelector->addItem("tab");
  sepTwoSwitcher = new QCheckBox("Separators within columns", this);
  sepTwoSwitcher->setChecked(0);
  sepTwoSelector = new QComboBox(this);
  sepTwoSelector->addItem("-Select a delimiter-");
  sepTwoSelector->addItem(",");
  sepTwoSelector->addItem(";");
  sepTwoSelector->addItem("|");
  sepTwoSelector->setEnabled(false);
  importFile = new QPushButton(tr("Import"));
  saveNodes = new QPushButton(tr("Save Nodes File"));
  saveEdges = new QPushButton(tr("Save Edges File"));
  saveNodes->setEnabled(false);
  saveEdges->setEnabled(false);
    
  connect(openFile, SIGNAL(clicked()), this, SLOT(getFile()));
  connect(sepSelector, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setSep(const QString &)));
  connect(sepTwoSwitcher, SIGNAL(stateChanged(const int &)), this, SLOT(switchSepTwo(const int &)));
  connect(sepTwoSelector, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setSep(const QString &)));
  connect(importFile, SIGNAL(clicked()), this, SLOT(resetFileImport()));
  connect(importFile, SIGNAL(clicked()), this, SLOT(fireFileSend()));
  connect(this, SIGNAL(sendFileOne(const QString &, const QString &, const QString &)), inputTable, SLOT(readDataOne(const QString &, const QString &, const QString &)));
  connect(this, SIGNAL(sendFileTwo(const QString &, const QString &)), inputTable, SLOT(readDataTwo(const QString &, const QString &)));
  // connect(inputTable, SIGNAL(importFinished()), this, SLOT(enableNetwork()));
  // connect(inputTable, SIGNAL(importFinished()), this, SLOT(getDetails()));
  connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

  // goEdges no longer exists, but I should replace it by another signal source that enables both the Nodes and Edges save buttons.
  // Probably this should be a button that activates the conversion of the data. This could just be the file import button.
  // Look at the two signals that I commented out above.
  //connect(goEdges, SIGNAL(clicked()), this, SLOT(enableSave()));
  connect(saveNodes, SIGNAL(clicked()), this, SLOT(saveNodesFile()));
  connect(saveEdges, SIGNAL(clicked()), this, SLOT(saveEdgesFile()));
    
  QVBoxLayout *topLayout = new QVBoxLayout;
  QHBoxLayout *topLayoutOne = new QHBoxLayout;
  topLayoutOne->addWidget(openFile);
  topLayoutOne->addWidget(sepSelector);
  QHBoxLayout *topLayoutTwo = new QHBoxLayout;
  topLayoutTwo->addWidget(sepTwoSwitcher);
  topLayoutTwo->addWidget(sepTwoSelector);
  QHBoxLayout *topLayoutThree = new QHBoxLayout;
  topLayoutThree->addWidget(importFile);
  topLayout->addLayout(topLayoutOne);
  topLayout->addLayout(topLayoutTwo);
  topLayout->addLayout(topLayoutThree);
    
  QVBoxLayout *lowerMiddleLayout = new QVBoxLayout;
  lowerMiddleLayout->addWidget(saveNodes);
  lowerMiddleLayout->addWidget(saveEdges);

  QHBoxLayout *lowerLayout = new QHBoxLayout;
  lowerLayout->addWidget(exitButton);
  
  QFrame *topLine = new QFrame();
  topLine->setFrameShape(QFrame::HLine);

  QFrame *lowerLine = new QFrame();
  lowerLine->setFrameShape(QFrame::HLine);
  
  QFrame *sepEdges = new QFrame();
  sepEdges->setFrameShape(QFrame::HLine);
    
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(topLabel);
  topLayout->setContentsMargins(QMargins(15,15,15,15));
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(topLine);
  mainLayout->addWidget(sepEdges);
  mainLayout->addWidget(lowerLabel);
  lowerMiddleLayout->setContentsMargins(QMargins(15,15,15,15));
  mainLayout->addLayout(lowerMiddleLayout);
  mainLayout->addWidget(lowerLine);
  lowerLayout->setContentsMargins(QMargins(15,15,15,15));
  mainLayout->addLayout(lowerLayout);
  setLayout(mainLayout);

  setWindowTitle(tr("Gephi CSV Parser"));
  setFixedHeight(sizeHint().height());
    
}

// Private slots

void MainDialog::getFile() 
{
  QString file = QFileDialog::getOpenFileName(this, tr("Open File"),"","Comma Delimited Files(*.csv *.txt)");
  if(file.isNull() == false) {
    fileName = file;
  }
}

void MainDialog::switchSepTwo(const int &state)
{
  if(state == 0)
    {
      sepTwoSelector->setEnabled(false);
    }
  else if(state == 2)
    {
      sepTwoSelector->setEnabled(true);
    }
}

void MainDialog::setSep(const QString &selection)
{
  if(selection == "tab") sepOne = "\t";
  else sepOne = selection;
}

void MainDialog::setSepTwo(const QString &selection)
{
  sepTwo = selection;
}

void MainDialog::fireFileSend()
{
  if (sepTwoSwitcher->isChecked()) {
    emit sendFileOne(fileName, sepOne, sepTwo);
  } else {
    emit sendFileTwo(fileName, sepOne);
  }
}

void MainDialog::getDetails()
{
  tableHeader = inputTable->GetHeader();
  //nameList = inputTable->GetRowNames();
  //maxGranularity = tableHeader.size();
  //chooseGranularity->setRange(1, maxGranularity);
  //chooseGranularity->setValue(1);
  //chooseLowerBound->setRange(1, maxGranularity);
  //chooseUpperBound->setRange(1, maxGranularity);
  //chooseLowerBound->setValue(1);
  //chooseUpperBound->setValue(maxGranularity);
  //monitorLower->setRange(1, maxGranularity);
  //monitorUpper->setRange(1, maxGranularity);
  //monitorUpper->setValue(maxGranularity);
}

void MainDialog::resetFileImport()
{
  saveNodes->setEnabled(false);
  saveEdges->setEnabled(false);
  sepTwoSwitcher->setEnabled(false);
  sepTwoSwitcher->setChecked(0);
}

void MainDialog::enableSave()
{
  saveNodes->setEnabled(true);
  saveEdges->setEnabled(true);
}

// I need to rewrite the stuff below, after I have rewritten the CsvOutput class.


//void MainDialog::saveEdgesFile()
//{    
//  QString QsaveFile = QFileDialog::getSaveFileName(this, tr("Save File"),"", "Comma Delimited Files (*.csv *.txt)");
//  std::string saveFile = QsaveFile.toStdString();
//  std::string stdSep = sep.toStdString();
//  if(networkType == "Whole Network - Dynamic" || networkType == "Ego Network - Dynamic") {
//    std::string type = networkType.toStdString();
//    CsvOutput(edgeFinder, type, saveFile, stdSep);
//  }
//  if(networkType == "Partial Network - Static" || networkType == "Ego Network - Static") {
//    std::string type = networkType.toStdString();
//    CsvOutput(edgeFinder, type, saveFile, stdSep);
//  }
//  if(networkType == "Two Mode Network - Dynamic") {
//    CsvOutput(twoModeEdges, saveFile, stdSep);
//  }
//}

void MainDialog::closing()
{
    delete inputTable;
}




