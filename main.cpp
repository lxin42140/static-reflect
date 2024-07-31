#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "reflect/json/reflect_json.h"
#include "reflect/reflect.hpp"

struct Address {
  std::string country;
  std::string city;
  std::vector<std::string> streets;

// REFLECT(country, city, streets)
};

REFLECT_EXTERNAL(Address, country, city, streets)

struct Score {
  int score;

  REFLECT(score)
};

struct Person {
  std::string name;
  int age;
  Address address;
  std::map<std::string, Score> scores;

  REFLECT(name, age, address, scores)
};

// REFLECT_EXTERNAL(Person, name, age, address, scores)



template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
  os << "[";
  for (const auto& e : v) {
    os << e << ", ";
  }
  if (!v.empty()) {
    os << "\b\b";
  }
  os << "]";
  return os;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::map<std::string, T>& m) {
  os << "{";
  for (const auto& [k, v] : m) {
    os << k << ": " << v << ", ";
  }
  if (!m.empty()) {
    os << "\b\b";
  }
  os << "}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Address& a) {
  os << "{city: " << a.city << ", streets: " << a.streets << "}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Person& p) {
  os << "{name: " << p.name << ", age: " << p.age << ", address: " << p.address << ", scores: " << p.scores << "}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Score& s){
  os << "{score: " << s.score << "}";
  return os;
}

int main() {
  Person p1 = {.name = "Alice",
               .age = 25,
               .address =
                   {
                       .country = "China",
                       .city = "Beijing",
                       .streets = {"Xingang Street", "Chaoyang Street"},
                   },
               .scores = {{"math", {85}}, {"english", {90}}} };
  nlohmann::json j = reflect::json::serialize(p1);
  std::cout << j.dump(4) << "\n";

  auto p2 = reflect::json::deserialize<Person>(j);
  std::cout << p2 << "\n";

  return 0;
}