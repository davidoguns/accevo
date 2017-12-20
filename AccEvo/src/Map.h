//David Oguns
//February 8, 2011

#ifndef _AEMAP_H_
#define _AEMAP_H_

#include <map>
#include <boost\optional.hpp>

namespace Accevo
{
	template<class Key, class T, class KeyCompare = std::less<Key>>
	class Map
	{
	private:
		typedef	std::map<Key, T, KeyCompare>		MAP;

	public:
		//Gets the value with a specified key.  If no value
		//exist, the behavior of this function is undefined.
		T Get(Key const & key) const
		{
			return m_map.find(key)->second;
		}

		//Gets the value with a specified key. If no value
		//exiss, returns the passed in default value
		T Get(Key const & key, T const & defValue) const
		{
			auto itemItr = m_map.find(key)->second;
			if(itemItr != m_map.end())
			{
				return itemItr->second;
			}
			return defValue;
		}

		//Gets the value with a specified key.  If no value 
		//exists it will return an undefined optional value
		//that can be checked by the caller.
		boost::optional<T> GetOptional(Key const & key) const
		{
			boost::optional<T> ret;
			auto itemItr = m_map.find(key);
			if(itemItr != m_map.end())
				ret = itemItr->second;
			return ret;
		}

		//Adds a new mapping <key, value> pair
		void Insert(Key const & key, T const & item)
		{
			m_map[key] = item;
		}

		//erases the mapping with a given key
		//performs in O(log N) time
		void Remove(Key const & key)
		{
			m_map.erase(key);
		}

		//iterator version of removing a mapping
		//performes in amortized constant time
		void Remove(typename MAP::const_iterator const & citr)
		{
			m_map.erase(citr);
		}

		void Clear()
		{
			m_map.clear();
		}

		//Gets the underlying map so the user can iterate over
		//the underlying elements
		MAP const & GetMap() const { return m_map; }

	protected:
		MAP										m_map;
	};
}

#endif