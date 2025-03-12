#include "Utils/SafePtr.hpp"

std::unordered_map<void*, size_t> FixedCounter::addrRefCount;
std::shared_mutex FixedCounter::mutex;