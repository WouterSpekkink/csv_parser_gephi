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

PropertiesDialog::PropertiesDialog(QWidget *parent) : QDialog(parent) {}

PropertiesDialog::PropertiesDialog(QWidget *parent, const QVector<QString> Qheader, const QString sourceInput, const QString targetInput) {
  sourceLabel = new QLabel(tr("Source"));
  targetLabel = new QLabel(tr("Target"));

  saveCloseButton = new QPushButton(tr("Save and Close"));
  cancelButton = new QPushButton(tr("Cancel"));

  connect(saveCloseButton, SIGNAL(clicked()), this, SLOT(saveAndClose()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
  
  // Creating two lists of check boxes.
  QVectorIterator<QString> it(Qheader);
  while (it.hasNext()) {
    QCheckBox *tempBoxOne = new QCheckBox(it.peekNext(), this);
    QCheckBox *tempBoxTwo = new QCheckBox(it.next(), this);

    if (tempBoxOne->text() != sourceInput && tempBoxOne->text() != targetInput) {
      sourceVector.push_back(tempBoxOne);
    } else {
      delete tempBoxOne;
    }
    if (tempBoxTwo->text() != targetInput && tempBoxTwo->text() != sourceInput) {
      targetVector.push_back(tempBoxTwo);
    } else {
      delete tempBoxTwo;
    }
  }

  QVBoxLayout *mainLayout = new QVBoxLayout;
  QHBoxLayout *mainBodyLayout = new QHBoxLayout;
  QVBoxLayout *mainBodyLeft = new QVBoxLayout;
  QVBoxLayout *mainBodyRight = new QVBoxLayout;

  mainBodyLeft->addWidget(sourceLabel);
  QVectorIterator<QCheckBox*> sI(sourceVector);
  while (sI.hasNext()) {
    mainBodyLeft->addWidget(sI.next());
  }
  
  mainBodyRight->addWidget(targetLabel);
  QVectorIterator<QCheckBox*> tI(targetVector);
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
  this->close();
}

void PropertiesDialog::saveAndClose() {
  
  QVectorIterator<QCheckBox*> spI(sourceVector);
  spI.toFront();
  while (spI.hasNext()) {
    QCheckBox *tempBox = spI.next();
    if (tempBox->checkState() == Qt::Checked) {
      sourceProperties.push_back(tempBox->text());
    }
  }

  QVectorIterator<QCheckBox*> tpI(targetVector);
  tpI.toFront();
  while (tpI.hasNext()) {
    QCheckBox *tempBox = tpI.next();
    if (tempBox->checkState() == Qt::Checked) {
      targetProperties.push_back(tempBox->text());
     }
   }

  emit propertiesCloseWith(sourceProperties, targetProperties);
  qDeleteAll(sourceVector);
  sourceVector.clear();
  qDeleteAll(targetVector);
  targetVector.clear();

  this->close();
}
