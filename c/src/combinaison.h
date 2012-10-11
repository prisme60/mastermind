//
// C++ Interface: combinaison
//
// Description: 
//
//
// Author:  <>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MASTERMINDCOMBINAISON_H
#define MASTERMINDCOMBINAISON_H

#include <iostream>
#include "typedefinition.h"

using namespace std;

namespace mastermind {

#undef COMBINAISON_DYNAMIC_ALLOCATION
#define COMBINAISON_STATIC_ALLOCATION_SIZE 10

typedef U32 tColor;
typedef U32 tCombinaisonId;

/**
This class implement the combinaison structure.

	@author 
*/
class Combinaison{
public:
    Combinaison          (const Combinaison &combi);
    Combinaison          (U32 nbElements=0, const tColor pColorTable[]=NULL);
	Combinaison          (string combiString);
    void buildCombinaison(U32 nbElements,   const tColor pColorTable[]=NULL);

    ~Combinaison();
	
	string& str(string &strCombi) const;

    bool getNextCombinaison(tColor nbColors);
    tColor operator[](U32 indexElement) const;
    tColor& operator[](U32 indexElement);

    Combinaison& clone() const;
    void operator=(const Combinaison &combinaisonToCopy);
    bool operator==(const Combinaison &rightCombinaison) const;
    bool operator!=(const Combinaison &rightCombinaison) const;

    bool contains(tColor color) const;
    S32  indexOfColor(tColor color) const;
    void getCorrection(Combinaison combiTotest, U32 &blackPigs, U32 &whitePigs) const;
    bool isCombinaisonCompatible(const Combinaison &combiToTest, U32 wantedBlackPigs, U32 wantedWhitePigs) const;
	
    void random(tColor nbColor);

    friend ostream& operator<<(ostream& os, const Combinaison& combinaison)
    {
      os << "[";
      for(U32 indexElement=0;indexElement<combinaison.m_nbElements;indexElement++)
      {
        os << combinaison.m_pColorTable[indexElement];
        if(indexElement<combinaison.m_nbElements-1)
          os << ",";
      }
      os << "]";
      return os;
    }

//    static tCombinaisonId Combinaison::ComputeCombinaisonId(U8 nbElements, U8 nbColors, U8 *pColorTable);
//    static void Combinaison::ComputeTable(U8 nbElements, U8 nbColors, tCombinaisonId combinaisonId, U8 *&pOutColorTable);

private:
  #ifdef COMBINAISON_DYNAMIC_ALLOCATION
    tColor * m_pColorTable;
  #else //notdef COMBINAISON_DYNAMIC_ALLOCATION
    tColor m_pColorTable[COMBINAISON_STATIC_ALLOCATION_SIZE];
  #endif //COMBINAISON_DYNAMIC_ALLOCATION
    U32      m_nbElements;
};

}

#endif
