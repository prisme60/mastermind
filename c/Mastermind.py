#!/bin/python

import sys
import time

def fillList0(elem):
    return 0

class game:
    def __init__(self,nb_pigs,nb_colors):
        self.nb_pigs = nb_pigs
        self.nb_colors = nb_colors
        self.secretCombinaison = []

    def generateSecretCombinaison(self):
        self.secretCombinaison = []
        for i in range(0,self.nb_pigs):
            self.secretCombinaison.append(random.randint(0,self.nb_colors-1))

    def getSecretCombinaison(self):
        return self.secretCombinaison

    def setSecretCombinaison(self, tSecretCombinaison):
        self.secretCombinaison=tSecretCombinaison

    def getCorrection(self, guessCombinaison):
        #determines well place
        i, black_pigs, white_pigs, remainingPigsOfSecretCombinaison = 0, 0, 0, []
        remainingPigsOfSecretCombinaison.extend(self.secretCombinaison)
        for color in guessCombinaison:
            if(self.secretCombinaison[i]==color):
                remainingPigsOfSecretCombinaison[i]=self.nb_colors+1 #we must destroy the color (so we use an unuse color number!)
                #now the color couldn't be used for a not well place color!
                black_pigs = black_pigs + 1
            i = i + 1
        for color in guessCombinaison:
            if(color in remainingPigsOfSecretCombinaison):
                remainingPigsOfSecretCombinaison.remove(color)#remove the first occurence of the color
                white_pigs = white_pigs + 1
        return black_pigs, white_pigs

class resolution(game):
    def __init__(self,nb_pigs,nb_colors):
        game.__init__(self,nb_pigs,nb_colors)

    def solve(self):
        self.estimationForFirstPattern();

    def generateFirstPattern(self):
        pass

    def estimationForFirstPattern(self):
        nbDuplicate = (2 * self.nb_pigs / self.nb_colors) + 1
        simulation = 0
        PigsDuplicate = map(fillList0,range(0,self.nb_pigs)) #creation d'un histogramme vide
        for i in range(0,1000000):#*self.nb_colors**self.nb_pigs):
            self.generateSecretCombinaison()
            ColorsDuplicate = []
            for color in range(0,self.nb_colors):
                ColorsDuplicate.append(self.secretCombinaison.count(color))
            #print i, ColorsDuplicate
            #print max(ColorsDuplicate)
            PigsDuplicate[max(ColorsDuplicate)-1] = PigsDuplicate[max(ColorsDuplicate)-1] + 1
        print "Estimation :", nbDuplicate, " simulation :", PigsDuplicate

def main():
    print 'Mastermind V0.1'

    listeDesNombres=map(int,sys.argv[1:])

    if (listeDesNombres==[] or len(listeDesNombres)!=2 ):
        print "No parameter are set. So we take the default values: 4 pigs and 6 colors"
        listeDesNombres=[4,6]
 
    tempsDebut = time.time()#temps en flottant

    problem = resolution(listeDesNombres[0],listeDesNombres[1])
    problem.generateSecretCombinaison()
    problem.solve()
    combi = [1,1,2,2]
    print "Correction for combinaison: ",combi, " is ",  problem.getCorrection(combi)
    print "Secret combinaison is       ", problem.getSecretCombinaison()

    tempsFin = time.time()#temps en flottant

    print "duree du calcul en secondes=", tempsFin-tempsDebut
    
main() 
    
    
    
