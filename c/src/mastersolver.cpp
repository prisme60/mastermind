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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "mastersolver.h"
#include "combinaison.h"
#include "nodecombinaison.h"

namespace mastermind {

    MasterSolver::MasterSolver(U32 nbColors, U32 nbPositions) noexcept : MasterGame(nbColors, nbPositions) {
        buildScoreSet();

        Combinaison combi(m_nbPositions);
        generateFirstPattern(combi);
        m_rootNodeCombinaison.setCombinaison(combi);
#if NUMBER_OF_THREADS > 1
        createThreads();
#endif
    }

    MasterSolver::~MasterSolver() noexcept {
#if NUMBER_OF_THREADS > 1
        destroyThreads();
#endif
    }

    vectorCombinaison* MasterSolver::solve() noexcept {
        U32 solutionSetSize;
        //reset previous resolve variables
        m_pastGuessSet.clear();
        m_possibleSolutionSet.clear();
        m_indexPastGuessSetTreated = 0;

        Combinaison combi(m_nbPositions);
        m_pCurrentNodeCombinaison = &m_rootNodeCombinaison;
        combi = *(m_pCurrentNodeCombinaison->getCombinaison()); //we retrieve the first pattern

        U32 blackPigs, whitePigs;
        getCorrectionOfSecretCombinaison(combi, blackPigs, whitePigs);
#ifndef NO_RESULT_OUTPUT
        cout << "Correction for combinaison: " << combi << " is " << "[" << blackPigs << "," << whitePigs << "]\n";
        cout << "Secret combinaison is       " << getSecretCombinaison() << "\n";
#endif

        m_pastGuessSet.push_back(combi);

        while ((solutionSetSize = UpdateSolutionSet()) > 1) {
            //Check if the pattern has been already compute for this correction! 
            NodeCombinaison *pNodeCombi = m_pCurrentNodeCombinaison->getNodeCombinaisonForScore(m_nbColors, mCONVERT_SCORE_TO_LOCALSCORE(blackPigs, whitePigs));

            if (pNodeCombi != nullptr && pNodeCombi->getCombinaison() != nullptr) {//the NodeCombi was already previously computed!
                combi = *(pNodeCombi->getCombinaison());
                m_pCurrentNodeCombinaison = pNodeCombi;
            } else {//the combi was not already computed
                guessNextPattern(combi, solutionSetSize > MIN_SOLUTIONSET_SIZE_FOR_FASTSEARCH); //it is possible to use combi because it has been saved in the m_pastGuessSet vector
                //we keep the computed combi for the next resolution. We store it into the NodeCombinaison
                if (m_pCurrentNodeCombinaison->addCombinaison(m_nbColors, combi, mCONVERT_SCORE_TO_LOCALSCORE(blackPigs, whitePigs), m_pCurrentNodeCombinaison) == false) {
                    cout << "An error occur #1 on combi " << combi << " NodeLevel=" << m_pCurrentNodeCombinaison->getLevel() << "\n";
                    exit(50);
                }
            }
            getCorrectionOfSecretCombinaison(combi, blackPigs, whitePigs);
#ifndef NO_RESULT_OUTPUT
            cout << "Correction for combinaison: " << combi << " is " << "[" << blackPigs << "," << whitePigs << "]" << "NodeLevel=" << m_pCurrentNodeCombinaison->getLevel() << "\n";
#endif
            m_pastGuessSet.push_back(combi);
            //cout << "Check guess Combi of Vector: " << m_pastGuessSet.back() <<"\n";
        }

        //  cout << "Correction for combinaison: " << combi << " is " << "["<< blackPigs <<","<< whitePigs <<"]\n";

        if (combi != m_secretCombinaison)//why getSecretCombinaison() is not working?
        {
            //cout << "Correction for combinaison: " << m_possibleSolutionSet.front() << "is [4,0]\n";
            if (m_possibleSolutionSet.size() > 0) {
                if (m_pCurrentNodeCombinaison->addCombinaison(m_nbColors, m_possibleSolutionSet.front(), mCONVERT_SCORE_TO_LOCALSCORE(blackPigs, whitePigs), m_pCurrentNodeCombinaison, true) == false) {
#ifndef NO_RESULT_OUTPUT
                    cout << "An error occur #2 on combi " << combi << " NodeLevel=" << m_pCurrentNodeCombinaison->getLevel() << " SetSize=" << m_possibleSolutionSet.size();
#endif		
                }
                m_pastGuessSet.push_back(m_possibleSolutionSet.front());
#ifndef NO_RESULT_OUTPUT
                cout << "supplement\n";
#endif
            } else
                cerr << "m_possibleSolutionSet is empty!!!!" << endl;
        }

        m_rootNodeCombinaison.buildDotFile("tree.dot");

        //cout << "Number of guesses:" << m_pastGuessSet.size() << "\n";
        return &m_pastGuessSet;
    }

