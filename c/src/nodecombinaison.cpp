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
//#include "mapsearchcondition.h"

namespace mastermind {

dbclient::nodeClientUdp NodeCombinaison::nodeDB("localhost",1500);//<--This is a static variable (declared in the header file in the class NodeCombinaison)

NodeCombinaison::NodeCombinaison(const Combinaison * pCombi):m_pParentCombi(NULL)
{
  if(pCombi!=NULL)
    m_combi = *pCombi;
}

/*NodeCombinaison::~NodeCombinaison()
{

}*/

//obtained by getting the level of the parent +1, if parent not NULL (recursive function)
U32 NodeCombinaison::getLevel()
{
  if(m_pParentCombi==NULL)
    return 0;
  else
    return m_pParentCombi->getLevel() + 1;
}

//set the combinaison of the node
void NodeCombinaison::setCombinaison(const Combinaison &combinaison)
{
  m_combi = combinaison;
}

//get the combinaison of the node
const Combinaison* NodeCombinaison::getCombinaison() const
{
  return &m_combi;
}

//get the combinaison of the child node corresponding to the score
NodeCombinaison* NodeCombinaison::getNodeCombinaisonForScore(tLocalScore localScore)
{
  mapNodeCombinaison::iterator itor=m_mapNodeCombinaison.find(localScore);
  if(itor==m_mapNodeCombinaison.end())
  {
#ifdef _DB_VIA_UDP_
	return retrieveNodeCombinaisonForScore(localScore);
#else
    return NULL;
#endif
  }
  else
    return &(itor->second);
}

#ifdef _DB_VIA_UDP_
NodeCombinaison* NodeCombinaison::retrieveNodeCombinaisonForScore(tLocalScore localScore)
{//TODO NEED BIG TESTING
	string result;
	if(retrieveCombinaisonForScore(localScore, result))
	{
		Combinaison combi(result);
		NodeCombinaison * nodeCombi = NULL;
		if(addCombinaison(combi,localScore,nodeCombi))
		{
			return nodeCombi;
		}
	}
	return NULL;
}

string& NodeCombinaison::buildScoreString(tLocalScore localScore, string &sScore)
{
	stringstream ss(sScore);
	buildScoreString(localScore, ss);
	sScore = ss.str();
	return sScore;
}

stringstream& NodeCombinaison::buildScoreString(tLocalScore localScore, stringstream &ssScore)
{
	ssScore << 'b' << mGET_BLACK_SCORE_FROM_LOCALSCORE(localScore)
	        << 'w' << mGET_WHITE_SCORE_FROM_LOCALSCORE(localScore);
	return ssScore;
}

void NodeCombinaison::buildScorePath(string &sScorepath)
{
	stringstream ss(sScorepath);
	sScorepath = buildScorePath(ss).str();
}

stringstream& NodeCombinaison::buildScorePath(stringstream &ssScorePath)
{
	if(m_pParentCombi!=NULL)
	{
		//mapSearchValue<tLocalScore, NodeCombinaison> mapSearchVal(*this);
		// mapNodeCombinaison::iterator it = find_if(m_pParentCombi->m_mapNodeCombinaison.begin(),
												  // m_pParentCombi->m_mapNodeCombinaison.end(),
												  // mapSearchVal.isSearchedValue);
		mapNodeCombinaison::iterator it;
		for(it = m_pParentCombi->m_mapNodeCombinaison.begin(); it!=m_pParentCombi->m_mapNodeCombinaison.end(); it++)
			if(&it->second==this)
				m_pParentCombi->buildScoreString(it->first,ssScorePath) << " ";
		return buildScorePath(ssScorePath);
	}
	else
	{
		return ssScorePath;
	}
}

bool NodeCombinaison::retrieveCombinaisonForScore(tLocalScore localScore, string &result)
{
	stringstream ssCommand;
	buildScorePath(ssCommand) << " ";
	buildScoreString(localScore, ssCommand);
	return nodeDB.sendCommand(ssCommand.str(),result);
}
#endif //_DB_VIA_UDP_

//return false, if the combinaison already exist and it is different from the new combinaison
bool NodeCombinaison::addCombinaison(const Combinaison &combi, tLocalScore localScore, NodeCombinaison *&pNewNodeCombinaison)
{
  NodeCombinaison *pNodeCombi;
  if((pNodeCombi = getNodeCombinaisonForScore(localScore))==NULL)
  {//the combi is not present, so we add it to the map
    NodeCombinaison nodeCombi(&combi);
    m_mapNodeCombinaison[localScore]=nodeCombi;
    m_mapNodeCombinaison[localScore].m_pParentCombi=this;
    pNewNodeCombinaison = &(m_mapNodeCombinaison[localScore]);
  }
  else if(*(pNodeCombi->getCombinaison())==combi)
  {//Combinaison is already present and it is the same!
    pNewNodeCombinaison = pNodeCombi;
  }
  else
  {
    cout << *(pNodeCombi->getCombinaison()) << " is different from " << combi << "score" << hex << localScore <<"\n";
    return false;
  }
  return true;
}

//build the dot file from the current node (should be the root node)
bool NodeCombinaison::buildDotFile(const char* fileName)
{
    ofstream fos("tree.dot");
    fos << "digraph g {\n";
    fos << "rankdir=LR\n";
    fos << *this;
    fos << "}" << endl;
	return true;
}

}
