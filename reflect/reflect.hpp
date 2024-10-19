// reflect.hpp
#pragma once

#include <type_traits>

namespace reflect {
#define PP_FOREACH_1(f, _1) f(_1)
#define PP_FOREACH_2(f, _1, _2) PP_FOREACH_1(f, _1) f(_2)
#define PP_FOREACH_3(f, _1, _2, _3) PP_FOREACH_2(f, _1, _2) f(_3)
#define PP_FOREACH_4(f, _1, _2, _3, _4) PP_FOREACH_3(f, _1, _2, _3) f(_4)
#define PP_FOREACH_5(f, _1, _2, _3, _4, _5) PP_FOREACH_4(f, _1, _2, _3, _4) f(_5)
#define PP_FOREACH_6(f, _1, _2, _3, _4, _5, _6) PP_FOREACH_5(f, _1, _2, _3, _4, _5) f(_6)
#define PP_FOREACH_7(f, _1, _2, _3, _4, _5, _6, _7) PP_FOREACH_6(f, _1, _2, _3, _4, _5, _6) f(_7)
#define PP_FOREACH_8(f, _1, _2, _3, _4, _5, _6, _7, _8) PP_FOREACH_7(f, _1, _2, _3, _4, _5, _6, _7) f(_8)
#define PP_FOREACH_9(f, _1, _2, _3, _4, _5, _6, _7, _8, _9) PP_FOREACH_8(f, _1, _2, _3, _4, _5, _6, _7, _8) f(_9)
#define PP_FOREACH_10(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
  PP_FOREACH_9(f, _1, _2, _3, _4, _5, _6, _7, _8, _9) f(_10)

#define PP_NARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define PP_NARGS(...) PP_NARGS_IMPL(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define PP_CONCAT_IMPL(x, y) x##y
#define PP_CONCAT(x, y) PP_CONCAT_IMPL(x, y)

#define PP_FOREACH(f, ...) PP_CONCAT(PP_FOREACH_, PP_NARGS(__VA_ARGS__))(f, __VA_ARGS__)

#define REFLECT_PER_MEMBER(x) f(#x, obj.x);

#define REFLECT_EXTERNAL(Type, ...)                              \
  template <>                                                    \
  struct reflect::reflect_trait<Type> {                          \
    static constexpr bool has_members() { return true; }         \
                                                                 \
    template <class Func>                                        \
    static constexpr void iterate_members(Type& obj, Func&& f) { \
      PP_FOREACH(REFLECT_PER_MEMBER, __VA_ARGS__)                \
    }                                                            \
  };

#define REFLECT(...)                                           \
  template <class This, class Func>                            \
  static constexpr void iterate_members(This& obj, Func&& f) { \
    PP_FOREACH(REFLECT_PER_MEMBER, __VA_ARGS__)                \
  }

template <class T>
constexpr auto has_members_test(int) -> decltype(T::template iterate_members<T>(std::declval<T&>(), nullptr), true) {
  return true;
}

template <class T>
constexpr bool has_members_test(...) {
  return false;
}

template <class T>
struct reflect_trait {
  static constexpr bool has_members() { return has_members_test<T>(0); }

  template <class Func>
  static constexpr void iterate_members(T& obj, Func&& func) {
    // fixme: not all template fulfills iterate_members
    // T::template iterate_members(obj, std::forward<Func>(func));
    T::iterate_members(obj, std::forward<Func>(func));
  }
};

}  // namespace reflect