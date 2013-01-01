//David Oguns
//March 9, 2011
//AnonHandleManager.h

#ifndef _HANDLE_MANAGER_H_
#define _HANDLE_MANAGER_H_

#include "GlobalInclude.h"
#include "Handle.h"
#include "Logger.h"
#include <vector>
#include <list>
#include <boost\optional.hpp>
#include <boost\format.hpp>
#include <boost\noncopyable.hpp>

/*********************************************************************
	Handle manager must be subclassed by implementations that require 
	it's functionality.  If contained, then the basic interface behavior
	of creating handles and 

	If a handle is requested that isn't found, the 
**********************************************************************/
namespace Accevo
{
	template<class DataType>
	class HandleManager : public boost::noncopyable
	{
	public:
		//ideally data should not be something heavy weight, but
		//rather be a pointer to something that is.  The expcted
		//size of a typical handle entry is 16 bytes if data is
		//a pointer type on 32 bit machines and 20 bytes on 64 bit.
		
		struct HandleEntry
		{	
			//Used to verify handle dereferencing
			unsigned int		magicNum;
			//Data associated with handle (obv)
			DataType			data;
		};
		
		typedef std::list<HandleEntry *>				ENTRYLIST;
		typedef std::vector<HandleEntry *>				ENTRYVECTOR;
		typedef std::list<unsigned int>					FREEENTRYINDEXLIST;

	public:
		explicit HandleManager(Logger *pLogger);
		virtual ~HandleManager();

		//creates a handle mapping - will clobber whatever was in the entry
		//previously without warning.  User must check if something is there
		//by calling Acquire() first and checking if returned handled is null
		Handle CreateEntry();

		//Removes/invalidates handle mapping
		void RemoveEntry(Handle const & handle);

		//gets the data from a handle - no checking involved
		DataType & GetData(Handle const & handle) const;
		//gets the data from a handle with checking
		//user will not get data until all checks verify that the
		//handle and access to the entry are valid
		boost::optional<DataType &> GetDataOptional(Handle const & handle) const;

		//returns a list of all of active handle entries
		ENTRYLIST const & getEntries() const;

	private:
		//gets the next magic num however that should be managed
		//this is overridable in case there is need to later
		virtual unsigned int GetNextMagicNum();

		//vector of entries that can be indexed quickly
		//this vector contains both used and unused entries.
		//knowing whether or not an entry is in use is
		//indicated by an index pointing to it in the string
		//registry
		ENTRYVECTOR						m_entries;
		//represents free entries that are within the size of the
		//vector as it is at present.  If this list is empty
		//the handle manager checks to see if it can still make
		//the m_entries vector grow.  m_freeEntries are those
		//entries that were previously occupied but have been
		//released
		FREEENTRYINDEXLIST				m_freeEntryIndices;
		//list of handle entry
		ENTRYLIST						m_entryList;
		//logger to use to report any abnormal conditions
		//worth logging
		Logger							*m_pLogger;
	};

	template<class DataType>
	HandleManager<DataType>::HandleManager(Logger *pLogger) :
		m_pLogger(pLogger)
	{

	}

	template<class DataType>
	HandleManager<DataType>::~HandleManager()
	{
		//free the memory used by the entry list
		std::for_each(m_entryList.begin(), m_entryList.end(),
			[] (HandleEntry * const & entry)
			{
				delete entry;
			}
		);
		m_entryList.clear();
	}

	//creates a handle mapping - will clobber whatever was in the entry
	//previously without warning.  User must check if something is there
	//by calling Acquire() first and checking if returned handled is
	//null
	template<class DataType>
	Handle HandleManager<DataType>::CreateEntry()
	{
		HandleEntry *pEntry = nullptr;
		unsigned int entryIndex = 0;
		//check free entries first
		FREEENTRYINDEXLIST::iterator freeItr = m_freeEntryIndices.begin();
		if(freeItr != m_freeEntryIndices.end())
		{	//pointer to entry to use
			entryIndex = *freeItr;
			pEntry = m_entries[entryIndex];
			//remove from free entries since we are now using it
			m_freeEntryIndices.erase(freeItr);
		}
		else
		{
			if(m_entries.size() < Handle::AEHANDLE_MAX_INDEX)
			{	//nothing free, but entries vector can grow
				pEntry = new HandleEntry();
				//add entry to vector for indexing
				m_entries.push_back(pEntry);
				//get index of newly created entry
				entryIndex = m_entries.size() - 1;
			}
			else
			{	//if we get here, we know free list is empty and the entry
				//vector is at max capacity, the manager is truly full and cant
				//create another entry which can be represented by a unique handle
				//properly.
				AELOG_ERROR(m_pLogger, "Could not create handle.  Manager full!!");
				return Handle(0);
			}
		}

		if(pEntry)
		{
			//add to entry list for memory management
			m_entryList.push_back(pEntry);
			pEntry->magicNum = GetNextMagicNum();
		}

		return Handle(entryIndex, pEntry->magicNum);
	}

	//Removes/invalidates handle mapping
	template<class DataType>
	void HandleManager<DataType>::RemoveEntry(Handle const &handle)
	{
		if(!handle.IsNull())
		{
			unsigned int index = handle.GetIndex();
			HandleEntry *pEntry = m_entries[index];
			//verify the magic number to make sure handle is consistent
			if(pEntry->magicNum == handle.GetMagic())
			{
				AELOG_ERROR(m_pLogger, "Invalid handle.  Cannot remove entry!!!");
				return;
			}
			
			pEntry->magicNum--;		//change magic num to something else
			//add to free list
			m_freeEntryIndices.push_back(index);
			//remove from entry list
			m_entryList.remove(pEntry);
		}
#if(defined(_DEBUG) || defined(DEBUG))
		else
		{
			AELOG_WARN(m_pLogger, "Null handle.  Cannot remove entry!!!");
		}
#endif
	}

	//gets the next magic num however that should be managed
	template<class DataType>
	unsigned int HandleManager<DataType>::GetNextMagicNum()
	{
		//magic number can be zero, unlike (Game Programming Gems 1, Section 1.6)
		//because null checks against index only not magic.
		static unsigned int magicNum = 0;
		unsigned int retMagic = magicNum++;
		if(magicNum > Handle::AEHANDLE_MAX_MAGIC)
		{
			magicNum = 0;
		}
		return retMagic;
	}

	//gets the data from a handle - no checking involved
	template<class DataType>
	DataType & HandleManager<DataType>::GetData(Handle const & handle) const
	{	//forces at least the null handle to  become undefined
		return m_entries[handle.GetIndex()]->data;
	}

	//gets the data from a handle with checking
	//user will not get data until all checks verify that the
	//handle and access to the entry are valid
	template<class DataType>
	boost::optional<DataType &> HandleManager<DataType>::GetDataOptional(Handle const & handle) const
	{
		boost::optional<DataType &> ret;
		unsigned int index = handle.GetIndex();
		//validate index first
		if(!handle.IsNull() && index < m_entries.size())
		{	//get entry using index
			HandleEntry *entry = m_entries[index];
			//validate entry, make sure ref count is non-zero, and magic number matches
			if(entry->magicNum == handle.GetMagic())
			{	//now that everything checks out, we can safely access the data
				ret = entry->data;
			}
		}
		return ret;
	}
	
	//Return all entries  this handle manager is handling.  This allows 
	//a manager built on top of it to iterate over all element they added
	//without keeping a copy
	template<class DataType>
	typename HandleManager<DataType>::ENTRYLIST const & HandleManager<DataType>::getEntries() const
	{
		return m_entryList;
	}
}

#endif