// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR 
 * THIS OSMC PUBLIC LICENSE (OSMC-PL). 
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
 * OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE. 
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or  
 * http://www.openmodelica.org, and in the OpenModelica distribution. 
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

        @file ModLoader.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 

  */
#include "ModLoader.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMutex>


ModLoader::ModLoader(MOomc *moomc)
{
        _moomc = moomc;
}



void ModLoader::loadMoFile(ModClass* rootClass,QString filePath,QMap<ModModel*,ModModelPlus*> & mapModelPlus, bool forceLoad)
{
	// Read it in moomc
	QFile file(filePath);
	if( !file.open( QIODevice::ReadOnly ) )
	{
		//InfoSender::instance()->send( Info(ListInfo::FILENOTEXISTS,filePath));
		return ;
	}
	file.close();

	// loading file in moomc
	QString error;
	bool loadOk;

	// Load file
        _moomc->loadModel(filePath,forceLoad,loadOk,error);

        if(rootClass)
            rootClass->setChildrenReaden(false); // force to reread (untested)
     //   QStringList newClasses = _moomc->getClassesOfFile(filePath);

	////Refresh
	//if(refreshAfter)
	//	refresh(rootClass,mapModelPlus);
}

void ModLoader::loadMoFiles(ModClass* rootClass,QStringList filePaths,QMap<ModModel*,ModModelPlus*> & mapModelPlus, bool forceLoad)
{
	for(int i=0;i<filePaths.size();i++)
                loadMoFile(rootClass,filePaths.at(i),mapModelPlus,forceLoad);
	////Refresh
	//if(refreshAfter)
	//	refresh(rootClass,mapModelPlus);
}



int ModLoader::getDepthMax()
{
        int depthMax = MOSettings::value("DepthReadWhileLoadingModModel").toInt();
	if(depthMax==-1)
			depthMax = 100000;
	return depthMax;
}




