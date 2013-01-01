//David Oguns
//January 20, 2011
//CLock.h

#ifndef _GAME_CLOCK_H_
#define _GAME_CLOCK_H_

#include <windows.h>	//for timing functions (probably can be more specific)
#include <list>
#include <algorithm>
#include "aetypes.h"

namespace Accevo
{
	class Clock
	{
	public:	//TODO: change to private and make friend of EngineKernel?
		static void InitClock()
		{
			AINT64 clockFreq;
			QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&clockFreq));
			//QueryPerformanceFrequency((LARGE_INTEGER*)&s_clockFrequency);
			s_clockFrequency = static_cast<AFLOAT32>(clockFreq);
		}
		
		//default constructor, times start at zero
		explicit Clock(AFLOAT32 startTimeSeconds = 0.0f) :
			m_timeCycles(SecondsToCycles(startTimeSeconds)),
			m_timeScale(1.f),
			m_isPaused(false)
		{};

		void AddChild(Clock *c)
		{
			m_ChildClockList.push_back(c);
		}
		void RemoveChild(Clock *c)
		{
			m_ChildClockList.remove(c);
		}

		//returns the time in cycles that of the state of this clock
		unsigned __int64 GetTimeCycles() const { return m_timeCycles; }

		//returns the time difference between this clock and another clock
		float GetTimeDeltaSeconds(Clock const &other)
		{
			return CyclesToSeconds(m_timeCycles-other.m_timeCycles);
		}

		//returns the time difference between this clock and another clock
		float GetTimeDeltaSeconds(unsigned __int64 cycles)
		{
			return CyclesToSeconds(m_timeCycles-cycles);
		}

		//only update clock if it isn't paused for some reason
		void Update(AFLOAT32 deltaSeconds)
		{
			if(!m_isPaused)
			{
				 AINT64 delta = SecondsToCycles(deltaSeconds * m_timeScale);
				 m_timeCycles += delta;
				 std::for_each(m_ChildClockList.begin(), m_ChildClockList.end(),
					 [delta](Clock *c) -> void { c->Update(delta); });
			}
		}

		//only update clock if it isn't paused for some reason
		void Update(AINT64 cycles)
		{
			if(!m_isPaused)
			{
				//following: could be done with 1 cast, but would lose precision
				//if the multiply was done as int before
				AINT64 delta = static_cast<AINT64>(static_cast<AFLOAT32>(cycles)*m_timeScale);
				m_timeCycles += delta;
				std::for_each(m_ChildClockList.begin(), m_ChildClockList.end(),
					[delta](Clock *c) -> void { c->Update(delta); });
			}
		}

		void SetPaused(ABOOL isPaused) { m_isPaused = isPaused; }
		ABOOL IsPaused() const { return m_isPaused; }
		void SetScale(AFLOAT32 scale) { m_timeScale = scale; }
		AFLOAT32 GetScale() const { return m_timeScale; }

	public:
		//converts a given duration in float-seconds to cycles
		static inline AINT64 SecondsToCycles(AFLOAT32 timeSeconds)
		{
			return static_cast<AINT64>(timeSeconds * s_clockFrequency);
		}

		//converts time durations from cycles to float-seconds
		//should only be used for small durations
		static inline AFLOAT32 CyclesToSeconds(AINT64 cycles)
		{
			AFLOAT32 fCycles = static_cast<AFLOAT32>(cycles);
			return fCycles / s_clockFrequency;
		}

	protected:
		//time measured in cycles that have ellapsed from this clock
		AINT64					m_timeCycles;
		//scaling factor this clock has compared to real time
		AFLOAT32				m_timeScale;
		//flag indicating time updates being paused
		ABOOL					m_isPaused;
		std::list<Clock *>		m_ChildClockList;

	protected:
		//this should be stored as a float to prevent the cast
		//from __int64 to float everytime we perform division in
		//cycles-to-seconds function
		static AFLOAT32				s_clockFrequency;

	public:
		//allow consumers of clock to access this data
		static AFLOAT32				GetClockFrequency() { return s_clockFrequency; }
	};
}

#endif