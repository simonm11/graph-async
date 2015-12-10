#ifndef GRAPHPART_H
#define GRAPHPART_H

#include "commun.h"

typedef std::vector< std::pair<int, int> >::const_iterator DataReaderIterator;

struct GraphPart {

GraphPart(DataReaderIterator &a, DataReaderIterator &b, DataReaderIterator &c, DataReaderIterator &d):
  deb_left(a), end_left(b), deb_right(c) ,end_right(d) { }

  DataReaderIterator deb_left;
  DataReaderIterator end_left;
  DataReaderIterator deb_right;
  DataReaderIterator end_right;
};

#endif
