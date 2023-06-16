#include <ctime>
#include <fstream>
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

struct BondListing {
  string name;
  double price;
};

struct OwnedBond {
  string name;
  int term;
  double value;
};

// stock listings
map<string, StockListing> stocks = {{"AMZN", {"Amazon  Inc.", "AMZN", 114.8}},
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

map<string, BondListing> bonds = {
    {"U.S. Treasury Bond", {"U.S. Treasury Bond", 1035.50}},
    {"Intermediate Government/Credit Bond",
     {"Intermediate Government/Credit Bond", 1325.50}},
};

void logBonds(map<string, BondListing> bonds) {
  cout << endl << "=====   Available Bonds   =====" << endl;

  for (std::map<string, BondListing>::iterator it = bonds.begin();
       it != bonds.end(); ++it) {
    cout << "Name: " << it->second.name << " || "
         << "Price: $" << it->second.price << endl;
  }
  cout << endl;
}

// Checking, Savings and CD accounts were created by Saba and Erekle with the
// guidance of Nika Checking class acts as a base for savings and CD account
// classes

class Account {
protected:
  string name;
  double balance;
  bool FDIC_Ins;
  vector<string> &historyLogs;

public:
  Account(string name, bool FDIC_Ins, vector<string> &historyLogs)
      : name(name), FDIC_Ins(FDIC_Ins), historyLogs(historyLogs), balance(0) {}

  void showBalance() {
    cout << this->name << " balance: $" << this->balance << endl;
  }

  double getBalance() { return this->balance; }
};

class CheckingAccount : public Account {
protected:
public:
  CheckingAccount(string name, bool FDIC_Ins, vector<string> &historyLogs)
      : Account(name, FDIC_Ins, historyLogs) {}

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

  int deduct(double amount) {

    if (amount - this->balance > 0.000001) {
      addLog(historyLogs, "Failed to deduct from " + name +
                              ": "
                              "deduction of $" +
                              to_string(amount) + " requested but only $" +
                              to_string(this->balance) + " available");
      return -1;
    }

    balance -= amount;
    balance = balance < 0 ? 0 : balance;
    addLog(historyLogs, "Deducting $" + to_string(amount) + " from " + name);
    return this->balance;
  }

  string getName() { return this->name; }

  void logData() { cout << "Balance: $" << this->balance << endl; }
  void writeStatement(ofstream &file) {
    file << "Balance: $" << this->balance << endl;
  }
};

class SavingsAccount : public Account {
protected:
  double interest;

public:
  SavingsAccount(string name, bool FDIC_Ins, vector<string> &historyLogs)
      : interest(0), Account(name, FDIC_Ins, historyLogs) {}

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

  void transferToChecking(CheckingAccount *checkingAccount, double amount) {
    if (amount - this->balance > 0.000001) {
      addLog(historyLogs, "Failed to transfer from " + this->name +
                              ": transfer of $" + to_string(amount) +
                              " requested but only $" +
                              to_string(this->balance) + " available");
      return;
    }

    addLog(historyLogs, "Transfering $" + to_string(amount) + " from " +
                            this->name + " to " + checkingAccount->getName());
    this->balance -= amount;

    checkingAccount->deposit(amount);
  }

  void transferFromChecking(CheckingAccount *checkingAccount, double amount) {
    if (checkingAccount->deduct(amount) == -1) {
      addLog(historyLogs, "Failed to transfer $" + to_string(amount) +
                              " from " + checkingAccount->getName() + " to " +
                              this->name);
      return;
    }

    addLog(historyLogs, "Transfering $" + to_string(amount) + " from " +
                            checkingAccount->getName() + " to " + this->name);

    this->balance += amount;
  }

  void logData() {
    cout << "Balance: $" << this->getBalance()
         << " || Interest: " << (int)(this->interest * 100) << "%" << endl;
  }

  void writeStatement(ofstream &file) {
    file << "Balance: $" << this->getBalance()
         << " || Interest: " << (int)(this->interest * 100) << "%" << endl;
  }
};

class CDAccount : public SavingsAccount {
  bool isBlocked;

public:
  CDAccount(string name, bool FDIC_Ins, vector<string> &historyLogs)
      : isBlocked(false), SavingsAccount(name, FDIC_Ins, historyLogs) {}

  void blockBalance() {
    addLog(historyLogs, "Blocking " + name + " balance");
    isBlocked = true;
  }

  void unblock() {
    addLog(historyLogs, "Blocking " + name + " balance");
    isBlocked = false;
  }

  void close(CheckingAccount *checkingAccount) {
    if (isBlocked) {
      addLog(historyLogs, "Can't close account because it is still blocked ");

      return;
    }
    transferToChecking(checkingAccount, this->balance);
  }

  void logData() {
    cout << endl
         << "- " + name + " -" << endl
         << " Balance: $" << this->getBalance()
         << " || Interest: " << (int)(this->interest * 100)
         << "% || isBlocked: " << prettyBool(this->isBlocked) << endl;
  }

  void writeStatement(ofstream &file) {
    file << endl
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
        new CheckingAccount(name + "_checking", true, historyLogs);
  }

  void createSavingsAccount() {
    if (this->savingsAccount != nullptr) {
      return;
    }

    addLog(historyLogs, "Creating Savings Account");
    this->savingsAccount =
        new SavingsAccount(name + "_savings", true, historyLogs);
  }

  void createCDAccount() {
    addLog(historyLogs, "Creating CD Account");
    this->cdAccounts.push_back(new CDAccount(
        name + "_CD_" + to_string(cdAccounts.size()), true, historyLogs));
  }

  string getName() { return this->name; }

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

  void writeStatement(ofstream &file) {
    if (checkingAccount != nullptr) {
      file << endl << "- Checking Account -" << endl;
      checkingAccount->writeStatement(file);
    }

    if (savingsAccount != nullptr) {
      file << endl << "- Savings Account -" << endl;
      savingsAccount->writeStatement(file);
    }

    if (cdAccounts.size() > 0) {
      file << endl << "--- CD Accounts ---" << endl;
      for (int i = 0; i < cdAccounts.size(); i++) {
        cdAccounts[i]->writeStatement(file);
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
  map<string, OwnedBond> ownedBonds;

public:
  InvestmentAccount(string name, vector<string> &historyLogs)
      : name(name), historyLogs(historyLogs),
        checkingAccount(
            new CheckingAccount(name + "_checking", true, historyLogs)) {}

  CheckingAccount *getCheckingAccount() { return this->checkingAccount; }

  string getName() { return this->name; }

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

  void sellStock(string code, double amount) {
    if (ownedStocks.find(code) == ownedStocks.end()) {
      addLog(historyLogs, "This account does not have any " + code);
      return;
    }

    if (amount - ownedStocks[code].amount > 0.000001) {
      addLog(historyLogs, "This account does not have " + to_string(amount) +
                              " " + code + " shares to sell");
      return;
    }

    addLog(historyLogs, "Selling " + to_string(amount) + " " + code + " for $" +
                            to_string(stocks[code].price * amount));
    this->ownedStocks[code].amount -= amount;
    this->checkingAccount->deposit(amount * stocks[code].price);
  }

  void buyBond(string name, int term, double payment) {
    // check if bond exists
    if (bonds.find(name) == bonds.end()) {
      addLog(historyLogs,
             "Failed to buy bond " + name + ": invalid bond specified");
      return;
    }

    // check if customer has sufficient funds
    if (payment - this->checkingAccount->getBalance() > 0.000001) {
      addLog(historyLogs,
             "Failed to buy bond " + name + ": insufficient funds");
      return;
    }

    // check if customer already owns the same bond
    if (ownedBonds.find(name) == ownedBonds.end()) {
      this->ownedBonds[name] = {bonds[name].name, term, payment * .9};
    } else {
      this->ownedBonds[name].value += payment * .9;
    }

    addLog(historyLogs, "Buying " + to_string(payment) + " worth of " + name +
                            " bond for " + to_string(term) + " years");

    this->checkingAccount->deduct(payment);
  }

  void sellBond(string name) {
    if (ownedBonds.find(name) == ownedBonds.end()) {
      addLog(historyLogs, "This account does not have any " + name + " bond");
      return;
    }

    OwnedBond bond = ownedBonds[name];

    addLog(historyLogs, "Selling " + name + " for $" + to_string(bond.value));
    this->checkingAccount->deposit(ownedBonds[name].value);
    this->ownedBonds.erase(name);
  }

  void logData() {

    cout << "Balance: $" << this->checkingAccount->getBalance() << endl;

    cout << endl << "- Owned Stocks -" << endl;

    for (std::map<string, OwnedStock>::iterator it = this->ownedStocks.begin();
         it != ownedStocks.end(); ++it) {
      cout << "Company: " << it->second.company << " || "
           << "Code: " << it->second.code << " || "
           << "Shares: " << it->second.amount << " || "
           << "Total: "
           << "$" << it->second.amount * stocks[it->second.code].price << endl;
    }

    cout << endl << "- Owned Bonds -" << endl;

    for (std::map<string, OwnedBond>::iterator it = this->ownedBonds.begin();
         it != ownedBonds.end(); ++it) {
      cout << "Name: " << it->second.name << " || "
           << "Term: " << it->second.term << " years || "
           << "Value: $" << it->second.value << endl;
    }

    cout << endl;
  }

  void writeStatement(ofstream &file) {
    file << "Balance: $" << this->checkingAccount->getBalance() << endl;

    file << endl << "- Owned Stocks -" << endl;

    for (std::map<string, OwnedStock>::iterator it = this->ownedStocks.begin();
         it != ownedStocks.end(); ++it) {
      file << "Company: " << it->second.company << " || "
           << "Code: " << it->second.code << " || "
           << "Shares: " << it->second.amount << " || "
           << "Total: "
           << "$" << it->second.amount * stocks[it->second.code].price << endl;
    }

    file << endl << "- Owned Bonds -" << endl;

    for (std::map<string, OwnedBond>::iterator it = this->ownedBonds.begin();
         it != ownedBonds.end(); ++it) {
      file << "Name: " << it->second.name << " || "
           << "Term: " << it->second.term << " || "
           << "Value: " << it->second.value << " || " << endl;
    }

    file << endl;
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

  string getName() { return this->name; }
  int getAge() { return getYearDiff(this->birthday); }
  vector<string> &getHistoryLogs() { return this->historyLogs; }

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

  // function to merge deposit and investment accoutns into one customer account
  void mergeCustomerAccounts(DepositAccount *depositAccount,
                             InvestmentAccount *investmentAccount) {
    if (this->depositAccount != nullptr || this->investmentAccount != nullptr) {
      addLog(historyLogs, "Cannot merge accounts because one of them already "
                          "exists for this customer");
      return;
    }

    this->depositAccount = depositAccount;
    this->investmentAccount = investmentAccount;
    addLog(historyLogs, "Merging " + depositAccount->getName() + " and " +
                            investmentAccount->getName());
  }

  void createBalanceStatement(ofstream &file) {

    file << endl
         << "=== User: " + name + " ===" << endl
         << endl
         << "| Basic Data |" << endl;
    file << "- id: " << this->id << endl;
    file << "- name: " << this->name << endl;
    file << "- age: " << this->getAge() << endl;
    file << "- is a citizen: " << prettyBool(this->isCitizen) << endl;
    file << "- gender: " << genderStr(this->gender) << endl;
    file << "- birthday: " << getFormattedDate(this->birthday) << endl;

    if (this->depositAccount != nullptr) {
      file << endl << "| Deposit Account Data |" << endl;
      this->depositAccount->writeStatement(file);
    }

    if (this->investmentAccount != nullptr) {
      file << endl << "| Investment Account Data |" << endl;
      this->investmentAccount->writeStatement(file);
    }

    file << endl << "| History Logs |" << endl;
    for (int i = 0; i < this->historyLogs.size(); i++) {
      file << this->historyLogs[i] << endl;
    }
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

void generateBankStatement(vector<Customer> &customers) {
  ofstream file("BankStatement.txt");

  for (int i = 0; i < customers.size(); i++) {
    customers[i].createBalanceStatement(file);
  }

  file.close();
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
  savingsAccount->transferFromChecking(checkingAccount, 300);
  savingsAccount->setInterest(0.01);
  savingsAccount->addInterest();
  savingsAccount->transferToChecking(checkingAccount, 280);

  // using the first cd account
  cdAccounts[0]->transferFromChecking(checkingAccount, 600);
  cdAccounts[0]->setInterest(0.15);
  cdAccounts[0]->blockBalance();
  cdAccounts[0]->addInterest();
  cdAccounts[0]->unblock();
  cdAccounts[0]->close(checkingAccount);

  // using the second cd account
  cdAccounts[1]->transferFromChecking(checkingAccount, 160);
  cdAccounts[1]->setInterest(0.20);
  cdAccounts[1]->blockBalance();
  cdAccounts[1]->addInterest();
  cdAccounts[1]->close(checkingAccount);

  // getting the joint customer investing account
  InvestmentAccount *investmentAccount = customer.getInvestmentAccount();

  CheckingAccount *investmentCheckingAccount =
      investmentAccount->getCheckingAccount();

  investmentCheckingAccount->deposit(2000);

  // using the joint customer investing account
  investmentAccount->buyStock("AMZN", 112.2);
  investmentAccount->buyStock("AMZN", 313.2);
  investmentAccount->buyStock("NFLX", 3000.16);
  investmentAccount->buyStock("NVDA", 334.6);
  investmentAccount->sellStock("AMZN", 1);

  investmentAccount->buyBond("U.S. Treasury Bond", 2, 600);
  investmentAccount->buyBond("Intermediate Government/Credit Bond", 1, 340);
  investmentAccount->sellBond("U.S. Treasury Bond");
}

void useDepositCustomer(Customer &customer) {
  customer.createDepositAccount();

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
  checkingAccount->deposit(3000);
  checkingAccount->withdraw(410);
  checkingAccount->withdraw(700);

  // using savings account
  savingsAccount->transferFromChecking(checkingAccount, 110);
  savingsAccount->setInterest(0.01);
  savingsAccount->addInterest();
  savingsAccount->transferToChecking(checkingAccount, 50);

  // using the first cd account
  cdAccounts[0]->transferFromChecking(checkingAccount, 400);
  cdAccounts[0]->setInterest(0.20);
  cdAccounts[0]->blockBalance();
  cdAccounts[0]->addInterest();
  cdAccounts[0]->close(checkingAccount);

  // using the second cd account
  cdAccounts[1]->transferFromChecking(checkingAccount, 200);
  cdAccounts[1]->setInterest(0.25);
  cdAccounts[1]->blockBalance();
  cdAccounts[1]->addInterest();
  cdAccounts[1]->close(checkingAccount);
}

void useInvestmentCustomer(Customer &customer) {
  customer.createInvestmentAccount();
  // getting the customer investing account
  InvestmentAccount *investmentAccount = customer.getInvestmentAccount();

  CheckingAccount *investmentCheckingAccount =
      investmentAccount->getCheckingAccount();

  investmentCheckingAccount->deposit(3500);

  // using the customer investing account
  investmentAccount->buyStock("NFLX", 1200.16);
  investmentAccount->buyStock("NVDA", 324.6);
  investmentAccount->sellStock("NVDA", 0.5);

  investmentAccount->buyBond("U.S. Treasury Bond", 4, 1337);
}

void useMergedCustomer(Customer &customer) {
  vector<string> &historyLogs = customer.getHistoryLogs();

  addLog(historyLogs, "Creating Deposit Account");
  DepositAccount *depositAccount =
      new DepositAccount(customer.getName() + "\'s investment", historyLogs);

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
  checkingAccount->deposit(3000);
  checkingAccount->withdraw(410);
  checkingAccount->withdraw(700);

  // using savings account
  savingsAccount->transferFromChecking(checkingAccount, 110);
  savingsAccount->setInterest(0.01);
  savingsAccount->addInterest();
  savingsAccount->transferToChecking(checkingAccount, 50);

  // using the first cd account
  cdAccounts[0]->transferFromChecking(checkingAccount, 400);
  cdAccounts[0]->setInterest(0.20);
  cdAccounts[0]->blockBalance();
  cdAccounts[0]->addInterest();
  cdAccounts[0]->close(checkingAccount);

  // using the second cd account
  cdAccounts[1]->transferFromChecking(checkingAccount, 200);
  cdAccounts[1]->setInterest(0.25);
  cdAccounts[1]->blockBalance();
  cdAccounts[1]->addInterest();
  cdAccounts[1]->close(checkingAccount);

  addLog(historyLogs, "Creating Investment Account");
  InvestmentAccount *investmentAccount =
      new InvestmentAccount(customer.getName() + "\'s deposit", historyLogs);

  CheckingAccount *investmentCheckingAccount =
      investmentAccount->getCheckingAccount();

  investmentCheckingAccount->deposit(3500);

  // using the customer investing account
  investmentAccount->buyStock("NFLX", 1200.16);
  investmentAccount->buyStock("NVDA", 324.6);
  investmentAccount->sellStock("NVDA", 0.5);

  investmentAccount->buyBond("U.S. Treasury Bond", 4, 1337);

  customer.mergeCustomerAccounts(depositAccount, investmentAccount);
  checkingAccount->withdraw(5); // withdrawing from the merged account
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

  customers.push_back({"01010101013",
                       "Mark Peterson",
                       true,
                       Gender::MALE,
                       {0, 0, 0, 9, 6, 92}});

  useJointCustomer(customers[0]); // creating and using the joint customer
  useDepositCustomer(
      customers[1]); // creating and using the deposit only customer
  useInvestmentCustomer(
      customers[2]); // creating and using the investment only customer
  useMergedCustomer(
      customers[3]); // creating both accoutns using them and then mergin them

  // log every important piece of info
  logStocks(stocks);
  logBonds(bonds);
  logCustomers(customers);
  generateBankStatement(customers);

  return 0;
}
