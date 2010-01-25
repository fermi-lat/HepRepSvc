#ifndef CALRECONFILLER_H
#define CALRECONFILLER_H

#include <vector>
#include <string>

#include "Filler.h"

class IGlastDetSvc;
class IDataProviderSvc;
class IParticlePropertySvc;
class HepRepInitSvc;

class Point;
class Vector;

#include "Event/Recon/CalRecon/CalCluster.h"
#include "Event/Recon/CalRecon/CalMipClasses.h" 
/** 
 *  @class CalReconFiller
 *
 *  @brief This is the HepRep fillers for all the Calorimeter relevant
 *  objects (recond and digi). This is a "bookmark" filler waiting for some real
 *  CAL expert to help in developing it
 *
 *  @author R.Giannitrapani
 */

class CalReconFiller: public Filler{
  
public:
    CalReconFiller(HepRepInitSvc* hrisvc,
                   IGlastDetSvc* gsvc,
                   IDataProviderSvc* dpsvc,
                   IParticlePropertySvc* ppsvc);
  
    /// This method init the type tree
    virtual void buildTypes ();
    /// This method fill the instance tree, using the string vector to decide
    /// which subinstances to fill
    virtual void fillInstances (std::vector<std::string>&);


private:
    void drawClusters(Event::CalClusterCol* clusters, Event::CalClusterHitTabList* xTal2ClusTabList);
    void drawCluster(Event::CalCluster* cluster, std::string color = "");
    void drawXtals(std::vector<Event::CalClusterHitRel*>& xTalRelVec, std::string color="");
    void drawXtals(Event::CalXtalRecCol* xTalCol, std::string color = "");
    void drawXtal(Event::CalXtalRecData* xTal, double eMax, std::string color="");
    void drawMips(Event::CalMipTrackCol* mipCol);

    HepRepInitSvc* m_hrisvc;
    IGlastDetSvc* m_gdsvc;
    IDataProviderSvc* m_dpsvc;
    IParticlePropertySvc* m_ppsvc;

    /// enums stolen from CalUtil/CalDefs.h (not in "uses" path)

    /** Volume ID field identifiers.  shouldn't be here, but there is no
        global def as far as i can tell.
    */
    enum {fLATObjects, fTowerY, fTowerX, fTowerObjects, fLayer,
          fMeasure, fCALXtal,fCellCmp, fSegment};

    std::string* m_colorArray;
    int          m_maxColors;
    int          m_colorIndex;
    /// Minimum energy for colors in display
    float m_minEnergy;

    /// crystal half height
    float m_xtalHalfHeight;

    /// crystal half width
    float m_xtalHalfWidth;

    /// crystal half length
    float m_xtalHalfLength;

    /// Z position of top calorimeter layer
    float m_calZtop;
  
    /// Z position of bottom calorimeter layer
    float m_calZbottom;
    //-- XML GEOMETRY CONSTANTS --//
  
    /// number of x towers
    int   m_xNum;    
    /// number of y towers
    int   m_yNum;    
    /// the value of fTowerObject field, defining calorimeter module 
    int   m_eTowerCAL;
    /// the value of fLATObjects field, defining LAT towers 
    int   m_eLATTowers; 
    /// the value of fCellCmp field defining CsI crystal
    int   m_eXtal;      
    /// number of geometric segments per Xtal
    int   m_nCsISeg;
    /// number of layers
    int   m_nLayers;
};

#endif //CALRECONFILLER_H
