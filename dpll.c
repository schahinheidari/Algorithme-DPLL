#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SATISFIED 1
#define NOTSATISFIED -1
#define UNKNOWN 0

int * valuation; // Array de valorisation global pour faciliter l'accès lors de la récursivité
int clauseNumber;
int valueNumber;
int DEBUG = 0; // met sur 1 pour le débogage des impressions

struct Verbal
{
    // pointe vers le littéral suivant dans la clause
    struct Verbal * suivant; 
    int index;
};

struct Clause
{
    // pointe vers le premier verbal de la clause
    struct Clause * premiere;
    // pointe vers la clause suivante dans l'ensemble 
    struct Clause * suivant; 
};

// crée, initialise et retourne une Verbal vide
void printValuation()
{
    for (int i = 1; i < valueNumber + 1; i++)
    {
        printf("%d ", valuation[i], "\n");
    }  
}

// crée, initialise et retourne une Clause vide
struct Clause * creeClause()
{
    struct Clause * instance = malloc(sizeof(struct Clause));
    instance -> premiere = NULL;
    instance -> suivant = NULL;
    return instance;
};

// affiche l'état actuel du Array de valorisation
void printValuation()
{
  for (int i = 1; i < valueNumber + 1; i++) {
    printf("%d ", valuation[i], "\n");
  }
}

void printClauseSet(struct Clause * root)
{
  struct Clause * itr = root;
  while (itr != NULL){
    struct Verbal * l = itr -> premiere;
    while (l != NULL){
      printf("%d ", l -> index);
      l = l -> suivant;
    }
    printf("\n");
    itr = itr -> suivant;
  }
}


// trouve un verbal pur en parcourant toutes les clauses
int findPureVerbal(struct Clause * root)
{
// crée une table de recherche pour garder une trace de la pureté verbal
  int * verbalLookup = (int*) calloc(valueNumber + 1, sizeof(int));
  struct Clause * itr = root;
  while (itr != NULL){
    struct Verbal * l = itr -> premiere;
    while (l != NULL){
      int vue = verbalLookup[abs(l -> index)];
      if (vue == 0) verbalLookup[abs(l -> index)] = sign(l -> index);
      else if (vue == -1 && sign(l->index) == 1) verbalLookup[abs(l -> index)] = 2;
      else if (vue == 1 && sign(l -> index) == -1) verbalLookup[abs(l -> index)] = 2;
      l = l -> suivant;
    }
    itr = itr -> suivant;
  }
// itération sur la table de recherche pour envoyer le premier verbal pur trouvé
  int i;
  for (i = 1; i < valueNumber + 1; i++) {
    if (verbalLookup[i] == -1 || verbalLookup[i] == 1) return i * verbalLookup[i];
  }
  // aucun littéral pur trouvé, renvoie 0
  return 0;
}

// trouve une clause d'unité et renvoie son index verbal pour l'étape de propagation d'unité
int findUniteClause(struct Clause * root)
{
  struct Clause * itr = root;
  while (itr != NULL){
    if (itr -> premiere == NULL) {
      if (DEBUG) printf("Vide clause\n");
      continue;
    }
    if(itr -> premiere -> suivant == NULL){
      return itr -> premiere -> suivant;
    }
    itr = itr -> suivant;
  }
  // aucune clause d'unité trouvée, renvoie 0
  return 0;
}

// fonction signal
int sign(int num)
{
  return (num > 0) - (num < 0);
}

