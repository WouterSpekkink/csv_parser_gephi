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
  The main purpose of this class is to provide the user with a dialog, through which
  the user can interact with the program.

  The interaction includes selecting a (csv-) file, selecting one or two delimiters,
  saving the nodes file and edges file that are created based on the input file,
  and closing the program.
 */


#include <QtGui>

#include "../include/MainDialog.h"
#include <QFileDialog>
#include <QMessageBox>

// This is the constructor for this class
MainDialog::MainDialog(QWidget *parent) : QDialog(parent) {

  /* InputTable is a class that imports a user-selected csv-file. The main dialog communicates
     with an instance of InputTable to let it know which filename was selected, and what delimiters are used.
     The created instance of InputTable then imports the data and processes it, so that it can be used by CsvOutput.
     The instance of InputTable then only let's the main dialog know that it has processed the data.
     The main dialog uses this information to activate some parts of the interface that are deactivated by default.
     This includes the save buttons for the node and edges file.
     The user can also quit the program from the main dialog.
  */
  inputTable = new InputTable(); 
  exitButton = new QPushButton(tr("Exit Program")); // The exit button
  topLabel = new QLabel(tr("<h3>File import</h3>")); // Just a title for part of the dialog.

  /* The program allows the user to import files that have multiple values in a single cell, separated by columns.
     However, it is not allowed to use the same separator/delimiter for both the columns, and the values within cells,
     because that would make it impossible for the program to distinguish between single value and multi value columns.
     The program is made so that the same delimiters cannot be used twice, and the label below informs the user of this.
  */
  noteSeps = new QLabel(tr("Note: You cannot use the same delimiter twice,\n because that makes the file unreadable to the program."));
  middleLabel = new QLabel(tr("<h3>Save files</h3>")); // Just a title for another part of the dialog
  openFile = new QPushButton(tr("Open File")); // Button to select filename
  sepSelector = new QComboBox(this); // Combobox to select delimiter for columns.
  sepSelector->addItem("-Select a delimiter-");
  sepSelector->addItem(",");
  sepSelector->addItem(";");
  sepSelector->addItem(":");
  sepSelector->addItem("|");
  sepTwoSwitcher = new QCheckBox("Separators within columns", this); // Checkbox to indicate whether multivalue columns exist.
  sepTwoSwitcher->setChecked(Qt::Unchecked); // We set it to unchecked by default.
  sepTwoSelector = new QComboBox(this); // Combobox to select delimiter for multivalue columns. The items are added later.
  sepTwoSelector->addItem("-Select a second delimiter-"); 
  sepTwoSelector->setEnabled(false); // This combobox is only activted if the sepTwoSwitcher is checked.
  importFile = new QPushButton(tr("Import")); // Button to start importing files.
  saveNodes = new QPushButton(tr("Save Nodes File")); // Save button for nodes.
  saveEdges = new QPushButton(tr("Save Edges File")); // Save button for edges.
  saveNodes->setEnabled(false); // The save buttons are disabled at first.
  saveEdges->setEnabled(false);
  
  // Here all the signals through which the main dialog communicates with itself and with other classes.
  // The first one calls a function that opens a dialog to select an input file.
  connect(openFile, SIGNAL(clicked()), this, SLOT(getFile()));
  // The next one resets some of the settings of the program. This is necessary if the user wants to
  // process multiple files in one session.
  connect(openFile, SIGNAL(clicked()), this, SLOT(resetFileImport()));
  // This handles any changes in the selection of the column delimiter.
  // It calls a function that sets the delimiter to what was chosen by the user.
  connect(sepSelector, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setSepOne(const QString &)));
  // This handles any change in the switch that turns on the control for selecting delimiters for multi-value columns.
  // It is a simple on/off switch, but it also deactivates the save options to make sure that the user reimports the input file first.
  connect(sepTwoSwitcher, SIGNAL(stateChanged(const int &)), this, SLOT(switchSepTwo(const int &)));
  // This handles any changes in the selection of the column delimiter.
  // It calls a function that sets the delimitor to what was chosen by the user.
  connect(sepTwoSelector, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setSepTwo(const QString &)));
  /* This signal is fired if the user clicks the import button. It calls a function of the main dialog that determines what function to
     call from the instantiated InputTable Classe. Once this is determined, one of the next two signals is fired.
     These pass the  necessary information to the instantiated InputTable class and tell it to import the file.  */
  connect(importFile, SIGNAL(clicked()), this, SLOT(fireFileSend()));
  connect(this, SIGNAL(sendFileOne(const QString &, const QString &, const QString &)), inputTable, SLOT(readDataOne(const QString &, const QString &, const QString &)));
  connect(this, SIGNAL(sendFileTwo(const QString &, const QString &)), inputTable, SLOT(readDataTwo(const QString &, const QString &)));
  // The exit button signal to close the program.
  connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
  // If this signal is fired, the buttons that allow the user to save files are activated.
  connect(inputTable, SIGNAL(importFinished()), this, SLOT(enableSave()));
  // The two buttons below call one of the save functions. Basically, they call one included in the appropriate functions of the CsvOutput header.
  connect(saveNodes, SIGNAL(clicked()), this, SLOT(saveNodesFile()));
  connect(saveEdges, SIGNAL(clicked()), this, SLOT(saveEdgesFile()));

  // Below the layout is created, which consists out of several building blocks. All the buttons and other widgets are layed out here.
  QVBoxLayout *topLayout = new QVBoxLayout;
  QHBoxLayout *topLayoutOne = new QHBoxLayout;
  topLayoutOne->addWidget(openFile);
  topLayoutOne->addWidget(sepSelector);
  QHBoxLayout *topLayoutTwo = new QHBoxLayout;
  topLayoutTwo->addWidget(sepTwoSwitcher);
  topLayoutTwo->addWidget(sepTwoSelector);
  QHBoxLayout *topLayoutThree = new QHBoxLayout;
  topLayoutThree->addWidget(noteSeps);
  QHBoxLayout *topLayoutFour = new QHBoxLayout;
  topLayoutFour->addWidget(importFile);
  topLayout->addLayout(topLayoutOne);
  topLayout->addLayout(topLayoutTwo);
  topLayout->addLayout(topLayoutThree);
  topLayout->addLayout(topLayoutFour);
  QVBoxLayout *lowerMiddleLayout = new QVBoxLayout;
  lowerMiddleLayout->addWidget(middleLabel);
  lowerMiddleLayout->addWidget(saveNodes);
  lowerMiddleLayout->addWidget(saveEdges);
  QHBoxLayout *lowerLayout = new QHBoxLayout;
  lowerLayout->addWidget(exitButton);
  QFrame *topLine = new QFrame();
  topLine->setFrameShape(QFrame::HLine);
  QFrame *lowerLine = new QFrame();
  lowerLine->setFrameShape(QFrame::HLine);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(topLabel);
  topLayout->setContentsMargins(QMargins(15,15,15,15));
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(topLine);
  lowerMiddleLayout->setContentsMargins(QMargins(15,15,15,15));
  mainLayout->addLayout(lowerMiddleLayout);
  mainLayout->addWidget(lowerLine);
  lowerLayout->setContentsMargins(QMargins(15,15,15,15));
  mainLayout->addLayout(lowerLayout);
  setLayout(mainLayout);
  setWindowTitle(tr("Gephi CSV Parser"));
  setFixedHeight(sizeHint().height());
  // And that finishes the constructor.
}