    void MasterSolver::generateFirstPattern(Combinaison &firstPattern) noexcept {
        U32 nbDuplicate = estimationForFirstPattern();
        U32 * pGuessCombinaison;
        pGuessCombinaison = new U32[m_nbPositions];
        for (U32 indexPigs = 0; indexPigs < m_nbPositions; indexPigs++) {
            pGuessCombinaison[indexPigs] = indexPigs / nbDuplicate;
        }
        firstPattern.buildCombinaison(m_nbPositions, pGuessCombinaison);
        delete pGuessCombinaison;
    }

    U32 MasterSolver::estimationForFirstPattern() noexcept {
        U32 * pColorCountTable = new U32[m_nbColors];
        U32 * pDuplicates = new U32[m_nbPositions];

        memset(pDuplicates, 0, m_nbPositions * sizeof (U32));
        for (U32 indexCombi = 0; indexCombi < ITERATION_FOR_DUPLICATION_SIMULATION; indexCombi++) {
            memset(pColorCountTable, 0, m_nbColors * sizeof (U32));
            for (U32 indexElement = 0; indexElement < m_nbPositions; indexElement++) {
                pColorCountTable[rand() % m_nbColors]++;
            }
            U32 max = 0;
            for (U32 indexColor = 0; indexColor < m_nbColors; indexColor++) {
                if (pColorCountTable[indexColor] > max) {
                    max = pColorCountTable[indexColor];
                }
            }
            pDuplicates[max - 1]++;
        }

        U32 indexOfMaxDuplicate = 0;
#ifndef NO_RESULT_OUTPUT
        cout << "simulation result[";
#endif
        for (U32 max = 0, indexElement = 0; indexElement < m_nbPositions; indexElement++) {
#ifndef NO_RESULT_OUTPUT
            cout << pDuplicates[indexElement];
#endif
            if (pDuplicates[indexElement] > max) {
                max = pDuplicates[indexElement];
                indexOfMaxDuplicate = indexElement;
            }
#ifndef NO_RESULT_OUTPUT
            if (indexElement < m_nbPositions - 1)
                cout << ",";
#endif
        }
#ifndef NO_RESULT_OUTPUT
        cout << "]=" << indexOfMaxDuplicate + 1 << "\n";
#endif
        delete[] pColorCountTable;
        delete[] pDuplicates;

        return indexOfMaxDuplicate + 1;
    }

    bool MasterSolver::isCombiCompatible(const Combinaison &givenTestedCombinaison, const Combinaison &combinaisonToTest, const tScore &score) const noexcept {
        //Permit to know if the guessCombinaisonResult is compatible with the given tested combinaison"
        U32 blackPigs, whitePigs;
        getCorrection(combinaisonToTest, givenTestedCombinaison, blackPigs, whitePigs);
        return ((blackPigs == score.blackPigs)&&(whitePigs == score.whitePigs));
    }

