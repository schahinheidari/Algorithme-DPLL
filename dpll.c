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
