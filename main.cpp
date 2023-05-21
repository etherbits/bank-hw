#include <iostream>
#include <string>

using namespace std;

string padLeft(string str, char padChar, int len) {
  while (str.length() < len) {
    str.insert(0, 1, padChar);
  }
  return str;
}

struct Date {
  int day;
  int month;
  int year;

  string getFormattedDate() {
    return padLeft(to_string(day), '0', 2) + "/" +
           padLeft(to_string(month), '0', 2) + "/" + to_string(year);
  }
};

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
  int age;
  bool isCitizen;
  string gender;
  Date *birthday;
  InvestmentAccout *investmentAccout;

public:
  User(string id, string name, int age, bool isCitizen, string gender,
       Date *birthday)
      : id(id), name(name), age(age), isCitizen(isCitizen), gender(gender),
        birthday(birthday) {}

  string getId() { return this->id; }

  void logBasicData() {
    cout << "id: " << this->id << endl;
    cout << "name: " << this->name << endl;
    cout << "age: " << this->age << endl;
    cout << "is a citizen: " << this->isCitizen << endl;
    cout << "gender: " << this->gender << endl;
    cout << "birthday: " << this->birthday->getFormattedDate() << endl;
  }

  void attachInvestmentAccount(InvestmentAccout *acc) {
    this->investmentAccout = acc;
  }

  ~User() {
    delete birthday;
    delete investmentAccout;
  }
};

int main() {
  User *user = new User("100", "Nika", 19, true, "Male", new Date{26, 3, 2004});
  user->logBasicData();
  user->attachInvestmentAccount(new InvestmentAccout(user->getId()));
  delete user;

  return 0;
}
