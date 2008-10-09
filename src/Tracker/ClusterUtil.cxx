
#include "ClusterUtil.h"
/** 
* @class ClusterUtil
*
* @brief base class for managing clusters
*
* @author(s) L. Rochester
*
*/

ClusterUtil::ClusterUtil(HepRepInitSvc* hrisvc, IGlastDetSvc* gsvc) :
m_hrisvc(hrisvc), m_gdsvc(gsvc)
{
    m_markerSize  = 25.0;
    m_markerWidth = 2.0;
    m_scalingMarker  = true;

    int    ladderNStrips;
    double ladderGap;

    m_gdsvc->getNumericConstByName("SiWaferActiveSide", &m_siWaferActiveSide);
    m_gdsvc->getNumericConstByName("stripPerWafer",     &ladderNStrips);
    m_gdsvc->getNumericConstByName("SiThick",           &m_siThickness);
    m_gdsvc->getNumericConstByName("SiWaferSide",       &m_siWaferSide);
    m_gdsvc->getNumericConstByName("ssdGap",            &m_ssdGap);
    m_gdsvc->getNumericConstByName("ladderGap",         &ladderGap);
    m_gdsvc->getNumericConstByName("nWaferAcross",      &m_nWaferAcross);
    m_gdsvc->getNumericConstByName("xNum",              &m_numXTowers);
    m_gdsvc->getNumericConstByName("yNum",              &m_numYTowers);
    m_gdsvc->getNumericConstByName("towerPitch",        &m_towerPitch);

    double deadWidth = m_siWaferSide - m_siWaferActiveSide;
    m_stripLength = m_nWaferAcross*m_siWaferSide + (m_nWaferAcross-1)*m_ssdGap - deadWidth;
    m_activeWidth = m_nWaferAcross*m_siWaferSide + (m_nWaferAcross-1)*ladderGap - deadWidth;
    m_siStripPitch = m_siWaferActiveSide / ladderNStrips;



}

ClusterUtil::~ClusterUtil() {}

std::string ClusterUtil::getClusterColor(Event::TkrCluster* pCluster, bool isAcc) 
{
    if (isAcc) { return "red"; }
    else if (pCluster->isSet(Event::TkrCluster::maskGHOST)) { 
        return "255,100,27";  // orange
    }
    else if (pCluster->isSet(Event::TkrCluster::maskSAMETRACK)) {
        return "yellow"; 
    } 
    else { return "green"; }
}

void ClusterUtil::buildClusterTypes(IBuilder* builder)
{
    builder->addType("TkrCluster", "Strip", "Strip", "");
    builder->addType("Strip", "ActiveStrip", "Active Part of Strip", "");
    builder->addAttValue("DrawAs","Prism","");
    //builder->addAttValue("Color","green","");
    builder->addAttDef("Sequence", "Position in TkrClusterCol", "Physics", "");
    builder->addAttDef("Tower","Cluster Tower #","Physics","");
    builder->addAttDef("Plane","Cluster Plane #","Physics","");
    builder->addAttDef("View","Cluster View","Physics","");
    builder->addAttDef("First Strip","Cluster First Strip","Physics","");
    builder->addAttDef("Last Strip","Cluster Last Strip","Physics","");
    builder->addAttDef("Status", "Cluster Low Status Bits","Physics","");
    builder->addAttDef("Position","Cluster Global Position","Physics","");
    builder->addAttDef("RawToT","Cluster Time over Threshold","Physics","");
    builder->addAttDef("Mips","ToT converted to Mips","Physics","");

    builder->addType("TkrCluster","TkrClusterToT"," ","");
    builder->addAttValue("DrawAs","Line","");
    builder->addAttValue("Color","red","");

    builder->addType("TkrCluster", "ClusterMarker", " ", "");
    //builder->addAttValue("Color", "green", "");
    if(m_scalingMarker) {
        // Tracy hates the fixed-size marker, try again!
        builder->addType("ClusterMarker", "MarkerArm", " ", "");
        builder->addAttValue("DrawAs", "Line", "");
        builder->addAttValue("LineWidth", m_markerWidth, "");
    } else {
        //Here's the real marker 
        builder->addAttValue("DrawAs", "Point", "");
        builder->addAttValue("MarkerName", "Cross", "");
        builder->addAttValue("MarkerSize", "1", "");
    }
}

