#include "MonteCarloFiller.h"
#include "HepRepSvc/IBuilder.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "CLHEP/Vector/LorentzVector.h"

#include "idents/VolumeIdentifier.h"

#include "Event/MonteCarlo/McTrajectory.h"
#include "Event/MonteCarlo/McParticle.h"
#include "Event/MonteCarlo/McPositionHit.h"
#include "Event/MonteCarlo/McIntegratingHit.h"

#include <algorithm>

// This method build the types for the HepRep
void MonteCarloFiller::buildTypes()
{
  m_builder->addType("","MC","Monte Carlo Tree","");

  m_builder->addType("MC","PosHitCol","Position Hits","");

  m_builder->addType("PosHitCol","PosHit","Position Hit","");
  m_builder->addAttDef("E","Energy deposited","Physics","MeV");
  m_builder->addAttValue("DrawAs","Prism","");
  m_builder->addAttValue("Color","red","");

  m_builder->addType("PosHit","PosHitSteps","Position Hit Steps","");
  m_builder->addAttValue("DrawAs","Point","");


  m_builder->addType("MC","IntHitCol","Integrating Hits","");
  m_builder->addType("IntHitCol","IntHit","Integrating Hit","");
  m_builder->addAttDef("E","Energy deposited","Physics","MeV");
  m_builder->addAttValue("DrawAs","Prism","");
  m_builder->addAttValue("Color","blue","");

  m_builder->addType("MC","ParticleCol","The particles collection","");  
  m_builder->addType("ParticleCol","Particle","An mc particle","");
  m_builder->addAttDef("Name","The name of the particle","Physics","");
  m_builder->addAttDef("PDG","The PDG code of the particle","Physics","");
  m_builder->addAttDef("Ei","Initial energy","Physics","MeV");
  m_builder->addAttDef("Eo","Final energy","Physics","MeV");
  m_builder->addAttDef("Proc","Process name","Physics","");
  m_builder->addAttDef("Charge","Electrical Charge (pos, neg, neutral)","Physics","");
  
  m_builder->addAttValue("DrawAs","Line","");

}