// implémente l'algorithme de propagation d'unité renvoie 0 
// s'il est incapable d'exécuter l'algorithme dans le cas où il n'y a pas de verbals d'unité
int unitePropagation(struct Clause * root)
{
  int uniteVerbalIndex = findUniteClause(root);
  if (DEBUG) printf("unit clause found with literal: %d\n", uniteVerbalIndex);
  if (uniteVerbalIndex == 0) return 0;
  // mettre la valorisation pour ce verbal
  if (DEBUG) printf("Setting value of literal %d as %d\n", abs(uniteVerbalIndex), uniteVerbalIndex > 0 ? 1 : 0);
  valuation[abs(uniteVerbalIndex)] = uniteVerbalIndex > 0 ? 1 : 0;
  // itération sur la clause définie sur
   // 1 - supprime les clauses contenant le verbal d'unité
   // 2 - supprime la version négative du verbal d'unité lorsqu'il est présent dans une clause

  struct Clause * itr = root;
  struct Clause * prev;
  while (itr != NULL){
    struct Verbal * currentL = itr->premiere;
    struct Verbal * previousL = creeVerbal();
    while (currentL != NULL){
      if (currentL->index == uniteVerbalIndex) {
        //verbal d'unité trouvé, supprimer la clause entière et réajuster les pointeurs
        if (DEBUG) printf("Removing the clause that starts with %d\n", itr -> premiere -> index);
        if (itr == root){
          // la racine doit changer si nous supprimons la première clause
          *root = *(root->suivant);
          itr = NULL;
        } else {
          prev->suivant = itr->suivant;
          itr = prev;
        }
        break;
      } else if (currentL->index == -uniteVerbalIndex) {
        // verbal d'unité inversé trouvé, supprimez-le de la clause. Les autres verbals doivent rester
        if (DEBUG) printf("Removing the verbal %d from the clause that starts with %d\n", currentL->index, itr->premiere -> index); 
        // si c'est le premier verbal de la clause, le pointeur principal doit changer
        if (currentL == itr->premiere) itr->premiere = currentL->suivant;
        else {
          previousL->suivant = currentL->suivant;
        }
        currentL = previousL;
        continue;
      }
      // mettre à jour les variables de boucle pour un accès plus facile dans les itérations suivantes
      previousL = currentL;
      currentL = currentL->suivant;
    }
    // met à jour les variables de boucle pour un accès plus facile dans les itérations suivantes 
    // si la clause racine est supprimée, alors que nous devrions toujours être dans la (nouvelle) racine
    prev = itr;
    itr = itr == NULL ? root : itr->suivant;
  }
  return 1;
}

// implémente l'algorithme d'élimination des verbals purs renvoie 0 
// s'il est incapable d'exécuter l'algorithme s'il n'y a pas de verbals purs
int pureVerbalElimination(struct Clause * root)
{
  int pureVerbalIndex = findPureVerbal(root);
  if (DEBUG) printf("pure verbal found: %d\n", pureVerbalIndex);
  if (pureVerbalIndex == 0) return 0;

  // mettre la valorisation pour ce verbal
  if (DEBUG) printf("Setting value of verbal %d as %d\n", abs(pureVerbalIndex), pureVerbalIndex > 0 ? 1 : 0);
  valuation[abs(pureVerbalIndex)] = pureVerbalIndex > 0 ? 1 : 0;

  // itération sur l'ensemble de clauses pour supprimer les clauses contenant le verbal pur
  struct Clause * itr = root;
  struct Clause * prev;
  while (itr != NULL){
    struct Verbal * l = itr->premiere;
    while (l != NULL){
      if (l->index == pureVerbalIndex) {
        // verbal d'unité trouvé, supprimer la clause entière et réajuster les pointeurs
        if (DEBUG) printf("Removing the clause that starts with %d\n", itr->premiere->index);
        if (itr == root){
          // la racine doit changer si nous supprimons la première clause
          *root = *(root->suivant);
          itr = NULL;
        } else {
          prev->suivant = itr->suivant;
          itr = prev;
        }
        break;
      }
      l = l->suivant;
    }
   // met à jour les variables de boucle pour un accès plus facile dans les itérations suivantes 
   // si la clause racine est supprimée, alors que nous devrions toujours être dans la (nouvelle) racine
    prev = itr;
    itr = itr == NULL ? root : itr->suivant;
  }
  return 1;
}

// lit l'ensemble de clauses à partir du fichier donné et construit une liste chaînée de listes chaînées
struct Clause * readClauseSet(char * filename)
{
  FILE * fp;
  char line[256];
  size_t len = 0;

  fp = fopen(filename, "r");
  if (fp == NULL) exit(1);

  // définir des variables de boucle
  char * token;
  struct Clause * root = NULL, * currentClause = NULL, * previousClause = NULL;
  struct Verbal * currentVerbal = NULL, * previousVerbal = NULL;

