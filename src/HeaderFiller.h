#ifndef HEADERFILLER_H
#define HEADERFILLER_H
#include <vector>
#include <string>

#include "Filler.h"

class IDataProviderSvc;
class HepRepInitSvc;
class IRootIoSvc;

/** 
 *  @class HeaderFiller
 *
 *  @brief The filler for the Event Header
 *
 *  This filler is used to populate the HepRep event with info retrived from the 
 *  EventHeader.
 *
 *  @author R.Giannitrapani
 */

class HeaderFiller: public Filler{
  
 public:
  HeaderFiller(HepRepInitSvc* /*hrisvc*/,
               IDataProviderSvc* dpsvc,
               IRootIoSvc* rootIoSvc ):
    m_dpsvc(dpsvc), m_rootIoSvc(rootIoSvc) {};

  /// This method init the type tree
  virtual void buildTypes ();

	/// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

	/// A method that return true if list contain type
	/// TODO STL already has this
 private:
  /// This is the list of subfillers for the event (MC and Recon)
  std::vector<IFiller*> m_subFillers;

  HepRepInitSvc* m_hrisvc;
  /// A pointer to the DataService
  IDataProviderSvc* m_dpsvc;
  /// root svc
  IRootIoSvc* m_rootIoSvc;

};

#endif //HEADERFILLER_H
