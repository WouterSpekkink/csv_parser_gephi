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
    void sendFile(const QString &file, const QString &sep);

private slots:
    void getFile();
    void setSep(const QString &selection);
    // void setNetworkType(const QString &selection);
    // void enableNetwork();
    void fireFileSend();
    // void enableCalc();
    // void calculateNetwork();
    void getDetails();
    // void setGranularity(int value);
    void resetFileImport();
    // void setLowerBound(int value);
    // void setUpperBound(int value);
    // void chooseGranBound(const QString &selection);
    // void findEdges();
    // void enableEdges();
    // void enableSave();
    // void saveEdgesFile();
    void closing();
   
private:
    QLabel *topLabel;
    QLabel *middleLabel;
    QLabel *lowerLabel;
    QString fileName;
    QString sep; 
    QString networkType;
    QPushButton *openFile;
    QComboBox *sepSelector;
    QPushButton *importFile;
    InputTable *inputTable;
    QComboBox *networkSelector;
    QPushButton *exitButton;
  //MatCollection *matCollection;
    QPushButton *goNetwork;
    std::vector<std::string> tableHeader;
    std::vector<std::string> nameList;
    int maxGranularity;  
    int curGranularity;
    QLabel *granLabel;
    QSpinBox *chooseGranularity;
    int lowerBound;
    int upperBound;
    QSlider *chooseLowerBound;
    QSlider *chooseUpperBound;
    QSpinBox *monitorUpper;
    QSpinBox *monitorLower;
    QLabel *lowerMonLabel;
    QLabel *upperMonLabel;
  //EdgeFinder *edgeFinder;
    QPushButton *goEdges;
    QPushButton *saveEdges;
    QPushButton *openMetrics;
    QString selectedAgent;
    QLabel *agentLabel;
    QLabel *currentAgent;
    QPushButton *changeAgent;
    QCheckBox *slide;
  //TwoModeEdges *twoModeEdges;

};

#endif
