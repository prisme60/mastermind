#ifndef __MAP_SEARCH_H__
#define __MAP_SEARCH_H__

#include <map>

namespace mastermind {
	template <class Key, class T>
	class mapSearchValue
	{
		public:
		mapSearchValue(const T _value);
		
		bool isSearchedValue (const std::pair<const Key,const T> pairKeyValue) const;
		
		private:
		T value;
	};
}

#endif