    /**
     *return the number of element in the solution Set
     */
    U32 MasterSolver::UpdateSolutionSet() noexcept {
        U32 blackPigs, whitePigs;
        if (m_possibleSolutionSet.empty())//if there is no reasearch done before, we have to consider all solutions
        {
            //cout << "First SolutionSet compute\n";
            Combinaison currentCombi(m_nbPositions); //we set the first combi
            //we can use the comparaison with the SecretCombinaison only with already guessedCombinaison
            getCorrectionOfSecretCombinaison(m_pastGuessSet.front(), blackPigs, whitePigs);
            tScore score = {blackPigs, whitePigs};
            bool bIteration = true;
            while (bIteration) {
                if (isCombiCompatible(m_pastGuessSet.front(), currentCombi, score))
                    m_possibleSolutionSet.push_back(currentCombi); //we populate valid solution to the set
                bIteration = currentCombi.getNextCombinaison(m_nbColors);
            }
            m_indexPastGuessSetTreated = 1;
            //cout << "END Construct first set solution => len=" << m_possibleSolutionSet.size() << "\n";
        } else {
            //it is now an incremental research: we eliminate not compatible solutions
            //cout << "Incremental Research\n";
            for (; m_indexPastGuessSetTreated < m_pastGuessSet.size(); m_indexPastGuessSetTreated++) {
                getCorrectionOfSecretCombinaison(m_pastGuessSet[m_indexPastGuessSetTreated], blackPigs, whitePigs); //OK with guessesCombinaisons
                for (listCombinaison::iterator itorOnSol = m_possibleSolutionSet.begin(); itorOnSol != m_possibleSolutionSet.end();) {
                    tScore score = {blackPigs, whitePigs};
                    if (!isCombiCompatible(m_pastGuessSet[m_indexPastGuessSetTreated], *itorOnSol, score))
                        itorOnSol = m_possibleSolutionSet.erase(itorOnSol);
                    else
                        itorOnSol++;
                }
            }

            //cout << "END incremental research solution len " << m_possibleSolutionSet.size() << "\n";
        }
        return m_possibleSolutionSet.size();
    }

    void MasterSolver::guessNextPattern(Combinaison &nextPatternCombi, bool bFastSearch) noexcept {
#if NUMBER_OF_THREADS <= 1
        U32 maximumGuessScore = 0;
#else
        updateFromIterationMT_init();
#endif
        if (bFastSearch) {
            for (const auto & possibleSolution : m_possibleSolutionSet) {
#if NUMBER_OF_THREADS <= 1
                updateFromIteration(possibleSolution, maximumGuessScore, nextPatternCombi);
#else
                updateFromIterationMT_emit(possibleSolution);
#endif
            }
        } else {
            Combinaison guess(m_nbPositions); //we set the guess to the first combi
            bool bIteration = true;
            while (bIteration) {
#if NUMBER_OF_THREADS <= 1
                updateFromIteration(guess, maximumGuessScore, nextPatternCombi);
#else
                updateFromIterationMT_emit(guess);
#endif
                bIteration = guess.getNextCombinaison(m_nbColors);
            }
        }
#if NUMBER_OF_THREADS > 1
        //cout << "MT # fin du travail pour les WT" << endl;
        //Wait the end of all threads and retrieve result
        updateFromIterationMT_end(nextPatternCombi);
        //cout << "MT # mise � jour des variables algo � partir des variables MT" << endl;
#endif
    }

#if NUMBER_OF_THREADS <= 1

    void MasterSolver::updateFromIteration(const Combinaison &guessComb, U32 &maximumGuessScore, Combinaison &nextPatternCombi) noexcept {
        U32 thisScore = testCurrentPattern(guessComb);
        if (thisScore > maximumGuessScore) {
            //cout << "guessComb " << guessComb << " number_of_removals score: " << thisScore<< " BETTER\n";
            maximumGuessScore = thisScore;
            nextPatternCombi = guessComb;
        }
    }
#else

    void MasterSolver::createThreads() noexcept {
        for (U32 t = 0; t < NUMBER_OF_THREADS; t++) {
            m_MTthreadExit[t] = true;
        }

        m_MTactivityCounter = 0;
        pthread_mutex_init(&MTactivityCounter_mutex, nullptr);

#ifdef __DEBUG__
        pthread_mutex_init(&MT_DEBUG_mutex, nullptr);
#endif

        /* Initialize mutex and condition variable objects */
        pthread_mutex_init(&MTcomb_mutex, nullptr);
        pthread_cond_init(&MTcomb_cond, nullptr);

        pthread_mutex_init(&MTmaximumGuessScore_mutex, nullptr);
        pthread_cond_init(&MTmaximumGuessScore_cond, nullptr);

        //Initialize and set thread detached attribute
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        for (U32 t = 0; t < NUMBER_OF_THREADS; t++) {
            //D_BEGIN cerr <<	"MT # Create thread " << t << endl; D_END
            //D_BEGIN cerr << "MT # this: " << this << endl; D_END
            int rc = pthread_create(&threads[t], &attr, thread_fun, new thread_fun_args(this, t));
            if (rc) {
                //D_BEGIN cerr <<	"MT # ERROR; return code from pthread_create() is "<< rc << endl; D_END
                exit(-1);
            }
        }

        //Free attribute
        pthread_attr_destroy(&attr);
    }

