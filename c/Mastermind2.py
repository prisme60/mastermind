#!/usr/bin/python
# -*- coding: cp1252 -*-

from __future__ import with_statement

import sys
import time
import random
import copy
import unittest
import thread


#import psyco
#psyco.full()


class TestMasterMind(unittest.TestCase):
    def setUp(self):
        self.problem = resolution(4,6)

    def testCorrection(self):
        """Unitary test ==> permit to test the correction function!"""
        a = self.problem.getFirstCombinaison()
        while(a!=None):
            b = self.problem.getFirstCombinaison()
            while (b!=None):
                sol_ab = self.problem.getCorrection(a,b)
                sol_ba = self.problem.getCorrection(b,a)
                self.assertEqual(sol_ab , sol_ba)
                #if(sol_ab != sol_ba):
                #    print "a",a,"b",b,"sol_ab",sol_ab,"sol_ba",sol_ba
                b = self.problem.getNextCombinaison(b)
            a = self.problem.getNextCombinaison(a)

    def testResolutionAlgorithm(self):
        """Unitary test ==> permit to test the solve algorithme validity!"""
        a = self.problem.getFirstCombinaison()
        with file("result.txt","w+") as f:
            while(a!=None):
                self.problem.setSecretCombinaison(copy.copy(a))
                self.assertEqual(self.problem.getSecretCombinaison(), a)
                guesses = self.problem.solve()
                self.assertEqual(guesses[-1], a)
                #self.assertEqual(len(guesses)<=5,True)
                f.write("%s\t%s\t%s\n"%(a,len(guesses),guesses))
                a = self.problem.getNextCombinaison(a)

def fillList0(elem):
    return 0

class game:
    def __init__(self,nb_pigs,nb_colors):
        self.nb_pigs = nb_pigs
        self.nb_colors = nb_colors
        self.secretCombinaison = []

    def fillListGenerator(self,elem):
        return random.randint(0,self.nb_colors-1)

    def generateSecretCombinaison(self):
        self.secretCombinaison = map(self.fillListGenerator,range(0,self.nb_pigs))
        #self.secretCombinaison = [1,0,0,0]

    def getSecretCombinaison(self):
        return self.secretCombinaison

    def setSecretCombinaison(self, tSecretCombinaison):
        self.secretCombinaison=tSecretCombinaison

    def getCorrectionForSecretCombinaison(self, guestCombinaison):
        return self.getCorrection(guestCombinaison, self.secretCombinaison)

    def getCorrection(self, guessCombinaison, referenceCombinaison):
        #determines well place
        black_pigs, white_pigs = 0, 0
        remainingPigsOfSecretCombinaison, guessCombinaisonLocal = copy.copy(referenceCombinaison), copy.copy(guessCombinaison)               #we must copy object before modify it!
        for i, guessCombi in enumerate(guessCombinaisonLocal):
            if(referenceCombinaison[i]==guessCombi):
                remainingPigsOfSecretCombinaison[i]=self.nb_colors+1 #we must destroy the color (so we use an unused color number!)
                guessCombinaisonLocal[i]=self.nb_colors+2#we must destroy the color (so we use another unused color number!)
                #now the color couldn't be used for a not well place color!
                black_pigs += 1
        #determine good color but not well place
        #print remainingPigsOfSecretCombinaison, "guess Combinaison", guessCombinaisonLocal
        for color in guessCombinaisonLocal:
            if(color in remainingPigsOfSecretCombinaison):
                remainingPigsOfSecretCombinaison.remove(color)#remove the first occurence of the color
                white_pigs += 1
        return [black_pigs, white_pigs]

    def getNextCombinaison(self, previousCombinaison):
        for i in range(0,self.nb_pigs):
            if(previousCombinaison[i]<self.nb_colors-1):
                previousCombinaison[i] = previousCombinaison[i] + 1
                for j in range(0,i):
                    previousCombinaison[j] = 0
                return previousCombinaison
        return None

    def getFirstCombinaison(self):
        return map(fillList0,range(0,self.nb_pigs))

    def getMidCombinaison(self):
        return map((lambda x: self.nb_colors/2),range(0,self.nb_pigs))

