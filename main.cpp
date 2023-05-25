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
protected:
  double balance;

public:
  CheckingAccount() : balance(0) {}

  void deposit(double amount) { balance += amount; }

  void withdraw(double amount) {
    if (amount - this->balance > 0.000001) {
      cout << "ERROR: The withdrawal requested was larger than your current "
              "balance!"
           << endl;

      return;
    }

    balance -= amount;
    balance = balance < 0 ? 0 : balance;
  }

  double getBalance() { return this->balance; }

  void logData() { cout << "Balance: $" << this->balance << endl; }
};

class SavingsAccount : public CheckingAccount {
protected:
  double interest;

public:
  SavingsAccount() : interest(0) {}

  void setInterest(double interest) { this->interest = interest; }

  void addInterest() { this->balance *= 1 + this->interest; };

  void logData() {
    cout << "Balance: $" << this->getBalance()
         << " || Interest: " << this->interest << "%" << endl;
  }
};

class CDAccount : public SavingsAccount {
  bool isBlocked;

public:
  CDAccount() : isBlocked(false) {}

  void blockBalance() { isBlocked = true; }

  void unblock() { isBlocked = false; }

  void withdraw(double amount) {
    if (this->isBlocked) {
      cout << "ERROR: Can't withdraw because your balance is blocked" << endl;
      return;
    }

    SavingsAccount::withdraw(amount);
  }

  void logData() {
    cout << "Balance: $" << this->getBalance()
         << " || Interest: " << this->interest
         << "% || isBlocked: " << prettyBool(this->isBlocked) << endl;
  }
};

class DepositAccount {
  string userId;
  CheckingAccount *checkingAccount;
  SavingsAccount *savingsAccount;
  CDAccount *cdAccount;

public:
  DepositAccount(string userId) : userId(userId), checkingAccount(nullptr) {}

  CheckingAccount *getCheckingAccount() { return this->checkingAccount; }
  SavingsAccount *getSavingsAccount() { return this->savingsAccount; }
  CDAccount *getCDAccount() { return this->cdAccount; }

  void createCheckingAccount() {
    if (this->checkingAccount != nullptr) {
      return;
    }

    this->checkingAccount = new CheckingAccount();
  }

  void createSavingsAccount() {
    if (this->savingsAccount != nullptr) {
      return;
    }

    this->savingsAccount = new SavingsAccount();
  }

  void createCDAccount() { this->cdAccount = new CDAccount(); }

  void logData() {
    if (checkingAccount != nullptr) {
      cout << "= Checking Account =" << endl;
      checkingAccount->logData();
    }

    if (savingsAccount != nullptr) {
      cout << "= Savings Account =" << endl;
      savingsAccount->logData();
    }

    if (cdAccount != nullptr) {
      cout << "= CD Account =" << endl;
      cdAccount->logData();
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
    } else {

      this->ownedStocks[code].amount += payment / stocks[code].price;
      return 1;
    }

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
  string name;
  bool isCitizen;
  Gender gender;
  tm birthday;
  vector<string> historyLogs;
  DepositAccount *depositAccount;
  InvestmentAccout *investmentAccout;

public:
  User(string id, string name, bool isCitizen, Gender gender, tm birthday)
      : id(id), name(name), isCitizen(isCitizen), gender(gender),
        birthday(birthday), historyLogs({}), depositAccount(nullptr),
        investmentAccout(nullptr) {}

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

    cout << "== History Logs ==" << endl;
    for (int i = 0; i < this->historyLogs.size(); i++) {
      cout << this->historyLogs[i] << endl;
    }
  }

  void addLog(string log) {
    this->historyLogs.push_back(to_string(this->historyLogs.size()) + ": " +
                                log);
  }

  void createInvestmentAccount() {
    this->addLog("Creating Investment Account");
    this->investmentAccout = new InvestmentAccout(this->id);
  }

  void createDepositAccount() {
    this->addLog("Creating Deposit Account");
    this->depositAccount = new DepositAccount(this->id);
  }

  DepositAccount *getDepositAccount() { return this->depositAccount; }

  void buyStock(string code, double payment) {
    CheckingAccount *checkingAccount =
        this->depositAccount->getCheckingAccount();

    // Checking like this to avoid floating point arithmetic inaccuracies
    if (payment - checkingAccount->getBalance() > 0.000001) {
      cout << payment << " | " << payment - checkingAccount->getBalance()
           << endl;
      this->addLog(
          "Failed to buy stock: insufficient funds on checking account");

      return;
    }

    if (this->investmentAccout->buyStock(code, payment)) {
      this->addLog("Buying " + code + " stock for $" + to_string(payment));
      checkingAccount->withdraw(payment);
      return;
    }

    this->addLog("Failed to buy stock: invalid stock specified");
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
  vector<User> users = {};

  users.push_back({"01010101010",
                   "Nika Qvrivishvili",
                   true,
                   Gender::MALE,
                   {0, 0, 0, 26, 2, 104}});

  users.push_back(
      {"01010101011", "John Doe", true, Gender::MALE, {0, 0, 0, 22, 3, 99}});

  User &firstUser = users[0];

  firstUser.createInvestmentAccount();
  firstUser.createDepositAccount();

  DepositAccount *firstUserDepositAccount = firstUser.getDepositAccount();

  firstUserDepositAccount->createCheckingAccount();
  firstUserDepositAccount->createSavingsAccount();
  firstUserDepositAccount->createCDAccount();

  CheckingAccount *firstUserCheckingAccount =
      firstUserDepositAccount->getCheckingAccount();
  SavingsAccount *firstUserSavingsAccount =
      firstUserDepositAccount->getSavingsAccount();
  CDAccount *firstUserCDAccount = firstUserDepositAccount->getCDAccount();

  firstUserCheckingAccount->deposit(1000);
  firstUserCheckingAccount->withdraw(240);

  firstUserSavingsAccount->deposit(1041);
  firstUserSavingsAccount->setInterest(0.01);
  firstUserSavingsAccount->addInterest();
  firstUserSavingsAccount->withdraw(5);

  firstUserCDAccount->deposit(20000);
  firstUserCDAccount->setInterest(0.15);
  firstUserCDAccount->blockBalance();
  firstUserCDAccount->addInterest();
  firstUserCDAccount->withdraw(100);

  firstUser.buyStock("AMZN", 112.2);
  firstUser.buyStock("AMZN", 313.2);
  firstUser.buyStock("NFLX", 1000.16);
  firstUser.buyStock("NVDA", 334.6);

  logStocks(stocks);
  logUsers(users);

  return 0;
}
