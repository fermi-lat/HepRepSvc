/** @file HepRepObs.cxx 
   @brief Implementation file for HepRepObs 


   finds instances of IRegister tools

 $Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/src/HepRepObs.cxx,v 1.2 2011/12/12 20:51:40 heather Exp $

*/

#include "HepRepObs.h"
#include "HepRepSvc/IRegister.h"

#include <iterator>
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataStoreItem.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/SmartDataPtr.h"


HepRepObs::HepRepObs():IToolSvc::Observer()
{
   
}


void HepRepObs::onCreate(IAlgTool& tool) {


    IRegister* gtool;
    StatusCode status =tool.queryInterface( IRegister::interfaceID(), (void**)&gtool);
    if( status.isSuccess() ){
        gtool->registerMe(m_hepRepSvc);
    }


}


