#pragma once

#include "SC_Enums.h"

#define SC_INDEX_MAX 3
// ------------------------------------------------------ EventX
public enum class EV_TYPE {
	EV_FIRST,		// first event
	EV_LAST,		// last event
	EV_MIDDLE,		// between first and last
	EV_NULL,		// null point
};

// used for origin, dest, next events to 
ref class scEvent : public System::Object
{
public:
	scEvent(void)
	{
		x_type = EV_TYPE::EV_NULL;
		x_event = nullptr;
		x_time = -1;		
	};

	MouseLLEvent^	x_event;
	EV_TYPE			x_type;
	double			x_time;
	int				x_EventID;
};


ref class StateChange : public System::Object
{
public:

	StateChange(void);
	StateChange(int new_StateID);

#pragma region Constructors: 
	//StateChange(SC_STATE statex, SC_DIR dirx, SC_REASON reasonx, double tstart, double tend);
#pragma endregion

	static const int SCI_ORIGIN = 0;
	static const int SCI_DEST	= 1;
	static const int SCI_NEXT	= 2;

	// ------------------------------------------------------properties
	int StateID;
	// ---- state values
	SC_DIR	  directionX;	// direction change
	SC_STATE  stateX;		// play / record state
	SC_SEQ    seqX;			// sequence change
	SC_EVENT  eventX;		// event change
	SC_REASON reasonX;		// what caused state change

	// --- other
	// events - origin can be after dest if moving back
	array<scEvent^>^ scEvents;
	// times
	array<double>^ scTimes;
	
#pragma region Initialization
	// --------------------------------------initialization
private:
	// set all defaults
	System::Void _DefaultAll(void);
	// initialize required values, e.g. arrays
	System::Void _InitRequired(void);
#pragma endregion


#pragma region access
	// -------------------------------------- access
public:
	// get pointer to specific event unless out of range
	scEvent^ get_scEvent(int index){
		if( ( (index < 0) || (index >= SC_INDEX_MAX) ) ) 
			return nullptr;
		else 
			return scEvents[index];
	}

#pragma endregion


#pragma region set values
public:
	// ------------------------------------ set values
	System::Void set_scEvent(int index, EV_TYPE new_x_type, MouseLLEvent^ new_x_event, double new_x_time);
#pragma endregion
};