// Private slots are listed below.

// This gets the filename, using a dialog that requires the user to select a file.
void MainDialog::getFile() { 
  QString file = QFileDialog::getOpenFileName(this, tr("Open File"),"","Comma Delimited Files(*.csv *.txt)");
  if(file.isNull() == false) {
    fileName = file;
  }
}

/* This function sets the combobox for selecting delimiters for multi-valued columns on or off.
   The save buttons are deactivated to force the user to click the import button again.
   Without this, any changes in the settings of the combobox would not be implemented.
*/
void MainDialog::switchSepTwo(const int &state) {
  if(state == Qt::Checked) {
    sepTwoSelector->setEnabled(true);
    saveNodes->setEnabled(false);
    saveEdges->setEnabled(false);
  } else {
    sepTwoSelector->setEnabled(false);
    saveNodes->setEnabled(false);
    saveEdges->setEnabled(false);
  }
}

/* This function sets the delimiter with which columns in the input file are delimited.
   The program cannot distinguish between columns and multi-value cells if the delimiters 
   for both are the same. Therefore, the options for the multi-value delimiters always
   exclude the column delimiter that is selected. The options for the multi-value 
   delimiter are reset each time the user sets another column delimiter.
*/
void MainDialog::setSepOne(const QString &selection) {
  sepOne = selection;
  
  if(sepOne == ",") {
    sepTwoSelector->clear();
    sepTwoSelector->addItem("-Select a second delimiter-");
    sepTwoSelector->addItem(";");
    sepTwoSelector->addItem(":");
    sepTwoSelector->addItem("|");
  } else if (sepOne == ";") {
    sepTwoSelector->clear();
    sepTwoSelector->addItem("-Select a second delimiter-");
    sepTwoSelector->addItem(",");
    sepTwoSelector->addItem(":");
    sepTwoSelector->addItem("|");
  } else if (sepOne == ":") {
    sepTwoSelector->clear();
    sepTwoSelector->addItem("-Select a second delimiter-");
    sepTwoSelector->addItem(",");
    sepTwoSelector->addItem(";");
    sepTwoSelector->addItem("|");
  } else if (sepOne == "|") {
    sepTwoSelector->clear();
    sepTwoSelector->addItem("-Select a second delimiter-");
    sepTwoSelector->addItem(",");
    sepTwoSelector->addItem(";");
    sepTwoSelector->addItem(":");
  }
}

