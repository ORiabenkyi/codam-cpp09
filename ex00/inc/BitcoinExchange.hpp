#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <cstdlib>

class BitcoinExchange
{
private:
    static const std::string DEFAULT_DB;
    static std::map<std::string, double> _db;

    static void   loadDatabase(const std::string& path);
    static double getRate(const std::string& date);
    static bool   isValidDate(const std::string& date);

public:
    BitcoinExchange();
    BitcoinExchange(const BitcoinExchange& other);
    BitcoinExchange& operator=(const BitcoinExchange& other);
    virtual ~BitcoinExchange() = 0;

    static void makeExchange(const std::string& inputFile);
};

#endif