  while(fgets(line, sizeof(line), fp)){
    // ignorer les lignes de commentaire
    if (line[0] == 'c') continue;
    // cette ligne est une information de métadonnées
    if (line[0] == 'p') {
      sscanf(line, "p cnf %d %d", &valueNumber, &clauseNumber);
      if (DEBUG) printf("Number of variables: %d\n", valueNumber);
      if (DEBUG) printf("Number of clauses: %d\n", clauseNumber);
      valuation = (int*) calloc(valueNumber + 1, sizeof(int));
      int i;
      for (i = 0; i < valueNumber + 1; i++) valuation[i] = -1;
    } else {
      // créer une clause pour chaque ligne
      currentClause = creeClause();
      if (root == NULL) {
        if (DEBUG) printf("setting root\n");
        root = currentClause;
      }
      if (previousClause != NULL) {
        if (DEBUG) printf("setting current as the suivant of previous clause\n");
        previousClause->suivant = currentClause;
      }

      // diviser la ligne par l'espace characté et analyser les entiers comme des verbal
      token = strtok(line, " ");
      while(token != NULL){
        int VerbalIndex = atoi(token);
        currentVerbal = creeVerbal();
        currentVerbal->index = VerbalIndex;
        if (VerbalIndex != 0){
          if (currentClause->premiere == NULL){
            if (DEBUG) printf("setting literal %d as premier of current clause\n", currentVerbal->index);
            currentClause->premiere = currentVerbal;
          }

          if (previousVerbal != NULL){
            if (DEBUG) printf("setting literal %d as the suivant of previous literal\n", currentVerbal->index);
            previousVerbal->suivant = currentVerbal;
          }
        }

        if (DEBUG) printf("current literal is now previous literal\n");
        previousVerbal = currentVerbal;

        token = strtok(NULL, " ");
      }
      if (DEBUG) printf("current clause is now previous clause\n");
      previousClause = currentClause;
    }
  }
  fclose(fp);

  return root;
}

// vérifie si toutes les clauses restantes contiennent des verbals non conflictuels, 
// c'est-à-dire que pour chaque verbal restant dans l'ensemble de clauses, 
// un index positif ou négatif doit être présent. Si tel est le cas, la satisfiabilité est résolue.
int areAllClausesUnit(struct Clause * root)
{
  int * verbalLookup = (int*) calloc(valueNumber + 1, sizeof(int));

  struct Clause* itr = root;
  while (itr != NULL){
    struct Verbal * l = itr->premiere;
    while (l != NULL){
      int vue = verbalLookup[abs(l->index)];
      if (vue == 0) verbalLookup[abs(l->index)] = sign(l->index);
      // si nous avons déjà vu ce verbal avec le signe opposé, retourner false
      else if (vue == -1 && sign(l->index) == 1) return 0;
      else if (vue == 1 && sign(l->index) == -1) return 0;
      l = l->suivant;
    }
    itr = itr->suivant;
  }

  // si nous avons atteint ici, cela signifie que l'ensemble de clauses ne contient aucun 
  // verbal en conflit itérer sur l'ensemble de clauses une dernière fois pour décider de leur évaluation
  itr = root;
  while (itr != NULL){
    struct Verbal * l = itr->premiere;
    while (l != NULL){
      valuation[abs(l->index)] = l->index > 0 ? 1 : 0;
      l = l->suivant;
    }
    itr = itr->suivant;
  }

  // renvoie true pour terminer dpll
  return 1;
}

// renvoie si l'ensemble de clauses contient une clause vide sans verbal à l'intérieur
int containsEmptyClause(struct Clause * root)
{
  struct Clause* itr = root;
  while (itr != NULL){
    // si le pointeur principal est nul, aucun verbal
    if(itr->premiere == NULL) return 1;
    itr = itr->premiere;
  }
  return 0;
}

// vérifie si l'état actuel de l'ensemble de clauses représente une solution
int checkSolution(struct Clause * root)
{
  if (containsEmptyClause(root)) return NOTSATISFIED;
  if (areAllClausesUnit(root)) return SATISFIED;
  return UNKNOWN;
}

// renvoie un index verbal aléatoire pour effectuer un branchement
int chooseVerbal(struct Clause * root)
{
  // renvoie simplement le premier verbal, cela ne change pas le résultat, 
  // mais il est peut-être préférable d'utiliser une approche plus intelligente 
  // pour la vitesse (par exemple, choisissez le verbal le plus fréquent)
  return root->premiere->index;
}

// clone en profondeur un ensemble de clauses et injecte une nouvelle clause d'unité avec
// l'index verbal donné c'est ainsi que le branchement est effectué
struct Clause * branch(struct Clause * root, int verbalIndex)
{
  if (DEBUG) printf("Branching with literal %d\n", verbalIndex);
  if (DEBUG) printf("Setting value of literal %d as %d\n", abs(verbalIndex), verbalIndex > 0 ? 1 : 0);

 // définit l'évaluation du verbal, nous pouvons revenir en arrière et cette évaluation peut devenir 
 // obsolète, mais cela n'a pas d'importance puisque la branche en arrière l'écrasera avec la nouvelle évaluation
  valuation[abs(verbalIndex)] = verbalIndex > 0 ? 1 : 0;

