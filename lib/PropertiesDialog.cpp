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
  ==NOTES==
  This class is a dialog that allows the user to assign properties to the source
  and target nodes that (s)he selected earlier. It takes information from the main dialog,
  which originates from an instant of the InputTable class used by the main dialog. It then
  dynamically creates lists of checkboxes that the user can use to assign the properties.
*/

#include "../include/PropertiesDialog.h"
#include <vector>
#include <string>
#include <QPointer>

// This is the default constructor, but I actually don't use it.
PropertiesDialog::PropertiesDialog(QWidget *parent) : QDialog(parent) {}

// This is the main constructor.
PropertiesDialog::PropertiesDialog(QWidget *parent, const QVector<QString> Qheader, const QString sourceInput, const QString targetInput) {
  // We first set up the standard elements of the interface, which are only two labels and two buttons.
  sourceLabel = new QLabel(sourceInput);
  targetLabel = new QLabel(targetInput);
  saveCloseButton = new QPushButton(tr("Save and Close"));
  cancelButton = new QPushButton(tr("Cancel"));

  // We dynamically create two lists of checkboxes as well, which represent potential properties.
  // Only variables that were not already selected as source and target nodes are included as options.
  QVectorIterator<QString> it(Qheader);
  while (it.hasNext()) {
    QPointer<QCheckBox> tempBoxOne = new QCheckBox(it.peekNext(), this);
    QPointer<QCheckBox> tempBoxTwo = new QCheckBox(it.next(), this);
    if (tempBoxOne->text() != sourceInput && tempBoxOne->text() != targetInput) {
      sourceVector.push_back(tempBoxOne);
    } else {
      delete tempBoxOne;
    }
    if (tempBoxTwo->text() != targetInput && tempBoxTwo->text() != sourceInput) {
      targetVector.push_back(tempBoxTwo);
      if (sourceInput == targetInput) {
	tempBoxTwo->setEnabled(false);
      }
    } else {
      delete tempBoxTwo;
    }
  }
  
  // We then set up the signals. Basically, save and close is the only signal that will send information from the properties dialog
  // to the main dialog.
  connect(saveCloseButton, SIGNAL(clicked()), this, SLOT(saveAndClose()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel())); // In case the user decides not to set properties.
  
  // Then we create our layout. 
  QPointer<QVBoxLayout> mainLayout = new QVBoxLayout;
  QPointer<QHBoxLayout> mainBodyLayout = new QHBoxLayout;
  QPointer<QVBoxLayout> mainBodyLeft = new QVBoxLayout;
  QPointer<QVBoxLayout> mainBodyRight = new QVBoxLayout;

  mainBodyLeft->addWidget(sourceLabel);
  QVectorIterator<QPointer<QCheckBox>> sI(sourceVector);
  while (sI.hasNext()) {
    mainBodyLeft->addWidget(sI.next());
  }
  
  mainBodyRight->addWidget(targetLabel);
  QVectorIterator<QPointer<QCheckBox>> tI(targetVector);
  while (tI.hasNext()) {
    mainBodyRight->addWidget(tI.next());
  }
  
  mainBodyLayout->addLayout(mainBodyLeft);
  mainBodyLayout->addLayout(mainBodyRight);
  mainLayout->addLayout(mainBodyLayout);
  QPointer<QHBoxLayout> lowerLayout = new QHBoxLayout;
  lowerLayout->addWidget(saveCloseButton);
  lowerLayout->addWidget(cancelButton);
  mainLayout->addLayout(lowerLayout);

  setLayout(mainLayout);
  setWindowTitle(tr("Set Properties"));
  setFixedHeight(sizeHint().height());
  // And that finishes our constructor
}

// This is the function that is called if the user clicks the cancel button.
// We basically just clear our vectors of pointers and then exit. We also let the main dialog know that nothing was saved.
void PropertiesDialog::cancel() {
  qDeleteAll(sourceVector);
  sourceVector.clear();
  qDeleteAll(targetVector);
  targetVector.clear();
  emit propertiesCloseWithout();
  this->close();
}

/*
  This is the function that is called if the user clicks the save and exit button.
  We first process the selected properties, put them in vectors, and send them to the main dialog.
  We then clear the vectors of pointers and exit.
*/
void PropertiesDialog::saveAndClose() {
  QVectorIterator<QPointer<QCheckBox>> spI(sourceVector);
  spI.toFront();
  while (spI.hasNext()) {
    QPointer<QCheckBox> tempBox = spI.next();
    if (tempBox->checkState() == Qt::Checked) {
      sourceProperties.push_back(tempBox->text());
    }
  }
  QVectorIterator<QPointer<QCheckBox>> tpI(targetVector);
  tpI.toFront();
  while (tpI.hasNext()) {
    QPointer<QCheckBox> tempBox = tpI.next();
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