void ClusterUtil::buildClusterInstance(IBuilder* builder, Event::TkrCluster* pCluster)
{

    double markerOffset = m_siStripPitch;
    double markerSize   = m_markerSize*m_siStripPitch;

    double halfLength   = 0.5 * (m_stripLength);

    Point clusPos  = pCluster->position();

    double halfWidth = 0.5 * pCluster->size() * m_siStripPitch;
    double x    = clusPos.x();
    double y    = clusPos.y();
    double z    = clusPos.z();
    double dz   = 0.5 * m_siThickness;

    double dx   = halfWidth;
    double dy   = halfLength;

    double xToT = x;
    double yToT = y - dy - markerOffset;
    double deltaY = 0.0;
    double deltaX = markerSize;

    int    view   = pCluster->getTkrId().getView();
    if (view == idents::TkrId::eMeasureY) {
        dy   = 0.5 * pCluster->size() * m_siStripPitch;
        dx   = 0.5 * m_stripLength;
        xToT = x - dx - markerOffset;
        yToT = y;
        deltaY = markerSize;
        deltaX = 0.0;
    }

    double ToT  = pCluster->getRawToT() / 64.;       // So, max ToT = 4 mm


    builder->addAttValue("Tower",       pCluster->tower(),"");
    builder->addAttValue("Plane",       (int)pCluster->getPlane(),"");
    builder->addAttValue("View",        view,"");
    builder->addAttValue("First Strip", pCluster->firstStrip(),"");
    builder->addAttValue("Last Strip",  pCluster->lastStrip(),"");

    unsigned int status = pCluster->getStatusWord();
    builder->addAttValue("Status",getBits(status, 15, 0),"");
    builder->addAttValue("Position",
        getPositionString(pCluster->position()),"");

    int rawToT = pCluster->getRawToT();
    builder->addAttValue("RawToT",         (float) rawToT,"");
    builder->addAttValue("Mips",           (float)(pCluster->getMips()),"");

    //Draw the width of the cluster

    //Now draw the hit strips
    // check for ToT==255
    bool isAcc = (rawToT==255);
    std::string clusterColor = getClusterColor(pCluster,isAcc);

    builder->addInstance("TkrCluster", "Strip");
    builder->setSubinstancesNumber("Strip",m_nWaferAcross);
    double waferPitch = m_siWaferSide + m_ssdGap;
    double offset = -0.5*(m_nWaferAcross-1)*waferPitch;
    for (int wafer=0;wafer<m_nWaferAcross; ++wafer) {
        builder->addInstance("Strip", "ActiveStrip");
        if(clusterColor!="green") builder->addAttValue("LineStyle","Dashed","");
        builder->addAttValue("Color", clusterColor, "");
        double delta = offset + wafer*waferPitch;
        x  = clusPos.x();
        y  = clusPos.y() + delta;
        dx = halfWidth;
        dy = 0.5*m_siWaferActiveSide;
        if (view == idents::TkrId::eMeasureY) {
            x = clusPos.x() + delta;
            y = clusPos.y();
            std::swap(dx, dy);
        }
        drawPrism(x, y, z, dx, dy, dz);
    }

    // quick and dirty wide cluster display.
    bool isWide = (pCluster->size()>4)&&m_hrisvc->getClusterFiller_showWide();
    if(m_scalingMarker) {
        // make the cross in the measured view
        builder->addInstance("TkrCluster", "ClusterMarker");
        builder->setSubinstancesNumber("ClusterMarker",2);
        builder->addInstance("ClusterMarker", "MarkerArm");
        if(isAcc) builder->addAttValue("LineStyle","Dashed","");
        builder->addAttValue("Color", clusterColor, "");
        builder->addPoint(xToT+deltaX, yToT+deltaY, z+markerSize);
        builder->addPoint(xToT-deltaX, yToT-deltaY, z-markerSize);
        if(isWide) builder->addPoint(xToT-deltaX, yToT-deltaY, z+markerSize);

        builder->addInstance("ClusterMarker", "MarkerArm");
        if(isAcc) builder->addAttValue("LineStyle","Dashed","");
        builder->addAttValue("Color", clusterColor, "");
        builder->addPoint(xToT-deltaX, yToT-deltaY, z+markerSize);
        builder->addPoint(xToT+deltaX, yToT+deltaY, z-markerSize);
        if(isWide) builder->addPoint(xToT+deltaX, yToT+deltaY, z+markerSize);
    } else {
        // Here's the code for the real marker
        builder->addInstance("TkrCluster", "ClusterMarker");

        if(isAcc) builder->addAttValue("LineStyle","Dashed","");
        builder->addAttValue("Color", clusterColor, "");
        builder->addPoint(xToT, yToT, z);       
    }

    //Time over threshold add here
    if(m_hasTypeToT)
    {
        builder->addInstance("TkrCluster","TkrClusterToT");
        builder->addPoint(xToT,yToT,z);
        builder->addPoint(xToT,yToT,z+ToT);
    }
}
