#include "MonteCarloFiller.h"
#include "HepRepSvc/IBuilder.h"

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


#include <algorithm>

// This method build the types for the HepRep
void MonteCarloFiller::buildTypes()
{
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
}


// This method fill the instance tree Event/MC with the actual TDS content
void MonteCarloFiller::fillInstances (std::vector<std::string>& typesList)
{
    if (!hasType(typesList,"MC")) 
        return;  

    m_builder->addInstance("","MC");

    if (hasType(typesList,"MC/PosHitCol/PosHit") ||
        hasType(typesList,"MC/PosHitCol/PosHit/PosHitSteps"))
    {
        SmartDataPtr<Event::McPositionHitVector>
            posHits(m_dpsvc, "/Event/MC/PositionHitsCol");
        if(posHits!=0)
        {
            m_builder->addInstance("MC","PosHitCol");
            for(Event::McPositionHitVector::const_iterator ihit=posHits->begin();
                ihit != posHits->end(); ihit++){
                    {
                        HepGeom::Transform3D global;
                        idents::VolumeIdentifier id = (*ihit)->volumeID();
                        m_gdsvc->getTransform3DByID(id, &global);

                        m_builder->addInstance("PosHitCol","PosHit");
                        m_builder->addAttValue("E",
                            (float)(*ihit)->depositedEnergy(),"");
                        m_builder->addAttValue("VolId", id.name(),"");

                        const HepPoint3D pos = (*ihit)->globalEntryPoint();
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
    if (hasType(typesList,"MC/IntHitCol") ||
        hasType(typesList,"MC/IntHitCol/IntHit")) 
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
    if (hasType(typesList,"MC/ParticleCol/Particle"))
    {      
        m_builder->addInstance("MC","ParticleCol");      
        // If there are trajectories in the TDS, we use them
        SmartDataPtr<Event::McTrajectoryCol> 
            mcTraj(m_dpsvc, "/Event/MC/TrajectoryCol");
        if (mcTraj !=0)
        {
            int trajSize = mcTraj->size();
            int maxTrajs = 20000;

            m_builder->setSubinstancesNumber("ParticleCol", mcTraj->size());
            for(Event::McTrajectoryCol::const_iterator traj=mcTraj->begin(); 
                traj != mcTraj->end(); traj++) 
            {
                if (maxTrajs-- < 0) break;

                Event::McParticle* part = (*traj)->getMcParticle();
                m_builder->addInstance("ParticleCol","Particle");

                if (part)
                {                  
                    std::string name;
                    Event::McParticle::StdHepId hepid= part->particleProperty();
                    ParticleProperty* ppty = m_ppsvc->findByStdHepID( hepid );
                    if (ppty)
                        name = ppty->particle(); 
                    else
                        name = "Unknown"; 

                    CLHEP::HepLorentzVector in = part->initialFourMomentum();
                    CLHEP::HepLorentzVector out = part->finalFourMomentum();

                    m_builder->addAttValue("Ei",(float)in.e()-(float)in.m(),"");
                    m_builder->addAttValue("Eo",(float)out.e()-(float)out.m(),"");

                    m_builder->addAttValue("Proc",part->getProcess(),"");

                    m_builder->addAttValue("PDG", hepid,"");
                    m_builder->addAttValue("Name",name,"");

                    if (ppty)
                        setCharge(ppty->charge());
                }
                else  setCharge((*traj)->getCharge());               

                std::vector<CLHEP::Hep3Vector> points = (*traj)->getPoints();
                std::vector<CLHEP::Hep3Vector>::const_iterator pit;

                for(pit = points.begin(); pit != points.end(); pit++) 
                {                
                    m_builder->addPoint((*pit).x(),(*pit).y(),(*pit).z());
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
                    part != mcPart->end(); part++) 
                {
                    m_builder->addInstance("ParticleCol","Particle");

                    Event::McParticle::StdHepId hepid= (*part)->particleProperty();
                    ParticleProperty* ppty = m_ppsvc->findByStdHepID( hepid );

                    if (ppty) m_builder->addAttValue("Name",ppty->particle(),"");
                    else      m_builder->addAttValue("Name","Unknown","");

                    CLHEP::HepLorentzVector in = (*part)->initialFourMomentum();
                    CLHEP::HepLorentzVector out = (*part)->finalFourMomentum();

                    Vector inDir(in.x(), in.y(), in.z());
                    double mag = inDir.mag();
                    if(mag>0.0) inDir /= inDir.mag();

                    m_builder->addAttValue("Ei",(float)in.e()-(float)in.m(),"");
                    m_builder->addAttValue("Eo",(float)out.e()-(float)out.m(),"");

                    HepPoint3D start = (*part)->initialPosition();
                    HepPoint3D end   = (*part)->finalPosition();
                    Point iniPos(start.x(), start.y(), start.z());
                    Point finPos(end.x(), end.y(), end.z());
                    m_builder->addAttValue("Initial Position", getPositionString(iniPos),"");
                    m_builder->addAttValue("Final Position", getPositionString(finPos),"");
                    m_builder->addAttValue("Direction", getDirectionString(inDir),"");

                    m_builder->addAttValue("Proc",(*part)->getProcess(),"");
                  
                    m_builder->addAttValue("PDG", hepid,"");                  

                    //Build strings for status bits
                    unsigned int statBits = (*part)->statusFlags();
                    m_builder->addAttValue("Status Low",getBits(statBits, 15, 0),"");


                    //HepPoint3D start = (*part)->initialPosition();
                    //HepPoint3D end = (*part)->finalPosition();

                    if(ppty) setCharge(ppty->charge());
                    int numDaughters = (int)(*part)->daughterList().size();
                    m_builder->addAttValue("NumDaughters", numDaughters, "");

                    m_builder->addPoint(start.x(),start.y(),start.z());
                    m_builder->addPoint(end.x(),end.y(),end.z());
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

std::string MonteCarloFiller::getTripleString(int precis, double x, double y, double z)
{
    std::stringstream triple;
    triple.setf(std::ios::fixed);
    triple.precision(precis);
    triple << " (" << x << "," << y << "," << z << ")";

    return triple.str();
}

std::string MonteCarloFiller::getPositionString(const Point& position)
{
    int precis = 3;
    return getTripleString(precis, position.x(), position.y(), position.z());
}

std::string MonteCarloFiller::getDirectionString(const Vector& direction)
{
    int precis = 5;
    return getTripleString(precis, direction.x(), direction.y(), direction.z());
}

std::string MonteCarloFiller::getBits(unsigned int statBits, int highBit, int lowBit)
{                    
    std::stringstream outString;
    int bit;
    for (bit=highBit; bit>=lowBit; --bit) {
        outString << (statBits>>(bit)&1) ;
        if (bit%4==0) outString << " ";
    }
    return outString.str();
}

