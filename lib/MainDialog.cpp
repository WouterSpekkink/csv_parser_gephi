
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
 */


#include <QtGui>

#include "../include/MainDialog.h"
#include <QFileDialog>
#include <QMessageBox>

// This is the constructor for this class
MainDialog::MainDialog(QWidget *parent) : QDialog(parent) {

  /* 
     InputTable is a class that imports a user-selected csv-file. The main dialog communicates
     with an instance of InputTable to let it know which filename was selected, and what delimiters are used.
     The created instance of InputTable then imports the data and processes it, so that it can be used by CsvOutput 
     and the properties dialog. The instance of InputTable then only let's the main dialog know that it has processed the data.
  */
  inputTable = new InputTable(); 
  exitButton = new QPushButton(tr("Exit Program")); // The exit button
  topLabel = new QLabel(tr("<h3>File import</h3>")); // Just a title for part of the dialog
  openFile = new QPushButton(tr("Open File")); // Button to select filename
  sepSelector = new QComboBox(this); // Combobox to select delimiter for columns.
  sepSelector->addItem(tr("-Select a delimiter-"));
  sepSelector->addItem(",");
  sepSelector->addItem(";");
  sepSelector->addItem(":");
  sepSelector->addItem("|");
  sepTwoSwitcher = new QCheckBox(tr("Separators within columns"), this); // Checkbox to indicate whether multivalue columns exist.
  sepTwoSwitcher->setChecked(Qt::Unchecked); // We set it to unchecked by default.
  sepTwoSelector = new QComboBox(this); // Combobox to select delimiter for multivalue columns. The items are added later.
  sepTwoSelector->addItem(tr("-Select a second delimiter-")); 
  sepTwoSelector->setEnabled(false); // This combobox is only activted if the sepTwoSwitcher is checked.
  importFile = new QPushButton(tr("Import")); // Button to start importing files.
 
  /* 
     The program allows the user to import files that have multiple values in a single cell, separated by columns.
     However, it is not allowed to use the same separator/delimiter for both the columns, and the values within cells,
     because that would make it impossible for the program to distinguish between single value and multi value columns.
     The program is made so that the same delimiters cannot be used twice, and the label below informs the user of this.
  */
  noteSeps = new QLabel(tr("Note: You cannot use the same delimiter twice,\n because that makes the file unreadable to the program."));
  middleLabel = new QLabel(tr("<h3>Configure Nodes and Edges</h3>")); // Label for the part where nodes and edges are configurated
  nodesEdgesLabel = new QLabel(tr("<h4>Set source and target nodes")); // Just an info label
  sourceSelector = new QComboBox(this); // Combobox that allows the user to select a source node
  sourceSelector->addItem("-Select Source Node-"); // The other options for this are created dynamically, based on the input file
  targetSelector = new QComboBox(this); // Combobox that allows the user to select a target node
  targetSelector->addItem("-Select Target Node-"); // The other options for this are created dynamically, based on the input file
  // The button below will open a dialog where the user can select properties to be assigned to nodes and edges. 
  setPropertiesButton = new QPushButton(tr("Set Properties")); 
  sourceSelector->setEnabled(false); // The selectors are disabled by default, and will be enabled once a file has been imported
  targetSelector->setEnabled(false);
  setPropertiesButton->setEnabled(false); // The same goes for the button to open the properties dialog.
  // The user can use the checkbox below to indicate that sources should be excluded from the nodes list, which is sometimes useful.
  excludeSourcesCheckBox = new QCheckBox("Exclude sources from nodes list\n(The associated properties will also be excluded)");
  excludeSourcesCheckBox->setEnabled(false); // Also disabled by default.
  excludeSources = false; // And we assume that source nodes are not excluded normally.
  /*
    We let the user indicate what the type of relationship in the edge list is, which can be either undirected or directed.
    We use an exclusive group of two checkboxes for this (exclusive meaning that checking one automatically unchecks the other).
    The checkboxes are disabled by default, and are activated when the user has selected appropriate source and target nodes.
   */
  relationsLabel = new QLabel(tr("<h4>Set relationship type</h4>"));
  relationsGroup = new QButtonGroup(this);
  relationsGroup->setExclusive(true);
  relationsDirectedCheckBox = new QCheckBox(tr("Relationships are directed"));
  relationsUndirectedCheckBox = new QCheckBox(tr("Relationships are undirected"));
  relationsGroup->addButton(relationsDirectedCheckBox, 1);
  relationsGroup->addButton(relationsUndirectedCheckBox, 2);
  relationsUndirectedCheckBox->setEnabled(false);
  relationsDirectedCheckBox->setEnabled(false);
  relationsUndirectedCheckBox->setCheckState(Qt::Checked);
  directedRelationships = false;
  /*
    The lower part of the main dialog is used to save the nodes and edges list as configured by the user, or to
    quit the program. The program assumes that the same delimiters should be used as those that were used in the 
    imported file. 
   */
  warningSeps = new QLabel(tr("The separators as they are currently set will also be used in the saved files"));
  lowerLabel = new QLabel(tr("<h3>Save files</h3>")); // Just a title for another part of the dialog
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
  connect(this, SIGNAL(sendFile(const QString &, const QString &)), inputTable, SLOT(readData(const QString &, const QString &)));
  // This signal activates the interface for configuring nodes and edges, after a file is imported.
  connect(inputTable, SIGNAL(importFinished()), this, SLOT(enableVariables()));
  // The next two signals indicate changes in the selected source or target node, and call the appropriate functions to handle that.
  connect(sourceSelector, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setSourceSelection(const QString &)));
  connect(targetSelector, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(setTargetSelection(const QString &)));
  // This signal makes it so that the properties button calls a function that opens the properties dialog.
  connect(setPropertiesButton, SIGNAL(clicked()), this, SLOT(openPropertiesDialog()));
  // This signal indicates a change in the setting for excluding sources or not, and calls the appropriate function.
  connect(excludeSourcesCheckBox, SIGNAL(stateChanged(const int &)), this, SLOT(setExcludeSources(const int &)));
  // This signal calls a function that handles a change in one of the checkboxes that indicate the type of edge relationships.
  connect(relationsGroup, SIGNAL(buttonClicked(const int &)), this, SLOT(setRelationshipType()));
  // The two buttons below call one of the save functions. Basically, they call one included in the appropriate functions of the CsvOutput header.
  connect(saveNodes, SIGNAL(clicked()), this, SLOT(saveNodesFile()));
  connect(saveEdges, SIGNAL(clicked()), this, SLOT(saveEdgesFile()));
  // The exit button signal to close the program.
  connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

  // Below the layout is created, which consists out of several building blocks. All the buttons and other widgets are layed out here.
  QVBoxLayout *topLayout = new QVBoxLayout;
  topLayout->addWidget(topLabel);
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
  
  QVBoxLayout *upperMiddleLayout = new QVBoxLayout;
  QHBoxLayout *upperMiddleLayoutMain = new QHBoxLayout;
  QVBoxLayout *upperMiddleLayoutMinor = new QVBoxLayout;
  upperMiddleLayoutMain->addWidget(sourceSelector);
  upperMiddleLayoutMain->addWidget(targetSelector);
  upperMiddleLayoutMain->addWidget(setPropertiesButton);
  upperMiddleLayoutMinor->addWidget(excludeSourcesCheckBox);
  upperMiddleLayoutMinor->addWidget(relationsLabel);
  upperMiddleLayoutMinor->addWidget(relationsUndirectedCheckBox);
  upperMiddleLayoutMinor->addWidget(relationsDirectedCheckBox);
  upperMiddleLayout->addWidget(middleLabel);
  upperMiddleLayout->addWidget(nodesEdgesLabel);
  upperMiddleLayout->addLayout(upperMiddleLayoutMain);
  upperMiddleLayout->addLayout(upperMiddleLayoutMinor);

  QVBoxLayout *lowerMiddleLayout = new QVBoxLayout;
  lowerMiddleLayout->addWidget(lowerLabel);
  lowerMiddleLayout->addWidget(saveNodes);
  lowerMiddleLayout->addWidget(saveEdges);
  lowerMiddleLayout->addWidget(warningSeps);

  QHBoxLayout *lowerLayout = new QHBoxLayout;
  lowerLayout->addWidget(exitButton);
  
  QFrame *topLine = new QFrame();
  topLine->setFrameShape(QFrame::HLine);
  QFrame *middleLine = new QFrame();
  middleLine->setFrameShape(QFrame::HLine);
  QFrame *lowerLine = new QFrame();
  lowerLine->setFrameShape(QFrame::HLine);
  
  QVBoxLayout *mainLayout = new QVBoxLayout; 
  topLayout->setContentsMargins(QMargins(15, 15, 15, 15));
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(topLine);
  upperMiddleLayout->setContentsMargins(QMargins(15, 15, 15, 15));
  mainLayout->addLayout(upperMiddleLayout);
  mainLayout->addWidget(middleLine);
  lowerMiddleLayout->setContentsMargins(QMargins(15, 15, 15, 15));  
  mainLayout->addLayout(lowerMiddleLayout);
  mainLayout->addWidget(lowerLine);
  lowerLayout->setContentsMargins(QMargins(15, 15, 15, 15));
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

