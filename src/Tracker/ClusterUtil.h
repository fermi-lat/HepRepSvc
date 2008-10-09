#ifndef ClusterUtil_h
#define ClusterUtil_h

/** @file ClusterUtil.h
@brief header file for ClusterUtil.cxx
@author Leon Rochester

$Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/src/ClusterUtil.h,v 1.1 2008/08/12 05:03:39 lsrea Exp $
*/

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

#include "Event/Recon/TkrRecon/TkrCluster.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"

#include "HepRepSvc/IBuilder.h"
#include "src/Filler.h"
#include "HepRepSvc/HepRepInitSvc.h"


#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Transform3D.h"

// TU: Hacks for CLHEP 1.9.2.2 and beyond
#ifndef HepPoint3D
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#ifndef HepVector3D
typedef HepGeom::Vector3D<double> HepVector3D;
#endif
#include "CLHEP/Geometry/Vector3D.h"

#include "geometry/Point.h"


/** @class ClusterUtil
@brief base class for the Cluster stuff... keep utility functions here!

@author Leon Rochester

*/

class ClusterUtil :public Filler
{
public:

    ClusterUtil(HepRepInitSvc* hrisvc, IGlastDetSvc* gsvc);
    
    ~ClusterUtil();



protected:

    std::string ClusterUtil::getClusterColor(Event::TkrCluster* pCluster, bool isAcc);

    void buildClusterTypes(IBuilder* builder);

    void ClusterUtil::buildClusterInstance(IBuilder* builder, Event::TkrCluster* pClus);

    IGlastDetSvc* m_gdsvc;
    HepRepInitSvc* m_hrisvc;
    
    double m_siStripPitch; 
    double m_stripLength;
    double m_siThickness;
    double m_siWaferSide;
    double m_siWaferActiveSide;
    double m_ssdGap;
    int    m_nWaferAcross;
    int    m_numXTowers;
    int    m_numYTowers;
    double m_towerPitch;
    double m_activeWidth;

    bool m_hasTypeToT;

private:

    float m_markerSize; // = 25.0;
    float m_markerWidth; // = 2.0;
    bool  m_scalingMarker; //  = true;
};
#endif