// This function sets the multi-value delimiter to what was chosen by the user.
void MainDialog::setSepTwo(const QString &selection) {
  sepTwo = selection;
}

/* This function first check whether a second type of delimiter is set.
   Depending one the outcome of that check, one of the signals for importing files
   is fired, which will then lead to a request to the instantiated InputTable Class
   to start importing the file.
*/
void MainDialog::fireFileSend()
{
  if (sepTwoSwitcher->checkState() == Qt::Checked) {
    emit sendFileOne(fileName, sepOne, sepTwo);
  } else {
    emit sendFileTwo(fileName, sepOne);
  }
}

// This function will reset some options to prevent unexpected results if the user
// opens another file.
void MainDialog::resetFileImport() {
  saveNodes->setEnabled(false);
  saveEdges->setEnabled(false);
  sepTwoSelector->setEnabled(false);
  sepTwoSwitcher->setChecked(Qt::Unchecked);
}

// This function enables the save buttons.
void MainDialog::enableSave() {
  saveNodes->setEnabled(true);
  saveEdges->setEnabled(true);
}

// This function triggers another function in the CsvOutput header that writes an edges file.
void MainDialog::saveEdgesFile() {
  // We first the user ask for the desired filename for the new edges file.
  QString QsaveFile = QFileDialog::getSaveFileName(this, tr("Save File"),"", "Comma Delimited Files (*.csv *.txt)");
  std::string saveFile = QsaveFile.toStdString();
  std::string stdSep = sepOne.toStdString();
  // After preparing some necessary information, we call the CsvOutputEdges() function.
  CsvOutputEdges(inputTable, saveFile, stdSep);
}

// This function triggers another function in the CsvOutput header that writes an nodes file.
void MainDialog::saveNodesFile() {
  // We first the user ask for the desired filename for the new nodes file.
  QString QsaveFile = QFileDialog::getSaveFileName(this, tr("Save File"),"", "Comma Delimited Files (*.csv *.txt)");
  std::string saveFile = QsaveFile.toStdString();
  std::string stdSep = sepOne.toStdString();
  // After preparing some necessary information, we call the CsvOutputNodes() function.
  CsvOutputNodes(inputTable, saveFile, stdSep);
}

// This function make sure that the memory used by the instantiated InputTable class is freed up again. 
void MainDialog::closing() {
    delete inputTable;
}




