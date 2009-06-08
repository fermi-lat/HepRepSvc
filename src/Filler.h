#ifndef Filler_h
#define Filler_h

/** @file Filler.h
@brief header file for Filler.cxx
@author Leon Rochester

$Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/src/Filler.h,v 1.1.160.1 2009/06/08 17:24:28 echarles Exp $
*/

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

#include "HepRepSvc/IBuilder.h"
#include "HepRepSvc/IFiller.h"

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


/** @class Filler
@brief base class for the Fillers... keep utility functions here!

@author Leon Rochester

*/

//namespace HepGeom {class Transform3D;}

class Filler : public IFiller
{
public:

    Filler();
    
    virtual ~Filler();

    virtual bool hasType(std::vector<std::string>& list, std::string type); 
    virtual std::string getTripleString(int precis, double x, double y, double z);
    virtual std::string getPositionString(const Point& position);
    virtual std::string getDirectionString(const Vector& direction);
    virtual std::string getPositionString(const HepPoint3D& position);
    virtual std::string getDirectionString(const HepVector3D& direction);
    virtual std::string getBits(unsigned int statBits, int highBit, int lowBit);
    virtual void drawPrism(double x, double y, double z, 
        double dx, double dy, double dz);
    virtual void drawTransformedPrism(double dx, double dy, double dz,
        HepGeom::Transform3D& trans);
    virtual void drawTransformedTrap(double dx1, double dx2, 
        double dxDiff, double dy, double dz,
        HepGeom::Transform3D& trans); 
};
#endif
