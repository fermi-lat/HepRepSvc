#ifndef ACDRECONFILLER_H
#define ACDRECONFILLER_H
#include <vector>
#include <string>

#include "HepRepSvc/IFiller.h"

class IGlastDetSvc;
class IDataProviderSvc;
class IParticlePropertySvc;

/** 
 *  @class AcdReconFiller
 *
 *  @brief This is the HepRep fillers for all the ACD relevant
 *  objects (recond and digi). This is a "bookmark" filler waiting for some real
 *  ACD expert to help in developing it
 *
 *  @author R.Giannitrapani
 */

class AcdReconFiller: public IFiller{
  
 public:
  AcdReconFiller(IGlastDetSvc* gsvc,
                 IDataProviderSvc* dpsvc,
                 IParticlePropertySvc* ppsvc);
  
  /// This method init the type tree
  virtual void buildTypes ();
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

  bool hasType(std::vector<std::string>& list, std::string type); 

 private:
  IGlastDetSvc* m_gdsvc;
  IDataProviderSvc* m_dpsvc;
  IParticlePropertySvc* m_ppsvc;

};

#endif //CALRECONFILLER_H