#include "MonteCarloFiller.h"
#include "HepRepSvc/IBuilder.h"
#include "HepRepSvc/HepRepInitSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Geometry/Transform3D.h"

#include "idents/VolumeIdentifier.h"

#include "Event/MonteCarlo/McTrajectory.h"
#include "Event/MonteCarlo/McParticle.h"
#include "Event/MonteCarlo/McPositionHit.h"
#include "Event/MonteCarlo/McIntegratingHit.h"
#include "Event/MonteCarlo/McRelTableDefs.h"


#include <algorithm>

// This method build the types for the HepRep
void MonteCarloFiller::buildTypes()
{
    if(!m_hrisvc->getMonteCarloFiller_useMcInfo()) return;

    m_builder->addType("","MC","Monte Carlo Tree","");
    m_builder->addAttValue("Layer","Event","");

    m_builder->addType("MC","PosHitCol","Position Hits","");

    m_builder->addType("PosHitCol","PosHit","Position Hit","");
    m_builder->addAttDef("E","Energy deposited","Physics","MeV");
    m_builder->addAttDef("VolId", "Volume Identifier","Geometry","");
    m_builder->addAttDef("Pos", "Global Position Hit","Physics","");
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
    m_builder->addAttDef("Status", "MC Status Bits","Physics","");
    m_builder->addAttDef("Ei","Initial energy","Physics","MeV");
    m_builder->addAttDef("Eo","Final energy","Physics","MeV");
    m_builder->addAttDef("Initial Position","Initial position","Physics","");
    m_builder->addAttDef("Final Position","Final position","Physics","");
    m_builder->addAttDef("Direction","Initial direction","Physics","");
    m_builder->addAttDef("Proc","Process name","Physics","");
    m_builder->addAttDef("Charge","Electrical Charge (pos, neg, neutral)","Physics","");
    m_builder->addAttDef("NumDaughters", "# of Daughteres", "Physics", "");

    m_builder->addAttValue("DrawAs","Line","");

    m_builder->addType("Particle","Daughters","An mc particle","");
    m_builder->addAttDef("Name","The name of the particle","Physics","");
    m_builder->addAttDef("PDG","The PDG code of the particle","Physics","");
    m_builder->addAttDef("Status", "MC Status Bits","Physics","");
    m_builder->addAttDef("Ei","Initial energy","Physics","MeV");
    m_builder->addAttDef("Eo","Final energy","Physics","MeV");
    m_builder->addAttDef("Initial Position","Initial position","Physics","");
    m_builder->addAttDef("Final Position","Final position","Physics","");
    m_builder->addAttDef("Direction","Initial direction","Physics","");
    m_builder->addAttDef("Proc","Process name","Physics","");
    m_builder->addAttDef("Charge","Electrical Charge (pos, neg, neutral)","Physics","");
    m_builder->addAttDef("NumDaughters", "# of Daughteres", "Physics", "");

    m_builder->addAttValue("DrawAs","Line","");

    m_builder->addType("Particle","PosHit","Position Hit","");
    m_builder->addAttDef("E","Energy deposited","Physics","MeV");
    m_builder->addAttDef("VolId", "Volume Identifier","Geometry","");
    m_builder->addAttDef("Pos", "Global Position Hit","Physics","");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttValue("Color","red","");

    m_builder->addType("PosHit","PosHitSteps","Position Hit Steps","");
    m_builder->addAttValue("DrawAs","Point","");

    m_builder->addType("Particle","IntHit","Integrating Hit","");
    m_builder->addAttDef("E","Energy deposited","Physics","MeV");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttValue("Color","blue","");
}


