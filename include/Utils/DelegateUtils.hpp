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