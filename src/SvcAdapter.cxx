#include "SvcAdapter.h"
#include "HepRepSvc.h"
#include "GaudiKernel/IAppMgrUI.h"
 
void SvcAdapter::nextEvent(int i)
{
  m_hrsvc->getAppMgrUI()->nextEvent(i);
}

std::vector<std::string>& SvcAdapter::getSources()
{
  std::vector<std::string> res;

  return res;
}

void SvcAdapter::setSource(std::string)
{

}
  
