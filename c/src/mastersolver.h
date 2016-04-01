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

#define MAX_COMB_BY_THREAD 15

#if NUMBER_OF_THREADS > 1
#include <pthread.h>
#endif

#include <list>
#include <vector>

#if NUMBER_OF_THREADS > 1
#include <atomic>
#endif

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

    using tScore = struct score{
        U32 blackPigs;
        U32 whitePigs;
    };
    
    using vectorCombinaison = vector<Combinaison>;
    using listCombinaison = list<Combinaison>;
    using vectorScore = vector<tScore>;

    /**
    This class permit to solve a mastermind problem with a minimum of tries

            @author 
     */
    class MasterSolver : public MasterGame {
    public:
        MasterSolver(U32 nbColors = 6, U32 nbPositions = 4) noexcept;

        virtual ~MasterSolver() noexcept;

        vectorCombinaison* solve() noexcept;

        void generateFirstPattern(Combinaison &firstPattern) noexcept;
        U32 estimationForFirstPattern() noexcept;
        bool isCombiCompatible(const Combinaison &givenTestedCombinaison, const Combinaison &combinaisonToTest, const tScore &score) const noexcept;
        U32 UpdateSolutionSet() noexcept;
        void guessNextPattern(Combinaison &nextPatternCombi, bool bFastSearch) noexcept;
        void updateFromIteration(const Combinaison &guessComb, U32 &maximumGuessScore, Combinaison &nextPatternCombi) noexcept;
        U32 testCurrentPattern(const Combinaison &currentGuessPattern) const noexcept;
        void buildScoreSet() noexcept;
        U32 countRemovals(const Combinaison &guessCombinaison, const tScore &score) const noexcept;

#if NUMBER_OF_THREADS > 1
        void createThreads() noexcept;
        void destroyThreads() noexcept;
        void updateFromIterationMT_emit(const Combinaison &guessComb) noexcept;
        void updateFromIterationMT_init() noexcept;
        void updateFromIterationMT_end(Combinaison &nextPatternCombi) noexcept;
        void* updateFromIterationWT(U32 id) noexcept;
#endif

        friend ostream& operator<<(ostream& os, const MasterSolver& masterSolver) noexcept {
            for (const auto& pastGuess : masterSolver.m_pastGuessSet) {
                os << pastGuess << "\t";
            }
            return os;
        }

    private:
        vectorScore m_scoreSet; //set of the possible score <== Never modified
        listCombinaison m_possibleSolutionSet; //Set of the possible solutions <== We add all possible Solutions first then we reduce it after
        vectorCombinaison m_pastGuessSet;
        U32 m_indexPastGuessSetTreated = 0;

        NodeCombinaison m_rootNodeCombinaison;
        NodeCombinaison *m_pCurrentNodeCombinaison = nullptr;

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

        std::atomic<U32> m_MTactivityCounter = {0};

#define ACTIVITY_COUNTER_INC() m_MTactivityCounter++;

#define ACTIVITY_COUNTER_DEC() m_MTactivityCounter--;

#define ACTIVITY_COUNTER_GET() m_MTactivityCounter.load();

        static void* thread_fun(void* args) noexcept;

        struct thread_fun_args {
            MasterSolver* This;
            // other args as required; in this case
            U32 Id;

            thread_fun_args(MasterSolver* t, U32 id) : This(t), Id(id) {
            }
        };
#endif
    };

}

#endif
