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
#ifndef MASTERMINDMASTERSOLVER_H
#define MASTERMINDMASTERSOLVER_H

#define NUMBER_OF_THREADS 1
#define MAX_COMB_BY_THREAD 15

#if NUMBER_OF_THREADS > 1
	#include <pthread.h>
#endif

#include <list>
#include <vector>

#include "typedefinition.h"

#include "mastergame.h"
#include "nodecombinaison.h"

using namespace std;

namespace mastermind {

#define MIN_SOLUTIONSET_SIZE_FOR_FASTSEARCH 100
#define ITERATION_FOR_DUPLICATION_SIMULATION 1000

#if NUMBER_OF_THREADS > 1
	#ifdef __DEBUG__
		#define	D_BEGIN pthread_mutex_lock(&MT_DEBUG_mutex);
		#define	D_END pthread_mutex_unlock(&MT_DEBUG_mutex);
	#else
		#define	D_BEGIN 
		#define	D_END 
	#endif
#else
	#define	D_BEGIN 
	#define	D_END 
#endif

typedef vector<Combinaison> vectorCombinaison;
typedef list<Combinaison>   listCombinaison;
typedef vector<tScore>      vectorScore;

/**
This class permit to solve a mastermind problem with a minimum of tries

	@author 
*/
class MasterSolver: public MasterGame{
public:
    MasterSolver(U32 nbColors=6, U32 nbPositions=4);

    virtual ~MasterSolver();

    vectorCombinaison* solve();

    void generateFirstPattern(Combinaison &firstPattern);
    U32  estimationForFirstPattern();
    bool isCombiCompatible(Combinaison &givenTestedCombinaison, Combinaison &combinaisonToTest,tScore &score);
    U32  UpdateSolutionSet();
    void guessNextPattern(Combinaison &nextPatternCombi, bool bFastSearch);
	void updateFromIteration(const Combinaison &guessComb, U32 &maximumGuessScore, Combinaison &nextPatternCombi);
    U32  testCurrentPattern(const Combinaison &currentGuessPattern);
    void buildScoreSet();
    U32  countRemovals(const Combinaison &guessCombinaison,tScore &score);
	
#if NUMBER_OF_THREADS > 1
	void createThreads();
	void destroyThreads();
	void updateFromIterationMT_emit(const Combinaison &guessComb);
	void updateFromIterationMT_init();
	void updateFromIterationMT_end(Combinaison &nextPatternCombi);
	void* updateFromIterationWT(void *t);
#endif

    friend ostream& operator<<(ostream& os, const MasterSolver& masterSolver)
    {
      for(vectorCombinaison::const_iterator itor = masterSolver.m_pastGuessSet.begin(); itor != masterSolver.m_pastGuessSet.end(); ++itor)
      {
        os << *itor << "\t";
      }
      return os;
    }

private:
    vectorScore       m_scoreSet;//set of the possible score <== Never modified
    listCombinaison   m_possibleSolutionSet;//Set of the possible solutions <== We add all possible Solutoins first then we reduce it after
    vectorCombinaison m_pastGuessSet;
    U32 m_indexPastGuessSetTreated;

    NodeCombinaison  m_rootNodeCombinaison;
    NodeCombinaison *m_pCurrentNodeCombinaison;
	
	#if NUMBER_OF_THREADS > 1
	//these variables are only used by the Multithread version (MT)
    pthread_t threads[NUMBER_OF_THREADS];
    pthread_attr_t attr;
	
	#define STATE_NO_SIGNAL 0
	typedef U32 tSignalState;
	
	#ifdef __DEBUG__
	pthread_mutex_t MT_DEBUG_mutex;
	#endif
	
	vectorCombinaison m_MTvectorComb;
	pthread_mutex_t MTcomb_mutex;
    pthread_cond_t MTcomb_cond;
	
	U32 m_MTmaximumGuessScore;
	Combinaison m_MTnextPatternCombi;
	volatile tSignalState m_MTSignalStateMaximumGuessScore;
	pthread_mutex_t MTmaximumGuessScore_mutex;
    pthread_cond_t MTmaximumGuessScore_cond;
	
	bool m_MTthreadExit[NUMBER_OF_THREADS];
	
	U32 m_MTactivityCounter;
	pthread_mutex_t MTactivityCounter_mutex;
	
	#define ACTIVITY_COUNTER_INC(id) pthread_mutex_lock (&MTactivityCounter_mutex);\
		/*D_BEGIN cerr << id <<" # Line A:" << __LINE__ << endl; D_END*/\
		m_MTactivityCounter++;\
		/*D_BEGIN cerr << id <<" # Line B:" << __LINE__ << "INC ActivityCounter: " << m_MTactivityCounter <<endl; D_END*/\
		pthread_mutex_unlock (&MTactivityCounter_mutex);
		
	#define ACTIVITY_COUNTER_DEC(id) pthread_mutex_lock (&MTactivityCounter_mutex);\
		/*D_BEGIN cerr << id <<" # Line A:" << __LINE__ << endl; D_END*/\
		m_MTactivityCounter--;\
		/*D_BEGIN cerr << id <<" # Line B:" << __LINE__ << "DEC ActivityCounter: " << m_MTactivityCounter <<endl; D_END*/\
		pthread_mutex_unlock (&MTactivityCounter_mutex);
		
	#define ACTIVITY_COUNTER_GET(GetVariable)  pthread_mutex_lock (&MTactivityCounter_mutex);\
		/*D_BEGIN cerr << "MT # Line A:" << __LINE__ << endl; D_END*/\
		GetVariable = m_MTactivityCounter;\
		pthread_mutex_unlock (&MTactivityCounter_mutex);\
		/*D_BEGIN cerr << "MT # Line B:" << __LINE__ << "GET ActivityCounter: " << GetVariable <<endl; D_END*/
		
	static void* thread_fun( void* args );
	struct thread_fun_args
	{
		MasterSolver* This ;
		// other args as required; in this case
		U32 Id;
		
		thread_fun_args( MasterSolver* t, U32 id ) : This(t), Id(id) {}
	};
	#endif
};

}

#endif
