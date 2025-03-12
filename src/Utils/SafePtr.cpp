#include "Utils/SafePtr.hpp"

std::unordered_map<void*, size_t> FixedCounter::addrRefCount;
std::shared_mutex FixedCounter::mutex;

std::vector<void*> cached_safe_ptr_wrappers;

void reserve_safe_ptrs(size_t count) {
    il2cpp_functions::Init();
    for(int i = 0; i < count; ++i) {
        cached_safe_ptr_wrappers.push_back(il2cpp_functions::gc_alloc_fixed(sizeof(void*)));
    }
}
void* alloc_safe_ptr(size_t sz) {
    if(cached_safe_ptr_wrappers.empty()) {
        SAFE_ABORT_MSG("No more SafePtrs available! Please increase the cache size.");
    }
    if(sz != sizeof(void*)) {
        SAFE_ABORT_MSG("SafePtr size mismatch! Expected %zu, got %zu", sizeof(void*), sz);
    }

    void* wrapper = cached_safe_ptr_wrappers.back();
    cached_safe_ptr_wrappers.pop_back();
    return wrapper;
}
void free_safe_ptr(void* ptr) {
    cached_safe_ptr_wrappers.push_back(ptr);
}