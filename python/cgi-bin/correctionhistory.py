import combinaison

class CorrectionHistory:
    def __init__(self, combinaison):
        self.correctioncombi = {}
        self.__combinaison = combinaison.clone()
    
    def addCorrectionCombiNode(self, combinaison, correction):
        correctionHistoryNode = CorrectionHistory(combinaison.clone())
        self.correctioncombi[correction] = correctionHistoryNode
        return correctionHistoryNode

    def IsThereANodeFromCorrection(self, correction):
        #print("Dictionnaire Correction combi: ",self.correctioncombi)
        return correction in self.correctioncombi

    def getNodeFromCorrection(self, correction):
        return self.correctioncombi[correction]

    @property
    def combinaison(self):
        return self.__combinaison

    def __repr__(self):
        return str(self.combinaison)


import pickle
import io

class PickleCorrectionHistory:
    def __init__(self, _position, _color):
        self.position = _position
        self.color = _color
        self.connectionHistory = None
    
    def load(self):
        with open(self.buildName(),"rb") as file_r:
            pickleconnectionHistory = pickle.load(file_r)
        return pickleconnectionHistory.connectionHistory

    def store(self, _connectionHistory):
        with open(self.buildName(),"wb") as file_w:
            self.connectionHistory = _connectionHistory
            pickle.dump(self,file_w)

    def buildName(self):
        return "p{}c{}.dat".format(self.position,self.color)
        
    
        