// This method fill the instance tree Event/MC with the actual TDS content
void MonteCarloFiller::fillInstances (std::vector<std::string>& typesList)
{
  m_builder->addInstance("","MC");
  
  if (hasType(typesList,"PosHitCol") ||
      hasType(typesList,"PosHitSteps"))
    {
      SmartDataPtr<Event::McPositionHitVector>
        posHits(m_dpsvc, "/Event/MC/PositionHitsCol");
      if(posHits!=0)
        {
          m_builder->addInstance("MC","PosHitCol");
          for(Event::McPositionHitVector::const_iterator ihit=posHits->begin();
              ihit != posHits->end(); ihit++){
            {
              HepTransform3D global;
              idents::VolumeIdentifier id = (*ihit)->volumeID();
              m_gdsvc->getTransform3DByID(id, &global);

              m_builder->addInstance("PosHitCol","PosHit");
              m_builder->addAttValue("E",
                                       (float)(*ihit)->depositedEnergy(),"");

              std::string shape;
              std::vector<double> params;
    
              m_gdsvc->getShapeByID(id, &shape, &params); 

              double dx = params[0]/2;
              double dy = params[1]/2;
              double dz = params[2]/2;
                            
              HepPoint3D temp;
              
              temp = global*HepPoint3D(dx,dy,dz);
              m_builder->addPoint(temp.x(),temp.y(),temp.z());
              temp = global*HepPoint3D(-dx,dy,dz);
              m_builder->addPoint(temp.x(),temp.y(),temp.z());
              temp = global*HepPoint3D(-dx,-dy,dz);
              m_builder->addPoint(temp.x(),temp.y(),temp.z());
              temp = global*HepPoint3D(dx,-dy,dz);
              m_builder->addPoint(temp.x(),temp.y(),temp.z());
              temp = global*HepPoint3D(dx,dy,-dz);
              m_builder->addPoint(temp.x(),temp.y(),temp.z());
              temp = global*HepPoint3D(-dx,dy,-dz);
              m_builder->addPoint(temp.x(),temp.y(),temp.z());
              temp = global*HepPoint3D(-dx,-dy,-dz);
              m_builder->addPoint(temp.x(),temp.y(),temp.z());
              temp = global*HepPoint3D(dx,-dy,-dz);
              m_builder->addPoint(temp.x(),temp.y(),temp.z());

              
              if(hasType(typesList,"PosHitSteps"))
                {
                  m_builder->addInstance("PosHit","PosHitSteps");
                  HepPoint3D origin(0,0,0);

                  HepPoint3D entry = global.getTranslation() + 
                    (global.getRotation()*(*ihit)->entryPoint());
                  HepPoint3D exit = global.getTranslation() + 
                    (global.getRotation()*(*ihit)->exitPoint());
                  
                  m_builder->addPoint(entry.x(),entry.y(),entry.z());
                  m_builder->addPoint(exit.x(),exit.y(),exit.z());
  
                }                  
            }      
        }
    }
		}
  if (hasType(typesList,"IntHitCol") ||
      hasType(typesList,"IntHit")) 
    {
      SmartDataPtr<Event::McIntegratingHitVector>
        intHits(m_dpsvc, "/Event/MC/IntegratingHitsCol");

      if(intHits != 0)
        {
          m_builder->addInstance("MC","IntHitCol");
          for(Event::McIntegratingHitVector::const_iterator inHit=intHits->begin(); 
              inHit != intHits->end(); inHit++) 
					{

            m_builder->addInstance("IntHitCol","IntHit");

            HepTransform3D global;
            idents::VolumeIdentifier id = (*inHit)->volumeID();
            m_gdsvc->getTransform3DByID(id, &global);
    
            std::string shape;
            std::vector<double> params;
            
            m_gdsvc->getShapeByID(id, &shape, &params); 
            
            HepPoint3D origin(0,0,0);
            
            m_builder->addAttValue("E", (float)(*inHit)->totalEnergy(), "");

            double dx = params[0]/2;
            double dy = params[1]/2;
            double dz = params[2]/2;
            
            HepPoint3D temp;
              
            
            temp = global*HepPoint3D(dx,dy,dz);
            m_builder->addPoint(temp.x(),temp.y(),temp.z());
            temp = global*HepPoint3D(-dx,dy,dz);
            m_builder->addPoint(temp.x(),temp.y(),temp.z());
            temp = global*HepPoint3D(-dx,-dy,dz);
            m_builder->addPoint(temp.x(),temp.y(),temp.z());
            temp = global*HepPoint3D(dx,-dy,dz);
            m_builder->addPoint(temp.x(),temp.y(),temp.z());
            temp = global*HepPoint3D(dx,dy,-dz);
            m_builder->addPoint(temp.x(),temp.y(),temp.z());
            temp = global*HepPoint3D(-dx,dy,-dz);
            m_builder->addPoint(temp.x(),temp.y(),temp.z());
            temp = global*HepPoint3D(-dx,-dy,-dz);
            m_builder->addPoint(temp.x(),temp.y(),temp.z());
            temp = global*HepPoint3D(dx,-dy,-dz);
            m_builder->addPoint(temp.x(),temp.y(),temp.z());
          }
        }
    }

  // TODO lot of duplicated code here; needs to refactor
  if (hasType(typesList,"Particle"))
    {      
      m_builder->addInstance("MC","ParticleCol");      
      // If there are trajectories in the TDS, we use them
      SmartDataPtr<Event::McTrajectoryCol> 
        mcTraj(m_dpsvc, "/Event/MC/TrajectoryCol");
      if (mcTraj !=0)
        {
          m_builder->setSubinstancesNumber("ParticleCol", mcTraj->size());
          for(Event::McTrajectoryCol::const_iterator traj=mcTraj->begin(); 
              traj != mcTraj->end(); traj++) {
            {
              Event::McParticle* part = (*traj)->getMcParticle();
	            m_builder->addInstance("ParticleCol","Particle");
                
              if (part)
                {                  
                  Event::McParticle::StdHepId hepid= part->particleProperty();
                  ParticleProperty* ppty = m_ppsvc->findByStdHepID( hepid );
                  std::string name = ppty->particle(); 
                  
                  HepLorentzVector in = part->initialFourMomentum();
                  HepLorentzVector out = part->finalFourMomentum();
                  
                  m_builder->addAttValue("Ei",(float)in.e()-(float)in.m(),"");
                  m_builder->addAttValue("Eo",(float)out.e()-(float)out.m(),"");
                  
                  m_builder->addAttValue("Proc",part->getProcess(),"");
                  
                  m_builder->addAttValue("PDG",hepid,"");
                  m_builder->addAttValue("Name",name,"");

									setCharge(ppty->charge());
                }
              else                  
								setCharge((*traj)->getCharge());               
							
							std::vector<Hep3Vector> points = (*traj)->getPoints();
							std::vector<Hep3Vector>::const_iterator pit;
              
							for(pit = points.begin(); pit != points.end(); pit++) 
							{                
								m_builder->addPoint((*pit).x(),(*pit).y(),(*pit).z());
							}      
	         }		        
				}
			}
      else // otherwise we use the McParticle 
        {
          SmartDataPtr<Event::McParticleCol> 
            mcPart(m_dpsvc, "/Event/MC/McParticleCol");
      
          if (mcPart !=0)
            {
              m_builder->setSubinstancesNumber("ParticleCol", mcPart->size());
              for(Event::McParticleCol::const_iterator part=mcPart->begin()++; 
                 part != mcPart->end(); part++) {
                {
                  m_builder->addInstance("ParticleCol","Particle");
                  
                  Event::McParticle::StdHepId hepid= (*part)->particleProperty();
                  ParticleProperty* ppty = m_ppsvc->findByStdHepID( hepid );
                  std::string name = ppty->particle(); 
                  
                  HepLorentzVector in = (*part)->initialFourMomentum();
                  HepLorentzVector out = (*part)->finalFourMomentum();
                  
                  m_builder->addAttValue("Ei",(float)in.e()-(float)in.m(),"");
                  m_builder->addAttValue("Eo",(float)out.e()-(float)out.m(),"");

                  m_builder->addAttValue("Proc",(*part)->getProcess(),"");
              
                  m_builder->addAttValue("PDG",hepid,"");
                  m_builder->addAttValue("Name",name,"");
                  
                  
                  HepPoint3D start = (*part)->initialPosition();
                  HepPoint3D end = (*part)->finalPosition();

									setCharge(ppty->charge());
 
                  m_builder->addPoint(start.x(),start.y(),start.z());
                  m_builder->addPoint(end.x(),end.y(),end.z());
                }      
              }
            }
        }
    }  
}

// A support method to add electrical charge attributes
void MonteCarloFiller::setCharge(int charge)
{
	if (charge>0)	
	{
		m_builder->addAttValue("Charge","pos","");
		m_builder->addAttValue("Color","green","");
	}
	else if (charge<0)	
	{	
		m_builder->addAttValue("Charge","neg","");							
		m_builder->addAttValue("Color","red","");		
	}
	else	
	{	
		m_builder->addAttValue("Charge","neutral","");		
		m_builder->addAttValue("Color","white","");		
	}

}

bool MonteCarloFiller::hasType(std::vector<std::string>& list, std::string type) 
{
  if (list.size() == 0) return 1;

  std::vector<std::string>::const_iterator i; 

  i = std::find(list.begin(),list.end(),type);
  if(i == list.end()) return 0;
  else return 1;

}

