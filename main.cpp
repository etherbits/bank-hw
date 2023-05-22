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
  double price;
};

void logStocks(map<string, Stock> stocks) {
  cout << endl << "=== Available Stocks ===" << endl;

  for (std::map<string, Stock>::iterator it = stocks.begin();
       it != stocks.end(); ++it) {
    cout << "Company: " << it->second.company << " || "
         << "Code: " << it->second.code << " || "
         << "Price: $" << it->second.price << endl;
  }
  cout << endl;
}

class InvestmentAccout {
  string userId;

public:
  InvestmentAccout(string userId) : userId(userId) {}
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
  User(string id, string name, int age, bool isCitizen, Gender gender,
       tm birthday)
      : id(id), name(name), isCitizen(isCitizen), gender(gender),
        birthday(birthday) {}

  string getId() { return this->id; }

  int getAge() { return getYearDiff(this->birthday); }

  void logBasicData() {
    cout << "id: " << this->id << endl;
    cout << "name: " << this->name << endl;
    cout << "age: " << this->getAge() << endl;
    cout << "is a citizen: " << prettyBool(this->isCitizen) << endl;
    cout << "gender: " << genderStr(this->gender) << endl;
    cout << "birthday: " << getFormattedDate(this->birthday) << endl;
  }

  void attachInvestmentAccount(InvestmentAccout *acc) {
    this->investmentAccout = acc;
  }

  ~User() { delete investmentAccout; }
};

void logUsers(vector<User *> users) {
  cout << endl << "=== All Customers ===" << endl;
  for (int i = 0; i < users.size(); i++) {
    cout << "---" << endl;
    users[i]->logBasicData();
  }
  cout << endl;
}

int main() {
  map<string, Stock> stocks = {{"AMZN", {"Amazon.com Inc.", "AMZN", 114.8}},
                               {"NFLX", {"Netflix Inc.", "NFLX", 367.4}},
                               {"NVDA", {"Nvidia Corp.", "NVDA", 313.2}}};
  vector<User *> users = {};

  User *user1 = new User("100", "Nika Qvrivishvili", 19, true, Gender::MALE,
                         {0, 0, 0, 26, 2, 104});

  User *user2 =
      new User("123", "John Doe", 19, true, Gender::MALE, {0, 0, 0, 22, 4, 99});

  users.push_back(user1);
  users.push_back(user2);

  user1->attachInvestmentAccount(new InvestmentAccout(user1->getId()));

  logStocks(stocks);
  logUsers(users);

  delete user1;
  return 0;
}
