//
// C++ Interface: nodecombinaison
//
// Description: 
//
//
// Author:  <Christian FOUCHER>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MASTERMINDNODECOMBINAISON_H
#define MASTERMINDNODECOMBINAISON_H

#include <map>

#include "typedefinition.h"

#ifdef _DB_VIA_UDP_
#include "node-client-udp.h"
#endif //_DB_VIA_UDP_

using namespace std;

namespace mastermind {

#define BLACK_PIGS_SHIFT (16)
#define WHITE_PIGS_SHIFT ( 0)
#define PIGS_MASK        (0x0000FFFFUL)
#define mCONVERT_SCORE_TO_LOCALSCORE(black_pigs,white_pigs)\
((tLocalScore)((black_pigs<<BLACK_PIGS_SHIFT)|(white_pigs<<WHITE_PIGS_SHIFT)))
#define mGET_BLACK_SCORE_FROM_LOCALSCORE(localScore) (((localScore)>>BLACK_PIGS_SHIFT) & PIGS_MASK)
#define mGET_WHITE_SCORE_FROM_LOCALSCORE(localScore) (((localScore)>>WHITE_PIGS_SHIFT) & PIGS_MASK)

    using tLocalScore = U32; //0xXXXXYYYY ==> XXXX = black pigs  /  YYYY = white pigs

    /**
    This class permit to solve a mastermind problem with a minimum of tries
     */
    class NodeCombinaison {
    public:
        NodeCombinaison(const Combinaison * pCombi = nullptr) noexcept;

        U32 getLevel() noexcept; //obtained by getting the level of the parent +1, if parent not nullptr (recursive function)

        void setCombinaison(const Combinaison &combinaison) noexcept; //set the combinaison of the node
        const Combinaison* getCombinaison() const noexcept; //get the combinaison of the node
        NodeCombinaison* getNodeCombinaisonForScore(U32 nbColors, tLocalScore localScore) noexcept; //get the combinaison of the child node corresponding to the score
        bool addCombinaison(U32 nbColors, const Combinaison &combi, tLocalScore localScore, NodeCombinaison *&newNodeCombinaison, bool storeIntoDB = false) noexcept; //return false, if the combinaison already exist and it is different from the new combinaison

        //TODO add a dot output script for graph generation
        bool buildDotFile(const std::string &fileName) noexcept;

#ifdef _DB_VIA_UDP_
        NodeCombinaison* retrieveNodeCombinaisonForScore(U32 nbColors, tLocalScore localScore);
        bool retrieveCombinaisonForScore(U32 nbColors, tLocalScore localScore, string &result);
        bool storeCombinaison(U32 nbColors, string &result);

        static string& buildScoreString(tLocalScore localScore, string &sScore);
        static stringstream& buildScoreString(tLocalScore localScore, stringstream &ssScore);

        void buildScorePath(string &sScorepath);
        stringstream& buildScorePath(stringstream &ssScorePath);

        stringstream& buildCombiScorePath(stringstream &ssScorePath);

#endif //_DB_VIA_UDP_

        typedef map<tLocalScore, NodeCombinaison> mapNodeCombinaison;

        friend ostream& operator<<(ostream& os, const NodeCombinaison& nodeCombinaison)  noexcept{
            static U32 indexNode = 0;
            U32 indexNodeParent = indexNode;
            os << "node" << indexNodeParent << "[label=\"" << nodeCombinaison.m_combi << "\"]\n";
            for (const auto& scoreCombinaison : nodeCombinaison.m_mapNodeCombinaison) {
                const Combinaison * combiChild = scoreCombinaison.second.getCombinaison();

                os << "node" << indexNodeParent << " -> " << "node" << ++indexNode;
                os << " [label=\"(";
                os << mGET_BLACK_SCORE_FROM_LOCALSCORE(scoreCombinaison.first) << " , ";
                os << mGET_WHITE_SCORE_FROM_LOCALSCORE(scoreCombinaison.first) << ")\"]\n";

                os << "node" << indexNode << "[label=\"" << *combiChild << "\"]\n";

                os << scoreCombinaison.second; //it is a recursive call of the child
            }
            return os;
        }

    private:
        mapNodeCombinaison m_mapNodeCombinaison;
        Combinaison m_combi;
        NodeCombinaison *m_pParentCombi = nullptr;

#ifdef _DB_VIA_UDP_
        static dbclient::nodeClientUdp nodeDB; //("localhost",1500);
#endif
    };

}

#endif //MASTERMINDNODECOMBINAISON_H