    void MasterSolver::destroyThreads() noexcept {
        for (U32 t = 0; t < NUMBER_OF_THREADS; t++) {
            m_MTthreadExit[t] = false;
        }

#ifdef __DEBUG__
        pthread_mutex_destroy(&MT_DEBUG_mutex);
#endif

        pthread_mutex_destroy(&MTcomb_mutex);
        pthread_cond_destroy(&MTcomb_cond);

        pthread_mutex_destroy(&MTmaximumGuessScore_mutex);
        pthread_cond_destroy(&MTmaximumGuessScore_cond);

        pthread_mutex_destroy(&MTactivityCounter_mutex);
    }

    void MasterSolver::updateFromIterationMT_init() noexcept {
        //don't need to protect the variable when changing it, because the working threads (WT) are waiting comb input from the MT thread
        m_MTmaximumGuessScore = 0;
    }

    void MasterSolver::updateFromIterationMT_emit(const Combinaison &guessComb) noexcept {
        //D_BEGIN cerr << "MT # EMIT BEGIN" << endl; D_END
        //transmit request to a thread
        pthread_mutex_lock(&MTcomb_mutex);
        //D_BEGIN cerr << "MT # EMIT AFTER pthread_mutex_lock (&MTcomb_mutex);" << endl; D_END
        m_MTvectorComb.push_back(guessComb); //it is a deep copy (we can't do only a pointer copy, because this variable is shared with other threads)
        //D_BEGIN cerr << "MT # EMIT AFTER Setting input comb" << endl; D_END
        //D_BEGIN cerr << "MT # EMIT number of comb inputs [" << m_MTvectorComb.size() << "]" << endl; D_END
        pthread_cond_signal(&MTcomb_cond);
        //D_BEGIN cerr << "MT # EMIT AFTER pthread_cond_signal(&MTcomb_cond);" << endl; D_END
        pthread_mutex_unlock(&MTcomb_mutex);
        //D_BEGIN cerr << "MT # EMIT AFTER pthread_mutex_unlock (&MTcomb_mutex);" << endl; D_END
        //static U32 dontKeepTheMonopole = 0;
        // if(++dontKeepTheMonopole%MAX_COMB_BY_THREAD==0)
        // usleep(1);
    }

