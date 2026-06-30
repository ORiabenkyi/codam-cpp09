#include "PmergeMe.hpp"
#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <utility>

PmergeMe::PmergeMe() : _comparisons(0) {}

PmergeMe::PmergeMe(const PmergeMe& o) : _vec(o._vec), _deq(o._deq), _comparisons(o._comparisons) {}

PmergeMe& PmergeMe::operator=(const PmergeMe& o) {
    if (this != &o) {
        _vec         = o._vec;
        _deq         = o._deq;
        _comparisons = o._comparisons;
    }
    return *this;
}

PmergeMe::~PmergeMe() {}

std::vector<int> PmergeMe::jacobsthalOrder(int n) const {
    std::vector<int> order;
    if (n == 0)
        return order;

    std::vector<int> jacob;
    jacob.push_back(1);
    jacob.push_back(3);
    while (jacob.back() - 1 < n) {
        int sz = static_cast<int>(jacob.size());
        jacob.push_back(jacob[sz - 1] + 2 * jacob[sz - 2]);
    }

    for (int k = 1; k < static_cast<int>(jacob.size()); k++) {
        int high = std::min(jacob[k] - 1, n);
        int low  = jacob[k - 1];
        for (int i = high; i >= low; i--)
            order.push_back(i);
        if (jacob[k] - 1 >= n)
            break;
    }
    return order;
}

void PmergeMe::fjSortVec(std::vector<int>& arr) {
    std::vector<std::pair<int, int> > tagged;
    tagged.reserve(arr.size());
    for (size_t i = 0; i < arr.size(); i++)
        tagged.push_back(std::make_pair(arr[i], 0));

    fjSortVecTagged(tagged);

    for (size_t i = 0; i < tagged.size(); i++)
        arr[i] = tagged[i].first;
}

void PmergeMe::fjSortVecTagged(std::vector<std::pair<int, int> >& arr) {
    int n = static_cast<int>(arr.size());
    if (n <= 1)
        return;

    bool                hasStraggler = (n % 2 != 0);
    std::pair<int, int> straggler;
    if (hasStraggler) {
        straggler = arr.back();
        arr.pop_back();
        n--;
    }

    CountingLess<int>      lessInt(_comparisons);
    CountingLessFirst      lessFirst(_comparisons);

    std::vector<std::pair<std::pair<int, int>, std::pair<int, int> > > pairs;
    pairs.reserve(n / 2);
    for (int i = 0; i < n; i += 2) {
        std::pair<int, int> a = arr[i], b = arr[i + 1];
        if (lessInt(a.first, b.first))
            std::swap(a, b);
        pairs.push_back(std::make_pair(a, b));
    }

    std::vector<std::pair<int, int> > larger;
    larger.reserve(pairs.size());
    for (size_t i = 0; i < pairs.size(); i++)
        larger.push_back(std::make_pair(pairs[i].first.first, static_cast<int>(i)));
    fjSortVecTagged(larger);

    std::vector<std::pair<int, int> > mainChain;
    mainChain.reserve(arr.size());
    std::vector<std::pair<int, int> > pendingA(larger.size());
    for (size_t i = 0; i < larger.size(); i++) {
        int localIdx = larger[i].second;
        mainChain.push_back(pairs[localIdx].first);
        pendingA[i] = pairs[localIdx].second;
    }

    std::vector<int> bPos(pendingA.size());
    for (size_t i = 0; i < bPos.size(); i++)
        bPos[i] = static_cast<int>(i);

    mainChain.insert(mainChain.begin(), pendingA[0]);
    for (size_t i = 0; i < bPos.size(); i++)
        bPos[i] += 1;

    int numPending = static_cast<int>(pendingA.size()) - 1;
    if (numPending > 0) {
        std::vector<int> order = jacobsthalOrder(numPending);
        for (size_t i = 0; i < order.size(); i++) {
            int                                             idx     = order[i];
            std::pair<int, int>                             aVal    = pendingA[idx];
            std::vector<std::pair<int, int> >::iterator     boundIt = mainChain.begin() + bPos[idx];
            std::vector<std::pair<int, int> >::iterator     pos =
                std::lower_bound(mainChain.begin(), boundIt, aVal, lessFirst);
            int insertIdx = static_cast<int>(pos - mainChain.begin());
            mainChain.insert(pos, aVal);
            for (size_t j = 0; j < bPos.size(); j++)
                if (bPos[j] >= insertIdx)
                    bPos[j]++;
        }
    }

    if (hasStraggler) {
        std::vector<std::pair<int, int> >::iterator pos =
            std::lower_bound(mainChain.begin(), mainChain.end(), straggler, lessFirst);
        mainChain.insert(pos, straggler);
    }

    arr = mainChain;
}

void PmergeMe::fjSortDeq(std::deque<int>& arr) {
    std::deque<std::pair<int, int> > tagged;
    for (size_t i = 0; i < arr.size(); i++)
        tagged.push_back(std::make_pair(arr[i], 0));

    fjSortDeqTagged(tagged);

    for (size_t i = 0; i < tagged.size(); i++)
        arr[i] = tagged[i].first;
}

