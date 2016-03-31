#ifndef __MAP_SEARCH_H__
#define __MAP_SEARCH_H__

#include <map>

namespace mastermind {

    template <class Key, class T>
    class mapSearchValue {
    public:
        mapSearchValue(const T _value) noexcept;

        bool isSearchedValue(const std::pair<const Key, const T> pairKeyValue) const noexcept;

    private:
        T value;
    };
}

#endif