    void MasterSolver::updateFromIterationMT_end(Combinaison &nextPatternCombi) noexcept {
        bool inputCombEmpty = false;
        U32 numberOfActiveThread;
        U32 lossOfTimeA = 0, lossOfTimeB = 0;

        //D_BEGIN cerr << "MT # Line :" << __LINE__ << endl; D_END
        pthread_mutex_lock(&MTmaximumGuessScore_mutex);
        m_MTSignalStateMaximumGuessScore = 0;
        pthread_mutex_unlock(&MTmaximumGuessScore_mutex);
        //D_BEGIN cerr << "MT # Line :" << __LINE__ << endl; D_END

        pthread_mutex_lock(&MTcomb_mutex);
        //check that the input comb is empty
        inputCombEmpty = m_MTvectorComb.empty();
        pthread_mutex_unlock(&MTcomb_mutex);
        //D_BEGIN cerr << "MT # Line :" << __LINE__ << endl; D_END

        while (!inputCombEmpty) {
            pthread_mutex_lock(&MTmaximumGuessScore_mutex);
            //We wait then we will check a new time that the input combi vect is empty after a result is sent.
            if (m_MTSignalStateMaximumGuessScore == 0) {
                //D_BEGIN cerr << "MT # Line :" << __LINE__ << endl; D_END
                pthread_cond_wait(&MTmaximumGuessScore_cond, &MTmaximumGuessScore_mutex);
                //D_BEGIN cerr << "MT # Line :" << __LINE__ << endl; D_END
            }
            m_MTSignalStateMaximumGuessScore = 0;
            pthread_mutex_unlock(&MTmaximumGuessScore_mutex);

            //D_BEGIN cerr << "MT # Line :" << __LINE__ << endl; D_END
            pthread_mutex_lock(&MTcomb_mutex);
            //D_BEGIN cerr << "MT # Line :" << __LINE__ << endl; D_END
            //check that the input comb is empty
            inputCombEmpty = m_MTvectorComb.empty();
            pthread_mutex_unlock(&MTcomb_mutex);
            //D_BEGIN cerr << "MT # Line :" << __LINE__ << endl; D_END
            lossOfTimeA++;
            //D_BEGIN cerr << "MT # Display Loss of time :" << lossOfTimeA << endl; D_END
        }

        ///////////////////////////////////////////////////////////////
        //D_BEGIN cerr << "MT # Line :" << __LINE__ << endl; D_END
        pthread_mutex_lock(&MTcomb_mutex);
        //check that all threads have finished their work
        ACTIVITY_COUNTER_GET(numberOfActiveThread);
        pthread_mutex_unlock(&MTcomb_mutex);

        while (numberOfActiveThread > 0) {
            pthread_mutex_lock(&MTmaximumGuessScore_mutex);
            //We wait then we will check a new time that all the WT threads are in the waiting state
            if (m_MTSignalStateMaximumGuessScore == 0) {
                //D_BEGIN cerr << "MT # Line :" << __LINE__ << endl; D_END
                pthread_cond_wait(&MTmaximumGuessScore_cond, &MTmaximumGuessScore_mutex);
                //D_BEGIN cerr << "MT # Line :" << __LINE__ << endl; D_END
            }
            m_MTSignalStateMaximumGuessScore = 0;
            //D_BEGIN cerr << "MT # Line :" << __LINE__ << " / " << m_MTSignalStateMaximumGuessScore << endl; D_END
            pthread_mutex_unlock(&MTmaximumGuessScore_mutex);

            //pthread_mutex_lock (&MTcomb_mutex);
            //check that all threads have finished their work
            ACTIVITY_COUNTER_GET(numberOfActiveThread);
            //pthread_mutex_unlock (&MTcomb_mutex);

            lossOfTimeB++;
            //D_BEGIN cerr << "MT # Display Loss of time :" << lossOfTimeB << endl; D_END
        }
        //D_BEGIN cerr << "MT # TOTAL Display Loss of time :" << lossOfTimeA << " , " << lossOfTimeB << endl; D_END
        nextPatternCombi = m_MTnextPatternCombi;
        //cout << "Final result Combinaison: " << nextPatternCombi << " number_of_removals score: " << m_MTmaximumGuessScore << "\n";
    }

    void* MasterSolver::thread_fun(void* args) noexcept {
        thread_fun_args *tfArgs = static_cast<thread_fun_args*> (args);
        void* ret = tfArgs->This->updateFromIterationWT(reinterpret_cast<void *> (tfArgs->Id));
        delete tfArgs;
        return ret;
    }

