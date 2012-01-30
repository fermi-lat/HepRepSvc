/** @file GuiObs.cxx 
   @brief Implementation file for GuiSvc

gets adresses 
 of CLHEP random number engines used in Gleam shared libraries
 and sets seeds for them based on run and particle sequence
 number obtained from the MCHeader

 $Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/src/HepRepObs.cxx,v 1.2 2011/12/12 20:51:40 heather Exp $

 Author: Toby Burnett, Karl Young
*/

#include "HepRepObs.h"
#include "facilities/Util.h"

#include <iterator>
#include <fstream>
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataStoreItem.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/SmartDataPtr.h"


HepRepObs::HepRepObs():IToolSvc::Observer(),m_guiMgr(0)
{
   
}


void HepRepObs::onCreate(IAlgTool& tool) {


    IGuiTool* gtool;
    StatusCode status =tool.queryInterface( IGuiTool::interfaceID(), (void**)&gtool);
    if( status.isSuccess() ){
        gtool->initialize(m_guiMgr);
    }


}


