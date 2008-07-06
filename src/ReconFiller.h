#ifndef RECONFILLER_H
#define RECONFILLER_H
#include <vector>
#include <string>

#include "HepRepSvc/IFiller.h"

class IGlastDetSvc;
class ITkrGeometrySvc;
class IDataProviderSvc;
class IParticlePropertySvc;
class CalReconFiller;
class HepRepInitSvc;

/** 
 *  @class ReconFiller
 *
 *  @brief 
 *
 *  @author R.Giannitrapani, M.Frailis
 */

class ReconFiller: public IFiller{
  
 public:
  ReconFiller(HepRepInitSvc* hrisvc,
              IGlastDetSvc* gsvc,
              ITkrGeometrySvc* tgsvc,
              IDataProviderSvc* dpsvc,
              IParticlePropertySvc* ppsvc);
  
  /// This method init the type tree
  virtual void buildTypes ();
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

  /// This has to be overiden
  virtual void setBuilder(IBuilder*);
  
  bool hasType(std::vector<std::string>& list, std::string type); 

 private:
  std::vector<IFiller*> m_subFillers;

  HepRepInitSvc* m_hrisvc;
  IGlastDetSvc* m_gdsvc;
  ITkrGeometrySvc* m_tgsvc;
  IDataProviderSvc* m_dpsvc;
  IParticlePropertySvc* m_ppsvc;
};

#endif //RECONFILLER_H
