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

#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QVector>
#include <QVectorIterator>
#include "../include/InputTable.h"

class PropertiesDialog : public QDialog {
  
  Q_OBJECT

public:
  // PropertiesDialog(QWidget *parent = 0, InputTable *input = new InputTable(), QString source = QString(), QString target = QString());
  
  PropertiesDialog(QWidget *parent = 0);
  
  void setDetails(std::vector<std::string> header, const QString sourceInput, const QString targetInput);
  
  ~PropertiesDialog() {};


signals:
  // One signal for the cancel button.
  // One signal for the save and close button.
  void propertiesCloseWithout();		       
  void propertiesCloseWith(const QVector<QString*> sourceProps, const QVector<QString*> targetProps);
			
private slots:

  void cancel();
  void saveAndClose();
      
private:
  QLabel *sourceLabel;
  QLabel *targetLabel;
  
  QString sourceNode;
  QString targetNode;
  
  //  InputTable *origin;
  
  QVector<QCheckBox*> sourceVector;
  QVector<QCheckBox*> targetVector;    

  QPushButton *saveCloseButton;
  QPushButton *cancelButton;

  QVector<QString*> sourceProperties;
  QVector<QString*> targetProperties;
};
  


#endif
