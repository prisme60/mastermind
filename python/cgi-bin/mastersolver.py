import sys
import copy
from multiprocessing import Process, Queue

import mastergame
import combinaison
import correctionhistory


class MasterSolver(mastergame.MasterGame):
    def __init__(self, _nbColors,_nbPositions, _multiprocessing = True, _verbose = True):
        mastergame.MasterGame.__init__(self, _nbColors, _nbPositions)
        self.indexPastGuessSetTreated = 0
        self.buildScoreSet()
        self.multiprocessing = _multiprocessing
        self.verbose = _verbose
        ####
        self.pickleCorrectionHistory = correctionhistory.PickleCorrectionHistory(self.nbPositions, self.nbColors)
        try:
            self.currentCorrectionHistoryNode = self.correctionHistoryRootNode = self.pickleCorrectionHistory.load()
            if self.verbose:
                print("====>RELOAD history")
        except:
            if self.verbose:
                print("---->Create an empty history")
            self.currentCorrectionHistoryNode = self.correctionHistoryRootNode = None
        if self.multiprocessing:
            self.threadWorkerInit(2)

    def __del__(self):
        if self.verbose:
            print("###destroy MasterSolver###")
        if self.multiprocessing:
            self.threadWorkerDestroy()

    def store(self):
        self.pickleCorrectionHistory.store(self.correctionHistoryRootNode)

    def solve(self):
        """returns list of combinaison"""
        #reset previous resolve variables
        self.pastGuessSet, self.possibleSolutionSet, self.indexPastGuessSetTreated = [], [], 0
        if(self.correctionHistoryRootNode == None):
            #print("Store a")
            combi = combinaison.Combinaison(self.nbPositions)
            self.generateFirstPattern(combi)
            self.correctionHistoryRootNode = correctionhistory.CorrectionHistory(combi)
        else:
            #print("Reuse a")
            combi = self.correctionHistoryRootNode.combinaison.clone()
            
        self.pastGuessSet.append(self.correctionHistoryRootNode.combinaison)
        self.currentCorrectionHistoryNode = self.correctionHistoryRootNode

        score = (blackPigs, whitePigs) = self.getCorrectionOfSecretCombinaison(combi)
        if self.verbose:
            print("***Secret combinaison is ***" , self.getSecretCombinaison())
            print("Correction for combinaison (B,W): " , combi , " is " , score)

        while(self.UpdateSolutionSet()>1):
            if self.currentCorrectionHistoryNode.IsThereANodeFromCorrection(score):
                #print("Reuse b")
                self.currentCorrectionHistoryNode = self.currentCorrectionHistoryNode.getNodeFromCorrection(score)
                combi = self.currentCorrectionHistoryNode.combinaison.clone()
            else:
                #print("Store b")
                self.guessNextPattern(combi)#it is possible to use combi because it has been saved in the pastGuessSet vector
                self.currentCorrectionHistoryNode = self.currentCorrectionHistoryNode.addCorrectionCombiNode(combi,(blackPigs,whitePigs))
                
            score = (blackPigs, whitePigs) = self.getCorrectionOfSecretCombinaison(combi)
            if self.verbose:
                print("Correction for combinaison (B,W): " , combi , " is " , score)
            self.pastGuessSet.append(combi.clone())
            #print("Check guess Combi of Vector: " , self.pastGuessSet[-1])

        if(combi!=self.getSecretCombinaison()):#why getSecretCombinaison() is not working?
            if(len(self.possibleSolutionSet)>0):
                if self.verbose:
                    print("Correction for combinaison: " , self.possibleSolutionSet[0] ,"is (", self.nbPositions  ,",0)")
                self.pastGuessSet.append(self.possibleSolutionSet[0])
            elif self.verbose:
                    print("possibleSolutionSet is empty!!!!", file=sys.stderr)

        if self.verbose:
            print("Number of guesses:" , len(self.pastGuessSet))
        return self.pastGuessSet

    def generateFirstPattern(self,firstPattern):
        nbDuplicate = self.estimationForFirstPattern()
        pGuessCombinaison = []
        for indexPigs in range(self.nbPositions):
           pGuessCombinaison.append(int((indexPigs+1)/nbDuplicate))
        firstPattern.buildCombinaison(pGuessCombinaison)

    def estimationForFirstPattern(self):
        return (2 * self.nbPositions / self.nbColors) + 1

    def isCombiCompatible(self,givenTestedCombinaison, combinaisonToTest, score):
        #Permit to know if the guessCombinaisonResult is compatible with the given tested combinaison
        scoreFromCorrection = self.getCorrection(combinaisonToTest,givenTestedCombinaison)# (blackPigs,whitePigs)
        return (scoreFromCorrection==score)

    def UpdateSolutionSet(self):
        #return the number of element in the solution Set
        if(len(self.possibleSolutionSet)==0):#if there is no reasearch done before, we have to consider all solutions
            #print("First SolutionSet compute")
            currentCombi = combinaison.Combinaison(self.nbPositions)#we set the first combi
            #we can use the comparaison with the SecretCombinaison only with already guessedCombinaison
            score = (blackPigs, whitePigs) = self.getCorrectionOfSecretCombinaison(self.pastGuessSet[0])
            bIteration = True
            while (bIteration):
                if(self.isCombiCompatible(self.pastGuessSet[0],currentCombi,score)):
                    self.possibleSolutionSet.append(currentCombi.clone())#we populate valid solution to the set
                bIteration = currentCombi.setNextCombinaison(self.nbColors)
            self.indexPastGuessSetTreated = 1
            #print("END Construct first set solution => len=" , len(self.possibleSolutionSet))
        else: #it is now an incremental research: we eliminate not compatible solutions
            #print("Incremental Research")
            while(self.indexPastGuessSetTreated < len(self.pastGuessSet)):
                score = (blackPigs, whitePigs) = self.getCorrectionOfSecretCombinaison(self.pastGuessSet[self.indexPastGuessSetTreated])#OK with guessesCombinaisons
                localPossibleSolutionSet = copy.copy(self.possibleSolutionSet)
                #print("Before ",self.possibleSolutionSet)
                numberOfElementRemoved = 0
                for indexSol, itorOnSol in enumerate(localPossibleSolutionSet):#use localPossibleSolutionSet, in order to avoid error with iteration and remove operation
                    if(not self.isCombiCompatible(self.pastGuessSet[self.indexPastGuessSetTreated], itorOnSol, score)):
                        self.possibleSolutionSet.pop(indexSol - numberOfElementRemoved)
                        numberOfElementRemoved += 1
                #print("After ",self.possibleSolutionSet)
                self.indexPastGuessSetTreated += 1
            #print("END incremental research solution len " , len(self.possibleSolutionSet))
        return len(self.possibleSolutionSet)

    def guessNextPattern(self,nextPatternCombi):
        if self.multiprocessing:
            self.guessNextPatternCombi_ThreadCreation(nextPatternCombi)
        else: #no multiprocessing
            guess = combinaison.Combinaison(self.nbPositions)#we set the guess to the first combi
            maximumGuessScore = 0
            bIteration = True      
            while (bIteration):
                thisScore = min([self.countRemovals(guess, score) for score in self.scoreSet])

                if(thisScore > maximumGuessScore):
                    #print("guess " , guess , " number_of_removals score: " , thisScore , " BETTER")
                    maximumGuessScore = thisScore
                    nextPatternCombi.buildCombinaison(guess.pColorTable)
                bIteration = guess.setNextCombinaison(self.nbColors)

        if self.verbose:
            print("output guess >>>", nextPatternCombi)

    def buildScoreSet(self):
        self.scoreSet = [] #remove all elements
        for iBlack in range(self.nbPositions + 1):
            for iWhite in range(self.nbPositions-iBlack+1):
                self.scoreSet.append((iBlack,iWhite))

    def countRemovals(self,guessCombinaison, score):
        """Score is corresponding to the correction"""
        count = 0
        for itor in self.possibleSolutionSet:
            scoreFromCorrection = self.getCorrection(guessCombinaison,itor)
            if(scoreFromCorrection == score):
                count+=1
        #print("countRemovals guessCombinaison, score, count", guessCombinaison, score, count)
        return len(self.possibleSolutionSet) - count

    ######## special stackless methods (performance purpose)
    ######## should replace the guessNextPatternCombi method ##########
    def patternEvaluation(self, qIN, qOUT):
        bCondition = True
        while(bCondition):
            [guess, self.possibleSolutionSet] = qIN.get();
            if((type(guess) != type(None)) and (type(self.possibleSolutionSet) != type(None))):
                thisScore = min([self.countRemovals(guess, score) for score in self.scoreSet])
                qOUT.put([guess, thisScore])
            else:
                bCondition = False
        if self.verbose:
            print("Exit Thread (because None object given)")

    def guessNextPatternCombi_ThreadCreation(self, nextPatternCombi):
        maximumGuessScore = 0
        nbofCreatedThread , bCondition, nextProcessnumber = 0, True, 0
        guess = combinaison.Combinaison(self.nbPositions)
        while(bCondition and nbofCreatedThread<self.numberOfThread):#launch some initial works
            #print("**Thread creation", nbofCreatedThread, "give work to thread " ,nextProcessnumber%(nbofCreatedThread+1))
            self.processInfo[nextProcessnumber%(nbofCreatedThread+1)][1].put([guess,self.possibleSolutionSet])
            bCondition = guess.setNextCombinaison(self.nbColors)
            nextProcessnumber += 1
            nbofCreatedThread += 1
            #print("Thread created", nbofCreatedThread)
        while(bCondition or nbofCreatedThread > 0):
            if(nbofCreatedThread > 0):#still running thread
                #assert(not qOUT.empty())
                #print("chan reception", nbofCreatedThread)
                [returnGuess, thisScore] = self.qOUT.get()
                if(thisScore > maximumGuessScore):
                    print("guess " , guess , " number_of_removals score: " , thisScore , " BETTER")
                    maximumGuessScore = thisScore
                    nextPatternCombi.buildCombinaison(returnGuess.pColorTable)
                nbofCreatedThread -= 1
                #print("chan received", nbofCreatedThread)
            if(bCondition):#remains combinaisons to guess
                #print("**Thread creation", nbofCreatedThread, "give work to thread " ,nextProcessnumber%(nbofCreatedThread+1))
                self.processInfo[nextProcessnumber%(nbofCreatedThread+1)][1].put([guess,self.possibleSolutionSet], block=False)
                bCondition = guess.setNextCombinaison(self.nbColors)
                nextProcessnumber += 1
                nbofCreatedThread += 1
                #print("**Thread created", nbofCreatedThread)

    def threadWorkerInit(self,numberOfThread):
        if self.verbose:
            print("threadWorkerInit ", numberOfThread)
        self.numberOfThread = numberOfThread
        self.processInfo = []
        self.qOUT = Queue()
        for i in range(numberOfThread):
            if self.verbose:
                print("Init thread ", i)
            qIN = Queue()
            p = Process(target=self.patternEvaluation, args=(qIN, self.qOUT)).start()#Start the process immediatly, because it cannot be started in a second time!
            self.processInfo.append((p,qIN))

    def threadWorkerDestroy(self):
        if self.multiprocessing:
            for i in range(self.numberOfThread):
                (p , qIN) = self.processInfo.pop()
                qIN.put([None,None], block=False)#send None object in order to warn the Thread that there is no more work!
                if self.verbose:
                    print("Destroy  thread ",i)
                #p.terminate()
                
            
            
            
            

        

