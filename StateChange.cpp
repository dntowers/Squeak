#include "StdAfx.h"
#include "MouseLL.h"
#include "StateChange.h"

StateChange::StateChange(void)
{
	_InitRequired();
	_DefaultAll();
}


#pragma region Initialization

// initialize required values, e.g. arrays
System::Void StateChange::_InitRequired(void)
{
	// create arrays
	scEvents = gcnew array<scEvent^>(SC_INDEX_MAX);
	scTimes = gcnew  array<double>(SC_INDEX_MAX);
	// init arrays
	for(int iIndex = 0; iIndex < SC_INDEX_MAX; iIndex++)
	{
		scEvents[iIndex] = gcnew scEvent();
		scTimes[iIndex] = -1;
	}
	return System::Void();
}

// set all defaults
System::Void StateChange::_DefaultAll(void)
{
	directionX= SC_DIR::SCD_NONE;
	stateX =	SC_STATE::SCS_NONE;
	seqX =		SC_SEQ::SCQ_NONE;
	eventX =	SC_EVENT::SCE_NONE;
	reasonX =	SC_REASON::SCR_NONE;

	return System::Void();
}

#pragma endregion

#pragma region Initialization
	// set event
	System::Void StateChange::set_scEvent(int index, EV_TYPE new_x_type, MouseLLEvent^ new_x_event, double new_x_time)
	{
		scEvents[index]->x_type = new_x_type;
		scEvents[index]->x_event = new_x_event;
		scEvents[index]->x_time = new_x_time;
		scEvents[index]->x_EventID = new_x_event->getEventID();
	}
#pragma endregion


//public enum class SC_DIR {
//	SCD_FORWARD,				// move forward in moviw
//	SCD_BACK,					// move back in movie
//	SCD_NONE					// no change
//};
//
//public enum class SC_STATE {
//	SCS_PLAY_REC,				// playing & recording
//	SCS_PLAY_NOREC,				// playing & no recording
//	SCS_NOPLAY_REC,				// no playing & recording
//	SCS_NOPLAY_NOREC,			// no playing & no recording
//	SCS_NONE,			// no playing & no recording
//};
//
//public enum class SC_SEQ {
//	SCQ_ADDED,					// added an event
//	SCQ_REMOVED,				// removed an event
//	SCQ_CHANGED,				// changed an event
//	SCQ_NONE,					// no event change
//};
//
//public enum class SC_EVENT {
//	SCE_MOVED_AFTER,			// just moved past an event (should be given)
//	SCE_MOVED_BEFORE,			// just moved before an event (should be given)
//	SCE_PAST_LAST,				// moved past last existing event
//	SCE_BEFORE_FIRST,			// moved before first event
//	SCE_NONE,					// no event move change
//};
//
//public enum class SC_REASON {
//	SCR_PLAYING,				// changed because of playing movie
//	SCR_SCROLLING,				// changed because of scrolling movie
//	SCR_STEP,					// changed because of stepping (button)
//	SCR_GRID,					// changed because of selecting on grid
//	SCR_NONE,					// none (should not happen??)
//};
