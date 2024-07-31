#pragma once

#include <vector>
#include <map>

#include "../../reflect.hpp"
#include "nlohmann/json.hpp"

namespace {
template <class T>
struct special_traits : std::false_type {};
}  // namespace

namespace reflect::json {
using json = nlohmann::json;

// -------------serialize-----------------

// 基础型：既不满足has_members，也不是特殊型
template <class T, std::enable_if_t<!reflect::reflect_trait<T>::has_members() && !special_traits<T>::value, int> = 0>
json serialize(const T& obj) {
  return json(obj);
}

// 特殊型：不满足has_members，但是是特殊性，通过special_traits做一层转发
template <class T, std::enable_if_t<!reflect::reflect_trait<T>::has_members() && special_traits<T>::value, int> = 0>
json serialize(const T& obj) {
  return special_traits<T>::serialize(obj);
}

// 反射对象型：但凡是做了反射的，不论是REFLECT还是REFLECT_EXTERNAL，都满足has_members
template <class T, std::enable_if_t<reflect::reflect_trait<T>::has_members(), int> = 0>
json serialize(const T& obj) {
  json root;
  reflect::reflect_trait<T>::iterate_members(const_cast<T&>(obj),
                                             [&](const char* key, auto& value) { root[key] = serialize(value); });
  return root;
}

// -------------deserialize---------------

template <class T, std::enable_if_t<!reflect::reflect_trait<T>::has_members() && !special_traits<T>::value, int> = 0>
T deserialize(const json& j) {
  return j.get<T>();
}

template <class T, std::enable_if_t<!reflect::reflect_trait<T>::has_members() && special_traits<T>::value, int> = 0>
T deserialize(const json& j) {
  return special_traits<T>::deserialize(j);
}

template <class T, std::enable_if_t<reflect::reflect_trait<T>::has_members(), int> = 0>
T deserialize(const json& j) {
  T obj;
  reflect::reflect_trait<T>::iterate_members(obj, [&](const char* key, auto& v) {
    if (j.count(key)) {
      v = deserialize<std::decay_t<decltype(v)>>(j.at(key));
    }
  });
  return obj;
}

// ------------json str wrapper functions-----------------

json from_json(const std::string& s) { return json::parse(s); }

std::string to_json(const json& j, int indent = -1) { return j.dump(indent); }

}  // namespace reflect::json

namespace {
using json = nlohmann::json;

template <class T, class Alloc>
struct special_traits<std::vector<T, Alloc>> : std::true_type {
  static json serialize(std::vector<T, Alloc> const& vec) {
    json root;
    for (const auto& value : vec) {
      root.push_back(reflect::json::serialize(value));
    }
    return root;
  }

  static std::vector<T, Alloc> deserialize(json const& j) {
    std::vector<T, Alloc> vec;
    for (const auto& value : j) {
      vec.push_back(reflect::json::deserialize<T>(value));
    }
    return vec;
  }
};

template <class T, class V, class Compare, class Alloc>
struct special_traits<std::map<T, V, Compare, Alloc>> : std::true_type {
  static json serialize(std::map<T, V, Compare, Alloc> const& map) {
    json root;
    for (const auto& [key, value] : map) {
      root[reflect::json::serialize(key)] = reflect::json::serialize(value);
    }
    return root;
  }

  static std::map<T, V, Compare, Alloc> deserialize(json const& j) {
    std::map<T, V, Compare, Alloc> map;
    for (const auto& [key, value] : j.items()) {
      map[reflect::json::deserialize<T>(key)] = reflect::json::deserialize<V>(value);
    }
    return map;
  };
};

}  // namespace