#include <chrono>
#include <ctime>
#include <iostream>
#include <map>
#include <math.h>
#include <string>
#include <vector>

using namespace std;

// utility functions
string padLeft(string str, char padChar, int len) {
  while (str.length() < len) {
    str.insert(0, 1, padChar);
  }
  return str;
}

string prettyBool(bool boolean) { return boolean ? "true" : "false"; }

enum Gender { FEMALE, MALE };

string genderStr(Gender gender) {
  return gender == Gender::MALE ? "Male" : "Female";
}

int getYearDiff(tm start) {

  time_t x = mktime(&start);
  time_t y = time(nullptr);

  if (x != (std::time_t)(-1) && y != (std::time_t)(-1)) {
    return floorf(std::difftime(y, x) / (60 * 60 * 24 * 365.25));
  }

  return -1;
}

string getFormattedDate(tm date) {
  return padLeft(to_string(date.tm_mday), '0', 2) + "/" +
         padLeft(to_string(date.tm_mon + 1), '0', 2) + "/" +
         to_string(date.tm_year + 1900);
}

struct Stock {
  string company;
  string code;
};

struct StockListing : public Stock {
  double price;
};

struct OwnedStock : public Stock {
  double amount;
};

void logStocks(map<string, StockListing> stocks) {
  cout << endl << "=== Available Stocks ===" << endl;

  for (std::map<string, StockListing>::iterator it = stocks.begin();
       it != stocks.end(); ++it) {
    cout << "Company: " << it->second.company << " || "
         << "Code: " << it->second.code << " || "
         << "Price: $" << it->second.price << endl;
  }
  cout << endl;
}

class InvestmentAccout {
  string userId;
  map<string, OwnedStock> ownedStocks;

public:
  InvestmentAccout(string userId) : userId(userId) {}

  void buyStock(string code, double payment,
                map<string, StockListing> availableStocks) {
    // check if stock exists
    if (availableStocks.find(code) == availableStocks.end()) {
      return;
    }

    // check if user owns stock
    if (ownedStocks.find(code) == ownedStocks.end()) {
      this->ownedStocks[code] = {availableStocks[code].company,
                                 availableStocks[code].code,
                                 payment / availableStocks[code].price};

      return;
    }

    this->ownedStocks[code].amount += payment / availableStocks[code].price;

    cout << code << this->ownedStocks[code].amount << endl;
  }

  void logStocks() {

    cout << endl << "=== Owned Stocks ===" << endl;

    for (std::map<string, OwnedStock>::iterator it = this->ownedStocks.begin();
         it != ownedStocks.end(); ++it) {
      cout << "Company: " << it->second.company << " || "
           << "Code: " << it->second.code << " || "
           << "Amount: " << it->second.amount << endl;
    }
    cout << endl;
  }

  void log() { cout << this->userId << endl; }
};

class User {
  string id;

protected:
  string name;
  bool isCitizen;
  Gender gender;
  tm birthday;
  InvestmentAccout *investmentAccout;

public:
  User(string id, string name, bool isCitizen, Gender gender, tm birthday)
      : id(id), name(name), isCitizen(isCitizen), gender(gender),
        birthday(birthday), investmentAccout(nullptr) {}

  int getAge() { return getYearDiff(this->birthday); }

  void logBasicData() {
    cout << "id: " << this->id << endl;
    cout << "name: " << this->name << endl;
    cout << "age: " << this->getAge() << endl;
    cout << "is a citizen: " << prettyBool(this->isCitizen) << endl;
    cout << "gender: " << genderStr(this->gender) << endl;
    cout << "birthday: " << getFormattedDate(this->birthday) << endl;
    if (this->investmentAccout != nullptr) {
      this->investmentAccout->logStocks();
    }
  }

  void createInvestmentAccount() {
    this->investmentAccout = new InvestmentAccout(this->id);
  }

  void buyStock(string code, double payment,
                map<string, StockListing> availableStocks) {
    this->investmentAccout->buyStock(code, payment, availableStocks);
  }

  ~User() { delete investmentAccout; }
};

void logUsers(vector<User> &users) {
  cout << endl << "=== All Customers ===" << endl;
  for (int i = 0; i < users.size(); i++) {
    cout << "---" << endl;
    users[i].logBasicData();
  }
  cout << endl;
}

int main() {
  // stock listings
  map<string, StockListing> stocks = {
      {"AMZN", {"Amazon.com Inc.", "AMZN", 114.8}},
      {"NFLX", {"Netflix Inc.", "NFLX", 367.4}},
      {"NVDA", {"Nvidia Corp.", "NVDA", 313.2}}};

  // users
  vector<User> users = {};

  users.push_back({"01010101010",
                   "Nika Qvrivishvili",
                   true,
                   Gender::MALE,
                   {0, 0, 0, 26, 2, 104}});

  users.push_back(
      {"01010101011", "John Doe", true, Gender::MALE, {0, 0, 0, 22, 3, 99}});

  users[0].createInvestmentAccount();

  users[0].buyStock("AMZN", 100, stocks);
  users[0].buyStock("AMZN", 200, stocks);

  users[0].buyStock("NFLX", 200, stocks);

  logStocks(stocks);
  logUsers(users);

  return 0;
}
