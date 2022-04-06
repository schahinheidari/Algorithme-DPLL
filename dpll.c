#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SATISFIED 1
#define NOTSATISFIED -1
#define UNKNOWN 0

int * valuation; // Array de valorisation global pour faciliter l'accès lors de la récursivité
int clauseNumber;
int variableNumber;
int DEBUG = 0; // met sur 1 pour le débogage des impressions

struct Verbal
{
    // pointe vers le littéral suivant dans la clause
    struct Verbal * suivent; 
    int index;
};

struct Clause
{
    // pointe vers le premier verbal de la clause
    struct Clause * premiere;
    // pointe vers la clause suivante dans l'ensemble 
    struct Clause * suivent; 
};

// crée, initialise et retourne une Verbal vide
void printValuation()
{
    for (int i = 1; i < variableNumber + 1; i++)
    {
        printf("%d ", valuation[i], "\n");
    }  
}

// crée, initialise et retourne une Clause vide
struct Clause * creeClause()
{
    struct Clause * instance = malloc(sizeof(struct Clause));
    instance -> premiere = NULL;
    instance -> suivent = NULL;
    return instance;
};

// affiche l'état actuel du Array de valorisation
void printValuation()
{
  for (int i = 1; i < variableNumber + 1; i++) {
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
      l = l -> suivent;
    }
    printf("\n");
    itr = itr -> suivent;
  }
}


// trouve un verbal pur en parcourant toutes les clauses
int findPureVerbal(struct Clause * root)
{
// crée une table de recherche pour garder une trace de la pureté verbal
  int * literalLookup = (int*) calloc(variableNumber + 1, sizeof(int));
  struct Clause * itr = root;
  while (itr != NULL){
    struct Verbal * l = itr -> premiere;
    while (l != NULL){
      int vue = literalLookup[abs(l -> index)];
      if (vue == 0) literalLookup[abs(l -> index)] = sign(l -> index);
      else if (vue == -1 && sign(l->index) == 1) literalLookup[abs(l -> index)] = 2;
      else if (vue == 1 && sign(l -> index) == -1) literalLookup[abs(l -> index)] = 2;
      l = l -> suivent;
    }
    itr = itr -> suivent;
  }
// itération sur la table de recherche pour envoyer le premier verbal pur trouvé
  int i;
  for (i = 1; i < variableNumber + 1; i++) {
    if (literalLookup[i] == -1 || literalLookup[i] == 1) return i * literalLookup[i];
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
    if(itr -> premiere -> suivent == NULL){
      return itr -> premiere -> suivent;
    }
    itr = itr -> suivent;
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
int unitPropagation(struct Clause * root)
{
  int unitVerbalIndex = findUniteClause(root);
  if (DEBUG) printf("unit clause found with literal: %d\n", unitVerbalIndex);
  if (unitVerbalIndex == 0) return 0;
  // mettre la valorisation pour ce verbal
  if (DEBUG) printf("Setting value of literal %d as %d\n", abs(unitVerbalIndex), unitVerbalIndex > 0 ? 1 : 0);
  valuation[abs(unitVerbalIndex)] = unitVerbalIndex > 0 ? 1 : 0;
  // itération sur la clause définie sur
   // 1 - supprime les clauses contenant le verbal d'unité
   // 2 - supprime la version négative du verbal d'unité lorsqu'il est présent dans une clause

  struct Clause * itr = root;
  struct Clause * prev;
  while (itr != NULL){
    struct Verbal * currentL = itr->premiere;
    struct Verbal * previousL = createVerbal();
    while (currentL != NULL){
      if (currentL->index == unitVerbalIndex) {
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
      } else if (currentL->index == -unitVerbalIndex) {
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
      currentClause = createClause();
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
        currentVerbal = createLiteral();
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
