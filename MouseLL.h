#pragma once


// Linked List mouse event
ref class MouseLLEvent : public System::Object
{
public:
	MouseLLEvent(void);
	MouseLLEvent(double dNewTime, bool bNewFeed, int iNewArm, MouseLLEvent^ prevEvent, MouseLLEvent^ nextEvent, int newEventID);

// -- override
     virtual System::String^ ToString() override 
     {
		  return System::String::Format("{0},{1},{2}\n", dTimestamp, iArm, bFed);
     }

private:
	// event parameters
	double dTimestamp;
	int iArm;
	bool bFed;
	// pointers
	MouseLLEvent^ prevEvent;
	MouseLLEvent^ nextEvent;
	// ID
	int EventID;


public:
	// -----------------------------------------------------access values
	// get next event
	MouseLLEvent^ getNextEvent(void){return nextEvent;};
	// get previous event
	MouseLLEvent^ getPreviousEvent(void){return prevEvent;};

	// access timestamp
	double getTimestamp(void){return dTimestamp;};
	// access all
	void getAll(double* p_dTimestamp, int* p_iArm, bool* p_bFed)
		{*p_dTimestamp = dTimestamp; *p_iArm = iArm; *p_bFed = bFed;};

	// access arm
	int getArm(void){return iArm;};
	// access feeding
	bool getFed(void){return bFed;};

	int getEventID(void){return EventID;}
	// -----------------------------------------------------set values
	// event params
	void setArm(int iNewArm){iArm = iNewArm;}
	void setFed(bool bNewFed){bFed = bNewFed;}
	// pointers
	void setNextEvent(MouseLLEvent^ newNextEvent){nextEvent = newNextEvent;}
	void setPrevEvent(MouseLLEvent^ newPrevEvent){prevEvent = newPrevEvent;}
	// -----------------------------------------------------test
	// check for equal values
	bool IsDifferentTime(double dNewTime){return dNewTime != dTimestamp;}

	// ---- data grid
	void PopulateGridRow(array<System::String^>^ rowDataArray);
};

public enum class ML_ERR {
	MLL_OK,							// no errors
	MLL_ERR_MISSING_PREVIOUS,		// current node missing previous (and not firstEvent)
	MLL_ERR_MISSING_NEXT,			// current node missing next (and not lastEvent)
	MLL_ERR_NEXT_TIME_EARLIER,		// next node timestamp earlier than current
	MLL_ERR_PREVIOUS_TIME_LATER,	// previous node timestamp later than current
	MLL_ERR_NEXT_TIME_EQUAL,		// next node timestamp equal to current
	MLL_ERR_PREVIOUS_TIME_EQUAL,	// previous node timestamp equal to current
	MLL_ERR_FIRST_HAS_PREVIOUS,		// firstEvent has previous
	MLL_ERR_LAST_HAS_NEXT,			// lastEvent has next
	MLL_ERR_COUNT_INCORRECT,		// count incorrect
	MLL_ERR_FORWARD_COUNT_INCORRECT,// forward count incorrect, back OK
	MLL_ERR_BACKWARD_COUNT_INCORRECT,// backward count incorrect, forward OK
	MLL_ERR_NO_COUNT_CORRECT,		// backward, forward different, don't match count
	MLL_ERR_NO_NODES,				// no nodes
	MLL_ERR_FIRST_NO_LAST,			// firstEvent points to event, last doesn't
	MLL_ERR_LAST_NO_FIRST,			// lastEvent points to event, first doesn't
};

// ---------------------------------------------------- MouseLL -----------------------------------

