#pragma once

#include <beatsaber-hook/shared/utils/typedefs-wrappers.hpp>

// This file is a copy of the SafePtr classes from typedefs-wrapper.hpp with the changes from https://github.com/QuestPackageManager/beatsaber-hook/pull/33
// TODO: delete this and use the original versions if the changes are (a) backported or (b) we move on to 1.40


#if __has_feature(cxx_exceptions)
#define __SAFE_PTR_NULL_HANDLE_CHECK(handle, ...) \
    if (handle) return __VA_ARGS__;               \
    throw NullHandleException()

#else
#include "utils.h"
#define __SAFE_PTR_NULL_HANDLE_CHECK(handle, ...) \
    if (handle) return __VA_ARGS__;               \
    CRASH_UNLESS(false)
#endif

/// @brief A thread-safe, static type that holds a mapping from addresses to reference counts.
struct FixedCounter {
    /// @brief Adds to the reference count of an address. If the address does not exist, initializes a new entry for it to 1.
    /// @param addr The address to add.
    static void add(void* addr) {
        std::unique_lock lock(mutex);
        auto itr = addrRefCount.find(addr);
        if (itr != addrRefCount.end()) {
            ++itr->second;
        } else {
            addrRefCount.emplace(addr, 1);
        }
    }
    /// @brief Decreases the reference count of an address. If the address has 1 or fewer references, erases it.
    /// @param addr The address to decrease.
    static void remove(void* addr) {
        std::unique_lock lock(mutex);
        auto itr = addrRefCount.find(addr);
        if (itr != addrRefCount.end() && itr->second > 1) {
            --itr->second;
        } else if (itr != addrRefCount.end()) {
            addrRefCount.erase(itr);
        } else {
            SAFE_ABORT_MSG("Tried to remove a non-existent address from FixedCounter!");
        }
    }
    /// @brief Gets the reference count of an address, or 0 if no such address exists.
    /// @param addr The address to get the count of.
    /// @return The reference count of the provided address.
    static size_t get(void* addr) {
        std::shared_lock lock(mutex);
        auto itr = addrRefCount.find(addr);
        if (itr != addrRefCount.end()) {
            return itr->second;
        } else {
            return 0;
        }
    }

   private:
    static std::unordered_map<void*, size_t> addrRefCount;
    static std::shared_mutex mutex;
};