/* This function sets the delimiter with which columns in the input file are delimited.
   The program cannot distinguish between columns and multi-value cells if the delimiters 
   for both are the same. Therefore, the options for the multi-value delimiters always
   exclude the column delimiter that is selected. The options for the multi-value 
   delimiter are reset each time the user sets another column delimiter.

   We also reset other functions and interface options for transparency reasons.
*/
void MainDialog::setSepOne(const QString &selection) {
  sepOne = selection;
  
  if(sepOne == ",") {
    sepTwoSelector->clear();
    sepTwoSelector->addItem(tr("-Select a second delimiter-"));
    sepTwoSelector->addItem(";");
    sepTwoSelector->addItem(":");
    sepTwoSelector->addItem("|");
  } else if (sepOne == ";") {
    sepTwoSelector->clear();
    sepTwoSelector->addItem(tr("-Select a second delimiter-"));
    sepTwoSelector->addItem(",");
    sepTwoSelector->addItem(":");
    sepTwoSelector->addItem("|");
  } else if (sepOne == ":") {
    sepTwoSelector->clear();
    sepTwoSelector->addItem(tr("-Select a second delimiter-"));
    sepTwoSelector->addItem(",");
    sepTwoSelector->addItem(";");
    sepTwoSelector->addItem("|");
  } else if (sepOne == "|") {
    sepTwoSelector->clear();
    sepTwoSelector->addItem(tr("-Select a second delimiter-"));
    sepTwoSelector->addItem(",");
    sepTwoSelector->addItem(";");
    sepTwoSelector->addItem(":");
  }
  sepTwoSelector->setCurrentIndex(sepTwoSelector->findText(tr("-Select a second delimiter-")));
  saveNodes->setEnabled(false);
  saveEdges->setEnabled(false);
  sourceSelector->setEnabled(false);
  targetSelector->setEnabled(false);
  setPropertiesButton->setEnabled(false);
  excludeSourcesCheckBox->setEnabled(false);
  excludeSourcesCheckBox->setCheckState(Qt::Unchecked);
  relationsDirectedCheckBox->setEnabled(false);
  relationsUndirectedCheckBox->setEnabled(false);
  relationsDirectedCheckBox->setCheckState(Qt::Unchecked);
  relationsUndirectedCheckBox->setCheckState(Qt::Checked);
  excludeSources = false;
  directedRelationships = false;
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
    sourceSelector->setEnabled(false);
    targetSelector->setEnabled(false);
    setPropertiesButton->setEnabled(false);
    excludeSourcesCheckBox->setEnabled(false);
    excludeSourcesCheckBox->setCheckState(Qt::Unchecked);
    relationsDirectedCheckBox->setEnabled(false);
    relationsUndirectedCheckBox->setEnabled(false);
    relationsDirectedCheckBox->setCheckState(Qt::Unchecked);
    relationsUndirectedCheckBox->setCheckState(Qt::Checked);
    excludeSources = false;
    directedRelationships = false;
  } else {
    sepTwoSelector->setCurrentIndex(sepTwoSelector->findText(tr("-Select a second delimiter-")));
    sepTwoSelector->setEnabled(false);
    saveNodes->setEnabled(false);
    saveEdges->setEnabled(false);
    sourceSelector->setEnabled(false);
    targetSelector->setEnabled(false);
    setPropertiesButton->setEnabled(false);
    excludeSourcesCheckBox->setEnabled(false);
    excludeSourcesCheckBox->setCheckState(Qt::Unchecked);
    relationsDirectedCheckBox->setEnabled(false);
    relationsUndirectedCheckBox->setEnabled(false);
    relationsDirectedCheckBox->setCheckState(Qt::Unchecked);
    relationsUndirectedCheckBox->setCheckState(Qt::Checked);
    excludeSources = false;
    directedRelationships = false;
  }
}

