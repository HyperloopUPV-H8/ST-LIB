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
#include <stdarg.h>

#define endl "\n\r"

using namespace std;

template<class Type>
concept Integral = is_integral<Type>::value;

template<class Type>
concept NotIntegral = !is_integral<Type>::value;