// Linked List
ref class MouseLL : public System::Object
{
// constructors

public:
	
	MouseLL(void);
	// called new sequence - loading
	MouseLL(System::String^ currentMovieURL, double currentMovieSecs, double currentMoviePosition); // only for load
	// called new sequence - Not recording
	MouseLL(System::String^ newSeqName, System::String^ currentMovieURL, double currentMovieSecs, double currentMoviePosition);
	// called new sequence - Recording
	MouseLL(System::String^ newSeqName, System::String^ currentMovieURL, double currentMovieSecs, 
		    double dNewTime, bool bNewFeed, int iNewArm);

// properties
private:

	// event updates
	double te_currentTime;  // time at or past this, not to next yet
	double te_nextTime;     // next event time
	double te_prevTime;		// previous event time, after moving (not from playing forward)

	double pl_currentTime; // current player time
	double pl_prevTime;    // previous player time

	MouseLLEvent^ ev_prevEvent;		// previous event
	MouseLLEvent^ ev_nextEvent;		// next event
	MouseLLEvent^ ev_currentEvent;	// last / current event

	// pointers to first and last event
	MouseLLEvent^ firstEvent;
	MouseLLEvent^ lastEvent;
	

	// name of sequence
	System::String^ seqName;
	// name of movie
	System::String^ movieURL;
	// latest event
	MouseLLEvent^ recentEvent;

	// new events without save
	bool IsDirty;

	// count of events
	int Count;

	// max movie time
	double dMaxMovieSecs;


	// for tracking
	int new_EventID;


// methods
public: 
	// number of nodes
	int get_Count(void){return Count;}

	// add new events
	bool addEvent(double dNewTime, bool bNewFeed, int iNewArm);
	bool addFirstEvent(double dNewTime, bool bNewFeed, int iNewArm);
	bool addLastEvent(double dNewTime, bool bNewFeed, int iNewArm);
	
	// tests
	bool IsBeforeOrAtFirst(double dNewTime){return dNewTime <= firstEvent->getTimestamp();}
	bool IsAfterOrAtLast(double dNewTime){return dNewTime >= lastEvent->getTimestamp();}

	// time data
	bool getFirstLastTimes(double* p_dFirstTimestamp, double* p_dLastTimestamp);

	// names
	System::String^ getSequenceName(void){return seqName;}

	// first and last
	MouseLLEvent^ getFirstEvent(void){return firstEvent;};
	MouseLLEvent^ getLastEvent(void){return lastEvent;};

	// test events
	bool IsPreviousFirst(MouseLLEvent^% testEvent);
	bool IsNextLast(MouseLLEvent^% testEvent);


	// ----- change events ------
	// send new event based on normal playing and timer, will not send if not passed next event
	// this should be SLOW - looks through entire list each time
	MouseLLEvent^ playEvent_All(double tm_new_player);
	
	// send new event based on normal playing and timer, will not send if not passed next event
	MouseLLEvent^ playEvent_Timer(double tm_new_player);

	// update tracking state for playing event change
	bool _update_playEvent_All(MouseLLEvent^ event_sent, MouseLLEvent^ event_sent_next, double tm_new_player);

private:
	// create events
	MouseLLEvent^ _createEvent(double dNewTime, bool bNewFeed, int iNewArm, MouseLLEvent^ prevEvent, MouseLLEvent^ nextEvent);
	// replace events
	bool _replaceEventParams(bool bNewFeed, int iNewArm, MouseLLEvent^ currentEvent);
	// check integrity of list, including timestamps
	ML_ERR _CheckIntegrity(MouseLLEvent^ refMouseEvent, bool bCheckNoNodes);

// -------------------- FILE IO
#pragma region file IO save
	private:
		// create string array out of events
		bool _EventsToStrings(System::Text::StringBuilder^ sb);
		// save sequence data
		bool _SaveSequenceData(System::String^ seqFilePath);
	public:
		// save current sequence
		bool Save_Sequence(void);
		// check save state
		bool get_IsDirty(void){return IsDirty;}
#pragma endregion

#pragma region file IO load
	public:
		// load existing sequence from file
		bool Load_Sequence(System::String^ strSeqLoadFile);
	private:
		// load header - sequence name, movie name, number of events
		int _LoadHeader(System::IO::StreamReader^ sr, System::String^% seqName, System::String^% movieName);
		// load all events
		int _LoadEvents(System::IO::StreamReader^ sr, System::String^ strSeqLoadFile, int iHeaderEvents);
		// load individual events
		bool _LoadSingleEvent(System::IO::StreamReader^ sr, int* p_iEventCount, int iLineNumber, 
							  array<double>^% dTimestampArray, array<int>^% iArmArray, array<bool>^% bFedArray);
		//MouseLLEvent^ _LoadSingleEvent(System::IO::StreamReader^ sr, System::String^ strSeqLoadFile, int iLineNumber, int* p_iEventNumber, bool* p_bErr);
		// parse line
		array<System::String^>^  _ParseLine(System::String^ strLoadLine);
		// general parse error
		System::String^ _ParseError(System::String^ strConvertColumn, System::String^ strConvertType, System::String^ strConvertText, int iLineNumber);
		// Create LL
		bool _Load_CreateLL(array<double>^ dTimestampArray, array<int>^ iArmArray, array<bool>^ bFedArray);
#pragma endregion

// -------- Data Grid
	public:
		// populate grid
		int PopulateDataGrid(System::Windows::Forms::DataGridView^ dataGridEvents); 

// ----- Find
public:
		// find node by index
		MouseLLEvent^ FindNodeByIndex(int iZeroIndex);
private:
	// searches for the node with a time the same or after the event, prior to next event
	bool _search_NoPreviousNode(double te_testTime, MouseLLEvent^% ev_test_current_event, MouseLLEvent^% ev_test_next_event);

// --- testing
private:
	// returns true if time is before event, false if after or at the same time
	bool _test_TimeBeforeEvent(double te_testTime, MouseLLEvent^ ev_testEvent)
		{return (te_testTime < ev_testEvent->getTimestamp());}
	
	// returns true if time is after or at the same time as event
	bool _test_TimeAfterEvent(double te_testTime, MouseLLEvent^ ev_testEvent)
		{return (te_testTime >= ev_testEvent->getTimestamp());}

// tracking setup
public:
	// set up event tracking variables after a load sequence event
	bool setup_loadSequence(void);

private:
	// debugging for tracking
	System::Void DEBUG_TRACKING(void);
};
