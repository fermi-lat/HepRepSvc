// File and Version Information:
// $Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/src/HepRepGeometry.cxx,v 1.10 2005/08/19 19:18:11 jrb Exp $
//
// Author(s):
//      R.Giannitrapani

#include "HepRepGeometry.h"
#include "HepRepSvc/IBuilder.h"


#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Transform3D.h"

#include <iomanip>
#include <algorithm>

HepRepGeometry::HepRepGeometry(unsigned int depth, std::string mode): 
  m_mode(mode), m_depth(depth)
{
  m_actualDepth = 0;
  m_builder = 0;
  HepTransform3D start;
  m_actualTransform.push_back(start);
}

HepRepGeometry::~HepRepGeometry()
{

}

IGeometry::VisitorRet 
HepRepGeometry::pushShape(ShapeType s, const UintVector& idvec, 
                      std::string name, std::string material, 
                      const DoubleVector& params, VolumeType type,
                      SenseType sense)
{
  if(m_hrMode == "type")
    {
      // Build the full path name in the list of already built types
      std::string fullName = "";
      for(unsigned int j = 0; j<m_actualType.size();j++)
      {
        fullName = fullName + m_actualType[j] + "/";
      }

      fullName = fullName + name;

      std::vector<std::string>::const_iterator i;
      i = std::find(m_types.begin(),m_types.end(),fullName);
      if (i == m_types.end())
        {          
          m_types.push_back(fullName);          
          std::string father;
          if (m_actualType.size() == 0)
            father = "";
          else
            father = m_actualType.back();
          
          m_actualDepth++;

          m_actualType.push_back(name);          
          m_builder->addType(father,name,"A volume of the geometry; it can be simple, a composition or a stack","");


					// The color is set only to the root of the geometry, so it will be possible
					// to change the color of subtypes 
					if (name == "LAT")
          {
						m_builder->addAttValue("Color","0.98,0.92,0.8","");
            m_builder->addAttValue("Layer","Geometry","");
          }

          //          m_builder->addAttDef("Volume type","The kind of volume (simple, composition, stack or sensitive)","Physics","");
          m_builder->addAttDef("Volume type","The kind of volume (simple, composition or stack)","Physics","");
          m_builder->addAttDef("Sensitivity","One of Nonsensitive, intSensitive, or posSensitive","Physics","");
          m_builder->addAttDef("Material","The material name of the volume","Physics","");
          m_builder->addAttDef("Shape","At the moment this can be just a Box","Physics","");

          if (m_actualDepth > m_depth)
            m_builder->addAttValue("Visibility", false, "");

          m_builder->addAttValue("Material",material,"");
          
          switch(type)
            {
            case Simple:
              m_builder->addAttValue("Volume type","Simple","");
              break;
            case Composite: 
              m_builder->addAttValue("Volume type","Composite","");
              break;
            case Xstack: 
              m_builder->addAttValue("Volume type","Xstack","");
              break;
            case Ystack: 
              m_builder->addAttValue("Volume type","Ystack","");
              break;
            case Zstack:
              m_builder->addAttValue("Volume type","Zstack","");
              break;
            }  
          switch(sense) {
            case posSensitive:
              m_builder->addAttValue("Sensitivity","posSensitive","");
              break;
            case intSensitive:
              m_builder->addAttValue("Sensitivity","intSensitive","");
              break;
            case Nonsensitive:
              m_builder->addAttValue("Sensitivity","Nonsensitive","");
              break;
          }

          switch(s)
            {
            case Box:
              m_builder->addAttValue("DrawAs","Prism","");
              m_builder->addAttValue("Shape","Box","");
              break;
            }
        }
      else
        {
          m_actualDepth++;
          m_actualType.push_back(name);
        }
    }

  else if (m_hrMode == "instance")
    {
      std::string temp;
      for(unsigned int k=0; k<m_actualInstance.size(); k++)
        temp = temp + m_actualInstance[k] + "/";
          
      double x=params[0], y=params[1], z=params[2];
      double rx=params[3], ry=params[4], rz=params[5];  

      HepRotationX xrot(rx*M_PI/180);
      HepRotationY yrot(ry*M_PI/180);
      HepRotationZ zrot(rz*M_PI/180);
      HepRotation rot = xrot*yrot*zrot;

      Hep3Vector t(x, y, z);  
      HepTransform3D tr(rot,t);
      HepTransform3D atr = (m_actualTransform.back())*tr;

      std::string father;
      if (m_actualInstance.size() == 0)
        father = "";
      else
        father = m_actualInstance.back();


      m_actualInstance.push_back(name);
      m_actualDepth++;            

      // Verify that this instance is in the typesList
      if (hasType(m_typesList, temp+name))
      {                                    
        
        m_builder->addInstance(father,name);

        if ((s == Box))
        {
          double dx = params[6]/2;
          double dy = params[7]/2;
          double dz = params[8]/2;

          HepPoint3D v;
          HepPoint3D v1;

//          if ((((type == Simple) || (type == posSensitive) || (type == intSensitive)) 
//                && (m_actualDepth < m_depth)) || (m_actualDepth == m_depth)) 
//          if ((type == Simple) || (type == posSensitive) || (type == intSensitive))
          if (type == Simple)
          {

            v.setX(dx); v.setY(dy); v.setZ(dz); v1 = (atr*v);
            m_builder->addPoint(v1.x(), v1.y(), v1.z());
            v.setX(-dx); v.setY(dy); v.setZ(dz); v1 = atr*v;
            m_builder->addPoint(v1.x(), v1.y(), v1.z());
            v.setX(-dx); v.setY(-dy); v.setZ(dz); v1 = atr*v;
            m_builder->addPoint(v1.x(), v1.y(), v1.z());
            v.setX(dx); v.setY(-dy); v.setZ(dz); v1 = atr*v;
            m_builder->addPoint(v1.x(), v1.y(), v1.z());


            v.setX(dx); v.setY(dy); v.setZ(-dz); v1 = atr*v;
            m_builder->addPoint(v1.x(), v1.y(), v1.z());
            v.setX(-dx); v.setY(dy); v.setZ(-dz); v1 = atr*v;
            m_builder->addPoint(v1.x(), v1.y(), v1.z());              
            v.setX(-dx); v.setY(-dy); v.setZ(-dz); v1 = atr*v;
            m_builder->addPoint(v1.x(), v1.y(), v1.z());
            v.setX(dx); v.setY(-dy); v.setZ(-dz); v1 = atr*v;
            m_builder->addPoint(v1.x(), v1.y(), v1.z());              
          }
        }
      }            
      m_actualTransform.push_back(atr);
    }
  else
    return AbortSubtree;

  if ((m_typesList.size() == 0) && (m_actualDepth == m_depth))
    return AbortSubtree;      
  else 
    return More;
}


void HepRepGeometry::popShape()
{
  if(m_hrMode == "type")
    {
      m_actualType.pop_back();
    }
  else if (m_hrMode == "instance")
    {
      m_actualTransform.pop_back();      
      m_actualInstance.pop_back();
    }
  
  m_actualDepth--;

}

void HepRepGeometry::reset()
{
  m_types.clear();
  m_actualType.clear();
  m_actualInstance.clear();
  m_actualDepth = 0;
}



bool HepRepGeometry::hasType(std::vector<std::string>& list, std::string type) 
{
  if (list.size() == 0) return 1;

  std::vector<std::string>::const_iterator i; 

  i = std::find(list.begin(),list.end(),type);
  if(i == list.end()) return 0;
  else return 1;

}
