#ifndef DATAREADER2_H
#define DATAREADER2_H

#include "commun.h"

class DataReader {

 private:

  const DataReaderIterator beg;
  const DataReaderIterator end;

 public:

  DataReaderIterator cursor;



 DataReader(const DataReaderIterator &beg, const DataReaderIterator &end) : beg(beg), end(end), cursor(beg) { }

  // met le curseur sur la prochaine ligne >= n et retourne la position courante
  DataReaderIterator jump_to(int n) {

    cursor = std::lower_bound(cursor, end, n,[](const std::pair<int, int> &a, const int &b){
        return (a.first < b);
      });

    return cursor;
  }

  DataReaderIterator readLine() {
    return ++cursor;
  }


  bool isOver() {
    return(cursor == end);
  }


  DataReaderIterator operator+(const int &n){
    if (n < distance(cursor, end))
      return cursor + n;
    else
      return end;
  }

  void resetCursor() {
    cursor = end;
  }
};


#endif
