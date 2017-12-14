//David Oguns
//February 1, 2011

//since _HANDLE_H_ might be taken
#ifndef _AEHANDLE_H_
#define _AEHANDLE_H_

namespace Accevo
{
	

	/*********************************************************
		This class can be subclassed to ensure type safety 
		between different types of handles.  The decision to 
		not use boost::noncopyable is because reference counting 
		in HandleManager can check whether or not an unregistered
		(freed) handle is still being used even if the handle 
		entry hasn't been overwritten. by another item that	uses 
		the entry.

		This allows handles to be used in boost::optional<>
		because items in boost::optional<> must be copy 
		constructable.

		Note: I chose not to us templates to define Handle with 
			a TAG type to enforce type safety to reduce code size.
			No point in having seperate instances of this code
			when the TAG type doesn't differenciate it.

			If subclassed for type-safety, then the sub-class 
			should retain the size  of 32 or 64 bits to keep it
			compact.
**********************************************************/
	class Handle
	{
	public:
		enum
		{
			//the following two value better add up to 32 
			//(or 64 - if decided to use 64 bit handles
			AEHANDLE_BITS_INDEX = 20,
			AEHANDLE_BITS_MAGIC = 12,
			AEHANDLE_MAX_INDEX = (1 << AEHANDLE_BITS_INDEX) - 1,
			AEHANDLE_MAX_MAGIC = (1 << AEHANDLE_BITS_MAGIC) - 1,			
		};

		Handle() :
			m_handle(0) {}
		//Note the plus one on index for this contructor version.  If the user
		//calls this version of Handle(), it is not possible to create a null
		//handle unless the caller hacks their way around and uses the max
		//value of an unsigned int is to cause the plus 1 to wrap around to 0
		//null handles can/should only be created using the Handle(unsigned int)
		//constructor		
			Handle(unsigned int index, unsigned int magic) :
			m_index(index+1), m_magic(magic) {}
		explicit Handle(unsigned int handle) : 
			m_handle(handle) { }

		~Handle() {};

		unsigned int GetHandle() const { return m_handle; }
		//There's a subtraction by 1 here because 0 is the null handle.
		//The underlying index to whoever manages the handles will still
		//start at zero.  It is imperative that checks for null handles
		//be done on the handle object itself rather than getting the index
		//and checking if it is equal to zero.  Given the return type of 
		//unsigned int for this function, a null handle will return a 
		//positive index that is likely out of bounds
		unsigned int GetIndex() const { return m_index-1; }
		unsigned int GetMagic() const { return m_magic; }
		bool IsNull() const { return !m_index; }

		//operator bool() { return IsNull(); }
		//operator unsigned int() { return GetHandle(); }

		bool operator==(Handle const & rhs) {
			return m_handle == rhs.m_handle;
		}

	private:
		union
		{
			struct
			{
				unsigned int m_index : AEHANDLE_BITS_INDEX;
				unsigned int m_magic : AEHANDLE_BITS_MAGIC;
			};
			unsigned int m_handle;
		};
	};
}

#endif
