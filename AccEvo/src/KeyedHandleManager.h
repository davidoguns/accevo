//David Oguns
//February 1, 2011
//KeyedHandleManager.h

#ifndef _KEYED_HANDLE_MANAGER_H_
#define _KEYED_HANDLE_MANAGER_H_

#include "GlobalInclude.h"
#include "HandleManager.h"

/********************************************************************
	Handle manager must be subclassed by implementations that require 
	it's functionality.  If contained, then the basic interface behavior
	of creating handles and 

	If a handle is requested that isn't found, the 
*********************************************************************/

namespace Accevo
{
	template<class DataType, class KeyType, class Compare = std::less<KeyType>>
	class KeyedHandleManager : public HandleManager<DataType>
	{
	private:
		typedef Map<KeyType, Handle, Compare>		HANDLEMAP;

	public:
		KeyedHandleManager(Logger *pLogger);
		virtual ~KeyedHandleManager();

	public:
		//creates a handle mapping - will clobber whatever was in the entry
		//previously without warning.  User must check if something is there
		//by calling Acquire() first and checking if returned handled is
		//null
		Handle CreateEntry(KeyType const & key);

		//creates a handle mapping with a key value.  The difference betwee
		//this version and CreateEntry(KeyType const & key) is that this
		//one automatically releases the initially created handle since
		//it can be retrieved by key later.
		void CreateEntryR(KeyType const & key);

		//removes a handle entry mapping based on a key
		void RemoveEntry(KeyType const & key);

		//Handle doesn't have to be optional here because
		//we will trap it, and use the NULL handle instead
		Handle GetHandle(KeyType const & key);
		
		//Lowers the reference count for a handle.  Not entirely
		//necessary to free up resources, but clients should to
		//enable the owner to free up resources the moment it the
		//data isn't needed
		void ReleaseHandle(Handle const & handle);

		//Retrieves data reference given key.  Runs in O(log N) time
		//This method shouldn't be used if a handle can be kept around
		//to access the data reference in O(1) time.
		DataType & GetData(KeyType const & key);

		//Retrieves the data given the key.  This method runs in
		//O(log N) time as well.  If the key doesn't map to a handle
		//or the handle is stale
		boost::optional<DataType &> GetDataOptional(KeyType const & key);

	private:
		HANDLEMAP						m_handleMap;
	};

	template<class DataType, class KeyType, class Compare>
	KeyedHandleManager<DataType, KeyType, Compare>::KeyedHandleManager(Logger *pLogger) :
		HandleManager(pLogger)
	{

	}

	template<class DataType, class KeyType, class Compare>
	KeyedHandleManager<DataType, KeyType, Compare>::~KeyedHandleManager()
	{

	}

	//creates a handle mapping - will clobber whatever was in the entry
	//previously without warning.  User must check if something is there
	//by calling Acquire() first and checking if returned handled is
	//null
	template<class DataType, class KeyType, class Compare>
	Handle KeyedHandleManager<DataType, KeyType, Compare>::CreateEntry(KeyType const & key)
	{
		/*******************************************************
			Note - If the underlying handle manger is full this
			will still overwrite the previous entry with a null
			handle.
		*******************************************************/
		Handle newHandle = CreateEntry();
		HANDLEMAP::iterator existingEntry = m_handleMap.find(key);
		if(existingEntry != m_handleMap.end())
		{
			AELOG_WARN(m_pLogger, "Overwriting existing entry");
		}
		//add entry to map
		m_handleMap[key] = newHandle;

		return newHandle;
	}

	template<class DataType, class KeyType, class Compare>
	Handle KeyedHandleManager<DataType, KeyType, Compare>::CreateEntryR(KeyType const & key)
	{
		Handle newHandle = CreateEntry(key);
		//release handle immediately - should make refCount == 0 for handle
		ReleaseHandle(newHandle);
	}

	//Removes/invalidates handle mapping
	template<class DataType, class KeyType, class Compare>
	void KeyedHandleManager<DataType, KeyType, Compare>::RemoveEntry(KeyType const & key)
	{
		HANDLEMAP::iterator handleItr = m_handleMap.find(key);
		if(handleItr != m_handleMap.end())
		{
			Handle handleToRemove = *handleItr;
			RemoveEntry(handleToRemove);
		}
#if(defined(_DEBUG) || defined(DEBUG))
		else
		{
			AELOG_WARN(m_pLogger, "Removing keyed entry that doesn't exist!!!");
		}
#endif
	}

	//Retrieves data reference given key.  Runs in O(log N) time
	//This method shouldn't be used if a handle can be kept around
	//to access the data reference in O(1) time.
	template<class DataType, class KeyType, class Compare>
	DataType & KeyedHandleManager<DataType, KeyType, Compare>::GetData(KeyType const & key)
	{
		HANDLEMAP::iterator handleItr = m_handleMap.find(key);
		if(handleItr != m_handleMap.end())
		{
			return GetData(*handleItr);
		}
		//return invalid reference that will blow something up if used (probably)
		return DataType();
	}

	//Retrieves the data given the key.  This method runs in
	//O(log N) time as well.  If the key doesn't map to a handle
	//or the handle is stale
	template<class DataType, class KeyType, class Compare>
	boost::optional<DataType &> KeyedHandleManager<DataType, KeyType, Compare>::GetDataOptional(KeyType const & key)
	{
		HANDLEMAP::iterator handleItr = m_handleMap.find(key);
		if(handleItr != m_handleMap.end())
		{
			return GetDataOptional(*handleItr);
		}
		//return invalid reference that will blow something up if used (probably)
		return DataType();
	}

	//Handle doesn't have to be optional here because
	//we will trap it, and use the NULL handle instead
	template<class DataType, class KeyType, class Compare>
	Handle KeyedHandleManager<DataType, KeyType, Compare>::GetHandle(KeyType const & key)
	{
		HANDLEMAP::iterator handleItr = m_handleMap.find(key);
		if(handleItr != m_handleMap.end())
		{
			return *handleItr;
		}
		return Handle(0);
	}

	//lowers the reference count for a handle.  Not entirely
	//necessary to free up resources, but clients should to
	//enable the owner to free up resources the moment it the
	//data isn't needed.  Pass by reference is used here to
	//make the passed in handle NULL
	template<class DataType, class KeyType, class Compare>
	void KeyedHandleManager<DataType, KeyType, Compare>::ReleaseHandle(Handle const & handle)
	{
		//check handle for null
		if(!handle.IsNull())
		{	//get index
			unsigned int entryIndex = handle.GetIndex();
			//verify index is in range
			if(entryIndex < m_entries.size())
			{	//get entry
				HandleEntry *pEntry = m_entries[entryIndex];
				if(pEntry->magicNum == handle.GetMagic())
				{
					if(pEntry->refCount == 0)
					{
						AELOG_WARN(m_pLogger, "Releasing handle when ref count is already = 0!!!");
						pEntry->refCount++;	//increment so later decrement doesn't wrap around to positive number
					}
					pEntry->refCount--;
				}
				else
				{
					AELOG_WARN(m_pLogger, "Not releasing handle due to magic number mismatch!!!");
				}
			}
			else
			{
				AELOG_WARN(m_pLogger, "Not releasing handle due to entry index out of bounds!!!");
			}
		}
		else
		{
			AELOG_WARN(m_pLogger, "Cannot release null handle!!!");
		}
	}

	/*
	*/

}



#endif