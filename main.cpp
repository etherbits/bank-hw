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

// stock listings
map<string, StockListing> stocks = {
    {"AMZN", {"Amazon.com Inc.", "AMZN", 114.8}},
    {"NFLX", {"Netflix Inc.", "NFLX", 367.4}},
    {"NVDA", {"Nvidia Corp.", "NVDA", 313.2}}};

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

class CheckingAccount {
  double balance;

public:
  CheckingAccount() : balance(0) {}

  void deposit(double amount) { balance += amount; }

  void withdraw(double amount) {
    if (amount > this->balance) {
      cout << "ERROR: The withdrawal requested was larger than your current "
              "balance!"
           << endl;

      return;
    }

    balance -= amount;
  }

  double getBalance() { return this->balance; }

  void logData() { cout << "Balance: $" << this->balance << endl; }
};

class SavingsAccount {
  double balance;
  double interest;

public:
  SavingsAccount() : balance(0), interest(0) {}

  void deposit(double amount) { balance += amount; }

  void withdraw(double amount) {
    if (amount > this->balance) {
      cout << "ERROR: The withdrawal requested was larger than your current "
              "balance!"
           << endl;

      return;
    }

    balance -= amount;
  }

  void setInterest(double interest) { this->interest = interest; }

  void addInterest() { this->balance *= 1 + this->interest; };

  double getBalance() { return this->balance; }

  void logData() {
    cout << "Balance: $" << this->getBalance() << " || "
         << "Interest (%): " << this->interest << endl;
  }
};

class DepositAccount {
  string userId;
  CheckingAccount *checkingAccount;
  SavingsAccount *savingsAccount;

public:
  DepositAccount(string userId) : userId(userId), checkingAccount(nullptr) {}

  void checkingDeposit(double amount) {
    if (this->checkingAccount == nullptr) {
      return;
    }

    this->checkingAccount->deposit(amount);
  }

  void checkingWithdraw(double amount) {
    if (this->checkingAccount == nullptr) {
      return;
    }

    this->checkingAccount->withdraw(amount);
  }

  double getCheckingBalance() { return this->checkingAccount->getBalance(); }

  void createCheckingAccount() {
    this->checkingAccount = new CheckingAccount();
  }

  void savingsDeposit(double amount) {
    if (this->savingsAccount == nullptr) {
      return;
    }

    this->savingsAccount->deposit(amount);
  }

  void savingsWithdraw(double amount) {
    if (this->savingsAccount == nullptr) {
      return;
    }

    this->savingsAccount->withdraw(amount);
  }

  double getSavingsBalance() { return this->savingsAccount->getBalance(); }

  void createSavingsAccount() { this->savingsAccount = new SavingsAccount(); }

  void logData() {
    if (checkingAccount != nullptr) {
      cout << "= Checking Account =" << endl;
      checkingAccount->logData();
    }

    if (savingsAccount != nullptr) {
      cout << "= Savings Account =" << endl;
      savingsAccount->logData();
    }
  }
};

class InvestmentAccout {
  string userId;
  map<string, OwnedStock> ownedStocks;

public:
  InvestmentAccout(string userId) : userId(userId) {}

  int buyStock(string code, double payment) {
    // check if stock exists
    if (stocks.find(code) == stocks.end()) {
      return 1;
    }

    // check if user owns stock
    if (ownedStocks.find(code) == ownedStocks.end()) {
      this->ownedStocks[code] = {stocks[code].company, stocks[code].code,
                                 payment / stocks[code].price};

      return 1;
    }

    this->ownedStocks[code].amount += payment / stocks[code].price;
    return 0;
  }

  void logStocks() {

    cout << endl << "=== Owned Stocks ===" << endl;

    for (std::map<string, OwnedStock>::iterator it = this->ownedStocks.begin();
         it != ownedStocks.end(); ++it) {
      cout << "Company: " << it->second.company << " || "
           << "Code: " << it->second.code << " || "
           << "Shares: " << it->second.amount << " || "
           << "Total: "
           << "$" << it->second.amount * stocks[it->second.code].price << endl;
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
  DepositAccount *depositAccount;
  InvestmentAccout *investmentAccout;

public:
  User(string id, string name, bool isCitizen, Gender gender, tm birthday)
      : id(id), name(name), isCitizen(isCitizen), gender(gender),
        birthday(birthday), depositAccount(nullptr), investmentAccout(nullptr) {
  }

  int getAge() { return getYearDiff(this->birthday); }

  void logBasicData() {
    cout << "id: " << this->id << endl;
    cout << "name: " << this->name << endl;
    cout << "age: " << this->getAge() << endl;
    cout << "is a citizen: " << prettyBool(this->isCitizen) << endl;
    cout << "gender: " << genderStr(this->gender) << endl;
    cout << "birthday: " << getFormattedDate(this->birthday) << endl;

    if (this->depositAccount != nullptr) {
      cout << endl << "= Deposit Account =" << endl;
      this->depositAccount->logData();
    }

    if (this->investmentAccout != nullptr) {
      cout << endl << "= Investment Account =" << endl;
      this->investmentAccout->logStocks();
    }
  }

  void createInvestmentAccount() {
    this->investmentAccout = new InvestmentAccout(this->id);
  }

  void createDepositAccount() {
    this->depositAccount = new DepositAccount(this->id);
  }

  void createCheckingAccount() {
    this->depositAccount->createCheckingAccount();
  }

  void createSavingsAccount(){
    this->depositAccount->createSavingsAccount();
  }

  void checkingDeposit(double amount) {
    this->depositAccount->checkingDeposit(amount);
  }
  void checkingWithdraw(double amount) {
    this->depositAccount->checkingWithdraw(amount);
  }
  void savingsDeposit(double amount) {
    this->depositAccount->savingsDeposit(amount);
  }
  void savingsWithdraw(double amount) {
    this->depositAccount->savingsWithdraw(amount);
  }

  void buyStock(string code, double payment) {
    if (payment > this->depositAccount->getCheckingBalance()) {
      cout << "ERROR: You don not have the sufficient funds for this request"
           << endl;
      return;
    }

    if (this->investmentAccout->buyStock(code, payment)) {
      checkingWithdraw(payment);
    }
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
  users[0].createDepositAccount();
  users[0].createCheckingAccount();
  users[0].createSavingsAccount();

  users[0].checkingDeposit(1000);
  users[0].checkingWithdraw(240);

  users[0].savingsDeposit(1000);
  users[0].savingsWithdraw(200);

  users[0].buyStock("AMZN", 112.2);
  users[0].buyStock("AMZN", 313.2);

  users[0].buyStock("NFLX", 2012.16);
  users[0].buyStock("NVDA", 647.16);

  logStocks(stocks);
  logUsers(users);

  return 0;
}
