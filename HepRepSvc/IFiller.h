#ifndef IFILLER_H
#define IFILLER_H

#include <vector>
#include <list>
#include <string>

class IBuilder;

/** 
 *  @class IFiller
 *
 *  @brief An abstract interface for the filler of an HepRep
 *
 *  This abstract class is the base for the fill mechanism of the HepRep in an
 *  independent way from the context. Concrete classes implementing this
 *  interface will be automatically used by the Registry mechanism to fill the
 *  HepRep with whatever information is needed by the experiment. These
 *  implementations will be developed by experts of each subsistems (ACD, CAL,
 *  TKR) that can decide what is more pertinent to put in the Event Display
 *  representables
 *
 *  @author R.Giannitrapani
 */

class IFiller{
  
 public:
  /// This method init the type tree; it is used to define the logical
  /// hierarchy of HepRep types representing the event
  virtual void buildTypes () = 0;
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill (if the vector is empty than all the
  /// subinstances are used)
  virtual void fillInstances (std::vector<std::string>&) = 0;

  /// Set the abstract builder
  virtual void setBuilder(IBuilder* b){m_builder = b;};

 protected:
  /// The builder to be used to build the heprep
  IBuilder* m_builder;
  
};

#endif //IFILLER_H