// This function sets the multi-value delimiter to what was chosen by the user.
// We also reset other features of the interface for transparency reasons.
void MainDialog::setSepTwo(const QString &selection) {
  sepTwo = selection;
  saveNodes->setEnabled(false);
  saveEdges->setEnabled(false);
  sourceSelector->setEnabled(false);
  targetSelector->setEnabled(false);
  setPropertiesButton->setEnabled(false);
  excludeSourcesCheckBox->setEnabled(false);
  excludeSourcesCheckBox->setCheckState(Qt::Unchecked);
  relationsDirectedCheckBox->setEnabled(false);
  relationsUndirectedCheckBox->setEnabled(false);
  relationsDirectedCheckBox->setCheckState(Qt::Unchecked);
  relationsUndirectedCheckBox->setCheckState(Qt::Checked);
  excludeSources = false;
  directedRelationships = false;
}

/* This function first check whether a second type of delimiter is set.
   Depending one the outcome of that check, one of the signals for importing files
   is fired, which will then lead to a request to the instantiated InputTable Class
   to start importing the file.
*/
void MainDialog::fireFileSend()
{
  emit sendFile(fileName, sepOne);
}

// This function enables the part of the interface where the nodes and edges configuration is set.
// It also retrieves the possible options for source and target nodes from the inputTable. 
void MainDialog::enableVariables() {
  sourceSelector->clear();
  targetSelector->clear();
  sourceSelector->addItem(tr("-Select Source Node-"));
  targetSelector->addItem(tr("-Select Target Node-"));
  // inputTable creates a separate vector for the header of the input file. We can use this vector
  // to identify the names of the variables that are candidates for the source node, the target node, and properties.
  std::vector<std::string> optionLabels = inputTable->GetHeader(); 
  std::vector<std::string>::iterator it;
  for(it = optionLabels.begin(); it != optionLabels.end(); it++) {
    QString currentLabel = QString::fromUtf8(it->c_str());
    sourceSelector->addItem(currentLabel);
    targetSelector->addItem(currentLabel);
  }
  sourceSelector->setEnabled(true);
  targetSelector->setEnabled(true);
}

