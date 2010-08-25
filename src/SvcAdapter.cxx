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

// The method delegate to the HepRepSvc the command to go back of one event, if
// possible
bool SvcAdapter::previousEvent(int i)
{
  return m_hrsvc->previousEvent(i);
}

bool SvcAdapter::setEventId(int run, int event)
{
  return m_hrsvc->setEventId(run, event);
}

bool SvcAdapter::setEventIndex(int index)
{
  return m_hrsvc->setEventIndex(index);
}

bool SvcAdapter::openFile(const char* mc, const char* digi, const char* rec, 
                          const char* relation, const char* gcr)
{
  return m_hrsvc->openFile(mc, digi, rec, relation, gcr);
}

std::string SvcAdapter::getEventId()
{
  return m_hrsvc->getEventId();
}

std::string SvcAdapter::getCommands()
{
  return m_hrsvc->getCommands();
}

std::string SvcAdapter::getSources()
{  
  return m_hrsvc->getSources();
}

void SvcAdapter::setSource(std::string source)
{
  m_hrsvc->setSource(source);
}
 
// Set the property of an algorithm to a given value
bool SvcAdapter::setAlgProperty(std::string algName, std::string propName, std::string propValue)
{
  return m_hrsvc->setAlgProperty(algName, propName, propValue);
}

// This method replay a given algorithm
bool SvcAdapter::replayAlgorithm(std::string algName)
{
  return m_hrsvc->replayAlgorithm(algName);
}

void SvcAdapter::shutDown()
{
  m_hrsvc->getAppMgrUI()->finalize();
}

// Get the optional FRED directory
std::string SvcAdapter::getStartFred()
{
  return m_hrsvc->getStartFred();
} 
// Same for WIRED
std::string SvcAdapter::getStartWired()
{
  return m_hrsvc->getStartWired();
} 

