#include "commun.h"

#include "DataReader.h"
#include "GraphPart.h"
#include "FileReader.h"
#include "FileWriter.h"

using namespace std;

/*
 * Fonction permettant de trouver les capitalistes parmis une liste d'id
 * dr  = liste normal
 * dr2 = liste inversé et trié
 */
vector<int> trouverCapitalistes(DataReader dr, DataReader dr2) {

  // Vecteur permetant de stocker les id des capitalistes trouvé par la fonction
  vector<int> capitalistesIds;

  // On reserve un certain espace dans le vecteur pour qu'il évite de réallouer de la place sans arrêts -> perte de temps
  capitalistesIds.reserve(3000);

  // On lit les 1ères lignes de chaque fichier pour éviter que res soit à null et que le programme s'arrête
  DataReaderIterator res = dr.readLine();
  DataReaderIterator res2 = dr2.readLine();

  // L'algorithme ci dessous sera détaillé dans le pdf join au projet
  while (!dr.isOver() && !dr2.isOver()) {
    if(res->first < res2->first) {

      res = dr.jump_to(res2->first);
		
    }
    else if(res->first > res2->first) {

      res2 = dr2.jump_to(res->first);

    }
    else {

      if((dr + 999)->first != res->first || (dr2 + 999)->first != res2->first) {

        res = dr.jump_to(res->first + 1);
        res2 = dr2.jump_to(res->first);

        continue;
      }

      int intersection = 0;
      int nbFollowers = 0;
			
      int id = res->first;
      DataReaderIterator tmp = res;
      res = dr.jump_to(id + 1);
      size_t nbFollowees = res - tmp;

      while (res2->first == id) {
        nbFollowers++;
        if(binary_search(tmp, tmp + nbFollowees, *res2)) {
          intersection++;
        }
        res2 = dr2.readLine();
      }

      if (getPourcentage(intersection, nbFollowers, nbFollowees) >= POURCENTAGE_MIN) {
        capitalistesIds.push_back(id);
      }
    }
  }

  return capitalistesIds;
}


/**
 *	Divise data et data2 en plusieurs parties (n)
 */
vector<GraphPart> split(const vector< pair<int, int> > &data, const vector< pair<int, int> > &data2, unsigned int n) {

  size_t block = data.size()/n;

  // iterator sur la position courante
  DataReaderIterator pos = data.begin();
  int prev;

  // vecteur contenant les coupures de la liste de gauche
  vector< pair<DataReaderIterator, int> > vecCoupuresGauche;

  for (unsigned int i = 0; i < (n - 1); i++) {

    pos+=block;

    prev = pos->first;
    while (pos->first == prev) {
      pos++;
    }

    // on rajoute une nouvelle coupure
    pair<DataReaderIterator, int> p(pos, prev);
    vecCoupuresGauche.push_back(p);
  }

  // étape 2 : on sépare les parties grace aux coupures

  vector<GraphPart> parts;

  auto pos1 = data.begin();
  auto pos2 = data2.begin();

  for (pair<DataReaderIterator, int> &c : vecCoupuresGauche){

    auto up = upper_bound(pos2, data2.end(), c.second, [](const int &a, const pair<int, int> &b) {
        return (a < b.first);
      });

    parts.push_back(GraphPart(pos1, c.first--, pos2, up));

    pos1 = c.first;
    pos2 = up;
  }

  parts.push_back(GraphPart(pos1, data.end(), pos2, data2.end()));

  return parts;
}


/**
 *	Fonction principal
 *
 *		Partie 1 : Lecture, Traitement du fichier
 *		Partie 2 : Copie et tri du tableau
 *		Partie 3 : Division des deux tableaux en plusieurs parties (pour pouvoir multithreader la partie 4)
 *		Partie 4 : Récupération des capitalistes pour chacune des parties
 *
 */
vector<int> init(string f1, int nbThreads, string mode) {
  clock_t start = clock();
  vector<int> idCapitalistes;

  /**
     Partie 1 : Lecture et traitement du fichier

  */

  FileReader fb1(f1);

  if (!fb1.isOpened()) {
    cout << "cannot open " << f1 << endl;
    return idCapitalistes;
  }
	
  vector< pair<int, int> > src;

  if(mode == "txt")
    fb1.convertToBinary(src, 160, 4, 150000000);
  else
    fb1.getData(src);

  cout << src.size()*2 << endl;

  size_t srcLen = src.size();
	
  cout << "Lecture du fichier : " << (double)(clock() - start)/CLOCKS_PER_SEC << endl;

  /**
     Partie 2

     1 - Copie et inversement du tableau
     2 - tri

  */

  start = clock();

  // allocation de la mémoire pour l'array inversé
  vector< pair<int, int> > invData;
  invData.resize(srcLen);

  // copie + inversion de l'array
  for (unsigned int i = 0; i < srcLen; i++) {
    invData[i].first = src[i].second;
    invData[i].second = src[i].first;
  }

  // tri
  t_sort(invData.begin(), invData.end(), srcLen/nbThreads);

  cout << "Inversement + Tri : " << (double)(clock() - start)/CLOCKS_PER_SEC << endl;

  /**
     Partie 3 : coupe nos deux array en plusieurs parties pour pouvoir multithreader
 	 
     1-  On coupe l'array 1 en N parties, chacune des N-1 coupures doivent s'arreter à la fin d'un id.
     La liste des followees de chaque personne ne doivent pas se trouver sur des parties differentes de l'array.
     Chaque coupure est enregistré sous la forme d'une pair : id, index. id étant le dernier id de la partie de l'array et index sa position.
	 
     2-  Pour chaque pair récupéréré par l'étape précedante, 
     on coupe l'array 2 de façon à se que chacune des parties de l'array 2 ne comporte que des id < à l'id de la coupure.
  */

  vector<GraphPart> parts = split(src, invData, nbThreads);

  /**

     Partie 4 : Traitement de chacune des parties.

     1-  Traitement des parties par la fonction trouverCapitalistes.

     2-  Mise en commun des résultats dans le vecteur res.

  */

  vector<future<vector<int>>> futures;

  for (GraphPart &s : parts) {

    DataReader drL(s.deb_left, s.end_left);
    DataReader drR(s.deb_right, s.end_right);

    futures.push_back(std::async(trouverCapitalistes, drL, drR));
  }

  for (int i = 0; i < nbThreads; ++i) {
    vector<int> c = futures[i].get();
    idCapitalistes.insert(idCapitalistes.end(), c.begin(), c.end());
  }

  return idCapitalistes;
}

int main(int argc, char** argv) {

  // nbTaches correspond au nombre de thread qui seront éxécutés mais aussi au nombre de taches (nombre de pair de fichier)
  const int nbTaches = 4;

  const char* inputFile  = "grapheProjet.txt";
  const char* outputFile = "Capitalistes.txt";
	
  // Permet de mesurer le temps que le programme prend
  clock_t start = clock();

  vector<int> c_vec = init(inputFile, nbTaches, "txt");

  cout << "time taken : " << (double)(clock() - start)/CLOCKS_PER_SEC << endl;

  // Le fichier dans lequel écrire le résultat
  FileWriter fw(outputFile, std::ios_base::out, "\n");
	
  fw.write(c_vec);
	
  // Affichage du temps prit par l'application pour effectuer toutes ses taches et affiche le nombre de capitaliste trouvé

  cout << "nb capitalistes : " << c_vec.size() << endl;
	
  return 0;
}
