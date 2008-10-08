#ifndef MeritTupleFiller_H
#define MeritTupleFiller_H

#include <vector>
#include <string>

#include "Filler.h"

class INTupleWriterSvc;
class HepRepInitSvc;
class TTree;

/** 
*  @class MeritTupleFiller
*
*  @brief This is the HepRep fillers for all the ACD relevant
*  objects (recond and digi). This is a "bookmark" filler waiting for some real
*  ACD expert to help in developing it
*
*  @author R.Giannitrapani
*/

class MeritTupleFiller: public Filler
{
public:
    MeritTupleFiller(HepRepInitSvc* hrisvc,
        INTupleWriterSvc* rtsvc);

    /// This method init the type tree
    virtual void buildTypes ();
    /// This method fill the instance tree, using the string vector to decide
    /// which subinstances to fill
    virtual void fillInstances (std::vector<std::string>&);
    /// gets the variable names from an existing meritTuple
    virtual bool parseTree(TTree* tree);


private:
    HepRepInitSvc* m_hrisvc;
    INTupleWriterSvc* m_rtsvc;

    TTree* m_tree;
    
    std::vector<std::string> m_names;
    std::vector<std::string> m_prefixes;
    std::vector<std::string> m_types;
    std::vector<int>         m_dims;
    std::vector<int>         m_conversionFlags;
    int                      m_prefixCount;

};

#endif //MeritTupleFiller_H
