#pragma once

#include <utility>
#include <beatsaber-hook/shared/utils/il2cpp-functions.hpp>


// Helper traits to extract the signature from a callable.
// Primary template: deduces the signature from T's operator()
template<typename T>
struct function_traits : public function_traits<decltype(&T::operator())> {};

// Specialization for non-const member function pointers.
template<typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)> {
    using signature = R(Args...);
};

// Specialization for const member function pointers (most lambdas).
template<typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const> {
    using signature = R(Args...);
};

// Also support function pointers.
template<typename R, typename... Args>
struct function_traits<R(*)(Args...)> {
    using signature = R(Args...);
};

// And plain function types.
template<typename R, typename... Args>
struct function_traits<R(Args...)> {
    using signature = R(Args...);
};


// A type-erased function wrapper similar to std::function, but using il2cpp_functions::gc_alloc_fixed and il2cpp_functions::gc_free_fixed.
// Ensures all captured variables of a lambda are still known by the GC
template<typename Signature>
class gc_aware_function;

template<typename R, typename... Args>
class gc_aware_function<R(Args...)> {
public:
    // Default constructor
    gc_aware_function() : callable_(nullptr) {}

    // Construct from any callable, using the provided custom allocator.
    template<typename F>
    gc_aware_function(F f)
    {
        il2cpp_functions::Init();
        void *wrapped = il2cpp_functions::gc_alloc_fixed(sizeof(Callable<F>));
        callable_ = new (wrapped) Callable<F>(std::move(f));
    }

    // Copy constructor: clone the stored callable using the custom allocator.
    gc_aware_function(const gc_aware_function& other)
    {
        callable_ = other.callable_ ? other.callable_->clone() : nullptr;
    }

    // Move constructor.
    gc_aware_function(gc_aware_function&& other) noexcept
        : callable_(other.callable_)
    {
        other.callable_ = nullptr;
    }

    // Copy assignment.
    gc_aware_function& operator=(const gc_aware_function& other) {
        if (this != &other) {
            if (callable_)
                callable_->destroy();
            callable_ = other.callable_ ? other.callable_->clone() : nullptr;
        }
        return *this;
    }

    // Move assignment.
    gc_aware_function& operator=(gc_aware_function&& other) noexcept {
        if (this != &other) {
            if (callable_)
                callable_->destroy();
            callable_ = other.callable_;
            other.callable_ = nullptr;
        }
        return *this;
    }

    // Destructor: deallocate the stored callable.
    ~gc_aware_function() {
        if (callable_)
            callable_->destroy();
    }

    // Invoke the stored callable.
    R operator()(Args... args) const {
        THROW_UNLESS(callable_);
        return callable_->invoke(args...);
    }
private:
    // The type-erasure base class.
    struct CallableBase {
        virtual R invoke(Args... args) const = 0;
        virtual CallableBase* clone() const = 0;
        virtual void destroy() = 0;
        virtual ~CallableBase() {}
    };

    // A template derived class that holds the actual callable.
    template<typename F>
    struct Callable : CallableBase {
        F f_;
        explicit Callable(F f) : f_(std::move(f)) {}

        virtual R invoke(Args... args) const override {
            return f_(args...);
        }
        virtual CallableBase* clone() const override {
            void *wrapped = il2cpp_functions::gc_alloc_fixed(sizeof(Callable<F>));
            return new (wrapped) Callable<F>(f_);
        }
        virtual void destroy() override {
            // Manually call the destructor then free the memory.
            this->~Callable();
            il2cpp_functions::gc_free_fixed(this);
        }
    };

    CallableBase* callable_;
};

template <typename F>
gc_aware_function(F) -> gc_aware_function<typename function_traits<std::decay_t<F>>::signature>;