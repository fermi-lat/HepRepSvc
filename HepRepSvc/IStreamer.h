// $Header:
//
// Description: 
//
// Author:R.Giannitrapni

#ifndef ISTREAMER_H
#define ISTREAMER_H

#include <string>

class IRegistry;

/** 
 * @class IStreamer
 *
 * @brief A streamer interface for file saving HepRep implementations
 *    
 */
class IStreamer
{
 public:
  /// Set the fillers registry
  void setRegistry(IRegistry* r){m_registry = r;};

  /// The pure virtual method to save the HepRep to a filename
  virtual void saveHepRep(std::string) = 0;

 protected:
  IRegistry* m_registry;
};

#endif //ISTREAMER_H
