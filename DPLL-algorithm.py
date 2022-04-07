# Solveur SAT utilisant l'algorithme DPLL
import random
import sys
import copy
from collections import OrderedDict


# La clause se compose d'un ensemble de symboles, dont chacun est negative ou non. 
class Clause:
    def __init__(self):
        pass

    def fromStr(self, s):
        s = s.split()
        self.symbols = {}
        for sign in s:
            if sign[0] == "-":
                sign = -1
                mark = sign[1:]
            else:
                sign = 1
                mark = sign
            self.symbols[mark] = sign

    def __str__(self):
        tokens = []
        for mark,sign in self.symbols.items():
            sign = ""
            if sign == -1:
                sign += "-"
            sign += mark
            tokens.append(sign)
        return " ".join(tokens)

# L'instance SAT consiste en un ensemble de clauses CNF. 
# Toutes les clauses doivent être satisfaites pour que l'instance SAT soit satisfaite.
class SatInstance:
    def __init__(self):
        pass

    def fromStr(self, s):
        self.symbols = set()
        self.clauses = []
        for line in s.splitlines():
            clause = Clause()
            clause.fromStr(line)
            self.clauses.append(clause)
            for mark in clause.symbols:
                self.symbols.add(mark)
        self.symbols = sorted(self.symbols)

    def __str__(self):
        s = ""
        for clause in self.clauses:
            s += str(clause)
            s += "\n"
        return s

	# Renvoie True ou False si l'instance est satisfaite
    def isSatisfied(self, assign):
        # Boucle dans chaque clause de l'instance
        for clause in self.clauses: 
            solve = False # Marquer si la clause a été satisfaite
            # Boucle à travers chaque symbole dans la clause
            for mark in clause.symbols:
                    # Si l'affectation rend le symbole vrai -> toute la clause est vraie                
                try:
                    if(clause.symbols[mark] == assign[mark]):
                        solve = True
                # Le symbole est introuvable Si l'affectation rend le symbole vrai -> toute la clause est vraie dans l'affectation
                except:
                    return False

            # Si la clause n'a jamais été résolue -> l'affectation échoue
            if(solve == False):
                return False

        # Toutes les clauses ont été résolues
        return True

    # Trouve une clause d'unité dans une instance.
    # Renvoie le symbole de la clause d'unité et l'entrée du dictionnaire de valeurs 
    # s'il est trouvé ou False si aucune clause d'unité n'a été trouvée
    def findUnitClause(self):
        for clause in self.clauses:
            #Rechercher des clauses de longueur 1
            if(len(clause.symbols) == 1):
                return {next(iter(clause.symbols.keys())): next(iter(clause.symbols.values()))}
        return False # Renvoie False si aucune clause d'unité n'est trouvée

    # Recherche dans l'instance une clause qui est l'opposé de la clause unitaire. 
    # Si elle est trouvée, l'instance est insatisfaisante
    # Renvoie True si aucune clause de ce type n'est trouvée, False si elle est trouvée
    def notOppositeUnitClause(self, mark):
        key = next(iter(mark.keys())) # Unit clause key
        value = next(iter(mark.values())) # Unit clause value

        for clause in self.clauses:
            # Rechercher des clauses unitaires
            if(len(clause.symbols) == 1):
                # Vérifiez si la clé correspond
                if(next(iter(clause.symbols.keys())) == key):
                    # Vérifier si valeur si contraire
                    if(next(iter(clause.symbols.values())) != value):
                        return False # Clause opposée trouvée
        # Aucune clause d'unité opposée n'a été trouvée
        return True 

    
        
    # Réduit les clauses de la clause unitaire.
    # Si la clé et la valeur correspondantes sont trouvées dans une clause, 
    # alors la clause est vraie et peut être supprimée de l'instance
    # Si la clé correspondante mais la valeur opposée est trouvée, 
    # ce symbole peut être supprimé de la clause
    def simplify(self, mark):
        key = next(iter(mark.keys())) # Clé de clause d'unité
        value = next(iter(mark.values())) # Valeur de la clause d'unité

        delete = [] # Liste des clauses à supprimer
        index = 0

        for clause in self.clauses:
            try:
                # La clause a le même symbole qui est en cours de simplification, 
                # elle est donc vraie et peut être supprimée
                if(clause.symbols[key] == value):
                    delete = delete + [index] # Ajouter l'index des clauses   
                    index = index - 1 # Lors de la suppression, lorsque les clauses indexées ci-dessus sont supprimées, toutes les autres clauses descendent d'une valeur d'index                

                # La clause a le symbole opposé qui est simplifié, ainsi le symbole peut être supprimé de la clause
                else:
                    clause.symbols.pop(key, None)
         
            # La clause n'a pas le symbole alors sauter-le
            except:
                pass

            index = index+1
        
        # Supprimer toutes les clauses résolues
        for i in delete:
            del self.clauses[i]

        return self
 

