#include "mapsearchcondition.h"

namespace mastermind {

    template <class Key, class T>
    mapSearchValue<Key, T>::mapSearchValue(const T _value) noexcept : value(_value) {
    }

    template <class Key, class T>
    bool mapSearchValue<Key, T>::isSearchedValue(const std::pair<const Key, const T> pairKeyValue) const noexcept {//no need to check the content, only need to check if it is the same object in memory by comparing the pointers
        return (&pairKeyValue.second == &value);
    }
}