// This method fill the instance tree Event/MC with the actual TDS content
void MonteCarloFiller::fillInstances (std::vector<std::string>& typesList)
{
    
    if(!m_hrisvc->getMonteCarloFiller_useMcInfo()) return;

    // quick fix for FRED crash on real data
    SmartDataPtr<Event::McParticleCol> mcPart(m_dpsvc, "/Event/MC/McParticleCol");
    if(!mcPart) return;

    if (!hasType(typesList,"MC")) 
        return;  

    m_builder->addInstance("","MC");

    // Recover the McPositionHits to trajectory points relational tables
    SmartDataPtr<Event::McPointToPosHitTabList> 
        mcPointToPosHitList(m_dpsvc, "/Event/MC/McPointToPosHit");
    bool fakePointToPosHitList = false;
    if (!mcPointToPosHitList)
    { 
        mcPointToPosHitList   = new Event::RelationList<Event::McTrajectoryPoint, Event::McPositionHit>;
        fakePointToPosHitList = true;
    }
    Event::McPointToPosHitTab mcPosHitTab(mcPointToPosHitList.ptr());

    // Go through and "draw" the McPositionHits which do not have a relation to a trajectory
    if (hasType(typesList,"MC/PosHitCol/PosHit") ||
        hasType(typesList,"MC/PosHitCol/PosHit/PosHitSteps"))
    {
        SmartDataPtr<Event::McPositionHitVector>
            posHits(m_dpsvc, "/Event/MC/PositionHitsCol");
        if(posHits!=0)
        {
            m_builder->addInstance("MC","PosHitCol");
            int numPosHits = 0;
            for(Event::McPositionHitVector::const_iterator ihit=posHits->begin();
                ihit != posHits->end(); ihit++)
            {
                Event::McPointToPosHitVec relVec = mcPosHitTab.getRelBySecond(*ihit);
                // If not related to a trajectory point then is "isolated" and we draw it here
                if (relVec.empty()) numPosHits++;
            }
            m_builder->setSubinstancesNumber("PosHitCol",numPosHits);
            for(Event::McPositionHitVector::const_iterator ihit=posHits->begin();
                ihit != posHits->end(); ihit++)
            {
                Event::McPointToPosHitVec relVec = mcPosHitTab.getRelBySecond(*ihit);

                // If not related to a trajectory point then is "isolated" and we draw it here
                if (relVec.empty())
                {
                    m_builder->addInstance("PosHitCol","PosHit");
                    fillMcPositionHit(typesList, *ihit);
                }
            }
        }
    }

    // Recover the McIntegratingHits to trajectory points relational tables
    SmartDataPtr<Event::McPointToIntHitTabList> 
        mcPointToIntHitList(m_dpsvc, "/Event/MC/McPointToIntHit");
    bool fakePointToIntHitList = false;
    if (!mcPointToIntHitList)
    { 
        mcPointToIntHitList   = new Event::RelationList<Event::McTrajectoryPoint, Event::McIntegratingHit>;
        fakePointToIntHitList = true;
    }
    Event::McPointToIntHitTab mcIntHitTab(mcPointToIntHitList.ptr());

    // Now go through and do the same for the McIntegrating Hits
    if (hasType(typesList,"MC/IntHitCol") ||
        hasType(typesList,"MC/IntHitCol/IntHit")) 
    {
        SmartDataPtr<Event::McIntegratingHitVector>
            intHits(m_dpsvc, "/Event/MC/IntegratingHitsCol");

        if(intHits != 0)
        {
            m_builder->addInstance("MC","IntHitCol");
            int numIntHits = 0;
            for(Event::McIntegratingHitVector::const_iterator inHit=intHits->begin(); 
                inHit != intHits->end(); inHit++) 
            {
                Event::McPointToIntHitVec relVec = mcIntHitTab.getRelBySecond(*inHit);
                // If not related to a trajectory point then is "isolated" and we draw it here
                if (relVec.empty()) numIntHits++;
            }
            m_builder->setSubinstancesNumber("IntHitCol",numIntHits);
            for(Event::McIntegratingHitVector::const_iterator inHit=intHits->begin(); 
                inHit != intHits->end(); inHit++) 
            {
                Event::McPointToIntHitVec relVec = mcIntHitTab.getRelBySecond(*inHit);

                // If not related to a trajectory point then is "isolated" and we draw it here
                if (relVec.empty())
                {
                    m_builder->addInstance("IntHitCol","IntHit");

                    fillMcIntegratingHit(typesList, *inHit);
                }
            }
        }
    }

    // TODO lot of duplicated code here; needs to refactor
    if (hasType(typesList,"MC/ParticleCol/Particle"))
    {      
        if (mcPart !=0 )
        {
            m_builder->addInstance("MC","ParticleCol");
            m_builder->setSubinstancesNumber("ParticleCol", mcPart->size());

            // Get the McParticle <--> McTrajectory relations from TDS 
            SmartDataPtr<Event::McPartToTrajectoryTabList> 
                mcPartToTraj(m_dpsvc, "/Event/MC/McPartToTrajectory");
            bool fakeMcPartToTrajectoryList = false;
            if (!mcPartToTraj)
            {
                mcPartToTraj = new Event::RelationList<Event::McParticle, Event::McTrajectory>;
                fakeMcPartToTrajectoryList = true;
            }
            Event::McPartToTrajectoryTab mcPartToTrajTab(mcPartToTraj.ptr());

            // We want to skip the first McParticle, so initialize here
            Event::McParticleCol::const_iterator part = mcPart->begin();
            if (!((*part)->statusFlags() & Event::McParticle::NOTTRACK)) part++;

//            for( ; part != mcPart->end(); part++)
//            {
//                if (!(*part)->primaryParticle()) break;
                fillMcParticle(typesList, "ParticleCol", *part, mcPartToTrajTab, mcPosHitTab, mcIntHitTab);
//            }
            // Clean up if fake relational table
            if (fakeMcPartToTrajectoryList) delete mcPartToTraj.ptr();
        }
    }

    // Clean up if we were using fake relational tables
    if (fakePointToPosHitList) delete mcPointToPosHitList.ptr();
    if (fakePointToIntHitList) delete mcPointToIntHitList.ptr();

    return;
}

