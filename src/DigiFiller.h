#ifndef DigiFiller_H
#define DigiFiller_H
#include <vector>
#include <string>

#include "Filler.h"

class IGlastDetSvc;
class ITkrGeometrySvc;
class IAcdGeometrySvc;
class IDataProviderSvc;
class IParticlePropertySvc;
class HepRepInitSvc;

/** 
 *  @class DigiFiller
 *
 *  @brief 
 *
 *  @author R.Giannitrapani, M.Frailis
 */

class DigiFiller: public Filler{
  
 public:
  DigiFiller(HepRepInitSvc* hrisvc,
              IGlastDetSvc* gsvc,
              ITkrGeometrySvc* tgsvc,
	     IAcdGeometrySvc* acdsvc,
              IDataProviderSvc* dpsvc);

  virtual ~DigiFiller(){;}
  

  /// This method init the type tree
  virtual void buildTypes ();
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

  /// This has to be overiden
  virtual void setBuilder(IBuilder*);
  
 private:
  std::vector<IFiller*> m_subFillers;

  HepRepInitSvc*    m_hrisvc;
  IGlastDetSvc*     m_gdsvc;
  ITkrGeometrySvc*  m_tgsvc;
  IAcdGeometrySvc*  m_acdsvc;
  IDataProviderSvc* m_dpsvc;
};

#endif //DigiFiller_H
