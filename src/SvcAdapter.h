// $Header:

#ifndef SVCADAPTER_H
#define SVCADAPTER_H

#include <list>
#include <string>
#include "HepRepSvc/ISvcAdapter.h"

class HepRepSvc;

/* @class: SvcAdapter 
 *
 * @brief: The HepRepSvc service adapter
 *
 * @author:R.Giannitrapni
 */ 
class SvcAdapter: public ISvcAdapter
{
 public:
  SvcAdapter(HepRepSvc* h):m_hrsvc(h){};
  virtual bool nextEvent(int);
  std::string getSources();
  virtual void setSource(std::string);
	/// stop the GAUDI job
	virtual void shutDown();
 private:
  HepRepSvc* m_hrsvc;
};

#endif //SVCADAPTER_H
