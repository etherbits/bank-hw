#include <ctime>
#include <iostream>
#include <map>
#include <math.h>
#include <string>
#include <vector>

using namespace std;

// utility functions, classes, structures and enums where created by Nika
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

// get the difference of two dates in years
int getYearDiff(tm start) {

  time_t x = mktime(&start);
  time_t y = time(nullptr);

  if (x != (std::time_t)(-1) && y != (std::time_t)(-1)) {
    return floorf(std::difftime(y, x) / (60 * 60 * 24 * 365.25));
  }

  return -1;
}

// for formating date
string getFormattedDate(tm date) {
  return padLeft(to_string(date.tm_mday), '0', 2) + "/" +
         padLeft(to_string(date.tm_mon + 1), '0', 2) + "/" +
         to_string(date.tm_year + 1900);
}

// helper for user history logging
void addLog(vector<string> &historyLogs, string log) {
  historyLogs.push_back(to_string(historyLogs.size()) + ": " + log);
}

// stocks and logic relating to stock where developed by Nika
struct StockListing {
  string company;
  string code;
  double price;
};

struct OwnedStock {
  string company;
  string code;
  double amount;
};

// stock listings
map<string, StockListing> stocks = {
    {"AMZN", {"Amazon  Inc.", "AMZN", 114.8}},
    {"NFLX", {"Netflix Inc.", "NFLX", 367.4}},
    {"NVDA", {"Nvidia Corp.", "NVDA", 313.2}}};

void logStocks(map<string, StockListing> stocks) {
  cout << endl << "=====   Available Stocks   =====" << endl;

  for (std::map<string, StockListing>::iterator it = stocks.begin();
       it != stocks.end(); ++it) {
    cout << "Company: " << it->second.company << " || "
         << "Code: " << it->second.code << " || "
         << "Price: $" << it->second.price << endl;
  }
  cout << endl;
}

// Checking, Savings and CD accounts were created by Saba and Erekle with the
// guidance of Nika Checking class acts as a base for savings and CD account
// classes
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
    balance = balance < 0 ? 0 : balance; // set to 0 in the case of a floating
                                         // point arithmetic error
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
         << " || Interest: " << (int)(this->interest * 100) << "%" << endl;
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

    SavingsAccount::withdraw(amount); // call to the base withdraw method
  }

  void logData() {
    cout << endl
         << "- " + name + " -" << endl
         << " Balance: $" << this->getBalance()
         << " || Interest: " << (int)(this->interest * 100)
         << "% || isBlocked: " << prettyBool(this->isBlocked) << endl;
  }
};

// The DepositAccount class was created by Erekle and Saba with the help of Nika
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

  // log every account data that exists when this method was called
  void logData() {
    if (checkingAccount != nullptr) {
      cout << endl << "- Checking Account -" << endl;
      checkingAccount->logData();
    }

    if (savingsAccount != nullptr) {
      cout << endl << "- Savings Account -" << endl;
      savingsAccount->logData();
    }

    if (cdAccounts.size() > 0) {
      cout << endl << "--- CD Accounts ---" << endl;
      for (int i = 0; i < cdAccounts.size(); i++) {
        cdAccounts[i]->logData();
      }
    }
  }

  ~DepositAccount() {
    delete checkingAccount;
    delete savingsAccount;

    for (int i = 0; i < cdAccounts.size(); i++) {
      delete cdAccounts[i];
    }
    cdAccounts.clear();
  }
};

// The InvestmentAccount class was developed by Nika
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

    addLog(historyLogs, "Buying " + to_string(payment / stocks[code].price) +
                            " ($" + to_string(payment) + ") of " + code +
                            " stock");
    this->checkingAccount->deduct(payment);
  }

  void logData() {

    cout << endl << "- Owned Stocks -" << endl;

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

  ~InvestmentAccount() { delete checkingAccount; }
};

// The Customer class was created by Guga and Nika
class Customer {
  string id;
  string name;
  bool isCitizen;
  Gender gender;
  tm birthday;
  vector<string> historyLogs;
  DepositAccount *depositAccount;
  InvestmentAccount *investmentAccount;

public:
  Customer(string id, string name, bool isCitizen, Gender gender, tm birthday)
      : id(id), name(name), isCitizen(isCitizen), gender(gender),
        birthday(birthday), historyLogs({}), depositAccount(nullptr),
        investmentAccount(nullptr) {}

  int getAge() { return getYearDiff(this->birthday); }

  void logBasicData() {
    cout << endl
         << "=== User: " + name + " ===" << endl
         << endl
         << "| Basic Data |" << endl;
    cout << "- id: " << this->id << endl;
    cout << "- name: " << this->name << endl;
    cout << "- age: " << this->getAge() << endl;
    cout << "- is a citizen: " << prettyBool(this->isCitizen) << endl;
    cout << "- gender: " << genderStr(this->gender) << endl;
    cout << "- birthday: " << getFormattedDate(this->birthday) << endl;

    if (this->depositAccount != nullptr) {
      cout << endl << "| Deposit Account Data |" << endl;
      this->depositAccount->logData();
    }

    if (this->investmentAccount != nullptr) {
      cout << endl << "| Investment Account Data |" << endl;
      this->investmentAccount->logData();
    }

    cout << endl << "| History Logs |" << endl;
    for (int i = 0; i < this->historyLogs.size(); i++) {
      cout << this->historyLogs[i] << endl;
    }
  }

  void createInvestmentAccount() {
    addLog(historyLogs, "Creating Investment Account");
    this->investmentAccount =
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

  InvestmentAccount *getInvestmentAccount() { return this->investmentAccount; }
  DepositAccount *getDepositAccount() { return this->depositAccount; }

  ~Customer() {
    delete investmentAccount;
    delete depositAccount;
  }
};

void logCustomers(vector<Customer> &customers) {
  cout << endl << "=====   All Customers   =====" << endl;
  for (int i = 0; i < customers.size(); i++) {
    customers[i].logBasicData();
  }
  cout << endl;
}

// use customer functions where developed by Beqa and Nika
void useJointCustomer(Customer &customer) {
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

void useDepositCustomer(Customer &customer) {
  customer.createDepositAccount();
  // write deposit only customer code here...
}

void useInvestmentCustomer(Customer &customer) {
  customer.createInvestmentAccount();
  // write investment only customer code here...
}

// main function created by Beqa with the help of Nika
int main() {
  // adding customer accounts to a "mock database"
  vector<Customer> customers = {};

  customers.push_back(
      {"01010101010", "Mike Smith", true, Gender::MALE, {0, 0, 0, 26, 2, 104}});

  customers.push_back(
      {"01010101011", "John Doe", true, Gender::MALE, {0, 0, 0, 22, 3, 99}});

  customers.push_back({"01010101012",
                       "Olivia Smith",
                       true,
                       Gender::FEMALE,
                       {0, 0, 0, 12, 10, 83}});

  useJointCustomer(customers[0]); // creating and using the joint customer
  useDepositCustomer(
      customers[1]); // creating and using the deposit only customer
  useInvestmentCustomer(
      customers[2]); // creating and using the investment only customer

  // log every important piece of info
  logStocks(stocks);
  logCustomers(customers);

  return 0;
}
