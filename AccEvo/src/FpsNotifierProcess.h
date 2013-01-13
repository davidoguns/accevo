//David Oguns
//January 12, 2013


#pragma once
#include "aetypes.h"
#include "Process.h"
#include <list>
#include <map>

namespace Accevo
{

	/************************************************************
	This process, when started starts recording FPS at specified
	intervals of time.

	And interval of time means that the number of frames will be
	counted at the entire interval and then the process notified.
	
	It is on the pre-update method that this class will

	*************************************************************/
	 
	class FpsNotifierProcess : public Process
	{
	public:
		FpsNotifierProcess();
		~FpsNotifierProcess();

		//called before the main context updates
		virtual void PreUpdate(AFLOAT32 dt);
		//called after the main context updates
		virtual void PostUpdate(AFLOAT32 dt);

		//called when process is first made active
		virtual void Start(PID const & pid);
		//called to stop the process from running
		virtual void Stop();

		//returns true if process is done running.  this is how
		//processes should kill themselves.  Stop will still
		//be called on the process at the end of the frame.
		virtual bool IsFinished() const;

		/*****************************************
		Interface class that sub-classes inherit to recieve
		Fps stat data.
		*****************************************/
		class FpsListener
		{
		public:
			//single method listeners must care about
			virtual void NotifyFps(AFLOAT32 fps) = 0;
		};

		//specific methods on this process

		//Registers an FPS listener to be notified of what the FPS has been at a
		//given interval, for that interval
		void RegisterNotifications(AFLOAT32 intervalInSeconds, FpsListener *pListener);

		
		void UnregisterNotifications(AFLOAT32 intervalInSeconds, FpsListener *pListener);
	private:
		/*************************************************************
		Structure used to track a particular timeslot interval.
		*************************************************************/
		struct FpsIntervalSlot {
			//amount of time since last notify for this slot
			AFLOAT32							elapsedTimeInSeconds;
			//number of frames since last notify for this slot
			AUINT32								elapsedFrames;
			//using a list in a "dumb" fashion means the same listener can register twice
			//and we won't care.
			std::list<FpsListener *>			listeners;
		};
		//internal representation for interval is only millisecond accurate.
		//not comfortable using floats for map keys
		typedef std::map<AUINT32, FpsIntervalSlot*>		INTERVAL_SLOTS;
		typedef INTERVAL_SLOTS::iterator				INTERVAL_SLOTS_ITR;

		//map of all interval slots we care about
		INTERVAL_SLOTS							m_intervalSlots;
		ABOOL									m_isFinished;
	};

}
