//
// C++ Implementation: combinaison
//
// Description: 
//
//
// Author:  <>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <stdlib.h>
#include <string.h>
#include "combinaison.h"

namespace mastermind {

Combinaison::Combinaison(U32 nbElements,const tColor pColorTable[]):
#ifdef COMBINAISON_DYNAMIC_ALLOCATION
m_pColorTable(NULL),
#endif// COMBINAISON_DYNAMIC_ALLOCATION
m_nbElements(0)
{
  buildCombinaison(nbElements, pColorTable);
}

Combinaison::Combinaison (const Combinaison &combi):
#ifdef COMBINAISON_DYNAMIC_ALLOCATION
m_pColorTable(NULL),
#endif// COMBINAISON_DYNAMIC_ALLOCATION
m_nbElements(0)
{
  //This function is very important because the automatic copy doesn't do a deep copy
  //(so there is no reallocation of the m_pColorTable) ==> There is a crash when we delete the table!
  *this = combi;//operator= overload
}

Combinaison::~Combinaison()
{
  #ifdef COMBINAISON_DYNAMIC_ALLOCATION
  delete[] m_pColorTable;
  #endif// COMBINAISON_DYNAMIC_ALLOCATION
}

void Combinaison::buildCombinaison(U32 nbElements, const tColor pColorTable[])
{
  #ifdef COMBINAISON_DYNAMIC_ALLOCATION
  if(m_nbElements<nbElements && m_pColorTable!=NULL)
  {//if the table of elements is too little.
    m_nbElements=nbElements;//we store the current size (so we lost the true size of the table, but it is not important)
    delete[] m_pColorTable;//we destroy the table
    m_pColorTable = new tColor[nbElements];//then we recreate it.
  }
  else if(m_pColorTable==NULL)
  {//the table is not allocated
    m_nbElements=nbElements;
    if(nbElements>0)//we allocate the table only if we have a valid nbElements
    {//avoid bad size allocation and memory leak!
      m_pColorTable = new tColor[nbElements];
    }
    else
    {
      m_pColorTable=NULL;
    }
  }
  #else //notdef COMBINAISON_DYNAMIC_ALLOCATION
    m_nbElements=nbElements;
  #endif //COMBINAISON_DYNAMIC_ALLOCATION
  if(pColorTable)
  {
    memcpy(m_pColorTable,pColorTable,sizeof(tColor)*nbElements);
  }
  else
  {
    memset(m_pColorTable,0,sizeof(tColor)*nbElements);
  }
}

bool Combinaison::getNextCombinaison(tColor nbColors)
{
  for(U32 indexElement=0;indexElement<m_nbElements;indexElement++)
  {
    if(m_pColorTable[indexElement]<nbColors-1)
    {// for the current indexElement, we have found that there is a color after the current one
       m_pColorTable[indexElement]++;
       for(U32 indexElementForNewCombi=0;indexElementForNewCombi<indexElement;indexElementForNewCombi++)
       {
            m_pColorTable[indexElementForNewCombi]=0;
       }
       return true;
    }
  }
  return false;
}

tColor Combinaison::operator[](U32 indexElement) const
{
  mLibAssert((indexElement<m_nbElements) && (m_pColorTable!=NULL));
  return m_pColorTable[indexElement];
}

tColor& Combinaison::operator[](U32 indexElement)
{
  mLibAssert((indexElement<m_nbElements) && (m_pColorTable!=NULL));
  return m_pColorTable[indexElement];
}

Combinaison& Combinaison::clone() const
{
  return *(new Combinaison(*this));
}

void Combinaison::operator=(const Combinaison &combinaisonToCopy)
{
  Combinaison::buildCombinaison(combinaisonToCopy.m_nbElements, combinaisonToCopy.m_pColorTable);
}

bool Combinaison::operator==(const Combinaison &rightCombinaison) const
{
  mLibAssert(m_pColorTable);
  if(m_pColorTable && rightCombinaison.m_pColorTable && m_nbElements!=rightCombinaison.m_nbElements)
    return false;
  for(U32 indexElement=0;indexElement<m_nbElements;indexElement++)
    if(m_pColorTable[indexElement]!=rightCombinaison.m_pColorTable[indexElement])
      return false;
  return true;
}

bool Combinaison::operator!=(const Combinaison &rightCombinaison) const
{
  return !(*this==rightCombinaison);
}

void Combinaison::random(tColor nbColor)
{
  for(U32 indexElement=0;indexElement<m_nbElements;indexElement++)
    m_pColorTable[indexElement]=rand()%nbColor;
}

bool Combinaison::contains(tColor color) const
{
  return (indexOfColor(color) != -1);
}

S32 Combinaison::indexOfColor(tColor color) const
{
  for(U32 i=0;i<m_nbElements;i++)
    if(m_pColorTable[i]==color)
      return i;
  return -1;
}

void Combinaison::getCorrection(Combinaison combiTotest, U32 &blackPigs, U32 &whitePigs) const
{
  Combinaison referenceCombinaison(*this);
  blackPigs = 0;
  whitePigs = 0;

  //cout << referenceCombinaison << combiTotest << endl;

  for(U32 i=0;i<m_nbElements;i++)
  {
    if(referenceCombinaison[i]==combiTotest[i])
    {
      referenceCombinaison[i]=~0;//we must destroy the color (so we use an unused color number!)
      combiTotest         [i]=~1;//we must destroy the color (so we use another unused color number!)
      //now the color couldn't be used for a not well place color!
      blackPigs++;
    }
  }

  //cout << referenceCombinaison << combiTotest << endl;
  //determine good color but not well place
  for(U32 i=0;i<m_nbElements;i++)
  {
    S32 colorIndex;
    if((colorIndex=referenceCombinaison.indexOfColor(combiTotest[i]))!=-1)
    {
      referenceCombinaison[colorIndex]=~2;//destroy the the first occurence of the color
      whitePigs++;
    }
  }
  //cout << referenceCombinaison << combiTotest << endl;
}

bool Combinaison::isCombinaisonCompatible(const Combinaison &combiToTest, U32 wantedBlackPigs, U32 wantedWhitePigs) const
{
  U32 computedBlackPigs, computedWhitePigs;
  getCorrection(combiToTest,computedBlackPigs,computedWhitePigs);
  return ((wantedBlackPigs==computedBlackPigs)&&(wantedWhitePigs==computedWhitePigs));
}

}//end namespace
