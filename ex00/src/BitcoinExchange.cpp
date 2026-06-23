#include "../inc/BitcoinExchange.hpp"

const std::string            BitcoinExchange::DEFAULT_DB = "data.csv";
std::map<std::string, double> BitcoinExchange::_db;

BitcoinExchange::BitcoinExchange() {}
BitcoinExchange::BitcoinExchange(const BitcoinExchange& other) { (void)other; }
BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other) { (void)other; return *this; }
BitcoinExchange::~BitcoinExchange() {}

void BitcoinExchange::loadDatabase(const std::string& path)
{
    std::ifstream file(path.c_str());
    if (!file.is_open())
        throw std::runtime_error("could not open database: " + path);

    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::size_t comma = line.find(',');
        if (comma == std::string::npos) continue;

        std::string date = line.substr(0, comma);
        double      rate = std::atof(line.substr(comma + 1).c_str());
        _db[date] = rate;
    }
}

static bool isLeapYear(int y)
{
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

static int daysInMonth(int m, int y)
{
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && isLeapYear(y))
        return 29;
    return days[m - 1];
}

bool BitcoinExchange::isValidDate(const std::string& date)
{
    if (date.size() != 10 || date[4] != '-' || date[7] != '-')
        return false;

    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (date[i] < '0' || date[i] > '9')
            return false;
    }

    int year  = std::atoi(date.substr(0, 4).c_str());
    int month = std::atoi(date.substr(5, 2).c_str());
    int day   = std::atoi(date.substr(8, 2).c_str());

    if (month < 1 || month > 12)
        return false;
    if (day < 1 || day > daysInMonth(month, year))
        return false;
    return true;
}

double BitcoinExchange::getRate(const std::string& date)
{
    if (_db.empty())
        throw std::runtime_error("Error: database is empty.");

    std::map<std::string, double>::const_iterator it = _db.upper_bound(date);
    if (it == _db.begin())
        throw std::runtime_error("Error: date out of range.");
    --it;
    return it->second;
}

void BitcoinExchange::makeExchange(const std::string& inputFile)
{
    loadDatabase(DEFAULT_DB);

    std::ifstream file(inputFile.c_str());
    if (!file.is_open())
        throw std::runtime_error("could not open file: " + inputFile);

    std::string line;
    std::getline(file, line); // skip header "date | value"

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::size_t sep = line.find(" | ");
        if (sep == std::string::npos) {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        std::string date     = line.substr(0, sep);
        std::string valueStr = line.substr(sep + 3);

        if (!isValidDate(date)) {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        double value;
        std::istringstream iss(valueStr);
        if (!(iss >> value)) {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        if (value < 0) {
            std::cerr << "Error: not a positive number." << std::endl;
            continue;
        }
        if (value > 1000) {
            std::cerr << "Error: too large a number." << std::endl;
            continue;
        }

        try {
            double rate = getRate(date);
            std::cout << date << " => " << value << " = " << (value * rate) << std::endl;
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }
}
