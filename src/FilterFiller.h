#ifndef FilterFiller_H
#define FilterFiller_H

#include <vector>
#include <string>

#include "Filler.h"

class IGlastDetSvc;
class IDataProviderSvc;
class HepRepInitSvc;

/** 
*  @class FilterFiller
*
*  @brief This is the HepRep fillers for all the ACD relevant
*  objects (recond and digi). This is a "bookmark" filler waiting for some real
*  ACD expert to help in developing it
*
*  @author R.Giannitrapani
*/

class FilterFiller: public Filler
{
public:
    FilterFiller(HepRepInitSvc* hrisvc,
        IGlastDetSvc* gsvc,
        IDataProviderSvc* dpsvc);

    /// This method init the type tree
    virtual void buildTypes ();
    /// This method fill the instance tree, using the string vector to decide
    /// which subinstances to fill
    virtual void fillInstances (std::vector<std::string>&);

private:
    HepRepInitSvc* m_hrisvc;
    IGlastDetSvc* m_gdsvc;
    IDataProviderSvc* m_dpsvc;
};

#endif //FilterFiller_H
