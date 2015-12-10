#include "DataReader.h"

using namespace std;

DataReader::DataReader(const DataReaderIterator &beg, const DataReaderIterator &end) : beg(beg), end(end), cursor(beg) { }

/* met le curseur sur la prochaine ligne >= n et retourne la nouvelle position */
DataReaderIterator DataReader::jump_to(int n) {

  cursor = std::lower_bound(cursor, end, n,[](const std::pair<int, int> &a, const int &b){
      return (a.first < b);
    });

  return cursor;
}

/* incrémente le cursor de 1 et retourne la nouvelle position */
DataReaderIterator DataReader::readLine() {
  return ++cursor;
}


/* Return true si on est arrivé à la fin */
bool DataReader::isOver() {
  return(cursor == end);
}

/* Retourne un itérateur sur cursor + n */
DataReaderIterator DataReader::operator+(const int &n){
  if (n < distance(cursor, end))
    return cursor + n;
  else
    return end;
}

/* Remet le curseur au debut */
void DataReader::resetCursor() {
  cursor = end;
}