void MonteCarloFiller::fillMcParticle(std::vector<std::string>&     typesList,
                                      std::string                   father,
                                      const Event::McParticle*      mcPart,
                                      Event::McPartToTrajectoryTab& partToTrajTab,
                                      Event::McPointToPosHitTab&    mcPosHitTab,
                                      Event::McPointToIntHitTab&    mcIntHitTab)
{
    if (!(mcPart->statusFlags() & Event::McParticle::NOTTRACK))
    {
        if (father == "ParticleCol") m_builder->addInstance(father,"Particle");
        else                         m_builder->addInstance("Particle","Daughters");

        Event::McParticle::StdHepId hepid= mcPart->particleProperty();
        ParticleProperty* ppty = m_ppsvc->findByStdHepID( hepid );

        if (ppty) m_builder->addAttValue("Name",ppty->particle(),"");
        else      m_builder->addAttValue("Name","Unknown","");

        CLHEP::HepLorentzVector in = mcPart->initialFourMomentum();
        CLHEP::HepLorentzVector out = mcPart->finalFourMomentum();

        Vector inDir(in.x(), in.y(), in.z());
        double mag = inDir.mag();
        if(mag>0.0) inDir /= inDir.mag();

        m_builder->addAttValue("Ei",(float)in.e()-(float)in.m(),"");
        m_builder->addAttValue("Eo",(float)out.e()-(float)out.m(),"");

        HepPoint3D start = mcPart->initialPosition();
        HepPoint3D end   = mcPart->finalPosition();
        Point iniPos(start.x(), start.y(), start.z());
        Point finPos(end.x(), end.y(), end.z());
        m_builder->addAttValue("Initial Position", getPositionString(iniPos),"");
        m_builder->addAttValue("Final Position", getPositionString(finPos),"");
        m_builder->addAttValue("Direction", getDirectionString(inDir),"");

        m_builder->addAttValue("Proc",mcPart->getProcess(),"");
          
        m_builder->addAttValue("PDG", hepid,"");                  

        //Build strings for status bits
        unsigned int statBits = mcPart->statusFlags();
        m_builder->addAttValue("Status Low",getBits(statBits, 15, 0),"");

        if(ppty) setCharge(ppty->charge());
        int numDaughters = (int)mcPart->daughterList().size();
        m_builder->addAttValue("NumDaughters", numDaughters, "");

        // Look up related McTrajectory (if it exists)
        Event::McPartToTrajectoryVec trajVec = partToTrajTab.getRelByFirst(mcPart);

        // In the case the trajectories don't exist (old school read back from root)
        // then all we can do is plot the start and end points
        if (trajVec.empty())
        {
            m_builder->addPoint(start.x(),start.y(),start.z());
            m_builder->addPoint(end.x(),end.y(),end.z());
        }
        // Otherwise plot the full trajectory
        else
        {
            // There is only one association possible here
            const Event::McTrajectory* traj = (*(trajVec.begin()))->getSecond();

            std::vector<Event::McTrajectoryPoint*> points = traj->getPoints();
            std::vector<Event::McTrajectoryPoint*>::const_iterator pit;

            std::map<Event::McIntegratingHit*,const Event::McTrajectory*> intHitMap;
            intHitMap.clear();

            // Loop through to draw the trajectory
            for(pit = points.begin(); pit != points.end(); pit++) 
            {
                const CLHEP::Hep3Vector hit = (*pit)->getPoint();
                m_builder->addPoint(hit.x(),hit.y(),hit.z());

                Event::McPointToIntHitVec relVec = mcIntHitTab.getRelByFirst(*pit);

                if (!relVec.empty())
                {
                    Event::McPointToIntHitRel* hitRel = *(relVec.begin());
                    intHitMap[hitRel->getSecond()] = traj;
                }
            }

            // Loop through to draw associated McPositionHits
            for(pit = points.begin(); pit != points.end(); pit++) 
            {
                Event::McPointToPosHitVec relVec = mcPosHitTab.getRelByFirst(*pit);

                if (!relVec.empty())
                {
                    Event::McPointToPosHitRel* hitRel = *(relVec.begin());

                    m_builder->addInstance("Particle","PosHit");
                    fillMcPositionHit(typesList, hitRel->getSecond());
                }
            }

            // Loop through to draw associated McIntegratingHits
            std::map<Event::McIntegratingHit*,const Event::McTrajectory*>::iterator intHitIter;
            for(intHitIter = intHitMap.begin(); intHitIter != intHitMap.end(); intHitIter++)
            {
                Event::McIntegratingHit* inHit = intHitIter->first;
                
                m_builder->addInstance("Particle","IntHit");
                fillMcIntegratingHit(typesList, inHit);
            }
        }
    }

    // Now draw the daughters of this particle
    const SmartRefVector<Event::McParticle>& daughterList = mcPart->daughterList();
    SmartRefVector<Event::McParticle>::const_iterator daughterIter;

    for(daughterIter = daughterList.begin(); daughterIter != daughterList.end(); daughterIter++)
    {
        const Event::McParticle* mcDaughter = *daughterIter;

        if (mcDaughter) fillMcParticle(typesList, "ParticleCol", mcDaughter, partToTrajTab, mcPosHitTab, mcIntHitTab);
    }

    return;
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

void MonteCarloFiller::fillMcPositionHit(std::vector<std::string>& typesList, Event::McPositionHit* hit)
{
    HepGeom::Transform3D global;
    idents::VolumeIdentifier id = hit->volumeID();
    m_gdsvc->getTransform3DByID(id, &global);

    //m_builder->addInstance("PosHitCol","PosHit");
    m_builder->addAttValue("E",
        (float)hit->depositedEnergy(),"");
    m_builder->addAttValue("VolId", id.name(),"");

    const HepPoint3D pos = hit->globalEntryPoint();
    std::stringstream posStr("");
    posStr << "(" << pos.x() << "," << pos.y() << "," << pos.z() << ")";
    m_builder->addAttValue("Pos", posStr.str(), "");

    if (id[0] == 1) m_builder->addAttValue("Color","yellow","");   

    std::string shape;
    std::vector<double> params;

    m_gdsvc->getShapeByID(id, &shape, &params); 

    double dx = params[0]/2;
    double dy = params[1]/2;
    double dz = params[2]/2;

    drawTransformedPrism(dx, dy, dz, global);

    if(hasType(typesList,"PosHitSteps"))
    {
        m_builder->addInstance("PosHit","PosHitSteps");
        HepPoint3D origin(0,0,0);

        HepPoint3D entry = global.getTranslation() + 
            (global.getRotation()*hit->entryPoint());
        HepPoint3D exit = global.getTranslation() + 
            (global.getRotation()*hit->exitPoint());

        m_builder->addPoint(entry.x(),entry.y(),entry.z());
        m_builder->addPoint(exit.x(),exit.y(),exit.z());

    }                  
}

void MonteCarloFiller::fillMcIntegratingHit(std::vector<std::string>& typesList, Event::McIntegratingHit* hit)
{
    HepTransform3D global;
    idents::VolumeIdentifier id = hit->volumeID();
    m_gdsvc->getTransform3DByID(id, &global);

    std::string shape;
    std::vector<double> params;

    m_gdsvc->getShapeByID(id, &shape, &params); 

    HepPoint3D origin(0,0,0);

    m_builder->addAttValue("E", (float)hit->totalEnergy(), "");

    double dx = params[0]/2;
    double dy = params[1]/2;
    double dz = params[2]/2;

    drawTransformedPrism(dx, dy, dz, global);
    
    return;
}