/// @brief Represents a smart pointer that has a reference count, which does NOT destroy the held instance on refcount reaching 0.
/// @tparam T The type to wrap as a pointer.
template <class T>
struct FixedCountPointer {
    /// @brief Default constructor for Count Pointer, defaults to a nullptr, with 0 references.
    explicit FixedCountPointer() : ptr(nullptr) {}
    /// @brief Construct a count pointer from the provided pointer, adding to the reference count (if non-null) for the provided pointer.
    /// @param p The pointer to provide. May be null, which does nothing.
    explicit FixedCountPointer(T* p) : ptr(p) {
        if (p) {
            FixedCounter::add(p);
        }
    }
    /// @brief Copy constructor, copies and adds to the reference count for the held non-null pointer.
    FixedCountPointer(const FixedCountPointer<T>& other) : ptr(other.ptr) {
        if (ptr) {
            FixedCounter::add(ptr);
        }
    }
    /// @brief Move constructor moves the pointer and keeps the reference count the same.
    FixedCountPointer(FixedCountPointer&& other) {
        ptr = other.ptr;
        other.ptr = nullptr;
    }
    /// @brief Destructor, decreases the ref count for the held non-null pointer.
    ~FixedCountPointer() {
        if (ptr) {
            FixedCounter::remove(ptr);
        }
    }
    /// @brief Gets the reference count held by this pointer.
    /// @return The reference count for this pointer, or 0 if the held pointer is null.
    size_t count() const {
        if (ptr) {
            return FixedCounter::get(ptr);
        }
        return 0;
    }
    /// @brief Emplaces a new pointer into the shared pointer, decreasing the existing ref count as necessary.
    /// @param val The new pointer to replace the currently held one with.
    inline void emplace(T* val) {
        if (val != ptr) {
            if (ptr) {
                FixedCounter::remove(ptr);
            }
            ptr = val;
            if (ptr) {
                FixedCounter::add(ptr);
            }
        }
    }
    /// Assignment operator.
    FixedCountPointer& operator=(T* val) {
        emplace(val);
        return *this;
    }
    /// Dereference operator.
    T& operator*() noexcept {
        if (ptr) {
            return *ptr;
        }
        SAFE_ABORT();
        return *ptr;
    }
    const T& operator*() const noexcept {
        if (ptr) {
            return *ptr;
        }
        SAFE_ABORT();
        return *ptr;
    }
    T* operator->() noexcept {
        if (ptr) {
            return ptr;
        }
        SAFE_ABORT();
        return nullptr;
    }
    T* const operator->() const noexcept {
        if (ptr) {
            return ptr;
        }
        SAFE_ABORT();
        return nullptr;
    }
    constexpr operator bool() const noexcept {
        return ptr != nullptr;
    }
    /// @brief Performs an il2cpp type checked cast from T to U.
    /// This should only be done if both T and U are reference types
    /// Currently assumes the `klass` field is the first pointer in T.
    /// This function may throw TypeCastException or crash.
    /// See try_cast for a version that does not.
    /// @tparam U The type to cast to.
    /// @return A new CountPointer of the cast value.
    template <class U>
    [[nodiscard]] inline FixedCountPointer<U> cast() const {
        // TODO: We currently assume that the first sizeof(void*) bytes of ptr is the klass field.
        // This should hold true for everything except value types.
        auto* k1 = CRASH_UNLESS(classof(U*));
        auto* k2 = *CRASH_UNLESS(reinterpret_cast<Il2CppClass**>(ptr));
        CRASH_UNLESS(k2);
        il2cpp_functions::Init();
        if (il2cpp_functions::class_is_assignable_from(k1, k2)) {
            return FixedCountPointer<U>(reinterpret_cast<U*>(ptr));
        }
#if __has_feature(cxx_exceptions)
        throw TypeCastException();
#else
        SAFE_ABORT();
        return CountPointer<U>();
#endif
    }
    /// @brief Performs an il2cpp type checked cast from T to U.
    /// This should only be done if both T and U are reference types
    /// Currently assumes the `klass` field is the first pointer in T.
    /// @tparam U The type to cast to.
    /// @return A new CountPointer of the cast value, if successful.
    template <class U>
    [[nodiscard]] inline std::optional<CountPointer<U>> try_cast() const noexcept {
        auto* k1 = classof(U*);
        if (!ptr || !k1) {
            return std::nullopt;
        }
        auto* k2 = *reinterpret_cast<Il2CppClass**>(ptr);
        if (!k2) {
            return std::nullopt;
        }
        il2cpp_functions::Init();
        if (il2cpp_functions::class_is_assignable_from(k1, k2)) {
            return CountPointer<U>(reinterpret_cast<U*>(ptr));
        }
        return std::nullopt;
    }

    /// @brief Get the raw pointer. Should ALMOST NEVER BE USED, UNLESS SCOPE GUARANTEES IT DIES BEFORE THIS INSTANCE DOES!
    /// @return The raw pointer saved by this instance.
    constexpr T* const __internal_get() const noexcept {
        return ptr;
    }

   private:
    T* ptr;
};

// TODO: Make an overall Ptr interface type, virtual destructor and *, -> operators
// TODO: Remove all conversion operators? (Basically force people to guarantee lifetime of held instance?)

// Fd UnityEngine.Object
#ifdef HAS_CODEGEN
namespace UnityEngine {
class Object;
}
#endif

template <typename T>
struct FixedSafePtrUnity;

// Until we are sure everyone uses the fixed counting, cache enough elements and never actually ask the GC for new objects later on
// This is a temporary solution until we can ensure that all SafePtrs are FixedSafePtrs

void reserve_safe_ptrs(size_t count);
void* alloc_safe_ptr(size_t sz);
void free_safe_ptr(void* ptr);