// This function processes the selection that the user makes for a source node.
// It also checks for some possible illegal combinations of source and target, and (de)activates parts of the interface accordingly.
void MainDialog::setSourceSelection(const QString &selection) {
  sourceSelection = selection;
  if (sourceSelection != tr("-Select Source Node-") && targetSelection != tr("-Select Target Node-") && sourceSelection != targetSelection) {
    setPropertiesButton->setEnabled(true);
    relationsDirectedCheckBox->setEnabled(true);
    relationsUndirectedCheckBox->setEnabled(true);
    saveNodes->setEnabled(true);
    saveEdges->setEnabled(true);
  } else {
    setPropertiesButton->setEnabled(false);
    relationsDirectedCheckBox->setEnabled(false);
    relationsUndirectedCheckBox->setEnabled(false);
    saveNodes->setEnabled(false);
    saveEdges->setEnabled(false);

  }
  // If properties were already assigned to a previous source node, we should make sure that they are cleared for the new source node.
  // See below and the properties dialog cpp file to understand how properties are assigned.
  sourceProperties.clear();
}

// This function processes the selection that the user makes for a target node.
// It also checks for some possible illegal combinations of source and target, and (de)activates parts of the interface accordingly.
void MainDialog::setTargetSelection(const QString &selection) {
  targetSelection = selection;
  if (sourceSelection != tr("-Select Source Node-") && targetSelection != tr("-Select Target Node-") && sourceSelection != targetSelection) {
    setPropertiesButton->setEnabled(true);
    excludeSourcesCheckBox->setEnabled(true);
    relationsDirectedCheckBox->setEnabled(true);
    relationsUndirectedCheckBox->setEnabled(true);
    // If an appropriate selection of source and target nodes is made, then it becomes possible to save the nodes and edges lists.
    saveNodes->setEnabled(true);
    saveEdges->setEnabled(true);
  } else {
    setPropertiesButton->setEnabled(false);
    excludeSourcesCheckBox->setEnabled(false);
    relationsDirectedCheckBox->setEnabled(false);
    relationsUndirectedCheckBox->setEnabled(false);
    saveNodes->setEnabled(false);
    saveEdges->setEnabled(false);
  }
  // If properties were already assigned to a previous target node, we should make sure that they are cleared for the new target node.
  // See below and the properties dialog cpp file to understand how properties are assigned.
  targetProperties.clear();
}

