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

	@author 
*/
class MasterGame{
public:
    MasterGame(U32 nbColors=6, U32 nbPositions=4);

//    ~MasterGame();

    void generateSecretCombinaison();

    void setSecretCombinaison(Combinaison &combi);
    const Combinaison& getSecretCombinaison() const;

    void getCorrection(const Combinaison &referenceCombinaison, const Combinaison &combiTotest, U32 &blackPigs, U32 &whitePigs);
    void getCorrectionOfSecretCombinaison(                      const Combinaison &combiTotest, U32 &blackPigs, U32 &whitePigs);

    friend ostream& operator<<(ostream& os, const MasterGame& masterGame)
    {
      os << "masterGame: SecretCombinaison" << masterGame.m_secretCombinaison 
         <<" nbColors ="  << masterGame.m_nbColors
         <<" nbPositions="<< masterGame.m_nbPositions;
      return os;
    }

protected:
    tColor m_nbColors;
    U32    m_nbPositions;
    Combinaison m_secretCombinaison;
};

}

#endif