/// @brief Represents a C++ type that wraps a C# pointer that will be valid for the entire lifetime of this instance.
/// This instance must be created at a time such that il2cpp_functions::Init is valid, or else it will throw a CreatedTooEarlyException
/// @tparam T The type of the instance to wrap.
/// @tparam AllowUnity Whether to permit convertible Unity Object types to be wrapped.
template <class T, bool AllowUnity = false>
struct FixedSafePtr {
#ifdef HAS_CODEGEN
    static_assert(!std::is_assignable_v<UnityEngine::Object, T> || AllowUnity, "Don't use Unity types with SafePtr. Ignore this warning by specifying SafePtr<T, true>");
#endif
    /// @brief Default constructor. Should be paired with emplace or = to ensure validity.
    FixedSafePtr() {}
    /// @brief Construct a SafePtr<T> with the provided instance pointer (which may be nullptr).
    /// If you wish to wrap a non-existent pointer (ex, use as a default constructor) see the 0 arg constructor instead.
    FixedSafePtr(T* wrappableInstance) : internalHandle(SafePointerWrapper::New(wrappableInstance)) {}
    /// @brief Construct a SafePtr<T> with the provided wrapper
    FixedSafePtr(T& wrappableInstance)
        requires(il2cpp_utils::has_il2cpp_conversion<T>)
        : internalHandle(SafePointerWrapper::New(wrappableInstance.convert())) {}
    /// @brief Construct a SafePtr<T> with the provided wrapper
    FixedSafePtr(T&& wrappableInstance)
        requires(il2cpp_utils::has_il2cpp_conversion<T>)
        : internalHandle(SafePointerWrapper::New(wrappableInstance.convert())) {}
    /// @brief Construct a SafePtr<T> with the provided reference
    FixedSafePtr(T& wrappableInstance)
        requires(!il2cpp_utils::has_il2cpp_conversion<T>)
        : internalHandle(SafePointerWrapper::New(std::addressof(wrappableInstance))) {}
    /// @brief Move constructor is default, moves the internal handle and keeps reference count the same.
    FixedSafePtr(FixedSafePtr&& other) = default;
    /// @brief Copy constructor copies the HANDLE, that is, the held pointer remains the same.
    /// Note that this means if you modify one SafePtr's held instance, all others that point to the same location will also reflect this change.
    /// In order to avoid a (small) performance overhead, consider using a reference type instead of a value type, or the move constructor instead.
    FixedSafePtr(const FixedSafePtr& other) : internalHandle(other.internalHandle) {}
    /// @brief Destructor. Destroys the internal wrapper type, if necessary.
    /// Aborts if a wrapper type exists and must be freed, yet GC_free does not exist.
    ~FixedSafePtr() {
        clear();
    }

    /// @brief Destroys the internal wrapper type, if necessary.
    /// Aborts if a wrapper type exists and must be freed, yet GC_free does not exist.
    inline void clear() {
        if (!internalHandle) {
            // Clearing up without an internal handle is trivial
            return;
        }
        // If our internal handle has 1 instance, we need to clean up the instance it points to.
        // Otherwise, some other SafePtr is currently holding a reference to this instance, so keep it around.
        if (internalHandle.count() <= 1) {
            internalHandle->instancePointer = nullptr;
            free_safe_ptr(internalHandle.__internal_get());

            /*il2cpp_functions::Init();
            #ifdef UNITY_2021
            il2cpp_functions::gc_free_fixed(internalHandle.__internal_get());
            #else
            if (!il2cpp_functions::hasGCFuncs) {
                SAFE_ABORT_MSG("Cannot use SafePtr without GC functions!");
            }
            il2cpp_functions::GC_free(internalHandle.__internal_get());
            #endif*/
        }

        // ensure we don't try to clear the same handle twice
        internalHandle = nullptr;
    }

    /// @brief Emplace a new value into this SafePtr, freeing an existing one, if it exists.
    /// @param other The instance to emplace.
    inline void emplace(T& other) {
        clear();
        internalHandle = SafePointerWrapper::New(std::addressof(other));
    }

    /// @brief Emplace a new value into this SafePtr, freeing an existing one, if it exists.
    /// @param other The instance to emplace.
    inline void emplace(T* other) {
        clear();
        internalHandle = SafePointerWrapper::New(other);
    }

    /// @brief Emplace a new pointer into this SafePtr, managing the existing one, if it exists.
    /// @param other The CountPointer to copy during the emplace.
    inline void emplace(FixedCountPointer<T>& other) {
        // Clear existing instance as necessary
        clear();
        // Copy other into handle
        internalHandle = other;
    }

    /// @brief Move an existing CountPointer<T> into this SafePtr, deleting the existing one, if necessary.
    /// @param other The CountPointer to move during this call.
    inline void move(FixedCountPointer<T>& other) {
        // Clear existing instance as necessary
        clear();
        // Move into handle
        internalHandle = std::move(other);
    }

    inline FixedSafePtr<T, AllowUnity>& operator=(T* other) {
        emplace(other);
        return *this;
    }

