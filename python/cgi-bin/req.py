import cgitb
import cgi
import time
import re

import mastersolver
import combinaison


def TransformCombiTextToTab(combiText):
    return [int(i) for i in re.split("\D", combiText)]  
    

cgitb.enable()

print("Content-Type: text/html")    # HTML is following
print()                             

print("""<html>
  <head>
   <title>SVG inclus avec une balise <object> dans un fichier XHTML</title>
  </head>
  <body>""")

form = cgi.FieldStorage()
#print("<p>"+str(form))
if "nbColor" not in form or "combiText" not in form:
    print("<H1>Error</H1>")
    print("<p>Please fill in the nbColor and combiText fields.")
else:
    nbColor, combiText = int(form["nbColor"].value), form["combiText"].value
    combiTab = TransformCombiTextToTab(combiText)
    nbPositions = len(combiTab)
    master = mastersolver.MasterSolver(nbColor,nbPositions, _multiprocessing=False, _verbose=False)#no multiprocessing
    combi = combinaison.Combinaison(combiTab)
    master.setSecretCombinaison(combi)
    resultat = master.solve()
    master.store()
    master.threadWorkerDestroy()

    #print("<p>" , resultat, "</p>")
    
    for comb in resultat:
        print('<p>')
        for col in comb.pColorTable:
            print('<img src="/images/',col,'.png"/>',sep='')
        (blackPigs, whitePigs) = master.getCorrectionOfSecretCombinaison(combinaison.Combinaison(comb))
        print("Correction:", blackPigs * '<img src="/images/00.png"/>',whitePigs * '<img src="/images/01.png"/>','</p>', sep='')
    print('<a href="/index.html">autre demande</a>')
print("""</body>
</html>""")


