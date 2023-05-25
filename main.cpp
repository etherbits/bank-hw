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

void addLog(vector<string> &historyLogs, string log) {
  historyLogs.push_back(to_string(historyLogs.size()) + ": " + log);
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
  string name;
  vector<string> &historyLogs;
  double balance;

public:
  CheckingAccount(string name, vector<string> &historyLogs)
      : name(name), historyLogs(historyLogs), balance(0) {}

  void deposit(double amount) {
    addLog(historyLogs, "Adding $" + to_string(amount) + " to " + name);
    balance += amount;
  }

  void withdraw(double amount) {
    if (amount - this->balance > 0.000001) {
      addLog(historyLogs, "Failed to withdraw from " + name +
                              ": "
                              "withdrawal of $" +
                              to_string(amount) + " requested but only $" +
                              to_string(this->balance) + " available");
      return;
    }

    balance -= amount;
    balance = balance < 0 ? 0 : balance;
    addLog(historyLogs, "Withdrawing $" + to_string(amount) + " from " + name);
  }

  void deduct(double amount) {

    if (amount - this->balance > 0.000001) {
      addLog(historyLogs, "Failed to deduct from " + name +
                              ": "
                              "deduction of $" +
                              to_string(amount) + " requested but only $" +
                              to_string(this->balance) + " available");
      return;
    }

    balance -= amount;
    balance = balance < 0 ? 0 : balance;
    addLog(historyLogs, "Deducting $" + to_string(amount) + " from " + name);
  }

  double getBalance() { return this->balance; }

  void logData() { cout << "Balance: $" << this->balance << endl; }
};

class SavingsAccount : public CheckingAccount {
protected:
  double interest;

public:
  SavingsAccount(string name, vector<string> &historyLogs)
      : interest(0), CheckingAccount(name, historyLogs) {}

  void setInterest(double interest) {
    addLog(historyLogs, "Setting " + name + " interest to " +
                            to_string((int)(interest * 100)) + "%");

    this->interest = interest;
  }

  void addInterest() {
    addLog(historyLogs, "Adding interest to " + name + ": $" +
                            to_string(this->balance) + " => $" +
                            to_string(this->balance * (1 + this->interest)));

    this->balance *= 1 + this->interest;
  };

  void logData() {
    cout << "Balance: $" << this->getBalance()
         << " || Interest: " << this->interest << "%" << endl;
  }
};

class CDAccount : public SavingsAccount {
  bool isBlocked;

public:
  CDAccount(string name, vector<string> &historyLogs)
      : isBlocked(false), SavingsAccount(name, historyLogs) {}

  void blockBalance() {
    addLog(historyLogs, "Blocking " + name + " balance");
    isBlocked = true;
  }

  void unblock() {
    addLog(historyLogs, "Blocking " + name + " balance");
    isBlocked = false;
  }

  void withdraw(double amount) {
    if (this->isBlocked) {
      addLog(historyLogs, "Failed to withdraw from " + name +
                              ": account balance is blocked");
      return;
    }

    SavingsAccount::withdraw(amount);
  }

  void logData() {
    cout << "--- CD Account " + name << endl
         << " Balance: $" << this->getBalance()
         << " || Interest: " << this->interest
         << "% || isBlocked: " << prettyBool(this->isBlocked) << endl;
  }
};

class DepositAccount {
  string name;
  vector<string> &historyLogs;
  CheckingAccount *checkingAccount;
  SavingsAccount *savingsAccount;
  vector<CDAccount *> cdAccounts;

public:
  DepositAccount(string name, vector<string> &historyLogs)
      : name(name), historyLogs(historyLogs), checkingAccount(nullptr),
        savingsAccount(nullptr), cdAccounts({}) {}

  CheckingAccount *getCheckingAccount() { return this->checkingAccount; }
  SavingsAccount *getSavingsAccount() { return this->savingsAccount; }
  vector<CDAccount *> &getCDAccounts() { return this->cdAccounts; }

  void createCheckingAccount() {
    if (this->checkingAccount != nullptr) {
      return;
    }

    addLog(historyLogs, "Creating Checking Account");
    this->checkingAccount =
        new CheckingAccount(name + "_checking", historyLogs);
  }

  void createSavingsAccount() {
    if (this->savingsAccount != nullptr) {
      return;
    }

    addLog(historyLogs, "Creating Savings Account");
    this->savingsAccount = new SavingsAccount(name + "_savings", historyLogs);
  }

  void createCDAccount() {
    addLog(historyLogs, "Creating CD Account");
    this->cdAccounts.push_back(new CDAccount(
        name + "_CD_" + to_string(cdAccounts.size()), historyLogs));
  }

  void logData() {
    if (checkingAccount != nullptr) {
      cout << "= Checking Account =" << endl;
      checkingAccount->logData();
    }

    if (savingsAccount != nullptr) {
      cout << "= Savings Account =" << endl;
      savingsAccount->logData();
    }

    if (cdAccounts.size() > 0) {
      cout << "= CD Accounts =" << endl;
      for (int i = 0; i < cdAccounts.size(); i++) {
        cdAccounts[i]->logData();
      }
    }
  }
};

class InvestmentAccount {
  string name;
  vector<string> &historyLogs;
  CheckingAccount *checkingAccount;
  map<string, OwnedStock> ownedStocks;

public:
  InvestmentAccount(string name, vector<string> &historyLogs)
      : name(name), historyLogs(historyLogs), checkingAccount(nullptr) {}

  void createCheckingAccount() {
    this->checkingAccount =
        new CheckingAccount(name + "_checking", historyLogs);
  }

