#pragma once


// Linked List mouse event
ref class MouseLLEvent : public System::Object
{
public:
	MouseLLEvent(void);
	MouseLLEvent(double dNewTime, bool bNewFeed, int iNewArm, MouseLLEvent^ prevEvent, MouseLLEvent^ nextEvent);

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

// Linked List
ref class MouseLL : public System::Object
{
// constructors
public:
	MouseLL(void);
	MouseLL(System::String^ currentMovieURL, double currentMovieSecs); // only for load
	MouseLL(System::String^ newSeqName, System::String^ currentMovieURL, double currentMovieSecs);
	MouseLL(System::String^ newSeqName, System::String^ currentMovieURL, double currentMovieSecs, double dNewTime, bool bNewFeed, int iNewArm);


// properties
private:
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
};
