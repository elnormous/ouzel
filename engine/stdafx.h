// Ouzel by Elviss Strazdins

#ifndef STDAFX_H
#define STDAFX_H

#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#ifdef __cplusplus
#  include <algorithm>
#  include <array>
#  include <atomic>
#  include <cassert>
#  include <cctype>
#  include <cfloat>
#  include <chrono>
#  include <cmath>
#  include <condition_variable>
#  include <cstddef>
#  include <cstdint>
#  include <cstdlib>
#  include <functional>
#  include <future>
#  include <iterator>
#  include <limits>
#  include <list>
#  include <map>
#  include <memory>
#  include <mutex>
#  include <numeric>
#  include <queue>
#  include <random>
#  include <set>
#  include <stdexcept>
#  include <string>
#  include <system_error>
#  include <thread>
#  include <tuple>
#  include <type_traits>
#  include <unordered_map>
#  include <utility>
#  include <vector>
#endif

#endif // STDAFX_H