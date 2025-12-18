#pragma once

#include <beatsaber-hook/shared/utils/typedefs-wrappers.hpp>
#include "logging.hpp"


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

/// @brief Represents a C++ type that wraps a C# value type. Makes sure all members of the value type are kept valid.
/// This instance must be created at a time such that il2cpp_functions::Init is valid, or else it will throw a CreatedTooEarlyException
/// @tparam T The type of the instance to wrap.
template <class T>
struct SafeValueType {
#ifdef HAS_CODEGEN
    static_assert(T::__IL2CPP_IS_VALUE_TYPE, "Only use SafeValueType with value types!");
#endif
    /// @brief Default constructor. Should be paired with emplace or = to ensure validity.
    SafeValueType() {}
    /// @brief Construct a SafeValueType<T> with the provided value type.
    SafeValueType(const T& wrappableInstance) : internalHandle(SafePointerWrapper::New(wrappableInstance)) {}

    /// @brief Move constructor is default, moves the internal handle and keeps reference count the same.
    SafeValueType(SafeValueType&& other) = default;
    /// @brief Copy constructor copies the HANDLE, that is, the held pointer remains the same.
    /// Note that this means if you modify one SafePtr's held instance, all others that point to the same location will also reflect this change.
    /// In order to avoid a (small) performance overhead, consider using a reference type instead of a value type, or the move constructor instead.
    SafeValueType(const SafeValueType& other) : internalHandle(other.internalHandle) {}
    /// @brief Destructor. Destroys the internal wrapper type, if necessary.
    /// Aborts if a wrapper type exists and must be freed, yet GC_free does not exist.
    ~SafeValueType() {
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
            il2cpp_functions::Init();
            #ifdef UNITY_2021
            il2cpp_functions::gc_free_fixed(internalHandle.__internal_get());
            #else
            if (!il2cpp_functions::hasGCFuncs) {
                SAFE_ABORT_MSG("Cannot use SafePtr without GC functions!");
            }
            il2cpp_functions::GC_free(internalHandle.__internal_get());
            #endif
        }

        // ensure we don't try to clear the same handle twice
        internalHandle = nullptr;
    }

    /// @brief Emplace a new value into this SafePtr, freeing an existing one, if it exists.
    /// @param other The instance to emplace.
    inline void emplace(const T& other) {
        clear();
        internalHandle = SafePointerWrapper::New(other);
    }

    inline SafeValueType<T>& operator=(const T& other) {
        emplace(other);
        return *this;
    }

    /// @brief Returns false if this is a defaultly constructed SafePtr, true otherwise.
    /// Note that this means that it will return true if it holds a nullptr value explicitly!
    /// This means that you should check yourself before calling anything using the held T*.
    inline bool isHandleValid() const noexcept {
        return static_cast<bool>(internalHandle);
    }

    /// @brief Returns false if this is a defaultly constructed SafePtr,
    /// or if the held pointer evaluates to false.
    operator bool() const noexcept {
        return isHandleValid();
    }

    /// @brief Dereferences the instance pointer to a reference type of the held instance.
    /// Throws a NullHandleException if there is no internal handle.
    [[nodiscard]] T& operator*() {
        __SAFE_PTR_NULL_HANDLE_CHECK(internalHandle, internalHandle->instance);
    }

    [[nodiscard]] const T& operator*() const {
        __SAFE_PTR_NULL_HANDLE_CHECK(internalHandle, internalHandle->instance);
    }

    [[nodiscard]] T* const operator->() const {
        __SAFE_PTR_NULL_HANDLE_CHECK(internalHandle, &internalHandle->instance);
    }

    /// @brief Explicitly cast this instance to a T*.
    /// Note, however, that the lifetime of this returned T* is not longer than the lifetime of this instance.
    /// Consider passing a SafePtr reference or copy instead.
    [[nodiscard]] explicit operator T* const() const {
        __SAFE_PTR_NULL_HANDLE_CHECK(internalHandle, &internalHandle->instance);
    }

   private:
    struct SafePointerWrapper {
        static SafePointerWrapper* New(const T &instance) {
            il2cpp_functions::Init();
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
            #endif

            CRASH_UNLESS(wrapper);
            wrapper->instance = instance;
            return wrapper;
        }
        // Must be explicitly GC freed and allocated
        SafePointerWrapper() = delete;
        ~SafePointerWrapper() = delete;
        T instance;
    };
    CountPointer<SafePointerWrapper> internalHandle;
};

#undef __SAFE_PTR_NULL_HANDLE_CHECK