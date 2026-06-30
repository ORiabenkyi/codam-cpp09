#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <deque>
#include <utility>
#include <vector>

class PmergeMe {
public:
    PmergeMe();
    PmergeMe(const PmergeMe& other);
    PmergeMe& operator=(const PmergeMe& other);
    ~PmergeMe();

    void run(int argc, char* argv[]);

private:
    std::vector<int> _vec;
    std::deque<int>  _deq;
    long             _comparisons;

    template <typename T>
    struct CountingLess {
        long& count;
        CountingLess(long& c) : count(c) {}
        bool operator()(const T& a, const T& b) const {
            ++count;
            return a < b;
        }
    };

    // Compares only .first (the sort key); .second is opaque payload that
    // rides along through swaps/inserts without costing comparisons.
    struct CountingLessFirst {
        long& count;
        CountingLessFirst(long& c) : count(c) {}
        bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) const {
            ++count;
            return a.first < b.first;
        }
    };

    void parseArgs(int argc, char* argv[]);

    void fjSortVec(std::vector<int>& arr);
    void fjSortVecTagged(std::vector<std::pair<int, int> >& arr);
    void fjSortDeq(std::deque<int>& arr);
    void fjSortDeqTagged(std::deque<std::pair<int, int> >& arr);

    std::vector<int> jacobsthalOrder(int n) const;
    long              theoreticalMax(int n) const;
};

#endif
