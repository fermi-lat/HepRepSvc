#ifndef MONTECARLOFILLER_H
#define MONTECARLOFILLER_H
#include <vector>
#include <string>

#include "HepRepSvc/IFiller.h"

#include "geometry/Point.h"

#include "Event/MonteCarlo/McRelTableDefs.h"


class IGlastDetSvc;
class IDataProviderSvc;
class IParticlePropertySvc;
class HepRepInitSvc;

namespace Event {
    class McParticle;
    class McPositionHit;
    class McIntegratingHit;
}

/** 
 *  @class MonteCarloFiller
 *
 *  @brief The filler for the Montecarlo data
 *
 *  This filler is used to populate the HepRep event with the MonteCarlo tree.
 *
 *  @author R.Giannitrapani, M.Frailis
 */

class MonteCarloFiller: public IFiller{
  
 public:
  MonteCarloFiller(HepRepInitSvc* hrisvc,
                   IGlastDetSvc* gsvc,
                   IDataProviderSvc* dpsvc,
                   IParticlePropertySvc* ppsvc):
    m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc){};

  /// This method init the type tree
  virtual void buildTypes ();

	/// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

	/// A support method to add electrical charge attributes
	void MonteCarloFiller::setCharge(int charge);

	/// A method that return true if list contain type
	/// TODO STL already has this
  bool hasType(std::vector<std::string>& list, std::string type); 

  std::string getTripleString(int precis, double x, double y, double z);
  std::string getBits(unsigned int statBits, int highBit, int lowBit);
  std::string getPositionString(const Point& position);
  std::string getDirectionString(const Vector& position);


 private:
  void fillMcPositionHit(std::vector<std::string>& typesList, Event::McPositionHit* hit);
  void fillMcIntegratingHit(std::vector<std::string>& typesList, Event::McIntegratingHit* hit);
  void fillMcParticle(std::vector<std::string>&     typesList,
                      std::string                   father,
                      const Event::McParticle*      mcPart,
                      Event::McPartToTrajectoryTab& partToTrajTab,
                      Event::McPointToPosHitTab&    mcPosHitTab,
                      Event::McPointToIntHitTab&    mcIntHitTab);

  HepRepInitSvc* m_hrisvc;
  IGlastDetSvc* m_gdsvc;
  IDataProviderSvc* m_dpsvc;
  IParticlePropertySvc* m_ppsvc;

};

#endif //MONTECARLOFILLER_H
