#include "SvcAdapter.h"
#include "HepRepSvc.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IObjManager.h"
#include "GaudiKernel/IToolFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"

 
bool SvcAdapter::nextEvent(int i)
{
	StatusCode sc; 
	MsgStream log( m_hrsvc->msgSvc(), m_hrsvc->name() );

	IAlgManager* theAlgMgr;
  sc = m_hrsvc->serviceLocator( )->getService( "ApplicationMgr",
        IID_IAlgManager,
        (IInterface*&)theAlgMgr );



	IAlgorithm* theIAlg;
  Algorithm*  theAlgorithm=0;
  IntegerProperty errorProperty("ErrorCount",0);

	sc = theAlgMgr->getAlgorithm( "Top", theIAlg );
	if ( sc.isSuccess( ) ) {
		try{
			theAlgorithm = dynamic_cast<Algorithm*>(theIAlg);
		} catch(...){
            sc = StatusCode::FAILURE;
		}
  }

	if ( sc.isFailure( ) ) {
    log << MSG::WARNING << "Could not find algorithm 'Top'; will not monitor errors" << endreq;
  }

	
	sc = m_hrsvc->getAppMgrUI()->nextEvent(i);

  // the single event may have created a failure. Check the ErrorCount propery of the Top alg.
	if( theAlgorithm !=0) theAlgorithm->getProperty(&errorProperty);
	if( sc.isFailure() || errorProperty.value() > 0){
		sc = StatusCode::FAILURE;
	}

 if (sc.isFailure())
		return false;
	else return true;
}

std::string SvcAdapter::getSources()
{  
  return m_hrsvc->getSources();
}

void SvcAdapter::setSource(std::string source)
{
  m_hrsvc->setSource(source);
}
 

void SvcAdapter::shutDown()
{
  m_hrsvc->getAppMgrUI()->finalize();
//  m_hrsvc->getAppMgrUI()->terminate();
}
