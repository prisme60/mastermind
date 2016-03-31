//
// C++ Implementation: nodecombinaison
//
// Description: 
//
//
// Author:  <Christian FOUCHER>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>

#include "combinaison.h"
#include "nodecombinaison.h"


namespace mastermind {

#ifdef _DB_VIA_UDP_
    dbclient::nodeClientUdp NodeCombinaison::nodeDB("localhost", 1500); //<--This is a static variable (declared in the header file in the class NodeCombinaison)
#endif

    NodeCombinaison::NodeCombinaison(const Combinaison * pCombi) : m_pParentCombi(nullptr) {
        if (pCombi != nullptr)
            m_combi = *pCombi;
    }

    //obtained by getting the level of the parent +1, if parent not nullptr (recursive function)

    U32 NodeCombinaison::getLevel() {
        if (m_pParentCombi == nullptr)
            return 0;
        else
            return m_pParentCombi->getLevel() + 1;
    }

    //set the combinaison of the node

    void NodeCombinaison::setCombinaison(const Combinaison &combinaison) {
        m_combi = combinaison;
    }

    //get the combinaison of the node

    const Combinaison* NodeCombinaison::getCombinaison() const {
        return &m_combi;
    }

    //get the combinaison of the child node corresponding to the score

    NodeCombinaison* NodeCombinaison::getNodeCombinaisonForScore(U32 nbColors, tLocalScore localScore) {
        mapNodeCombinaison::iterator itor = m_mapNodeCombinaison.find(localScore);
        if (itor == m_mapNodeCombinaison.end()) {
#ifdef _DB_VIA_UDP_
            return retrieveNodeCombinaisonForScore(nbColors, localScore);
#else
            return nullptr;
#endif
        } else
            return &(itor->second);
    }

#ifdef _DB_VIA_UDP_

NodeCombinaison* NodeCombinaison::retrieveNodeCombinaisonForScore(U32 nbColors, tLocalScore localScore) {
        string result;
        if (retrieveCombinaisonForScore(nbColors, localScore, result) && result.find_first_of('[') != string::npos) {
            Combinaison combi(result);
            NodeCombinaison * nodeCombi = nullptr;
            if (addCombinaison(nbColors, combi, localScore, nodeCombi))//TODO NEED BIG TESTING and INVESTIGATION
            {
                return nodeCombi;
            }
        }
        return nullptr;
    }

    string& NodeCombinaison::buildScoreString(tLocalScore localScore, string &sScore) {
        stringstream ss(sScore);
        buildScoreString(localScore, ss);
        sScore = ss.str();
        return sScore;
    }

    stringstream& NodeCombinaison::buildScoreString(tLocalScore localScore, stringstream &ssScore) {
        ssScore << 'b' << mGET_BLACK_SCORE_FROM_LOCALSCORE(localScore)
                << 'w' << mGET_WHITE_SCORE_FROM_LOCALSCORE(localScore);
        return ssScore;
    }

    void NodeCombinaison::buildScorePath(string &sScorepath) {
        stringstream ss(sScorepath);
        sScorepath = buildScorePath(ss).str();
    }

    stringstream& NodeCombinaison::buildScorePath(stringstream &ssScorePath) {
        if (m_pParentCombi != nullptr) {
            mapNodeCombinaison::iterator it;
            for (it = m_pParentCombi->m_mapNodeCombinaison.begin(); it != m_pParentCombi->m_mapNodeCombinaison.end(); it++) {
                if (&it->second == this) {
                    m_pParentCombi->buildScorePath(ssScorePath);
                    m_pParentCombi->buildScoreString(it->first, ssScorePath) << " ";
                }
            }
        } else {
            ssScorePath << " b#w# ";
        }
        return ssScorePath;
    }

    stringstream& NodeCombinaison::buildCombiScorePath(stringstream &ssScorePath) {
        if (m_pParentCombi != nullptr) {
            mapNodeCombinaison::iterator it;
            for (it = m_pParentCombi->m_mapNodeCombinaison.begin(); it != m_pParentCombi->m_mapNodeCombinaison.end(); it++) {
                if (&it->second == this) {
                    m_pParentCombi->buildCombiScorePath(ssScorePath);
                    ssScorePath << m_combi;
                    buildScoreString(it->first, ssScorePath);
                }
            }
        } else {
            ssScorePath << m_combi << " b#w# ";
        }
        return ssScorePath;
    }

    bool NodeCombinaison::retrieveCombinaisonForScore(U32 nbColors, tLocalScore localScore, string &result) {
        stringstream ssCommand;
        ssCommand << "get p" << m_combi.getNbPositions() << 'c' << nbColors;
        buildScorePath(ssCommand) << " ";
        buildScoreString(localScore, ssCommand);
        return nodeDB.sendCommand(ssCommand.str(), result);
    }

    bool NodeCombinaison::storeCombinaison(U32 nbColors, string &result) {
        stringstream ssCommand;
        ssCommand << "set p" << m_combi.getNbPositions() << 'c' << nbColors;
        buildCombiScorePath(ssCommand);
        return nodeDB.sendCommand(ssCommand.str(), result);
    }
#endif //_DB_VIA_UDP_

    //return false, if the combinaison already exist and it is different from the new combinaison

    bool NodeCombinaison::addCombinaison(U32 nbColors, const Combinaison &combi, tLocalScore localScore, NodeCombinaison *&pNewNodeCombinaison, bool storeIntoDB) {
        NodeCombinaison *pNodeCombi;
        if ((pNodeCombi = getNodeCombinaisonForScore(nbColors, localScore)) == nullptr) {//the combi is not present, so we add it to the map
            NodeCombinaison nodeCombi(&combi);
            m_mapNodeCombinaison[localScore] = nodeCombi;
            m_mapNodeCombinaison[localScore].m_pParentCombi = this;
            pNewNodeCombinaison = &(m_mapNodeCombinaison[localScore]);
#ifdef _DB_VIA_UDP_
            string result;
            if (storeIntoDB)
                pNewNodeCombinaison->storeCombinaison(nbColors, result);
#endif //_DB_VIA_UDP_
        } else if (*(pNodeCombi->getCombinaison()) == combi) {//Combinaison is already present and it is the same!
            pNewNodeCombinaison = pNodeCombi;
        } else {
            cout << *(pNodeCombi->getCombinaison()) << " is different from " << combi << "score" << hex << localScore << "\n";
            return false;
        }
        return true;
    }

    //build the dot file from the current node (should be the root node)

    bool NodeCombinaison::buildDotFile(const std::string &fileName) {
        ofstream fos(fileName);
        fos << "digraph g {\n";
        fos << "rankdir=LR\n";
        fos << *this;
        fos << "}" << endl;
        return true;
    }
}
