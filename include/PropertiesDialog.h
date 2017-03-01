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
  See propertiesDialog.cpp for more details on what this class and its methods do.  

*/

#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QtGui>
//nclude "../include/DataInterface.h"

class PropertiesDialog : public QDialog {
  
  Q_OBJECT

public:
  PropertiesDialog(QWidget *parent);
  
  PropertiesDialog(QWidget *parent, QVector<QString> = QVector<QString>(), QString source = QString(), QString target = QString());
    
  ~PropertiesDialog() {};

signals:
  // One signal for the cancel button.
  // One signal for the save and close button.
  void propertiesCloseWithout();		       
  void propertiesCloseWith(const QVector<QString> sourceProps, const QVector<QString> targetProps);
			
private slots:

  void cancel();
  void saveAndClose();
      
private:
  QPointer<QLabel> sourceLabel;
  QPointer<QLabel> targetLabel;
  QVector<QPointer<QCheckBox> > sourceVector;
  QVector<QPointer<QCheckBox> > targetVector;    
  QPointer<QPushButton> saveCloseButton;
  QPointer<QPushButton> cancelButton;
  
  QVector<QString> sourceProperties;
  QVector<QString> targetProperties;
  QString sourceNode;
  QString targetNode;

};
  


#endif
