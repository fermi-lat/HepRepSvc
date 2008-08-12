
#include "Filler.h"
/** 
* @class Filler
*
* @brief base class for the fillers, with utility methods
*
* @author(s) L. Rochester
*
*/

Filler::Filler() {}

Filler::~Filler() {}

bool Filler::hasType(std::vector<std::string>& list, std::string type) 
{
    if (list.size() == 0) return 1;
    std::vector<std::string>::const_iterator i; 
    i = std::find(list.begin(),list.end(),type);
    if(i == list.end()) return 0;
    else return 1;
}

std::string Filler::getTripleString(int precis, double x, double y, double z)
{
    std::stringstream triple;
    triple.setf(std::ios::fixed);
    triple.precision(precis);
    triple << " (" << x << "," << y << "," << z << ")";

    return triple.str();
}

std::string Filler::getPositionString(const Point& position)
{
    int precis = 3;
    return getTripleString(precis, position.x(), position.y(), position.z());
}

std::string Filler::getDirectionString(const Vector& direction)
{
    int precis = 5;
    return getTripleString(precis, direction.x(), direction.y(), direction.z());
}

std::string Filler::getBits(unsigned int statBits, int highBit, int lowBit)
{                    
    std::stringstream outString;
    int bit;
    for (bit=highBit; bit>=lowBit; --bit) {
        outString << (statBits>>(bit)&1) ;
        if (bit%4==0) outString << " ";
    }
    return outString.str();
}

void Filler::drawPrism(double x, double y, double z, 
                       double dx, double dy, double dz)
{
    m_builder->addPoint(x+dx,y+dy,z+dz);
    m_builder->addPoint(x-dx,y+dy,z+dz);
    m_builder->addPoint(x-dx,y-dy,z+dz);
    m_builder->addPoint(x+dx,y-dy,z+dz);
    m_builder->addPoint(x+dx,y+dy,z-dz);
    m_builder->addPoint(x-dx,y+dy,z-dz);
    m_builder->addPoint(x-dx,y-dy,z-dz);
    m_builder->addPoint(x+dx,y-dy,z-dz);
    return;
}

void Filler::drawTransformedPrism(double dx, double dy, double dz,
                                  HepGeom::Transform3D& global)
{
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

void Filler::drawTransformedTrap(double dx1, double dx2, 
                                 double dxDiff, double dy, double dz, 
                                 HepGeom::Transform3D& atr)
{
    HepPoint3D v, v1;

    v.setX(dx2+dxDiff); v.setY(dy); v.setZ(dz);    v1 = atr*v;
    m_builder->addPoint(v1.x(), v1.y(), v1.z());
    v.setX(-dx2+dxDiff); v.setY(dy); v.setZ(dz);   v1 = atr*v;
    m_builder->addPoint(v1.x(), v1.y(), v1.z());
    v.setX(-dx1-dxDiff); v.setY(-dy); v.setZ(dz);  v1 = atr*v;
    m_builder->addPoint(v1.x(), v1.y(), v1.z());
    v.setX(dx1-dxDiff); v.setY(-dy); v.setZ(dz);   v1 = atr*v;
    m_builder->addPoint(v1.x(), v1.y(), v1.z());

    v.setX(dx2+dxDiff); v.setY(dy); v.setZ(-dz);   v1 = atr*v;
    m_builder->addPoint(v1.x(), v1.y(), v1.z());
    v.setX(-dx2+dxDiff); v.setY(dy); v.setZ(-dz);  v1 = atr*v;
    m_builder->addPoint(v1.x(), v1.y(), v1.z());
    v.setX(-dx1-dxDiff); v.setY(-dy); v.setZ(-dz); v1 = atr*v;
    m_builder->addPoint(v1.x(), v1.y(), v1.z());
    v.setX(dx1-dxDiff); v.setY(-dy); v.setZ(-dz);  v1 = atr*v;
    m_builder->addPoint(v1.x(), v1.y(), v1.z());
}
