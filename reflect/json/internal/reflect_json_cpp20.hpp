#pragma once

#include <type_traits>
#include <vector>
#include <map>

#include "../../reflect.hpp"
#include "nlohmann/json.hpp"

namespace reflect::json {
using json = nlohmann::json;

// -----------concepts--------------

template <class T>
concept is_json_object = reflect::reflect_trait<T>::has_members();

template <class T>
concept is_vector = std::is_same_v<std::vector<typename T::value_type>, T>;

template <class T>
concept is_map = std::is_same_v<std::map<typename T::key_type, typename T::mapped_type>, T>;

// -----------serialize--------------

template <class T>
json serialize(const T& obj) {
  return json(obj);
}

template <class T>
requires(is_json_object<T>) json serialize(const T& obj);

template <class T>
requires(is_vector<T>) json serialize(const T& obj) {
  json root;
  for (const auto& value : obj) {
    root.push_back(reflect::json::serialize(value));
  }
  return root;
}

template <class T>
requires(is_map<T>) json serialize(const T& obj) {
  json root;
  for (const auto& [key, value] : obj) {
    root[key] = reflect::json::serialize(value);
  }
  return root;
}

template <class T>
requires(is_json_object<T>) json serialize(const T& obj) {
  json root;
  reflect::reflect_trait<T>::iterate_members(
      const_cast<T&>(obj), [&](const char* key, auto& value) { root[key] = reflect::json::serialize(value); });
  return root;
}

// -----------deserialize--------------

template <class T>
T deserialize(const json& j) {
  return j.get<T>();
}

template <class T>
requires(is_json_object<T>) T deserialize(const json& j);

template <class T>
requires(is_vector<T>) T deserialize(const json& j) {
  T obj;
  for (const auto& value : j) {
    obj.push_back(deserialize<typename T::value_type>(value));
  }
  return obj;
}

template <class T>
requires(is_map<T>) T deserialize(const json& j) {
  T obj;
  for (const auto& [key, value] : j.items()) {
    obj[key] = deserialize<typename T::mapped_type>(value);
  }
  return obj;
}

template <class T>
requires(is_json_object<T>) T deserialize(const json& j) {
  T obj;
  reflect::reflect_trait<T>::iterate_members(obj, [&](const char* key, auto& value) {
    if (j.count(key)) {
      value = deserialize<std::decay_t<decltype(value)>>(j[key]);
    }
  });
  return obj;
}

// -----------json string wrapper------------

std::string to_json(const json& j, int indent = -1) { return j.dump(indent); }

json from_json(const std::string& s) { return json::parse(s); }

}  // namespace reflect::json