/*
  This function first creates a new instance of the properties dialog, sets it up, and then shows it to the user.
  The dialog is deleted on closure, so a new one is created everytime the user clicks the button to open the dialog.
  The dialog will allow the user to assign properties to the source node and the target node, and then the properties dialog
  will tell the main dialog what the properties are, so that this information is retained even after the properties dialog is deleted.
*/
void MainDialog::openPropertiesDialog() {
  std::vector<std::string> tempHeader = inputTable->GetHeader();
  QVector<QString> tempQHeader;
  std::vector<std::string>::iterator it;
  for (it = tempHeader.begin(); it != tempHeader.end(); it++) {
    QString currentEntry = QString::fromUtf8(it->c_str());
    tempQHeader.push_back(currentEntry);
  }
  propertiesDialog = new PropertiesDialog(this, tempQHeader, sourceSelection, targetSelection);
  propertiesDialog->setAttribute(Qt::WA_DeleteOnClose);
  connect(propertiesDialog, SIGNAL(propertiesCloseWith(const QVector<QString> &, const QVector<QString> &)), this, SLOT(setProperties(const QVector<QString> &, const QVector<QString> &)));
  propertiesDialog->exec();
}

// As described above, the properties dialog tells the main dialog which properties the user has assigned to the source and target nodes.
// This function is called to process the selection of properties.
void MainDialog::setProperties(const QVector<QString> &sourceProps, const QVector<QString> &targetProps) {
 QVectorIterator<QString> sIt(sourceProps);
  while (sIt.hasNext()) {
    QString QcurrentProperty = sIt.next();
    std::string currentProperty = QcurrentProperty.toUtf8().constData();
    sourceProperties.push_back(currentProperty);
  }
  QVectorIterator<QString> tIt(targetProps);
  while (tIt.hasNext()) {
    QString QcurrentProperty = tIt.next();
    std::string currentProperty = QcurrentProperty.toUtf8().constData();
    targetProperties.push_back(currentProperty);
  }
}

// This function sets the type of edge relationships, based on the user's selection.
void MainDialog::setRelationshipType() {
  if (relationsGroup->checkedId() == 1) {
    directedRelationships = true;
  } else {
    directedRelationships = false;
  }
}

// This function triggers another function in the CsvOutput header that writes an edges file.
void MainDialog::saveEdgesFile() {
  // We first the user ask for the desired filename for the new edges file.
  QString QsaveFile = QFileDialog::getSaveFileName(this, tr("Save File"),"", tr("Comma Delimited Files (*.csv *.txt)"));
  std::string saveFile = QsaveFile.toStdString();
  std::string stdSepOne = sepOne.toStdString();
  std::string stdSepTwo = sepTwo.toStdString();
  // After preparing some necessary information, we call the CsvOutputEdges() function.
  CsvOutputEdges(inputTable, sourceSelection, targetSelection, directedRelationships, saveFile, stdSepOne, stdSepTwo);
}

// This function processes the choice of the user to exclude the source node (or not) in the nodes list.
void MainDialog::setExcludeSources(const int &state) {
  if(state == Qt::Checked) {
    excludeSources = true;
  } else {
    excludeSources = false;
  }
}

// This function triggers another function in the CsvOutput header that writes an nodes file.
void MainDialog::saveNodesFile() {
  // We first the user ask for the desired filename for the new nodes file.
  QString QsaveFile = QFileDialog::getSaveFileName(this, tr("Save File"),"", tr("Comma Delimited Files (*.csv *.txt)"));
  std::string saveFile = QsaveFile.toStdString();
  std::string stdSepOne = sepOne.toStdString();
  std::string stdSepTwo = sepTwo.toStdString();
  // After preparing some necessary information, we call the CsvOutputNodes() function.
  CsvOutputNodes(inputTable, sourceSelection, targetSelection, sourceProperties, targetProperties, excludeSources, saveFile, stdSepOne, stdSepTwo);
}

// This function make sure that the memory used by the instantiated InputTable class is freed up again. 
void MainDialog::closing() {
    delete inputTable;
}

// This function will reset some options to prevent unexpected results if the user
// opens another file.
void MainDialog::resetFileImport() {
  saveNodes->setEnabled(false);
  saveEdges->setEnabled(false);
  sepTwoSelector->setEnabled(false);
  sepTwoSelector->setCurrentIndex(sepTwoSelector->findText(tr("-Select a second delimiter-")));
  sepTwoSwitcher->setCheckState(Qt::Unchecked);
  sourceSelector->setEnabled(false);
  targetSelector->setEnabled(false);
  setPropertiesButton->setEnabled(false);
  excludeSourcesCheckBox->setEnabled(false);
  excludeSourcesCheckBox->setCheckState(Qt::Unchecked);
  relationsDirectedCheckBox->setEnabled(false);
  relationsUndirectedCheckBox->setEnabled(false);
  relationsDirectedCheckBox->setCheckState(Qt::Unchecked);
  relationsUndirectedCheckBox->setCheckState(Qt::Checked);
  excludeSources = false;
  directedRelationships = false;
}




