#ifndef CALRECONFILLER_H
#define CALRECONFILLER_H
#include <vector>
#include <string>

#include "HepRepSvc/IFiller.h"

class IGlastDetSvc;
class IDataProviderSvc;
class IParticlePropertySvc;

/** 
 *  @class CalReconFiller
 *
 *  @brief 
 *
 *  @author R.Giannitrapani
 */

class CalReconFiller: public IFiller{
  
 public:
  CalReconFiller(IGlastDetSvc* gsvc,
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

  /// crystal height
  float m_xtalHeight;

  /// Z position of top calorimeter layer
  float m_calZtop;
  
  /// Z position of bottom calorimeter layer
  float m_calZbottom;
};

#endif //CALRECONFILLER_H
