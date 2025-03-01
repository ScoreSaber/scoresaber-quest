#pragma once

#include <System/Threading/Tasks/Task_1.hpp>
#include <System/Action.hpp>

#include <custom-types/shared/delegate.hpp>

// Credits to https://github.com/kodenamekrak/Cinema/blob/7cab615b4a0fa3e35c6ee3076334d25823718bd4/include/Util/DelegateHelper.hpp


namespace DelegateHelper {

    template <typename T>
    void ContinueWith(System::Threading::Tasks::Task_1<T>* task, const std::function<void(T)>& callback)
    {
        auto delegate = custom_types::MakeDelegate<System::Action*>(std::function([callback, task]()
                {
                    callback(task->get_Result());
                }));
        task->GetAwaiter().OnCompleted(delegate);
    }
}

// Credits to https://github.com/Royston1999

namespace DelegateUtils
{
    template <typename T>
    struct delegate_func;

    template <typename TRet, typename T, typename... TArgs>
    struct delegate_func<TRet(T::*)(TArgs...)>
    {
        using FuncType = TRet(TArgs...);
    };

    template<typename T>
    using delegate_func_t = typename delegate_func<decltype(&T::Invoke)>::FuncType;

    template <typename O, typename T>
    struct delegate_mem_func;

    template <typename O, typename TRet, typename T, typename... TArgs>
    struct delegate_mem_func<O, TRet(T::*)(TArgs...)>
    {
        using MemFuncPtr = TRet(O::*)(TArgs...);
    };

    template<typename O, typename T>
    using delegate_mem_func_t = typename delegate_mem_func<O, decltype(&T::Invoke)>::MemFuncPtr;

    template <typename O, typename TRet, typename... TArgs>
    const auto wrap_member_function(TRet(O::*fp)(TArgs...), O* obj)
    {
        std::function<TRet(TArgs...)> wrapper = [fp, obj](TArgs... args) { return (obj->*fp)(args...); };
        return wrapper;
    }

    template<typename T>
    struct DelegateW
    {
        static_assert(std::is_convertible_v<T*, System::MulticastDelegate*>, "T must be a delegate type");
        DelegateW() {}
        DelegateW(std::function<delegate_func_t<T>> function)
        {
            this->internalCSharpDelegate = custom_types::MakeDelegate<T*>(function);
        }
        DelegateW(const DelegateW<T>& other) : internalCSharpDelegate(other.internalCSharpDelegate) {}

        template<typename O>
        DelegateW(delegate_mem_func_t<O,T> function, O* object)
        {
            this->internalCSharpDelegate = custom_types::MakeDelegate<T*>(wrap_member_function(function, object));
        }

        DelegateW<T>& operator=(DelegateW<T>&& other)
        {
            this->internalCSharpDelegate = other.internalCSharpDelegate ? other.internalCSharpDelegate.ptr() : nullptr;
            return *this;
        }
        DelegateW<T>& operator=(std::function<delegate_func_t<T>> function)
        {
            this->internalCSharpDelegate = custom_types::MakeDelegate<T*>(function);
            return *this;
        }
        constexpr SafePtr<T>* operator->() noexcept { return &internalCSharpDelegate; }
        constexpr SafePtr<T> const* operator->() const noexcept { return &internalCSharpDelegate; }
        constexpr operator bool() const noexcept { return internalCSharpDelegate; }

        private:
        SafePtr<T> internalCSharpDelegate;
    };

    template<typename T>
    requires (std::is_convertible_v<T*, System::MulticastDelegate*>)
    void operator+=(T*& csDelegate, DelegateW<T> delegateW) noexcept
    {
        if (!delegateW) return;
        T* newDelegate = reinterpret_cast<T*>(System::Delegate::Combine(csDelegate, delegateW->ptr()));
        csDelegate = newDelegate;
    }

    template<typename T>
    requires (std::is_convertible_v<T*, System::MulticastDelegate*>)
    void operator-=(T*& csDelegate, DelegateW<T>& delegateW)
    {
        if (!delegateW) return;
        T* newDelegate = reinterpret_cast<T*>(System::Delegate::Remove(csDelegate, delegateW->ptr()));
        csDelegate = newDelegate;
    }
}