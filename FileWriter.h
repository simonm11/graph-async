#ifndef FILEWRITER_H
#define FILEWRITER_H

#include "commun.h"

// Permet d'écrire le contenu d'un vecteur dans un fichier sous 2 formats -> texte ou binaire (O_TEXT, O_BINARY)
class FileWriter {

 private:

  ofstream fd;
  ios_base::openmode mode;
  string sep;
  bool streamOpened;

 public:

  // Créer le fichier dans un certain mode (O_TEXT ou O_BINARY).
  // Si le fichier est ouvert en O_TEXT il y a la possibilité de spécifier un séparateur entre chaque donnée
 FileWriter(const char *file, ios_base::openmode mode, const string &sep = " ") : mode(mode), sep(sep) {

    fd.open(file, mode);

    if(!fd.is_open()) {
      streamOpened = false;
    }
    else {
      streamOpened = true;
    }
  }

  // Permet d'écrire le contenu du vector dans un fichier
  void write(const vector<int> &vec) {
    if (streamOpened) {
      if(mode == ofstream::binary) {
        fd.write((char*)&vec[0], vec.size()*sizeof(int));
      }
      else{
        for(auto &s : vec) {
          fd << s << sep;
        }
      }
    }
  }

  // Ferme le fichier
  ~FileWriter() {
    if (streamOpened) {
      fd.close();
    }
  }
};

#endif
