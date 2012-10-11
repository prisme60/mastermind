import random
import copy

class Combinaison:
    def __init__(self, pColorTable_or_Combi_or_nbPosition):
        if(isinstance(pColorTable_or_Combi_or_nbPosition, int)):
            #print("nbPOsition")
            self.buildCombinaison([0 for i in range(pColorTable_or_Combi_or_nbPosition)])
        elif(isinstance(pColorTable_or_Combi_or_nbPosition, Combinaison)):
            #print("combi")
            self.buildCombinaison(pColorTable_or_Combi_or_nbPosition.pColorTable)
        else:
            #print("table")
            self.buildCombinaison(pColorTable_or_Combi_or_nbPosition)

    def buildCombinaison(self, pColorTable):
        self.pColorTable = copy.copy(pColorTable)
        #print("build combinaison:\nout:{}\nin:{}".format(self.pColorTable,pColorTable))
        assert(not isinstance(pColorTable, int))

    def setNextCombinaison(self, nbColors):
        bNewCombinaison = False
        for indexElement in range(len(self.pColorTable)):
            if(self.pColorTable[indexElement]<nbColors-1):
                #print("before ",self.pColorTable)
                #print("increment index",indexElement)
                self.pColorTable[indexElement] += 1
                bNewCombinaison = True
                #print("after ",self.pColorTable)
                for indexSubElement in range(0,indexElement):
                    #print("set to ",0)
                    self.pColorTable[indexSubElement] = 0
                return bNewCombinaison
        return bNewCombinaison

    def getColor(self, index):
        return self.pColorTable[indexElement]

    def setColor(self, index, colorToSet):
        self.pColorTable[indexElement] = colorToSet;

    def getNbPositions(self):
        return len(self.pColorTable)

    def clone(self):
        return Combinaison(self)

    def __eq__(self, rightCombinaison):
##        print("self=",self)
##        print("self.pColorTable=",self.pColorTable)
##        print("rightCombinaison=",rightCombinaison)
##        print("rightCombinaison.pColorTable=",rightCombinaison.pColorTable)
        try:
            return self.pColorTable == rightCombinaison.pColorTable;
        except:
            return False

    def __ne__(self, rightCombinaison):
        return not(self==rightCombinaison)

    def random(self, nbColor):
        self.pColorTable = map(rand()%nbColor, self.pColorTable)

    def contains(self, color):
        return color in self.pColorTable

    def indexOfColor(self, color):
        try:
            return index(self.pColorTable)
        except ValueError:
            return -1

    def getCorrection(self, combiTotest):
        #print("combinaison.getCorrection ",self.pColorTable," / ",combiTotest.pColorTable)
        referenceCombinaison = Combinaison(self)
        combiTotestLocal = Combinaison(combiTotest)
        (blackPigs, whitePigs) = (0 , 0)
        #print(referenceCombinaison.pColorTable)
        for i, elementReference in enumerate(referenceCombinaison.pColorTable):
            if(elementReference==combiTotestLocal.pColorTable[i]):
                referenceCombinaison.pColorTable[i]=~0;#we must destroy the color (so we use an unused color number!)
                combiTotestLocal.pColorTable[i]=~1;#we must destroy the color (so we use another unused color number!)
                #now the color couldn't be used for a not well place color!
                blackPigs += 1

        #determine good color but not well place
        for elementCombiToTest in combiTotestLocal.pColorTable:
            try:
                colorIndex=referenceCombinaison.pColorTable.index(elementCombiToTest)
                #an exception occur, if the color is not found (the following lines are not executed in this case)
                referenceCombinaison.pColorTable[colorIndex]=~2;#destroy the the first occurence of the color
                whitePigs += 1
            except ValueError:
                pass
        #print((blackPigs, whitePigs))
        return (blackPigs, whitePigs)

    def isCombinaisonCompatible(self, combiToTest, wantedBlackPigs, wantedWhitePigs):
        return self.getCorrection(combiToTest) == (wantedBlackPigs,wantedWhitePigs)

    def __repr__(self):
        return str(self.pColorTable)
    
    def __str__(self):
        return "<[" + str(self.pColorTable) + "]>"

import unittest

class TestSequenceFunctions(unittest.TestCase):

    def setUp(self):
        self.nbColors= 6
        self.tab1 = [1, 5, 3, 5, 5]
        self.tab2 = [random.randrange(0,self.nbColors) for obj in range(5)]

    def test_buildCombinaison(self):
        comb1 = Combinaison(self.tab1)
        comb1b = Combinaison(comb1)
        #print(comb1.pColorTable,comb1b.pColorTable)
        self.assertTrue(comb1 == comb1b)
        comb1.setNextCombinaison(self.nbColors)
        #print(comb1.pColorTable,comb1b.pColorTable)
        self.assertTrue(comb1 != comb1b)
        comb1b.setNextCombinaison(self.nbColors)
        #print(comb1.pColorTable,comb1b.pColorTable)
        self.assertTrue(comb1 == comb1b)
        
    def test_GetCorrection(self):
        #print(self.tab1,self.tab2)
        comb1 = Combinaison(self.tab1)
        comb2 = Combinaison(self.tab2)
        for blackIndex in range(6):
            for whiteIndex in range(6):
                if((blackIndex, whiteIndex) == (5,0)):
                    #print("un")
                    self.assertTrue(comb2.isCombinaisonCompatible(comb2,blackIndex,whiteIndex))
                else:
                    #print("deux")
                    self.assertFalse(comb2.isCombinaisonCompatible(comb2,blackIndex,whiteIndex))
        self.assertTrue(Combinaison([0,1,2]).isCombinaisonCompatible(Combinaison([1,2,0]),0,3))
        self.assertTrue(Combinaison([0,1,2]).isCombinaisonCompatible(Combinaison([2,1,0]),1,2))
        self.assertFalse(Combinaison([0,1,2]).isCombinaisonCompatible(Combinaison([2,1,0]),0,3))
        self.assertFalse(Combinaison([0,1,2]).isCombinaisonCompatible(Combinaison([2,1,0]),2,1))
        self.assertFalse(Combinaison([0,1,2]).isCombinaisonCompatible(Combinaison([2,1,0]),3,0))

    def test_NextCombi(self):
        comb1 = Combinaison(5)
        #print("0\t==>", comb1.pColorTable)
        for it in range(0,self.nbColors**len(self.tab1)):
            comb1.setNextCombinaison(self.nbColors)
        #print(it,"\t==>", comb1.pColorTable)
        self.assertTrue(Combinaison([5,5,5,5,5]) == comb1)


if __name__ == '__main__':
    unittest.main()

