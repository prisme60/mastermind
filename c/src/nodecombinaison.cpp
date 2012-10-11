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

#include "combinaison.h"
#include "nodecombinaison.h"

namespace mastermind {

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
    return NULL;
  else
    return &(itor->second);
}

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
    return true;
  }
  else if(*(pNodeCombi->getCombinaison())==combi)
  {//Combinaison is already present and it is the same!
    pNewNodeCombinaison = pNodeCombi;
    return true;
  }
  else
  {
    cout << *(pNodeCombi->getCombinaison()) << " is different from " << combi << "score" << hex << localScore <<"\n";
    return false;
  }
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
