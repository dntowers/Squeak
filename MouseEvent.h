#pragma once

using namespace System::Collections::Generic;


ref class MouseEvent : public System::Object
{
public:
	MouseEvent(void);
	MouseEvent(double dNewTime, bool bFeedState, int iNewArm);

	//int getArm(void);
	//bool getFed(void);
	
	double getTimestamp(void){ return dTimestamp;}

	void setAll(double dNewTimestamp, bool bNewFed, int iNewArm)
	{
		dTimestamp = dNewTimestamp; bFed = bNewFed; iArm = iNewArm;
	}

	void getAll(double* p_dTimeStamp, bool* p_bFed, int* p_iArm)
	{
		*p_dTimeStamp = dTimestamp;
		*p_bFed = bFed;
		*p_iArm = iArm;
	}

	//void setFed(bool bNewFed);
	//void setTimestamp(double dNewTimestamp);

	// ---- override
public:
      virtual System::String^ ToString() override 
      {
		  return System::String::Format("{0},{1},{2}\n", dTimestamp, iArm, bFed);
      }

private:
	double dTimestamp;  // time event occurred
	int iArm;			// arm of maze (1 if not in arm)
	bool bFed;			// true of fed (feeding?)

};


ref class MouseEvents
{
public:
	MouseEvents(void);
	MouseEvents(System::String^ newSequenceName);
	MouseEvents(System::String^ newSequenceName, double dInitialTime, bool bInitialFed, int iInitialArm);

	// ----------------------- information
	// number of events
	int get_NumberEvents(void){return mouseEvents->Count;}
	// saved after event addition
	bool get_IsDirty(void){ return IsDirty;}

	// event times
	bool getFirstLastTimes(double* p_dFirstTime, double* p_dLastTime);
	bool getFirstTime(double* p_dFirstTime);
	bool getLastTime(double* p_dLastTime);

	// all info
	System::Void getEventData(MouseEvent^ me, double* p_dTimeStamp, bool* p_bFed, int* p_iArm)
		{me->getAll(p_dTimeStamp, p_bFed, p_iArm);}
	
	//bool getEventDataAtIndex(int iIndex, double* p_dTimeStamp, bool* p_bFed, int* p_iArm);
	
	// get all data, add to arrays
	bool getAllEventData(array<double>^ arr_dTimeStamp, array<bool>^ arr_bFed, array<int>^ arr_iArm);

	// -----------------------save, load
	// save the current sequence
	bool Save_Sequence(System::String^ strMovieFileName);

	// ----------------------- change event sequence
	MouseEvent^ addEvent(double dNewTime, bool bNewFed, int iNewArm);

private:
	// name of current sequence
	System::String^ sequenceName;
	// flag indicate event change since save
	bool IsDirty;

	// list of mouse events
	LinkedList<MouseEvent^>^ mouseEvents;

	// handles to specific events
	MouseEvent^ eventRecentAdd; // most recently added event

	// ----- methods ------
	// internal information
	double getEventTime(MouseEvent^ mEvent){ return mEvent->getTimestamp();}

	// compate time to events: -1 for before, 1 for after, 0 for same time
	int compareTimeToEvent(double dCompareTime, MouseEvent^ mEvent){ return System::Math::Sign(dCompareTime - getEventTime(mEvent));}

	// sets the values of a given event
	bool setEvent(MouseEvent^ mEvent, double dNewTime, bool bNewFed, int iNewArm);

	// save & load
	bool _SaveSequenceData(System::String^ seqFilePath, System::String^ movieFileURL);
};
