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
  /// used to go back to the previous event 
  virtual bool previousEvent(int) = 0;  
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
  /// This method sets the ROOT files for input
  virtual bool openFile(const char* mc, const char* digi, const char* rec, const char* rel, const char* gcr) = 0;
  /// This method return the Event ID as a pair Run/Event
  virtual std::string getEventId() = 0;  
  /// Set the property of an algorithm to a given value
  virtual bool setAlgProperty(std::string, std::string, std::string) = 0;
  /// This method replay a given algorithm
  virtual bool replayAlgorithm(std::string algName) = 0;
  /// This method return a dir with a FRED installation, if set so in the
  /// jobOptions file; otherwise it returns (by default) an empty string
  virtual std::string getStartFred() = 0; 
  /// same, wired
  virtual std::string getStartWired() = 0;

  /// get the number of events in the current file
  virtual long long getNumberOfEvents() = 0;
};

#endif //ISVCADAPTER_H