#Trouve une affectation satisfaisante à une instance SAT,
def solveDpll(instance):
    c = copy.deepcopy(instance)

    assign = {}
    assign = dpllRecurse(instance, assign) # Obtenir l'affectation qui résout l'instance
    
    # Remplissez les symboles qui peuvent être attribués
    for mark in set(instance.symbols) - set(assign.keys()):
        assign[mark] = 1

    assign = dict(OrderedDict(sorted(assign.items(), key=lambda t: int(t[0])))) # Trier le dictionnaire d'affectations par symbole

    return assign
    


# Trouve récursivement l'affectation satisfaisante
# Renvoie l'affectation sous forme de dictionnaire ou False si aucune affectation satisfaisante
def dpllRecurse(instance, assign):
    # If instance has no more clauses then all clauses have been satisfied
    if(len(instance.clauses) == 0):
        return assign

    # Trouver toutes les clauses d'unité dans l'instance
    while(instance.findUnitClause()):
        unitClause = instance.findUnitClause()

# Si l'instance a également la clause d'unité opposée (a et !a) alors l'instance ne peut pas être résolue
        if(instance.notOppositeUnitClause(unitClause) == False):
            return False
        # Ajouter la clause d'unité à l'affectation et simplifiez les clauses d'instance
        else:
            instance.simplify(unitClause)
            assign[next(iter(unitClause.keys()))] = next(iter(unitClause.values()))

    # Si l'instance n'a plus de clauses alors toutes les clauses ont été satisfaites
    if(len(instance.clauses) == 0):
        return assign

    # Parcourez chaque symbole et essayez de l'attribuer pour voir s'il résout l'instance
    for mark in instance.symbols:
        instanceCopy = copy.deepcopy(instance) # Créer une copie de l'instance à utiliser si l'affectation positive échoue

        # Vérifier si le symbole est déjà dans l'affectation
        if(mark not in list(assign.keys())): 
            # Ajouter le symbole positif à l'affectation, simplifier l'instance et récurez dessus
            assign[mark] = 1
            
            # Si la clause contraire n'existe pas
            if(instance.notOppositeUnitClause({mark:1}) == True):
                if(dpllRecurse(instance.simplify({mark:1}), assign)): # Recurse avec symbole positif
                    return assign

                # Si l'affectation positive échoue, essayer l'affectation négative
                else:
                    assign[mark] = -1
                    # Si la clause contraire n'existe pas
                    if(instanceCopy.notOppositeUnitClause({mark:-1}) == True):
                        return dpllRecurse(instanceCopy.simplify({mark:-1}), assign) # Recurse avec symbole negative   





						
def main(file):
	with open(file, "r") as inputFile:
		fileStrs = inputFile.read()

	fileStrs = fileStrs.split("\n\n")

	with open("assignments.txt", "w") as outputFile:
		for fileStr in fileStrs:
			if fileStr.strip() == "":
				continue
			instance = SatInstance()
			instance.fromStr(fileStr)
			assign = solveDpll(instance)
			for markIndex, (mark,sign) in enumerate(assign.items()):
				if markIndex != 0:
					outputFile.write(" ")
				sign = ""
				if sign == -1:
					sign += "-"
				sign += mark
				outputFile.write(sign)
			outputFile.write("\n")
        
        

   
if __name__ == "__main__":
    main(sys.argv[1])
   