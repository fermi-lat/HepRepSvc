#ifndef TKRRECONFILLER_H
#define TKRRECONFILLER_H
#include <vector>
#include <string>

#include "HepRepSvc/IFiller.h"

class IGlastDetSvc;
class ITkrGeometrySvc;
class IDataProviderSvc;
class IParticlePropertySvc;
class HepRepInitSvc;

/** 
 *  @class TkrReconFiller
 *
 *  @brief 
 *
 *  @author R.Giannitrapani
 */

class TkrReconFiller: public IFiller{
  
 public:
  TkrReconFiller(HepRepInitSvc* hrisvc,
                 IGlastDetSvc* gsvc,
                 ITkrGeometrySvc* tgsvc,
                 IDataProviderSvc* dpsvc,
                 IParticlePropertySvc* ppsvc);

  virtual ~TkrReconFiller();
  
  /// This method init the type tree
  virtual void buildTypes ();
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);
  /// This method to initialize the builder for the sub fillers
  virtual void setBuilder(IBuilder* b);

  bool hasType(std::vector<std::string>& list, std::string type); 

 private:
  HepRepInitSvc* m_hrisvc;
  IGlastDetSvc* m_gdsvc;
  ITkrGeometrySvc* m_tgsvc;
  IDataProviderSvc* m_dpsvc;
  IParticlePropertySvc* m_ppsvc;


  std::vector<IFiller*> fillVector;

};

#endif //TKRRECONFILLER_H
