#pragma once
#include <beatsaber-hook/shared/utils/typedefs.h>

namespace QuestUI::ArrayUtil {

    template <class T>
    inline T First(ArrayW<T> array)
    {
        if(array.Length() > 0)
            return array[0];
        return nullptr;
    }

    template <class T, class Predicate>
    inline T First(ArrayW<T> array, Predicate pred)
    {
        for (T item : array) {
            if (pred(item)){
                return item;
            }
        }
        return nullptr;
    }

    template <class T>
    inline T Last(ArrayW<T> array)
    {
        if(array.Length() > 0)
            return array[array.Length() - 1];
        return nullptr;
    }

    template <class T, class Predicate>
    inline T Last(ArrayW<T> array, Predicate pred)
    {
        for (int i = array.Length()-1; i >= 0; i--) {
            T item = array[i];
            if (pred(item)){
                return item;
            }
        }
        return nullptr;
    }

    template <class T, class Predicate>
    inline ArrayW<T> Where(ArrayW<T> array, Predicate pred)
    {
        List<T>* newArray = List<T>::New_ctor();
        for (T item : array) {
            if(pred(item))
                newArray->Add(item);
        }
        return newArray->ToArray();
    }
    
    template <class Out, class T, class Predicate>
    inline ArrayW<Out> Select(ArrayW<T> array, Predicate pred)
    {
        ArrayW<Out> newArray(array.Length()); 
        for (int i = 0; i < array.Length(); i++) { 
            newArray[i] = pred(array[i]); 
        } 
        return newArray;
    }

    template <class T, class Predicate>
    inline bool Any(ArrayW<T> array, Predicate pred)
    {
        for (T item : array) {
            if(pred(item))
                return true;
        }
        return false;
    }

    template <class Out, class T, class Predicate>
    inline Out Min(ArrayW<T> array, Predicate pred)
    {
        bool first = true;
        Out min;
        for (T item : array) {
            auto value = pred(item);
            if(first || value < min){
                first = false;
                min = value;
            }
        }
        return min;
    }

    template <class Out, class T, class Predicate>
    inline Out Max(ArrayW<T> array, Predicate pred)
    {
        bool first = true;
        Out max;
        for (T item : array) {
            auto value = pred(item);
            if(first || value > max){
                first = false;
                max = value;
            }
        }
        return max;
    }

}