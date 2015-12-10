#ifndef FILEREADER_H
#define FILEREADER_H

#include "commun.h" 

using namespace std;

class FileReader {

 private:

  ifstream is;
  size_t length;
  size_t bytesRead;
  bool streamOpened;

 public:

 FileReader() : streamOpened(false), length(0), bytesRead(0) {}

  // Ouvre le fichier et récupère le nombre d'octets contenu par ce fichier
 FileReader(string &file) : length(0), bytesRead(0) {

    is.open(file, ios::binary);

    if (!is.is_open()) {
      streamOpened = false;
    }
    else {
      streamOpened = true;
      is.seekg(0, ios::end);
      length = is.tellg();
      is.seekg(0, ios::beg);
    }

  }

  ~FileReader() {
    if (streamOpened)
      is.close();
  }

  size_t getLength() { 
    return length;
  }

  bool isOpened() { 
    return streamOpened;
  }

  /**
     Transforme data en une suite de int en utilisant atoi().
	 
     Met le resultat dans res.
  */
  static int charToInt(char *data, size_t size, vector<int> &res) {
    int num = 0;
    int elemsRead = 0;
    size_t bytesRead = 0;

    while(bytesRead < size-1) {

      num = atoi(&data[bytesRead]);

      if(num != 0) {
        res[elemsRead] = num;
        elemsRead++;
      }
			
      bytesRead+=numDigits(num)+1;
    }
    return elemsRead;
  }

  /**
     Même chose en utilisant strtol()
  */
  static int charToInt2(char *data, size_t size, vector<int> &res) {

    char *tmp = data;
    int elemsRead = 0;

    while(tmp < &data[size-1]) {

      res[elemsRead] = strtol(tmp, &tmp, 10);
      elemsRead++;
    }

    return elemsRead;
  }

  /**
     Prend en parametre un array de char et sa taille pour le couper en plusieurs parties (nbPart).
     La méthode fait attention de ne pas couper une ligne en 2.
	 
     Retourne une pair (char* + size) pour chaque parties.
  */
  vector<pair<char*, size_t>> splitBuffer(char *buffer, size_t bufferSize, int nbPart){

    size_t offsetCoupure = bufferSize/nbPart;

    vector<pair<char*, size_t>> parts;

    char *prec = buffer;
    char *courant = buffer;

    for(int i = 0; i < (nbPart - 1); i++) {

      courant += offsetCoupure;

      while(*courant != '\n') {
        courant++;
      }

      parts.push_back(make_pair(prec, courant-prec));

      prec = courant;
    }

    parts.push_back(make_pair(prec, &buffer[bufferSize]-prec));

    return parts;
  }

  /**
     Méthode principal pour lire le fichier et le convertir en un vecteur de int
  */
  void convertToBinary(vector<pair<int, int>> &result, int nbPart, int nbTaches, size_t maxSize){
		
    result.reserve(maxSize/2);

    /* allocation de la mémoire pour les vecteur qui vont contenir le resultat des threads */
    vector<vector<int>> vec_inter;
    for(int i = 0; i < nbTaches; i++){
      vec_inter.push_back(vector<int>());
      vec_inter[i].resize((maxSize/(nbPart*nbTaches)*1.2)); //taille max d'un vecteur
    }

    /* allocation de la mémoire pour notre buffer qui va servir à lire le fichier morceau par morceau */
    size_t bufferSize = length/nbPart;
    char* buffer = new char[bufferSize + 20];

    for(int i = 0; i < nbPart; i++){

      /* on lis une partie du fichier */
      size_t sizeRead = this->getDataTxt(buffer, bufferSize);

      /* on sépare le buffer en plusieur parties (pour pouvoir multithreader la conversion en int) */
      vector<pair<char*, size_t>> vp = splitBuffer(buffer, sizeRead, nbTaches);


      vector<future<int> > future_vec;
      for(int i = 0; i < vp.size(); i++) {
        /* on traite chaque partie parallelement, les resultats vont dans vec_inter */
        future_vec.push_back(async(charToInt, vp[i].first, vp[i].second, ref(vec_inter[i])));
      }

      /* On ajoute les resultats, dans l'ordre, dans le vecteur result */
      for(int i = 0; i < future_vec.size();i++) {
					
        int nbElems = future_vec[i].get();

        for(int j = 0; j < nbElems; j+=2) {
          result.push_back(make_pair((vec_inter[i])[j], (vec_inter[i])[j+1]));
        }

        //result.resize(result.size()+nbElems/2);
        //int * test = reinterpret_cast<int*>((result.end()-nbElems/2)._Ptr);
        //for(int j = 0; j < nbElems; j++){
        //	*test = (*vec_inter[i])[j];
        //	test++;
        //}
      }
    }

    delete[] buffer;
  }

  /**
     Lit le fichier associé au FileReader et met le résultat dans data en faisant 
     attention de ne pas couper une ligne en deux (on s'arrête sur un \n).

     Retourne le nombre d'octets vraiment lus (surement superieur à size)
  */
  size_t getDataTxt(char *data, size_t size) {
		
    /* On fait attention de ne pas dépasser la taille du fichier */
    if(size + bytesRead > length) {
      size = length - bytesRead;
    }

    is.read(data, size);

    /* On continue de lire octet par octet de façon à s'arréter sur une fin de ligne */
    while(data[size-1] != '\n' && !is.fail()) {
      is.read(&data[size], 1);
      size++;
    }

    bytesRead += size;

    return size;
  }

  void getData(vector<pair<int, int>> &data) {
    if (streamOpened) {
      data.resize(length / (sizeof(int)*2));
      is.read((char*)&data[0], length);
    }
  }
};

#endif
