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

#include "../include/PropertiesDialog.h"
#include <vector>
#include <string>

//PropertiesDialog::PropertiesDialog(QWidget *parent, InputTable *input, QString source, QString target) : QDialog(parent), origin(input), sourceNode(source), targetNode(target) {

PropertiesDialog::PropertiesDialog(QWidget *parent) : QDialog(parent) {
  // There is a lot of stuff that won't work if I call them during construction.
  // Therefore I wrote the SetDetails instruction instead, which can be called later. 
}

PropertiesDialog::setDetails(std::vector<std::string> header, const QString sourceInput, const QString targetInput) {
  sourceLabel = new QLabel(tr("Source"));
  targetLabel = new QLabel(tr("Target"));

  saveCloseButton = new QPushButton(tr("Save and Close"));
  cancelButton = new QPushButton(tr("Cancel"));

  std::vector<std::string>::iterator it;

  // Creating two lists of check boxes.

  for (it = header.begin(); it != header.end(); it++, index++) {
    QString currentLabel = QString::fromUtf8(it->c_str());
    QCheckBox *tempBox = new QCheckBox(currentLabel, this);
      sourceVector.push_back(tempBox);
      targetVector.push_back(tempBox);
  }

  QVBoxLayout *mainlayout = new QVBoxLayout;
  QHBoxLayout *mainBodyLayout = new QHBoxLayout;
  QVBoxLayout *mainBodyLeft = new QVBoxLayout;
  QVBoxLayout *mainBodyRight = new QVBoxLayout;

  mainBodyLeft->addWidget(sourceLabel);
  QVectorIterator<QCheckBox*> sI(sourceVector);
  while (sI.hasNext()) {
    mainBodyLeft->addWidget(sI.next());
  }
  
  mainBodyRight->addWidget(targetLabel);
  QVectorIterator<QCheckBox*> sI(targetVector);
  while (tI.hasNext()) {
    mainBodyRight->addWidget(tI.next());
  }
  mainBodyLayout->addLayout(mainBodyLeft);
  mainBodyLayout->addLayout(mainBodyRight);
  mainLayout->addLayout(mainBodyLayout);
  QHBoxLayout *lowerLayout = new QHBoxLayout;
  lowerLayout->addWidget(saveCloseButton);
  lowerLayout->addWidget(cancelButton);
  mainLayout->addLayout(lowerLayout);

  setLayout(mainLayout);
  setWindowTitle("Set Properties");
  setFixedHeight(sizeHint().height());
}

void PropertiesDialog::cancel() {
  qDeleteAll(sourceVector);
  sourceVector.clear();
  qDeleteAll(targetVector);
  targetVector.clear();

  // I should find a way to give some stuff back to the main dialog.
  // Maybe just two vectors.

  emit propertiesCloseWithout();
}

void PropertiesDialog::saveAndClose() {
  qDeleteAll(sourceVector);
  sourceVector.clear();
  qDeleteAll(targetVector);
  targetVector.clear();

  QVectorIterator<QCheckBox*> spI(sourceVector);
  while (spI.hasNext()) {
    if (spI->peekNext()->CheckState() == Qt::Checked) {
      sourceProperties->push_back(spI->next()->text());
    } else {
      spI->next();
    }
  }

  QVectorIterator<QCheckBox*> tpI(targetVector);
  while (tpI.hasNext()) {
    if (tpI->peekNext()->CheckState() == Qt::Checked) {
      targetProperties->push_back(tpI->next()->text());
    } else {
      tpI->next();
    }
    
  }
  
  emit propertiesCloseWith(sourceProperties, targetProperties);
}
