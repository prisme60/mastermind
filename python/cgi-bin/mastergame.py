import combinaison

class MasterGame:
    def __init__(self, _nbColors, _nbPositions):
        self.nbColors = _nbColors
        self.nbPositions = _nbPositions
        self.secretCombinaison = combinaison.Combinaison([0 for i in range(_nbPositions)])

    def setSecretCombinaison(self, combi):
        #print("setSecretCombinaison", combi.pColorTable)
        self.secretCombinaison = combi.clone()

    def getSecretCombinaison(self):
        return  self.secretCombinaison

    SecretCombinaison = property(getSecretCombinaison, setSecretCombinaison)

    def generateSecretCombinaison(self):
        self.secretCombinaison.random(m_nbColors)

    def getCorrection(self, referenceCombinaison, combiTotest):
        """returns (blackPigs,whitePigs)"""
        return referenceCombinaison.getCorrection(combiTotest);

    def getCorrectionOfSecretCombinaison(self, combiTotest):
        """returns (blackPigs,whitePigs)"""
        return self.secretCombinaison.getCorrection(combiTotest);

