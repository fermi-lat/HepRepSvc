#include "SvcAdapter.h"
#include "HepRepSvc.h"
#include "GaudiKernel/IAppMgrUI.h"
 
void SvcAdapter::nextEvent(int i)
{
  m_hrsvc->getAppMgrUI()->nextEvent(i);
}

std::string SvcAdapter::getSources()
{  
  return m_hrsvc->getSources();
}

void SvcAdapter::setSource(std::string source)
{
  m_hrsvc->setSource(source);
}
  
