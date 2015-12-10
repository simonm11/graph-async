#include "commun.h"

using namespace std;

typedef std::vector< std::pair<int, int> >::const_iterator DataReaderIterator;

/**
 *	Multithreaded merge sort
 *	min  : nombre d'element minimum par threads (normalement len/nbThreads)
 */
void t_sort(std::vector< std::pair<int, int> >::iterator first, std::vector< std::pair<int, int> >::iterator end, size_t min) {
	
  if (size_t(distance(first, end)) <= min) {
    std::sort(first, end, [](const std::pair<int, int> &a, const std::pair<int, int> &b) {
        return (a.first < b.first);	
      });
  }
  else {
    auto mid = first + (end - first)/2;	
    auto future = std::async(t_sort, first, mid, min);
    t_sort(mid, end, min);
		
    future.wait();

    std::inplace_merge (first, mid, end, [](const std::pair<int, int> &a,const std::pair<int, int> &b){
        return (a.first < b.first);
      });
  }
}


double getPourcentage(const int &nbIntersections, const int &nbFollowers, const size_t &nbFollowees) {

  if (nbFollowees < nbFollowers) {
    return ((double)nbIntersections / (double)nbFollowees) * 100.00;
  }
  else {
    return ((double)nbIntersections / (double)nbFollowers) * 100.00;
  }
}


std::string getType(size_t nbFriends, size_t nbFollowers) {
  if (nbFriends >= nbFollowers) {
    return "FMIFY";
  }
  else {
    return "IFYFM";
  }
}