    inline FixedSafePtr<T, AllowUnity>& operator=(T& other) {
        emplace(other);
        return *this;
    }
    /// @brief Performs an il2cpp type checked cast from T to U.
    /// This should only be done if both T and U are reference types
    /// Currently assumes the `klass` field is the first pointer in T.
    /// This function may throw TypeCastException or NullHandleException or otherwise abort.
    /// See try_cast for a version that does not.
    /// @tparam U The type to cast to.
    /// @tparam AllowUnityPrime Whether the casted SafePtr should allow unity conversions.
    /// @return A new SafePtr of the cast value.
    template <class U, bool AllowUnityPrime = AllowUnity>
    [[nodiscard]] inline FixedSafePtr<U, AllowUnityPrime> cast() const {
        // TODO: We currently assume that the first sizeof(void*) bytes of ptr is the klass field.
        // This should hold true for everything except value types.
        if (!internalHandle) {
#if __has_feature(cxx_exceptions)
            throw NullHandleException();
#else
            SAFE_ABORT();
            return SafePtr<U, AllowUnityPrime>();
#endif
        }
        auto* k1 = CRASH_UNLESS(classof(U*));
        auto* k2 = *CRASH_UNLESS(reinterpret_cast<Il2CppClass**>(internalHandle->instancePointer));
        il2cpp_functions::Init();
        if (il2cpp_functions::class_is_assignable_from(k1, k2)) {
            return FixedSafePtr<U, AllowUnityPrime>(reinterpret_cast<U*>(internalHandle->instancePointer));
        }
#if __has_feature(cxx_exceptions)
        throw TypeCastException();
#else
        SAFE_ABORT();
        return FixedSafePtr<U, AllowUnityPrime>();
#endif
    }
    /// @brief Performs an il2cpp type checked cast from T to U.
    /// This should only be done if both T and U are reference types
    /// Currently assumes the `klass` field is the first pointer in T.
    /// @tparam U The type to cast to.
    /// @tparam AllowUnityPrime Whether the casted SafePtr should allow unity conversions.
    /// @return A new SafePtr of the cast value, if successful.
    template <class U, bool AllowUnityPrime = AllowUnity>
    [[nodiscard]] inline std::optional<FixedSafePtr<U, AllowUnityPrime>> try_cast() const noexcept {
        auto* k1 = classof(U*);
        if (!internalHandle || !internalHandle->instancePointer || k1) {
            return std::nullopt;
        }
        auto* k2 = *reinterpret_cast<Il2CppClass**>(internalHandle->instancePointer);
        if (!k2) {
            return std::nullopt;
        }
        il2cpp_functions::Init();
        if (il2cpp_functions::class_is_assignable_from(k1, k2)) {
            return FixedSafePtr<U, AllowUnityPrime>(reinterpret_cast<U*>(internalHandle->instancePointer));
        }
        return std::nullopt;
    }

    /// @brief Returns false if this is a defaultly constructed SafePtr, true otherwise.
    /// Note that this means that it will return true if it holds a nullptr value explicitly!
    /// This means that you should check yourself before calling anything using the held T*.
    inline bool isHandleValid() const noexcept {
        return static_cast<bool>(internalHandle);
    }

    T* ptr() {
        __SAFE_PTR_NULL_HANDLE_CHECK(internalHandle, internalHandle->instancePointer);
    }

    T* const ptr() const {
        __SAFE_PTR_NULL_HANDLE_CHECK(internalHandle, internalHandle->instancePointer);
    }

    /// @brief Returns false if this is a defaultly constructed SafePtr,
    /// or if the held pointer evaluates to false.
    operator bool() const noexcept {
        return isHandleValid() && ptr();
    }

    /// @brief Dereferences the instance pointer to a reference type of the held instance.
    /// Throws a NullHandleException if there is no internal handle.
    [[nodiscard]] T& operator*() {
        return *ptr();
    }

    [[nodiscard]] const T& operator*() const {
        return *ptr();
    }

    [[nodiscard]] T* const operator->() const {
        return const_cast<T*>(ptr());
    }

    /// @brief Explicitly cast this instance to a T*.
    /// Note, however, that the lifetime of this returned T* is not longer than the lifetime of this instance.
    /// Consider passing a SafePtr reference or copy instead.
    [[nodiscard]] explicit operator T* const() const {
        return const_cast<T*>(ptr());
    }

   private:
    friend struct FixedSafePtrUnity<T>;

