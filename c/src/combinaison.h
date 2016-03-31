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

    using tColor = U32;
    using tCombinaisonId = U32;

    /**
    This class implement the combinaison structure.

            @author 
     */
    class Combinaison {
    public:
        Combinaison(const Combinaison &combi) noexcept;
        Combinaison(U32 nbElements = 0, const tColor pColorTable[] = nullptr) noexcept;
        Combinaison(string combiString);
        void buildCombinaison(U32 nbElements, const tColor pColorTable[] = nullptr) noexcept;

        ~Combinaison() noexcept;

        string str() const noexcept;

        bool getNextCombinaison(tColor nbColors) noexcept;
        tColor operator[](U32 indexElement) const noexcept;
        tColor& operator[](U32 indexElement) noexcept;

        Combinaison& clone() const noexcept;
        void operator=(const Combinaison &combinaisonToCopy) noexcept;
        bool operator==(const Combinaison &rightCombinaison) const noexcept;
        bool operator!=(const Combinaison &rightCombinaison) const noexcept;

        bool contains(tColor color) const noexcept;
        S32 indexOfColor(tColor color) const noexcept;
        void getCorrection(Combinaison combiTotest, U32 &blackPigs, U32 &whitePigs) const noexcept;
        bool isCombinaisonCompatible(const Combinaison &combiToTest, U32 wantedBlackPigs, U32 wantedWhitePigs) const noexcept;

        void random(tColor nbColor) noexcept;

        inline U32 getNbPositions() noexcept{
            return m_nbElements;
        }

        friend ostream& operator<<(ostream& os, const Combinaison& combinaison) noexcept {
            os << "[";
            for (U32 indexElement = 0; indexElement < combinaison.m_nbElements; indexElement++) {
                os << combinaison.m_pColorTable[indexElement];
                if (indexElement < combinaison.m_nbElements - 1)
                    os << ",";
            }
            os << "]";
            return os;
        }

        //    static tCombinaisonId Combinaison::ComputeCombinaisonId(U8 nbElements, U8 nbColors, U8 *pColorTable);
        //    static void Combinaison::ComputeTable(U8 nbElements, U8 nbColors, tCombinaisonId combinaisonId, U8 *&pOutColorTable);

    private:
#ifdef COMBINAISON_DYNAMIC_ALLOCATION
        tColor m_pColorTable = nullptr;
#else //notdef COMBINAISON_DYNAMIC_ALLOCATION
        tColor m_pColorTable[COMBINAISON_STATIC_ALLOCATION_SIZE];
#endif //COMBINAISON_DYNAMIC_ALLOCATION
        U32 m_nbElements = 0;
    };

}

#endif