void PmergeMe::fjSortDeqTagged(std::deque<std::pair<int, int> >& arr) {
    int n = static_cast<int>(arr.size());
    if (n <= 1)
        return;

    bool                hasStraggler = (n % 2 != 0);
    std::pair<int, int> straggler;
    if (hasStraggler) {
        straggler = arr.back();
        arr.pop_back();
        n--;
    }

    CountingLess<int> lessInt(_comparisons);
    CountingLessFirst lessFirst(_comparisons);

    std::vector<std::pair<std::pair<int, int>, std::pair<int, int> > > pairs;
    pairs.reserve(n / 2);
    for (int i = 0; i < n; i += 2) {
        std::pair<int, int> a = arr[i], b = arr[i + 1];
        if (lessInt(a.first, b.first))
            std::swap(a, b);
        pairs.push_back(std::make_pair(a, b));
    }

    std::deque<std::pair<int, int> > larger;
    for (size_t i = 0; i < pairs.size(); i++)
        larger.push_back(std::make_pair(pairs[i].first.first, static_cast<int>(i)));
    fjSortDeqTagged(larger);

    std::deque<std::pair<int, int> > mainChain;
    std::vector<std::pair<int, int> > pendingA(larger.size());
    for (size_t i = 0; i < larger.size(); i++) {
        int localIdx = larger[i].second;
        mainChain.push_back(pairs[localIdx].first);
        pendingA[i] = pairs[localIdx].second;
    }

    std::vector<int> bPos(pendingA.size());
    for (size_t i = 0; i < bPos.size(); i++)
        bPos[i] = static_cast<int>(i);

    mainChain.push_front(pendingA[0]);
    for (size_t i = 0; i < bPos.size(); i++)
        bPos[i] += 1;

    int numPending = static_cast<int>(pendingA.size()) - 1;
    if (numPending > 0) {
        std::vector<int> order = jacobsthalOrder(numPending);
        for (size_t i = 0; i < order.size(); i++) {
            int                                          idx     = order[i];
            std::pair<int, int>                          aVal    = pendingA[idx];
            std::deque<std::pair<int, int> >::iterator   boundIt = mainChain.begin() + bPos[idx];
            std::deque<std::pair<int, int> >::iterator   pos =
                std::lower_bound(mainChain.begin(), boundIt, aVal, lessFirst);
            int insertIdx = static_cast<int>(pos - mainChain.begin());
            mainChain.insert(pos, aVal);
            for (size_t j = 0; j < bPos.size(); j++)
                if (bPos[j] >= insertIdx)
                    bPos[j]++;
        }
    }

    if (hasStraggler) {
        std::deque<std::pair<int, int> >::iterator pos =
            std::lower_bound(mainChain.begin(), mainChain.end(), straggler, lessFirst);
        mainChain.insert(pos, straggler);
    }

    arr = mainChain;
}

long PmergeMe::theoreticalMax(int n) const {
    long total = 0;
    for (int i = 1; i <= n; i++) {
        long k = 0;
        while ((4L << k) < 3L * i)
            k++;
        total += k;
    }
    return total;
}

void PmergeMe::parseArgs(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        errno    = 0;
        char* ep = NULL;
        long  v  = std::strtol(argv[i], &ep, 10);
        if (argv[i] == ep || *ep != '\0' || errno == ERANGE || v < 0 || v > 2147483647L)
            throw std::runtime_error("Error. Invalid input");
        _vec.push_back(static_cast<int>(v));
        _deq.push_back(static_cast<int>(v));
    }
    if (_vec.empty())
        throw std::runtime_error("Error. Empty");
}

void PmergeMe::run(int argc, char* argv[]) {
    
    // input data to vector and deq
    parseArgs(argc, argv);

    std::cout << "Before:";
    for (size_t i = 0; i < _vec.size(); i++)
        std::cout << " " << _vec[i];
    std::cout << "\n";

    _comparisons     = 0;
    std::clock_t t0v = std::clock();
    //sort vector
    fjSortVec(_vec);
    std::clock_t t1v = std::clock();
    long cmpVec      = _comparisons;

    _comparisons     = 0;
    std::clock_t t0d = std::clock();
    //sort deq
    fjSortDeq(_deq);
    std::clock_t t1d = std::clock();
    long cmpDeq      = _comparisons;

    std::cout << "After: ";
    for (size_t i = 0; i < _vec.size(); i++)
        std::cout << " " << _vec[i];
    std::cout << "\n";

    //show result
    double usVec = static_cast<double>(t1v - t0v) / CLOCKS_PER_SEC * 1e6;
    double usDeq = static_cast<double>(t1d - t0d) / CLOCKS_PER_SEC * 1e6;

    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Time to process a range of " << _vec.size()
              << " elements with std::vector : " << usVec << " us\n";
    std::cout << "Time to process a range of " << _deq.size()
              << " elements with std::deque  : " << usDeq << " us\n";

    std::cout << "Comparisons with std::vector : " << cmpVec << "\n";
    std::cout << "Comparisons with std::deque  : " << cmpDeq << "\n";

    long maxCmp = theoreticalMax(static_cast<int>(_vec.size()));
    std::cout << "Maximum allowed by algorithm for " << _vec.size()
              << " elements: " << maxCmp << "\n";
}