  CheckingAccount *getCheckingAccount() { return this->checkingAccount; }

  void buyStock(string code, double payment) {
    // check if stock exists
    if (stocks.find(code) == stocks.end()) {
      addLog(historyLogs,
             "Failed to buy stock " + code + ": invalid stock specified");
      return;
    }

    // check if customer has sufficient funds
    if (payment - this->checkingAccount->getBalance() > 0.000001) {
      addLog(historyLogs,
             "Failed to buy stock " + code + ": insufficient funds");
      return;
    }

    // check if customer already owns the same stock
    if (ownedStocks.find(code) == ownedStocks.end()) {
      this->ownedStocks[code] = {stocks[code].company, stocks[code].code,
                                 payment / stocks[code].price};
    } else {
      this->ownedStocks[code].amount += payment / stocks[code].price;
    }

    addLog(historyLogs, "Buying " + to_string(this->ownedStocks[code].amount) +
                            " ($" + to_string(payment) + ") of " + code +
                            " stock");
    this->checkingAccount->deduct(payment);
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
};

class Customer {
  string id;
  string name;
  bool isCitizen;
  Gender gender;
  tm birthday;
  vector<string> historyLogs;
  DepositAccount *depositAccount;
  InvestmentAccount *investmentAccout;

public:
  Customer(string id, string name, bool isCitizen, Gender gender, tm birthday)
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

  void createInvestmentAccount() {
    addLog(historyLogs, "Creating Investment Account");
    this->investmentAccout =
        new InvestmentAccount(name + "\'s investment", historyLogs);
  }

  void createDepositAccount() {
    addLog(historyLogs, "Creating Deposit Account");
    this->depositAccount =
        new DepositAccount(name + "\'s deposit", historyLogs);
  }

  void createJointAccount() {
    this->createInvestmentAccount();
    this->createDepositAccount();
  }

  InvestmentAccount *getInvestmentAccount() { return this->investmentAccout; }
  DepositAccount *getDepositAccount() { return this->depositAccount; }

  void checkingWithdraw(double amount) {
    if (this->depositAccount == nullptr) {
      addLog(historyLogs,
             "Failed to withdraw from your checking account: no deposit "
             "account exists");
      return;
    }

    CheckingAccount *checkingAccount =
        this->depositAccount->getCheckingAccount();

    if (checkingAccount == nullptr) {
      addLog(historyLogs,
             "Failed to withdraw from your checking account: no checking "
             "account exists");
      return;
    }

    checkingAccount->withdraw(amount);
  }

  ~Customer() { delete investmentAccout; }
};

void logCustomers(vector<Customer> &customers) {
  cout << endl << "=== All Customers ===" << endl;
  for (int i = 0; i < customers.size(); i++) {
    cout << "---" << endl;
    customers[i].logBasicData();
  }
  cout << endl;
}

void useJointAccount(Customer &customer) {

  // create joint account
  customer.createJointAccount();

  // getting the deposit account of this customer
  DepositAccount *depositAccount = customer.getDepositAccount();

  // creating one checking, one savings and two CD accounts for the customer
  depositAccount->createCheckingAccount();
  depositAccount->createSavingsAccount();

  depositAccount->createCDAccount();
  depositAccount->createCDAccount();

  // getting all deposit accounts
  CheckingAccount *checkingAccount = depositAccount->getCheckingAccount();
  SavingsAccount *savingsAccount = depositAccount->getSavingsAccount();
  vector<CDAccount *> cdAccounts = depositAccount->getCDAccounts();

  // using checking account
  checkingAccount->deposit(1000);
  checkingAccount->withdraw(220);
  checkingAccount->withdraw(900);

  // using savings account
  savingsAccount->deposit(1041);
  savingsAccount->setInterest(0.01);
  savingsAccount->addInterest();
  savingsAccount->withdraw(280);

  // using the first cd account
  cdAccounts[0]->deposit(20000);
  cdAccounts[0]->setInterest(0.15);
  cdAccounts[0]->blockBalance();
  cdAccounts[0]->addInterest();
  cdAccounts[0]->withdraw(100);

  // using the second cd account
  cdAccounts[1]->deposit(10000);
  cdAccounts[1]->setInterest(0.20);
  cdAccounts[1]->blockBalance();
  cdAccounts[1]->addInterest();
  cdAccounts[1]->deposit(1000);
  cdAccounts[1]->withdraw(150);

  // getting the joint customer investing account
  InvestmentAccount *investmentAccount = customer.getInvestmentAccount();

  // creating checking account and adding some funds to it
  investmentAccount->createCheckingAccount();

  CheckingAccount *investmentCheckingAccount =
      investmentAccount->getCheckingAccount();

  investmentCheckingAccount->deposit(1000);

  // using the joint customer investing account
  investmentAccount->buyStock("AMZN", 112.2);
  investmentAccount->buyStock("AMZN", 313.2);
  investmentAccount->buyStock("NFLX", 1000.16);
  investmentAccount->buyStock("NVDA", 334.6);
}

int main() {
  // adding customer accounts to a "mock database"
  vector<Customer> customers = {};

  customers.push_back({"01010101010",
                       "Nika Qvrivishvili",
                       true,
                       Gender::MALE,
                       {0, 0, 0, 26, 2, 104}});

  customers.push_back(
      {"01010101011", "John Doe", true, Gender::MALE, {0, 0, 0, 22, 3, 99}});

  useJointAccount(customers[0]);

  // log every important piece of info
  logStocks(stocks);
  logCustomers(customers);

  return 0;
}