class resolution(game):
    def __init__(self,nb_pigs,nb_colors):
        game.__init__(self,nb_pigs,nb_colors)
        self.pastguesses, self.solutionSet, self.scoreSet = [], [], self.buildScoreSet()

    def solve(self):
        self.pastguesses, self.solutionSet = [], [] #reset previous resolve variable
        #print "Generation of the 1st pattern:"
        combi = self.generateFirstPattern()

        print "Correction for combinaison: ", combi, " is ",  self.getCorrectionForSecretCombinaison(combi)
        print "Secret combinaison is       ", self.getSecretCombinaison()
        self.pastguesses.append(copy.copy(combi))

        #print "Compute the new solution set"
        while(self.computeLastSolutionSet()>1):
            #print "Estimate the next guess:"
            combi = self.guessNextPattern2(self.getFirstCombinaison(),None)
            print "Correction for combinaison: ", combi, " is ",  self.getCorrectionForSecretCombinaison(combi)
            #print "Secret combinaison is       ", self.getSecretCombinaison()
            self.pastguesses.append(copy.copy(combi))

        if(combi!=self.getSecretCombinaison()):
            print "Correction for combinaison: ",self.solutionSet[0],"is", [4,0]
            self.pastguesses.append(self.solutionSet[0])
            
            
        print "Number of guesses:", len(self.pastguesses)
        return self.pastguesses

    def generateFirstPattern(self):
        NbDuplicate = self.estimationForFirstPattern();
        color, guessCombinaison = 0, []
        for indexPigs in range(0,self.nb_pigs):
            guessCombinaison.append(color)
            if(indexPigs % NbDuplicate == NbDuplicate - 1):
                color += 1
        return guessCombinaison

    def estimationForFirstPattern(self):
        nbDuplicate = (2 * self.nb_pigs / self.nb_colors) + 1
        simulation = 0
        PigsDuplicate = map(fillList0,range(0,self.nb_pigs)) #creation d'un histogramme vide
        for i in range(0,self.nb_colors*self.nb_pigs):
            generatedCombi = map(self.fillListGenerator,range(0,self.nb_pigs))
            ColorsDuplicate = []
            for color in range(0,self.nb_colors):
                ColorsDuplicate.append(generatedCombi.count(color))
            PigsDuplicate[max(ColorsDuplicate)-1] = PigsDuplicate[max(ColorsDuplicate)-1] + 1
        print "Estimation :", nbDuplicate, " simulation :", PigsDuplicate
        return PigsDuplicate.index(max(PigsDuplicate)) + 1#return the index of the max duplicate

    def isCombiCompatible(self, givenTestedCombinaison, guessCombinaisonResult, combinaisonToTest):
        """Permit to know if the guessCombinaisonResult is compatible with the given tested combinaison"""
        #correction = self.getCorrection(combinaisonToTest,givenTestedCombinaison)
        #print "getCorrection(combinaisonToTest,",combinaisonToTest,"givenTestedCombinaison",givenTestedCombinaison,"=",correction,"<==?==>", guessCombinaisonResult
        #return (correction==guessCombinaisonResult)
        return (self.getCorrection(combinaisonToTest,givenTestedCombinaison)==guessCombinaisonResult)

    def computeLastSolutionSet(self):
        return self.computeSolutionSet([self.pastguesses[-1]])
            
    def computeSolutionSet(self, guessCombinaisons):
        if(self.solutionSet == []):#if there is no reasearch done before, we have to consider all solutions
            print "First SolutionSet compute"
            currentCombi = self.getFirstCombinaison()
            currentCorrection = self.getCorrectionForSecretCombinaison(guessCombinaisons[0])#we can use the comparaison with the SecretCombinaison only with already guessedCombinaison
            while (currentCombi != None):
                #print "Combi evaluation solutionSet ==> currentCombi=",currentCombi
                if(self.isCombiCompatible(guessCombinaisons[0],currentCorrection,currentCombi)):
                    #print "Combi Compatible ==> Tested combinaison=",currentCombi
                    self.solutionSet.append(copy.copy(currentCombi))
                currentCombi = self.getNextCombinaison(currentCombi)
            guessCombinaisons2=guessCombinaisons[1:]
        else:
            guessCombinaisons2=guessCombinaisons
        #it is now an incremental research
        #print "incremental research for", guessCombinaisons2, "len solutions", len(self.solutionSet)
        for currentGuess in guessCombinaisons2:
            currentCorrection = self.getCorrectionForSecretCombinaison(currentGuess)#OK with guessesCombinaisons
            indexCombi = 0
            while (indexCombi<len(self.solutionSet)):
                #print "current tested solution",self.solutionSet[indexCombi]
                if(not self.isCombiCompatible(currentGuess, currentCorrection,self.solutionSet[indexCombi])):
                    #print "retrait combi",self.solutionSet[indexCombi]
                    del self.solutionSet[indexCombi]
                    #because the list is one element shorter than before, we mustn't increment index
                else:
                    indexCombi += 1
        print "END incremental research solution len", len(self.solutionSet)
        #if len(self.solutionSet) < 100:
        #    print "solutions :",self.solutionSet
        return len(self.solutionSet)


