#include <iostream>

#ifdef DEFECT_NO_STRINGSTREAM
#include <strstream>
#else
#include <sstream>
#endif

#include "XmlBuilder.h"


void XMLBuilder::addAttDef(std::string name,
                             std::string desc,
                             std::string category,
                             std::string extra)
{
  if (m_actualType.size() !=0)
    gzprintf(m_file,"%s",addIdent(m_actualType.size()).c_str());
  else
    gzprintf(m_file,"%s",addIdent(m_actualInstance.size()).c_str());
  
  gzprintf(m_file,"<attdef name=\"%s\" desc =\"%s\" category =\"%s\" extra =\"%s\"/>\n", 
          name.c_str(), 
          desc.c_str(),
          category.c_str(),
          extra.c_str());

}
  
void XMLBuilder::addAttValue(std::string name,
                               std::string value,
                               std::string showlabel)
{
  if (m_actualType.size() !=0)
    gzprintf(m_file,"%s",addIdent(m_actualType.size()).c_str());
  else
    gzprintf(m_file,"%s",addIdent(m_actualInstance.size()).c_str());

  gzprintf(m_file,"<attvalue name=\"%s\" value =\"%s\" showlabel =\"%s\"/>\n", 
          name.c_str(), 
          value.c_str(),
          showlabel.c_str());
}

void XMLBuilder::addAttValue(std::string name,
                               int value,
                               std::string showlabel)
{
  if (m_actualType.size() !=0)
    gzprintf(m_file,"%s",addIdent(m_actualType.size()).c_str());
  else
    gzprintf(m_file,"%s",addIdent(m_actualInstance.size()).c_str());

  gzprintf(m_file,"<attvalue name=\"%s\" value =\"%d\" showlabel =\"%s\"/>\n", 
          name.c_str(), 
          value,
          showlabel.c_str());
}

void XMLBuilder::addAttValue(std::string name,
                               float value,
                               std::string showlabel)
{
  if (m_actualType.size() !=0)
    gzprintf(m_file,"%s",addIdent(m_actualType.size()).c_str());
  else
    gzprintf(m_file,"%s",addIdent(m_actualInstance.size()).c_str());

  gzprintf(m_file,"<attvalue name=\"%s\" value =\"%f\" showlabel =\"%s\"/>\n", 
          name.c_str(), 
          value,
          showlabel.c_str());

}
  
void XMLBuilder::addPoint(float x, float y, float z)
{
  gzprintf(m_file,"%s",addIdent(m_actualInstance.size()+1).c_str());
  
  gzprintf(m_file,"<point x=\"%f\" y=\"%f\" z=\"%f\">\n", 
          x, y, z);

  gzprintf(m_file,"%s",addIdent(m_actualInstance.size()+1).c_str());
  gzprintf(m_file,"</point>\n");
}
  
void XMLBuilder::addInstance(std::string father, 
                               std::string type)
{
  if (m_actualInstance.size() != 0)
    {
      while (m_actualInstance.back() != father)
        {
          m_actualInstance.pop_back();
          gzprintf(m_file,"%s",addIdent(m_actualInstance.size()).c_str());
          gzprintf(m_file,"</instance>\n");
        }
    }
  
  gzprintf(m_file,"%s",addIdent(m_actualInstance.size()).c_str());
  gzprintf(m_file,"<instance type=\"%s\">\n",type.c_str());

  m_actualInstance.push_back(type);

}

void XMLBuilder::addType(std::string father, 
                           std::string name, 
                           std::string desc, 
                           std::string info)
{  
  if (m_actualType.size() != 0)
    {
      while (m_actualType.back() != father)
        {
          m_actualType.pop_back();
          gzprintf(m_file,"%s",addIdent(m_actualType.size()).c_str());
          gzprintf(m_file,"</type>\n");
        }
    }

  gzprintf(m_file,"%s",addIdent(m_actualType.size()).c_str());
  gzprintf(m_file,"<type name=\"%s\">\n",name.c_str());
  
  m_actualType.push_back(name);
}

void XMLBuilder::endTypes()
{
  for(unsigned int i=0; i<m_actualType.size();i++)
    {
      gzprintf(m_file,"%s",addIdent(m_actualType.size()-i-1).c_str());
      gzprintf(m_file,"</type>\n");
    }
}

void XMLBuilder::endInstances()
{
  for(unsigned int i=0; i<m_actualInstance.size();i++)
    {
      gzprintf(m_file,"%s",addIdent(m_actualInstance.size()-i-1).c_str());
      gzprintf(m_file,"</instance>\n");
    }
}

void XMLBuilder::reset()
{
  m_actualType.clear();
  m_actualInstance.clear();
}

std::string XMLBuilder::addIdent(int n)
{

#ifdef DEFECT_NO_STRINGSTREAM
  std::strstream ident;
#else
  std::stringstream ident;
#endif

  for(unsigned int i=0;i<n;i++)
    ident << "  ";
  return ident.str();
}