    struct SafePointerWrapper {
        static SafePointerWrapper* New(T* instance) {
            static constexpr auto sz = sizeof(SafePointerWrapper);
            auto* wrapper = reinterpret_cast<SafePointerWrapper*>(alloc_safe_ptr(sz));

            /*il2cpp_functions::Init();
            // It should be safe to assume that gc_alloc_fixed returns a non-null pointer. If it does return null, we have a pretty big issue.
            static constexpr auto sz = sizeof(SafePointerWrapper);

            #ifdef UNITY_2021
            auto* wrapper = reinterpret_cast<SafePointerWrapper*>(il2cpp_functions::gc_alloc_fixed(sz));

            #else

            if (!il2cpp_functions::hasGCFuncs) {
                #if __has_feature(cxx_exceptions)
                throw CreatedTooEarlyException();
                #else
                SAFE_ABORT_MSG("Cannot use a SafePtr this early/without GC functions!");
                #endif
            }
            auto* wrapper = reinterpret_cast<SafePointerWrapper*>(il2cpp_functions::GarbageCollector_AllocateFixed(sz, nullptr));
            #endif*/

            CRASH_UNLESS(wrapper);
            wrapper->instancePointer = instance;
            return wrapper;
        }
        // Must be explicitly GC freed and allocated
        SafePointerWrapper() = delete;
        ~SafePointerWrapper() = delete;
        T* instancePointer;
    };
    FixedCountPointer<SafePointerWrapper> internalHandle;
};

#if __has_feature(cxx_exceptions)
#define __SAFE_PTR_UNITY_NULL_HANDLE_CHECK(...) \
    if (isAlive()) return __VA_ARGS__;          \
    throw NullHandleException()

#else
#include "utils.h"
#define __SAFE_PTR_UNITY_NULL_HANDLE_CHECK(...) \
    if (isAlive()) return __VA_ARGS__;          \
    CRASH_UNLESS(false)
#endif

template <typename T>
struct FixedSafePtrUnity : public FixedSafePtr<T, true> {
   private:
    using Parent = FixedSafePtr<T, true>;

   public:
    FixedSafePtrUnity() = default;

    FixedSafePtrUnity(T* wrappableInstance) : Parent(wrappableInstance) {}
    FixedSafePtrUnity(T& wrappableInstance) : Parent(wrappableInstance) {}
    FixedSafePtrUnity(Parent&& p) : Parent(p) {}
    FixedSafePtrUnity(Parent const& p) : Parent(p) {}

    FixedSafePtrUnity(FixedSafePtrUnity&& p) : Parent(p) {}
    FixedSafePtrUnity(FixedSafePtrUnity const& p) : Parent(p) {}

    T* ptr() {
        __SAFE_PTR_UNITY_NULL_HANDLE_CHECK(Parent::internalHandle->instancePointer);
    }

    T* const ptr() const {
        __SAFE_PTR_UNITY_NULL_HANDLE_CHECK(Parent::internalHandle->instancePointer);
    }

    inline FixedSafePtrUnity<T>& operator=(T* other) {
        Parent::emplace(other);
        return *this;
    }

    inline FixedSafePtrUnity<T>& operator=(T& other) {
        Parent::emplace(other);
        return *this;
    }

    /// @brief Explicitly cast this instance to a T*.
    /// Note, however, that the lifetime of this returned T* is not longer than the lifetime of this instance.
    /// Consider passing a SafePtrUnity reference or copy instead.
    explicit operator T* const() const {
        return const_cast<T*>(ptr());
    }

    T* const operator->() {
        return const_cast<T*>(ptr());
    }

    T* const operator->() const {
        return ptr();
    }

    T& operator*() {
        return *ptr();
    }

    T const& operator*() const {
        return *ptr();
    }

    operator bool() const {
        return isAlive();
    }

    template <typename U = T>
        requires(std::is_assignable_v<T, U> || std::is_same_v<T, U>) bool
    operator==(FixedSafePtrUnity<U> const& other) const {
        if (!other.isAlive() || !isAlive()) {
            return other.isAlive() == isAlive();
        }

        return static_cast<T*>(other.internalHandle) == static_cast<T*>(Parent::ptr());
    }

    template <typename U = T>
    bool operator==(U const* other) const {
        if (!other || !isAlive()) {
            return static_cast<bool>(other) == isAlive();
        }

        return static_cast<T*>(other) == static_cast<T*>(Parent::ptr());
    }

    inline bool isAlive() const {
#ifdef HAS_CODEGEN
        return static_cast<bool>(Parent::internalHandle) && (Parent::ptr()) && Parent::ptr()->m_CachedPtr.m_value.convert();
#else
        // offset yay
        // the offset as specified in the codegen header of [m_CachedPtr] is 0x10
        // which is also the first field of the instance UnityEngine.Object
        return static_cast<bool>(Parent::internalHandle) && (Parent::ptr()) && *reinterpret_cast<void* const*>(reinterpret_cast<uint8_t const*>(Parent::ptr()) + 0x10);
#endif
    }
};

#undef __SAFE_PTR_NULL_HANDLE_CHECK