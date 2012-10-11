import combinaison
import mastersolver

def fullEnumeration(nbColors, nbPositions):
    master = mastersolver.MasterSolver(nbColors,nbPositions)
    combi = combinaison.Combinaison(nbPositions)
    bCondition = True
    while(bCondition):
        master.setSecretCombinaison(combi)
        master.solve()
        bCondition = combi.setNextCombinaison(nbColors)
    master.store()
    master.threadWorkerDestroy()

def unitaryTest(nbColors, nbPositions):
    master = mastersolver.MasterSolver(nbColors,nbPositions)
    combi = combinaison.Combinaison([0 for i in range(nbPositions)])
    combi2 = combinaison.Combinaison(combi)
    condition1, condition2 = True, True

    while(condition1):
        combi2.buildCombinaison(nbPositions)
        while(condition2):
            master.setSecretCombinaison(combi)
            score = master.getCorrectionOfSecretCombinaison(combi2)
            score2 = combi2.getCorrection(combi)
            print(combi ,"<==>" , combi2 , "correction(" , score[0] , "," , score[1] , ")",sep='')
            if(score != score2):
                print("##Error##(" , black2 , "," , white2 , ")")
            else:
                print("")
            condition2 = combi2.setNextCombinaison(nbColors)
        conditon1 = combi.setNextCombinaison(nbColors)

if __name__ == "__main__":
    import sys
    if(len(sys.argv)>1):
        listOfFiles = sys.argv[1:]
        
#    DEFAULT_NB_POSITIONS = 4
#    DEFAULT_NB_COLORS = 5

 #   nb_Colors, nb_Positions = DEFAULT_NB_COLORS, DEFAULT_NB_POSITIONS
    
#    fullEnumeration  (nb_Colors, nb_Positions)
    for somme in range(11,16):
        nb_Positions = somme
        for nb_Colors in range(1, somme):
            nb_Positions -= 1
            print("(",nb_Colors,",",nb_Positions,")")
            fullEnumeration  (nb_Colors, nb_Positions)

#    unitaryTest      (nb_Colors, nb_Positions)
