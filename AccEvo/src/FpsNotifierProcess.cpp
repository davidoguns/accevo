//David Oguns
//January 12, 2013

#include "FpsNotifierProcess.h"
#include <algorithm>

namespace Accevo
{
	using std::map;
	using std::for_each;
	using std::pair;
	using std::remove_if;
	
	FpsNotifierProcess::FpsNotifierProcess() :
		m_isFinished(false)
	{ }


	FpsNotifierProcess::~FpsNotifierProcess()
	{
		for_each(m_intervalSlots.begin(), m_intervalSlots.end(),
			[](std::pair<AUINT32, FpsIntervalSlot*> const & slot) -> void
			{
				delete slot.second;		//free memory
			}
		);
	}

	//called before the main context updates
	void FpsNotifierProcess::PreUpdate(AFLOAT32 dt)
	{
		for_each(m_intervalSlots.begin(), m_intervalSlots.end(),
			[&](pair<AUINT32, FpsIntervalSlot*> const &slotPair) -> void
			{
				AFLOAT32 cap = static_cast<AFLOAT32>(slotPair.first) / 1000.0f;
				FpsIntervalSlot *pSlot = slotPair.second;
				++pSlot->elapsedFrames;	//increment frame counter
				pSlot->elapsedTimeInSeconds += dt;
				//if we have exceeded the time to notify for this slot
				if(pSlot->elapsedTimeInSeconds > cap)
				{
					//calculate FPS
					AFLOAT32 fps = static_cast<AFLOAT32>(pSlot->elapsedFrames) / pSlot->elapsedTimeInSeconds;
					
					//notify all listeners
					for_each(pSlot->listeners.begin(), pSlot->listeners.end(),
						[&fps](FpsListener *listener) -> void
						{
							listener->NotifyFps(fps);
						}
					);
					pSlot->elapsedTimeInSeconds = 0.0f;
					pSlot->elapsedFrames = 0;
				}
			}
		);
	}
	
	void FpsNotifierProcess::PostUpdate(AFLOAT32 dt) { /* do nothing */ }

	//called when process is first made active
	void FpsNotifierProcess::Start(PID const & pid)
	{
		//nothing to do here..except maybe store PID if we care
	}

	//called to stop the process from running
	void FpsNotifierProcess::Stop()
	{
		m_isFinished = true;
	}

	//returns true if process is done running.  this is how
	//processes should kill themselves.  Stop will still
	//be called on the process at the end of the frame.
	ABOOL FpsNotifierProcess::IsFinished() const
	{
		return m_isFinished;	//this process never finishes, only dies
	}
		
	//Registers an FPS listener to be notified of what the FPS has been at a
	//given interval, for that interval
	void FpsNotifierProcess::RegisterNotifications(AFLOAT32 intervalInSeconds, FpsListener *pListener)
	{
		AUINT32 intervalMillis = static_cast<AUINT32>(intervalInSeconds * 1000.0f);
		INTERVAL_SLOTS_ITR itr = m_intervalSlots.find(intervalMillis);
		FpsIntervalSlot *pSlot = nullptr;
		if(itr == m_intervalSlots.end())
		{	//no interval slot isn't in map at all, add one
			pSlot = new FpsIntervalSlot();
			pSlot->elapsedTimeInSeconds = 0.0f;
			pSlot->elapsedFrames = 0;
			//put slot in map
			m_intervalSlots[intervalMillis] = pSlot;
		}
		else {
			pSlot = itr->second;
		}
		pSlot->listeners.push_back(pListener);
	}

	void FpsNotifierProcess::UnregisterNotifications(AFLOAT32 intervalInSeconds, FpsListener *pListener)
	{
		AUINT32 intervalMillis = static_cast<AUINT32>(intervalInSeconds * 1000.0f);		INTERVAL_SLOTS_ITR itr = m_intervalSlots.find(intervalMillis);		
		if(itr != m_intervalSlots.end())
		{
			FpsIntervalSlot *pSlot = itr->second;
			remove_if(pSlot->listeners.begin(), pSlot->listeners.end(),
				[&](FpsListener * const &listener) -> bool	{ return pListener == listener;	}
			);
			//if it's empty, remove the slot entirely
			if(pSlot->listeners.empty())
			{
				m_intervalSlots.erase(itr);
			}
		}
		//nothing to do otherwise
	}

}