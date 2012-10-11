/***************************************************************************
 *   Copyright (C) 2005 by christian Foucher   *
 *   christian@localhost   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/*
* $Log$
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <time.h>

#include "mastersolver.h"

using namespace std;
using namespace mastermind;

#define DEFAULT_NB_POSITIONS 5
#define DEFAULT_NB_COLORS 6

#define STR_FULL_ENUMERATION   "-F"
#define STR_RANDOM_COMBINAISON "-R"
#define STR_UNITARY_TEST       "-U"
#define STR_HELP               "-H"

#define STR_NB_COLORS          "--nbColors="
#define STR_NB_POSITIONS       "--nbPositions="

typedef enum{
  cCHOICE_FULL_ENUMERATION,
  cCHOICE_RANDOM_COMBINAISON,
  cCHOICE_UNITARY_TEST,
  cCHOICE_HELP,
} eChoice;

static void help();
static void fullEnumeration(U32 nbColors, U32 nbPositions);
static void randomCombinaison(U32 nbColors, U32 nbPositions);
static void unitaryTest(U32 nbColors, U32 nbPositions);

int main(int argc, char *argv[])
{
  U32 nb_Colors = DEFAULT_NB_COLORS;
  U32 nb_Positions = DEFAULT_NB_POSITIONS;
  eChoice choice = cCHOICE_FULL_ENUMERATION;

  if(argc > 1)
  {
    for(S32 i=1;i<argc;i++)
    {
      if      (strcasecmp(argv[i],STR_FULL_ENUMERATION)==0)
        choice=cCHOICE_FULL_ENUMERATION;
      else if (strcasecmp(argv[i],STR_RANDOM_COMBINAISON)==0)
        choice=cCHOICE_RANDOM_COMBINAISON;
      else if (strcasecmp(argv[i],STR_UNITARY_TEST)==0)
        choice=cCHOICE_UNITARY_TEST;
      else if (strcasecmp(argv[i],STR_HELP)==0)
        choice=cCHOICE_HELP;
      else if (strncasecmp(argv[i],STR_NB_COLORS,strlen(STR_NB_COLORS))==0)
      {
        nb_Colors = strtoul(argv[i]+strlen(STR_NB_COLORS),NULL/*no invalid character treatment*/,10/*decimal base*/);
      }
      else if (strncasecmp(argv[i],STR_NB_POSITIONS,strlen(STR_NB_POSITIONS))==0)
      {
        nb_Positions = strtoul(argv[i]+strlen(STR_NB_POSITIONS),NULL/*no invalid character treatment*/,10/*decimal base*/);
      }
      else
      {
        choice=cCHOICE_HELP;
        cerr << "invalid options or choice" << endl;
      }
    }
  }
  
  cerr << "nb_Positions=" << nb_Positions << "\t nb_Colors=" << nb_Colors << endl;
  if(nb_Positions <1 || nb_Colors <1)
  {
    cerr << "Invalid position or color number" << endl;
    return 1;
  }

  switch(choice)
  {
      case cCHOICE_FULL_ENUMERATION:  fullEnumeration  (nb_Colors, nb_Positions);break;
      case cCHOICE_RANDOM_COMBINAISON:randomCombinaison(nb_Colors, nb_Positions);break;
      case cCHOICE_UNITARY_TEST:      unitaryTest      (nb_Colors, nb_Positions);break;
      default:
      case cCHOICE_HELP:              help();                                    break;
  }

  return 0;
}

#define xstr(s) str(s)
#define str(s) #s

static void help()
{
  cerr << "Mastermind by Christian FOUCHER" << "\n"
       << "\tChoices:"                        << "\n"
       << "\t\t"STR_FULL_ENUMERATION  "    Full Enumeration"   << "\n"
       << "\t\t"STR_RANDOM_COMBINAISON"    Random Combinaison" << "\n"
       << "\t\t"STR_UNITARY_TEST      "    Unitary Test"       << "\n"
       << "\t\t"STR_HELP              "    Help"               << "\n"
       << "\tOptions:"                                     << "\n"
       << "\t\t"STR_NB_COLORS         "    Set the number of colors (default value is " << DEFAULT_NB_COLORS << ")"      << "\n"
       << "\t\t"STR_NB_POSITIONS      " Set the number of positions(default value is " << DEFAULT_NB_POSITIONS << ")" << "\n"
	   << "\n"
	   << "\t" xstr(CLOCKS_PER_SEC)   " Clock per seconds (used for computing the time elapsed)" << "\n"
#ifdef COMBINAISON_DYNAMIC_ALLOCATION
       << "This software has been compiled with a static allocation for the combinaison\n"
       << "so you mustn't choose a value superior than " << COMBINAISON_STATIC_ALLOCATION_SIZE
#endif// COMBINAISON_DYNAMIC_ALLOCATION
       << endl;
}

static void fullEnumeration(U32 nbColors, U32 nbPositions)
{
  MasterSolver master(nbColors,nbPositions);
  Combinaison combi(nbPositions);
  //double msTotalTime=0;
  combi.buildCombinaison(nbPositions);//reset the combinaison!
  clock_t clock1 = clock()/*, clock2*/;
  do
  {
    //double tempTime;
    master.setSecretCombinaison(combi);
    master.solve();
#ifndef NO_RESULT_OUTPUT
    cout << master << "\n";
#endif
    //clock2=clock();  tempTime = difftime(clock2,clock1)/CLOCKS_PER_SEC; clock1=clock2;
    //msTotalTime += tempTime;
    //cout << "Elapsed time =" << tempTime << "ms\n";
  }
  while(combi.getNextCombinaison(nbColors));

  cout << "Total elapsed time =" << difftime(clock(),clock1) << "ms\n";
  cout << endl;
}

static void randomCombinaison(U32 nbColors, U32 nbPositions)
{
  MasterSolver master(nbColors,nbPositions);
  Combinaison combi(nbPositions);

  while(1)
  {
    clock_t clock1 = clock();
    master.generateSecretCombinaison();
    master.solve();
    cout << master << "\n";
    cout << "Elapsed time =" << difftime(clock(),clock1)/CLOCKS_PER_SEC << "ms\n";
  }
}

static void unitaryTest(U32 nbColors, U32 nbPositions)
{
  MasterSolver master(nbColors,nbPositions);
  Combinaison combi(nbPositions);
  Combinaison combi2(nbPositions);
  U32 white, black, white2, black2;

  combi.buildCombinaison(nbPositions);//reset the combinaison!
  do
  {
    combi2.buildCombinaison(nbPositions);//reset the combinaison!
    do
    {
      master.setSecretCombinaison(combi);
      master.getCorrectionOfSecretCombinaison(combi2,black,white);
      combi2.getCorrection(combi,black2,white2);
      cout << combi << "<==>" << combi2 << "correction(" << black << "," << white << ")";
      if(black!=black2 || white!=white2)
        cout << "##Error##(" << black2 << "," << white2 << ")\n";
      else
        cout << "\n";
    }
    while(combi2.getNextCombinaison(nbColors));
  }
  while(combi.getNextCombinaison(nbColors));
}