##    def guessNextPattern(self):
##        maximumGuess, guess, maximumGuessScore, thisScore = self.getFirstCombinaison(), self.getFirstCombinaison(), 0, 0
##        while (guess != None):
##            number_of_removals = []
##            for score in self.scoreSet:
##                number_of_removals.append(self.countRemovals(guess, score))
##            thisScore = min(number_of_removals)
##            if (thisScore > maximumGuessScore and (guess not in self.pastguesses)):
##                print "guess",guess,"number_of_removals:",number_of_removals, "BETTER"
##                maximumGuessScore = copy.copy(thisScore)
##                maximumGuess = copy.copy(guess)
##            guess = self.getNextCombinaison(guess)
##        return maximumGuess


    def guessNextPattern2(self, intervalStart, intervalEnd):
        maximumGuess, guess, maximumGuessScore, thisScore = copy.copy(intervalStart), intervalStart, 0, 0
        #print "self.scoreSet",self.scoreSet
        print "intervalEnd",intervalEnd
        while (guess != intervalEnd):
            number_of_removals = []
            for score in self.scoreSet:
                number_of_removals.append(self.countRemovals(guess, score))
            thisScore = min(number_of_removals)
            if (thisScore > maximumGuessScore and (guess not in self.pastguesses)):
                print "guess",guess,"number_of_removals:",number_of_removals, "BETTER"
                maximumGuessScore = copy.copy(thisScore)
                maximumGuess = copy.copy(guess)
            guess = self.getNextCombinaison(guess)
        return maximumGuess

    def guessNextPattern3(self, string, lock, intervalStart, intervalEnd, maximumGuess, *args):
        print "Coucou:", string
        maximumGuess.extend(self.guessNextPattern2(intervalStart, intervalEnd))
        print maximumGuess
        lock.release()
        print "Close", string

    def guessNextPattern4(self, intervalStart, intervalEnd):
        combiMid = self.getMidCombinaison();
        combi1, combi2, lock1, lock2 = [], [], thread.allocate_lock(), thread.allocate_lock()
        lock1.acquire(); lock2.acquire()
        thread.start_new_thread(self.guessNextPattern3,("Thread N_1", lock1, self.getFirstCombinaison(),combiMid,combi1))
        thread.start_new_thread(self.guessNextPattern3,("Thread N_2", lock2, combiMid                  ,None    ,combi2))
        lock1.acquire(); print "lock1 OK"; lock2.acquire(); print "lock2 OK"
        print "combi1", combi1, "combi2", combi2
        combi = combi1 if(self.compareRemovalCombi(combi1,combi2)>0) else combi2
        return combi

    def buildScoreSet(self):
        scoreSet = []
        for iBlack in range(0,self.nb_pigs+1):
            for iWhite in range(0,self.nb_pigs + 1 - iBlack):
                scoreSet.append([iBlack,iWhite])
        #print "scoreSet not sorted",scoreSet
        #scoreSet.sort(cmp=lambda x,y: ((x[0]*4+x[1]*3) - (y[0]*4+y[1]*3)))
        scoreSet.sort(cmp=lambda x,y: ((x[0]+x[1]) - (y[0]+y[1])))
        #print "scoreSet sorted",scoreSet
        return scoreSet               

    def countRemovals(self,guessCombinaison,score):
        """Score is corresponding to the correction"""
        count = 0
        for combi in self.solutionSet:#test all the remaining combinaisons
            #correctionget = self.getCorrection(guessCombinaison,combi)
            #print "combi:", guessCombinaison,"compare with",combi,"compare score", score, "with getCorrection", correctionget
            #if(correctionget==score):#we count all the combinaison that are the guess combinaison
            if(self.getCorrection(guessCombinaison,combi)==score):#we count all the combinaison that are the guess combinaison
                #print "[OK]" , "combi:", guessCombinaison,"compare with",combi,"compare score", score                 
                count += 1 #* (score[0]%2+1)
        return len(self.solutionSet) - count

    def compareRemovalCombi(self, combi1, combi2):
        number_of_removals1, number_of_removals2 = [], []
        for score in self.scoreSet:
            number_of_removals1.append(self.countRemovals(combi1, score))
            number_of_removals2.append(self.countRemovals(combi2, score))
        return min(number_of_removals1) - min(number_of_removals2)
    

def main():
    print 'Mastermind V0.1'
    #unittest.main()#launch unitary tests

    listeDesNombres=map(int,sys.argv[1:])

    if (listeDesNombres==[] or len(listeDesNombres)!=2 ):
        print "No parameter are set. So we take the default values: 4 pigs and 6 colors"
        listeDesNombres=[4,6]
 
    tempsDebut = time.time()#temps en flottant

    problem = resolution(listeDesNombres[0],listeDesNombres[1])

    problem.generateSecretCombinaison()
    problem.solve()

    tempsFin = time.time()#temps en flottant

    print "duree du calcul en secondes=", tempsFin-tempsDebut
    
main()
