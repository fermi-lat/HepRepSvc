#include "Registry.h"

#include <sstream>

#include "HepRepSvc/IFiller.h"
#include "HepRepSvc/IBuilder.h"


// This method return the fillers by the type
const Registry::fillerCol& Registry::getFillersByType(std::string type)
{
  return m_fillers[type];
}

const std::vector<std::string>& Registry::getTypeTrees()
{
  std::vector<std::string>* types = new std::vector<std::string>; 

  std::map<std::string, fillerCol>::iterator i;
  
  for(i=m_fillers.begin();i!=m_fillers.end();i++)
    {
      types->push_back(i->first);
    }
  
  return *types;
}

// Register a filler with the service
void Registry::registerFiller(IFiller* f, std::string tree)
{
  m_fillers[tree].push_back(f);
}

// This method sets a builder to be used by the registered fillers
void Registry::useBuilder(IBuilder* b)
{
  std::map<std::string, fillerCol>::iterator i;

  for(i=m_fillers.begin();i!=m_fillers.end();i++)
    {
      fillerCol::iterator j;
      
      for(j=(i->second).begin();j!=(i->second).end();j++)
        {
          (*j)->setBuilder(b);
        }
    }
}


const std::vector<std::string>& Registry::getDependencies(std::string tree){
  return m_dependencies[tree];
}

std::string Registry::getTypeByInstance(std::string name)
{
  return m_instances[name];
}
