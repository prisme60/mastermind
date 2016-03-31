//
// C++ Interface: mastersolver
//
// Description: 
//
//
// Author:  <>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MASTERMINDMASTERGAME_H
#define MASTERMINDMASTERGAME_H

#include "typedefinition.h"
#include "combinaison.h"

using namespace std;

namespace mastermind {

    /**
    This class permit to solve a mastermind problem with a minimum of tries
     */
    class MasterGame {
    public:
        MasterGame(U32 nbColors = 6, U32 nbPositions = 4) noexcept;

        void generateSecretCombinaison() noexcept;

        void setSecretCombinaison(Combinaison &combi) noexcept;
        const Combinaison& getSecretCombinaison() const noexcept;

        void getCorrection(const Combinaison &referenceCombinaison, const Combinaison &combiTotest, U32 &blackPigs, U32 &whitePigs) const noexcept;
        void getCorrectionOfSecretCombinaison(const Combinaison &combiTotest, U32 &blackPigs, U32 &whitePigs) const noexcept;

        friend ostream& operator<<(ostream& os, const MasterGame& masterGame) noexcept{
            os << "masterGame: SecretCombinaison" << masterGame.m_secretCombinaison
                    << " nbColors =" << masterGame.m_nbColors
                    << " nbPositions=" << masterGame.m_nbPositions;
            return os;
        }

    protected:
        tColor m_nbColors;
        U32 m_nbPositions;
        Combinaison m_secretCombinaison;
    };

}

#endif
