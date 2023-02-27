#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <stdint.h>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <chrono>
#include <algorithm>
#include <functional>
#include <array>
#include <bitset>
#include <ctype.h>
#include <malloc.h>
#include <math.h>
#include <deque>
#include <queue>
#include <stack>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <random>
#include <typeinfo>
#include <optional>
#include <forward_list>
#include <ostream>
#include <cstring>
#include <span>
#include <ranges>
#include <cstdarg>
#include <stdarg.h>

namespace chrono = std::chrono;

using std::map;
using std::optional;
using std::array;
using std::span;
using std::pair;
using std::function;
using std::string;
using std::to_string;
using std::reference_wrapper;
using std::set;
using std::stack;
using std::nullopt;
using std::is_integral;
using std::is_same;
using std::remove_reference;
using std::integral_constant;
using std::snprintf;
using std::make_unique;
using std::unique_ptr;
using std::string;
using std::unordered_map;
using std::vector;
using std::queue;
using std::hash;
using std::unordered_map;
using std::vector;
using std::queue;
using std::map;

template<class Type>
concept Integral = is_integral<Type>::value;

template<class Type>
concept NotIntegral = !is_integral<Type>::value;