    void* MasterSolver::updateFromIterationWT(void *t) noexcept {
        U32 id = reinterpret_cast<U32> (t);
        //D_BEGIN cerr << id << " # updateFromIterationWT" << endl; D_END
        vectorCombinaison vectComb;
        //D_BEGIN cerr << id << " # Before While" << endl; D_END
        while (m_MTthreadExit[id]) {
            //D_BEGIN cerr << id << " # Enter While" << endl; D_END
            pthread_mutex_lock(&MTcomb_mutex);
            //D_BEGIN cerr << id << " # AFTER pthread_mutex_lock (&MTcomb_mutex);" << endl; D_END
            while (m_MTvectorComb.size() == 0) {//if the signal is not yet sent, we need to wait the signal, in order to receive it
                pthread_cond_wait(&MTcomb_cond, &MTcomb_mutex);
                //D_BEGIN cerr << id << " # AFTER pthread_cond_wait(&MTcomb_cond, &MTcomb_mutex);" << endl; D_END
            }//otherwise, the signal is already sent, so don't wait for a signal that has been already sent (and "lost")
            ACTIVITY_COUNTER_INC(id)
            //D_BEGIN cerr << id << " # number of comb inputs [" << m_MTvectorComb.size() << "]" << endl; D_END
            U32 vectorSize = m_MTvectorComb.size();
            //U32 NbIteration = mMin(vectorSize, MAX_COMB_BY_THREAD);
            U32 NbIteration;
            if (vectorSize > MAX_COMB_BY_THREAD * NUMBER_OF_THREADS)
                NbIteration = vectorSize / (NUMBER_OF_THREADS + 1);
            else if (vectorSize > MAX_COMB_BY_THREAD)
                NbIteration = MAX_COMB_BY_THREAD;
            else
                NbIteration = vectorSize;
            //D_BEGIN cerr <<vectorSize << "/" << NbIteration << "\n"; D_END
            for (U32 i = 0; i < NbIteration; i++) {
                vectComb.push_back(m_MTvectorComb.back()); //it is a deep copy (we can't do only a pointer copy, because this variable is shared with other threads)
                m_MTvectorComb.pop_back();
            }
            //D_BEGIN cerr << id << " # number of USED comb inputs [" << NbIteration << "]" << endl; D_END
            //D_BEGIN cerr << id << " # number of remaining comb inputs [" << m_MTvectorComb.size() << "]" << endl; D_END
            pthread_mutex_unlock(&MTcomb_mutex);

            U32 thisScore = 0;
            Combinaison guessComb;
            for (vectorCombinaison::iterator itorVectComb = vectComb.begin(); itorVectComb != vectComb.end(); ++itorVectComb) {
                U32 localScore = testCurrentPattern(*itorVectComb);
                if (localScore > thisScore) {
                    thisScore = localScore;
                    guessComb = *itorVectComb;
                }
                //D_BEGIN cerr << id ; D_END
            }
            //D_BEGIN cerr << endl ; D_END
            vectComb.clear();
            //D_BEGIN cerr << id << " # BEFORE pthread_mutex_lock (&MTmaximumGuessScore_mutex);" << endl; D_END
            pthread_mutex_lock(&MTmaximumGuessScore_mutex);
            //D_BEGIN cerr << id << " # AFTER pthread_mutex_lock (&MTmaximumGuessScore_mutex);" << endl; D_END
            //D_BEGIN cerr << id << " # Line: " << __LINE__ << " local better removal comb: " << guessComb << " number of removal: " << thisScore <<endl; D_END
            if (thisScore > m_MTmaximumGuessScore) {
                //cout << id << " # guessComb " << guessComb << " number_of_removals score: " << thisScore<< " BETTER\n";
                m_MTmaximumGuessScore = thisScore;
                m_MTnextPatternCombi = guessComb;
            }
            //D_BEGIN cerr << id << " # Line :" << __LINE__ << endl; D_END
            m_MTSignalStateMaximumGuessScore++;
            pthread_cond_signal(&MTmaximumGuessScore_cond);
            ACTIVITY_COUNTER_DEC(id)
            //D_BEGIN cerr << id << " # Line :" << __LINE__ << " / " << m_MTSignalStateMaximumGuessScore << endl; D_END
            pthread_mutex_unlock(&MTmaximumGuessScore_mutex);
        }
        pthread_exit(nullptr);
        return nullptr;
    }
#endif

    U32 MasterSolver::testCurrentPattern(const Combinaison &currentGuessPattern) const noexcept {
        U32 thisScore = ~0;
        for (const auto& score : m_scoreSet) {
            U32 tempScore = countRemovals(currentGuessPattern, score);
            if (thisScore > tempScore)//Search the minimum
                thisScore = tempScore;
        }
        return thisScore;
    }

    void MasterSolver::buildScoreSet() noexcept {
        m_scoreSet.clear(); //remove all elements
        for (U32 iBlack = 0; iBlack <= m_nbPositions; iBlack++) {
            for (U32 iWhite = 0; iWhite <= m_nbPositions - iBlack; iWhite++) {
                tScore score = {iBlack, iWhite};
                m_scoreSet.push_back(score);
            }
        }
    }

    /**
     *Score is corresponding to the correction#include "combinaison.h"
     */
    U32 MasterSolver::countRemovals(const Combinaison &guessCombinaison, const tScore &score) const noexcept {
        U32 count = 0;

        for (const auto& possibleSolution : m_possibleSolutionSet) {
            U32 blackPigs, whitePigs;
            getCorrection(guessCombinaison, possibleSolution, blackPigs, whitePigs);
            if ((blackPigs == score.blackPigs)&&(whitePigs == score.whitePigs))
                count++;
        }
        return m_possibleSolutionSet.size() - count;
    }
}
