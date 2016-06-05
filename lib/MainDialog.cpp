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
  importFile = new QPushButton(tr("Import"));

  // I probably have to make two separate save buttons, for nodes and edges
  saveEdges = new QPushButton(tr("Save Edges File"));
  saveEdges->setEnabled(false);
  
  
  connect(openFile, SIGNAL(clicked()), this, SLOT(getFile()));
  connect(sepSelector, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setSep(const QString &)));
  connect(importFile, SIGNAL(clicked()), this, SLOT(resetFileImport()));
  connect(importFile, SIGNAL(clicked()), this, SLOT(fireFileSend()));
  connect(this, SIGNAL(sendFile(const QString &, const QString &)), inputTable, SLOT(readData(const QString &, const QString &)));
  // connect(inputTable, SIGNAL(importFinished()), this, SLOT(enableNetwork()));
  // connect(inputTable, SIGNAL(importFinished()), this, SLOT(getDetails()));
  connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
  
  
  // connect(goEdges, SIGNAL(clicked()), this, SLOT(enableSave()));
  // connect(saveEdges, SIGNAL(clicked()), this, SLOT(saveEdgesFile()));
  
  QHBoxLayout *topLayout = new QHBoxLayout;
  topLayout->addWidget(openFile);
  topLayout->addWidget(sepSelector);
  topLayout->addWidget(importFile);
  
  QVBoxLayout *lowerMiddleLayout = new QVBoxLayout;
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

void MainDialog::setSep(const QString &selection)
{
  if(selection == "tab") sep = "\t";
  else sep = selection;
}

void MainDialog::fireFileSend()
{
  emit sendFile(fileName, sep);
}

void MainDialog::getDetails()
{
  tableHeader = inputTable->GetHeader();
  nameList = inputTable->GetRowNames();
  maxGranularity = tableHeader.size();
  chooseGranularity->setRange(1, maxGranularity);
  chooseGranularity->setValue(1);
  chooseLowerBound->setRange(1, maxGranularity);
  chooseUpperBound->setRange(1, maxGranularity);
  chooseLowerBound->setValue(1);
  chooseUpperBound->setValue(maxGranularity);
  monitorLower->setRange(1, maxGranularity);
  monitorUpper->setRange(1, maxGranularity);
  monitorUpper->setValue(maxGranularity);
}

void MainDialog::resetFileImport()
{
  saveEdges->setEnabled(false);
}

void MainDialog::enableSave()
{
  saveEdges->setEnabled(true);
}

void MainDialog::saveEdgesFile()
{    
  QString QsaveFile = QFileDialog::getSaveFileName(this, tr("Save File"),"", "Comma Delimited Files (*.csv *.txt)");
  std::string saveFile = QsaveFile.toStdString();
  std::string stdSep = sep.toStdString();
  if(networkType == "Whole Network - Dynamic" || networkType == "Ego Network - Dynamic") {
    std::string type = networkType.toStdString();
    CsvOutput(edgeFinder, type, saveFile, stdSep);
  }
  if(networkType == "Partial Network - Static" || networkType == "Ego Network - Static") {
    std::string type = networkType.toStdString();
    CsvOutput(edgeFinder, type, saveFile, stdSep);
  }
  if(networkType == "Two Mode Network - Dynamic") {
    CsvOutput(twoModeEdges, saveFile, stdSep);
  }
}

void MainDialog::closing()
{
    delete inputTable;
    delete matCollection;
    delete edgeFinder;
    delete twoModeEdges;
}




