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
  virtual std::string getSources();
  virtual void setSource(std::string);
	/// stop the GAUDI job
	virtual void shutDown();
  /// sent the valid setEvent command accepted by this server
  virtual std::string getCommands(); 
  /// This method set the Event ID to a pair Run/Event
  virtual bool setEventId(int run, int event);
 private:
  HepRepSvc* m_hrsvc;
};

#endif //SVCADAPTER_H