  struct Clause * newClone = NULL,
                * currentClause = NULL,
                * previousClause = NULL,
                * iterator = root;
  // cloner en profondeur chaque clause une par une
  while (iterator != NULL){
    struct Clause * clone = cloneClause(iterator);
    if (newClone == NULL) {
      newClone = clone;
    }
    if (previousClause != NULL) {
      previousClause->suivant = clone;
    }
    previousClause = clone;
    iterator = iterator->suivant;
  }
  // créez une nouvelle clause d'unité avec le verbalIndex donné ajoutez-le à la première place en tant que
  // nouvelle racine, car nous voulons nous assurer que le même verbalIndex sera choisi dans la propagation d'unité immédiate suivante
  struct Clause * addedClause = creeClause();
  struct Verbal * addedVerbal = creeVerbal();
  addedVerbal->index = verbalIndex;
  addedClause->premiere = addedVerbal;

  addedClause->suivant = newClone;
  return addedClause;
}

// clone en profondeur une clause en construisant une nouvelle clause et des structures verbal
struct Clause * cloneClause(struct Clause * origin)
{
  struct Clause * cloneClause = creeClause();
  struct Verbal * iteratorVerbal = origin->premiere;
  struct Verbal * previousVerbal = NULL;

  // itérer sur la clause pour cloner également les verbals
  while (iteratorVerbal != NULL){
    struct Verbal * verbalClone = creeVerbal();
    verbalClone->index = iteratorVerbal->index;
    if (cloneClause->premiere == NULL) {
      cloneClause->premiere = verbalClone;
    }
    if (previousVerbal != NULL) {
      previousVerbal->suivant = verbalClone;
    }
    previousVerbal = verbalClone;
    iteratorVerbal = iteratorVerbal->suivant;
  }
  return cloneClause;
}


void removeClause(struct Clause * root){
  while (root != NULL) {
    struct Clause * suivant = root->suivant;
    if (root->premiere != NULL) removeVerbal(root->premiere);
    free(root);
    root = suivant;
  }
}

void removeVerbal(struct Verbal * verbal){
  while (verbal != NULL) {
    struct Verbal * suivant = verbal->suivant;
    free(verbal);
    verbal = suivant;
  }
}

// écrit la solution dans le fichier donné
void writeSolution(struct Clause * root, char * filename){
  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  // itérer sur le tableau d'évaluation pour imprimer les valeurs de chaque verbal
  for (int i = 1; i < valueNumber + 1; i++) {
    fprintf(f, "%d %d\n", i, valuation[i]);
  }

  fclose(f);
}

int main(int argc, char *argv[]){
  if (argc < 3) {
    printf("usage: ./dpll [problemX.cnf] [solutionX.sol]\n");
    return 1;
  }

  struct Clause * root = readClauseSet(argv[1]);

  if (dpll(root) == SATISFIED) {
    printf("SATISFIED\n");
    writeSolution(root, argv[2]);
  } else {
    printf("UNSATISFIED\n");
  }
  removeClause(root);
  return 0;
}

// Algorithme DPLL avec recursive backtracking
int dpll(struct Clause * root)
{
  // vérifiez d'abord si nous sommes déjà dans un état résolu
  int solution = checkSolution(root);
  if (solution != UNKNOWN){
    removeClause(root);
    return solution;
  }

  // faire la propagation unitaire tant que l'ensemble de clauses le permet
  while(1){
    solution = checkSolution(root);
    if (solution != UNKNOWN){
      removeClause(root);
      return solution;
    }
    if (!unitePropagation(root)) break;
  }

  // puis faites une élimination verbale pure tant que l'ensemble de clauses le permet
  while(1){
    int solution = checkSolution(root);
    if (solution != UNKNOWN) {
      removeClause(root);
      return solution;
    }
    if (!pureLiteralElimination(root)) break;
  }

  // si nous sommes coincés, alors choisissons un verbal au hasard et branchez-le dessus
  int verbalIndex = chooseVerbal(root);
  if (DEBUG) printf("Branching on literal %d\n", verbalIndex);

  // insérez une nouvelle clause d'unité avec ce verbal choisi, et récurez
  if (dpll(branch(root, verbalIndex)) == SATISFIED) return SATISFIED;

  // si cela ne donne pas de solution, essayez la même chose avec le verbal nié
  return dpll(branch(root, -verbalIndex));
}

