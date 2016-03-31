//
// C++ Implementation: mastersolver
//
// Description: 
//
//
// Author:  <>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "mastersolver.h"
#include "combinaison.h"

namespace mastermind {

    MasterGame::MasterGame(tColor nbColors, U32 nbPositions) : m_nbColors(nbColors), m_nbPositions(nbPositions), m_secretCombinaison(nbPositions) {

    }

    void MasterGame::setSecretCombinaison(Combinaison &combi) {
        m_secretCombinaison = combi; //overloaded operator=
    }

    const Combinaison& MasterGame::getSecretCombinaison() const {
        return m_secretCombinaison;
    }

    void MasterGame::generateSecretCombinaison() {
        m_secretCombinaison.random(m_nbColors);
    }

    void MasterGame::getCorrection(const Combinaison &referenceCombinaison, const Combinaison &combiTotest, U32 &blackPigs, U32 &whitePigs) const {
        referenceCombinaison.getCorrection(combiTotest, blackPigs, whitePigs);
    }

    void MasterGame::getCorrectionOfSecretCombinaison(const Combinaison &combiTotest, U32 &blackPigs, U32 &whitePigs) const {
        m_secretCombinaison.getCorrection(combiTotest, blackPigs, whitePigs);
    }

}//end namespace
