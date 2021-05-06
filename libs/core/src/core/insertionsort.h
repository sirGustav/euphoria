#pragma once

#include <vector>
#include <cstddef>

namespace euphoria::core
{
    template <typename T, typename SortFunc>
    void
    insertion_sort(std::vector<T>* arr, SortFunc sort_func)
    {
        auto&      A      = *arr;
        const auto length = A.size();

        size_t i = 1;
        while(i < length)
        {
            size_t j = i;
            while(j > 0 && sort_func(A[j - 1], A[j]) > 0)
            {
                std::swap(A[j], A[j - 1]);
                j = j - 1;
            }
            i = i + 1;
        }
    }

    template <typename T, typename SortFunc>
    std::vector<T>
    insertion_sort(const std::vector<T>& arr, SortFunc sort_func)
    {
        auto copy = arr;
        insertion_sort(&copy, sort_func);
        return copy;
    }

    template <typename T>
    int
    default_insertion_sort(const T& lhs, const T& rhs)
    {
        if(lhs == rhs)
        {
            return 0;
        }
        return lhs < rhs ? -1 : 1;
    }

    template <typename T>
    std::vector<T>
    insertion_sort(const std::vector<T>& arr)
    {
        return insertion_sort(arr, default_insertion_sort<T>);
    }

}

