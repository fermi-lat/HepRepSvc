// $Header:

#ifndef ISVCADAPTER_H
#define ISVCADAPTER_H

#include <list>
#include <string>

/* @class: ISvcAdapter 
 *
 * @brief: The abstract interface to the Gaudi service adapter
 *
 * @author:R.Giannitrapni
 */ 

class ISvcAdapter
{
 public:
  /// used to step to the next event 
  virtual bool nextEvent(int) = 0;
  /// return a list of sources names to be used by FluxSvc
  virtual std::string getSources() = 0;
  /// set the source to be used by FluxSvc
  virtual void setSource(std::string) = 0;
	/// stop the GAUDI job
	virtual void shutDown() = 0;
  /// sent the valid setEvent command accepted by this server
  virtual std::string getCommands() = 0;  
  /// This method set the Event ID to a pair Run/Event
  virtual bool setEventId(int run, int event) =  0;
  /// This method set the Event index
  virtual bool setEventIndex(int index) = 0;

};

#endif //ISVCADAPTER_H
