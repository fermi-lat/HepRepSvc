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
  /// This method return the Event ID as a pair Run/Event
  virtual std::string getEventId();  
  /// This method set the Event index 
  virtual bool setEventIndex(int index);
  /// Set the property of an algorithm to a given value
  virtual bool setAlgProperty(std::string algName, std::string propName, std::string propValue);  
  /// This method replay a given algorithm
  virtual bool replayAlgorithm(std::string algName);

 private:
  HepRepSvc* m_hrsvc;
};

#endif //SVCADAPTER_H
