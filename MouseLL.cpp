#include "StdAfx.h"
#include "MouseLL.h"
#include <math.h>

MouseLLEvent::MouseLLEvent(void)
{
	EventID = -1;
}

MouseLLEvent::MouseLLEvent(double dNewTime, bool bNewFeed, int iNewArm, MouseLLEvent^ new_prevEvent, MouseLLEvent^ new_nextEvent, int newEventID)
{
	// values
	dTimestamp = dNewTime;
	iArm = iNewArm;
	bFed = bNewFeed;
	// pointers
	prevEvent = new_prevEvent;
	nextEvent = new_nextEvent;
	// event ID
	EventID = newEventID;

}

//// populate grid row
//void MouseLLEvent::PopulateGridRow(array<System::String^>^ rowDataArray)
//{
//	// convert time to movie stamp
//	double dMinutes = floor(dTimestamp / 60);
//	double dSeconds = fmod(dTimestamp, 60);
//	// add data
//	rowDataArray[0] = System::String::Format("{0}:{1}",dMinutes,dSeconds.ToString("00.00"));
//	rowDataArray[1] = System::String::Format("{0}",iArm);
//	rowDataArray[2] = System::String::Format("{0}",bFed);
//}

// ---------------------------------------------------- MouseLL -----------------------------------
#pragma region MouseLL constructors
MouseLL::MouseLL(void)
{
	new_EventID = 0;

	// initialize event updating
	te_currentTime	= -1.0;	// time at or past this, not to next yet
	te_nextTime		= -1.0; // time of next event
	te_prevTime		= -1.0; // previous event time, after moving

	pl_currentTime	= -1.0; // init current player time
	pl_prevTime		= -1.0; // init previous player time

	ev_prevEvent	= nullptr; // previous event
	ev_nextEvent	= nullptr; // next event
	ev_currentEvent = nullptr; // current event

	te_currentTime;      // time at or past this, not to next yet
}


MouseLL::MouseLL(System::String^ currentMovieURL, double currentMovieSecs, double currentMoviePosition)
{
	// movie name
	movieURL = currentMovieURL;
	// max movie seconds
	dMaxMovieSecs = currentMovieSecs;
	// setup event ID
	new_EventID = 0;
	
	// initialize event updating
	pl_currentTime	= currentMoviePosition;  // init current player time
	pl_prevTime		= -1.0;					 // init previous player time

	te_currentTime	= -1.0;		// last/current event time
	te_nextTime		= -1.0;		// next event time
	te_prevTime		= -1.0;		// previous event time

	ev_prevEvent	= nullptr;	// previous event
	ev_nextEvent	= nullptr;	// next event
	ev_currentEvent	= nullptr;	// current event

	te_currentTime;      // time at or past this, not to next yet

}

MouseLL::MouseLL(System::String^ newSeqName, System::String^ currentMovieURL, double currentMovieSecs, double currentMoviePosition)
{
	// add name
	seqName = newSeqName;
	// movie name
	movieURL = currentMovieURL;
	// init pointers
	firstEvent = nullptr;
	lastEvent = nullptr;
	// init count
	Count = 0;
	// recent event
	recentEvent = nullptr;
	// new events without save
	IsDirty = false;
	// max movie seconds
	dMaxMovieSecs = currentMovieSecs;
	// setup event ID
	new_EventID = 0;

	// initialize event updating
	pl_currentTime = currentMoviePosition;  // init current player time
	pl_prevTime = -1.0;						// init previous player time

	te_currentTime	= -1.0;						// last event time
	te_nextTime		= -1.0;						// next event time
	te_prevTime		= -1.0;						// previous event time

	ev_prevEvent	= nullptr;					// previous event
	ev_nextEvent	= nullptr;					// next event
	ev_currentEvent = nullptr;				// current event

}

MouseLL::MouseLL(System::String^ newSeqName, System::String^ currentMovieURL, double currentMovieSecs, double dNewTime, bool bNewFeed, int iNewArm)
{
	// add name
	seqName = newSeqName;
	// movie name
	movieURL = currentMovieURL;
	// init pointers
	firstEvent = nullptr;
	lastEvent = nullptr;
	// init count
	Count = 0;
	// recent event
	recentEvent = nullptr;
	// new events without save
	IsDirty = false;
	// max movie seconds
	dMaxMovieSecs = currentMovieSecs;
	// setup event ID
	new_EventID = 0;

	// initialize event updating
	pl_currentTime	= dNewTime;  // init current player time
	pl_prevTime		= -1.0;		// init previous player time

	te_currentTime	= -1.0;		// last/current event time
	te_nextTime		= -1.0;		// next event time
	te_prevTime		= -1.0;						// previous event time

	ev_prevEvent	= nullptr;					// previous event
	ev_nextEvent	= nullptr;					// next event
	ev_currentEvent = nullptr;				// current event

	// ADD NEW EVENT - no, will do update twice: once from end of Add event (count = 0), once from setup_loadRecordStart
	//if(iNewArm!=-1) // only add if there is an event state
	//	addEvent(dNewTime, bNewFeed, iNewArm);

}
#pragma endregion



// buttons
#pragma region MouseLL add event
	// add new event
	bool MouseLL::addEvent(double dNewTime, bool bNewFeed, int iNewArm, bool bStartFromRecord) // bStartFromRecord = true if event added when recording starts
	{
		MouseLLEvent^ newLLEvent;
		int iLocation; // for updating, -1 for before first, 1 for after last, 0 for middle

		if(Count == 0)
		{
			// bStartFromRecord = true if event added after new mouseLL, recording started with event on
			//			don't update here!  already using setup_loadRecordStart

			// create event
			newLLEvent = _createEvent(dNewTime, bNewFeed, iNewArm, nullptr, nullptr);
			// set internal pointers
			firstEvent = newLLEvent;
			lastEvent = newLLEvent;
			// set count
			Count++;
			// set recent
			recentEvent = newLLEvent;

			if(bStartFromRecord = false)
			{
				// update events: only one so far, from recording that started without event on
				pl_currentTime = dNewTime;

				// events
				ev_currentEvent = newLLEvent; // others still null

				// event times
				te_currentTime = dNewTime;

				//double te_currentTime;  // time at or past this, not to next yet
				//double te_nextTime;     // next event time
				//double te_prevTime;		// previous event time, after moving (not from playing forward)
				//double pl_currentTime; // current player time
				//double pl_prevTime;    // previous player time
				//MouseLLEvent^ ev_prevEvent;		// previous event
				//MouseLLEvent^ ev_nextEvent;		// next event
				//MouseLLEvent^ ev_currentEvent;	// last / current event

			}

			QuickMsgBox::QTrace("addEvent: none existed, time {0}, count {1}\n",dNewTime, Count);
		}else
		{
			// check if time is before or at first
			if(IsBeforeOrAtFirst(dNewTime))
			{
				// inserted before, or if same time replaced arm and fed
				if(addFirstEvent(dNewTime, bNewFeed, iNewArm, newLLEvent))
				{
					Count++;
					iLocation = -1;
					QuickMsgBox::QTrace("addEvent: added before first, time {0}, count {1}\n",dNewTime, Count);
				}
			}else
			{
				// check if time is after or at last
				if(IsAfterOrAtLast(dNewTime))
				{
					// inserted after, or if same time replaced arm and fed
					if(addLastEvent(dNewTime, bNewFeed, iNewArm, newLLEvent))
					{
						Count++;
						iLocation = 1;
						QuickMsgBox::QTrace("addEvent: added after last, time {0}, count {1}\n",dNewTime, Count);
					}
				}else
				{
					// between current first and last
					//Count++;
					//iLocation = 0;
					QuickMsgBox::QTrace("addEvent: in between (doing nothing): time {0}, count {1}\n",dNewTime, Count);
					//System::Diagnostics::Trace::WriteLine( "PLACEHOLDER: New Event Between existing events");
				}
			}
			// only do this when events exist
			update_newEvent(newLLEvent, dNewTime, iLocation);
		}
		
		// DEBUG
		// System::Diagnostics::Trace::WriteLine( recentEvent->ToString() );
		// END DEBUG 
		IsDirty = true;
		return true;
	}
	// add event to beginning
	bool MouseLL::addFirstEvent(double dNewTime, bool bNewFeed, int iNewArm, MouseLLEvent^% new_event_track)
	{
		bool bReturn = false;
		// check if new event time is before first event
		if(firstEvent->IsDifferentTime(dNewTime))
		{
			// not the same time stamp, so create new: _createEvent(...,new_prevEvent, new_nextEvent);
			MouseLLEvent^ newEvent = _createEvent(dNewTime, bNewFeed, iNewArm, nullptr, firstEvent); //new next = current first
			if(newEvent != nullptr)
			{
				// success:
				firstEvent->setPrevEvent(newEvent); // set previous event pointer of current first to new first
				firstEvent = newEvent;				// set first event pointer to new event
				// set recent
				recentEvent = newEvent;
				bReturn =  true;
				new_event_track = newEvent;
			}
			else
				bReturn = false; // something went wrong
		}else
		{
			// timestamp is the same
			_replaceEventParams(bNewFeed, iNewArm, firstEvent);
			// set recent
			recentEvent = firstEvent;
			new_event_track = firstEvent;
			bReturn =  true;
		}

		return bReturn;
		
	}
	
	// add last to beginning
	bool MouseLL::addLastEvent(double dNewTime, bool bNewFeed, int iNewArm, MouseLLEvent^% new_event_track)
	{
		bool bReturn = false;
		// check if new event time is before first event
		if(lastEvent->IsDifferentTime(dNewTime))
		{
			// not the same time stamp, so create new: _createEvent(...,new_prevEvent, new_nextEvent);
			MouseLLEvent^ newEvent = _createEvent(dNewTime, bNewFeed, iNewArm, lastEvent, nullptr); //new previous = current last
			if(newEvent != nullptr)
			{
				// success:
				lastEvent->setNextEvent(newEvent); // set next event pointer of current last to new last
				lastEvent = newEvent;			   // set first event pointer to new event
				// set recent
				recentEvent = newEvent;
				bReturn =  true;
				new_event_track = newEvent;
			}
			else
				bReturn = false; // something went wrong
		}else
		{
			// timestamp is the same
			_replaceEventParams(bNewFeed, iNewArm, lastEvent);
			// set recent
			recentEvent = lastEvent;
			new_event_track = lastEvent;
			bReturn =  true;
		}
		return bReturn;
	}
	
	// create a new event
	MouseLLEvent^ MouseLL::_createEvent(double dNewTime, bool bNewFeed, int iNewArm, MouseLLEvent^ new_prevEvent, MouseLLEvent^ new_nextEvent)
	{
			// create new event
			MouseLLEvent^ newEvent = gcnew MouseLLEvent(dNewTime, bNewFeed, iNewArm, new_prevEvent, new_nextEvent, new_EventID);
			// increment Event ID tracker
			new_EventID++;
			return newEvent;
	}
	
	// replace event parameters
	bool MouseLL::_replaceEventParams(bool bNewFeed, int iNewArm, MouseLLEvent^ currentEvent)
	{
		currentEvent->setArm(iNewArm);
		currentEvent->setFed(bNewFeed);
		return true;
	}
#pragma endregion

#pragma region File IO Output
	// create string array out of events
	bool MouseLL::_EventsToStrings(System::Text::StringBuilder^ sb)
	{
		bool bOK = false;
		MouseLLEvent^ currentEvent = firstEvent;
		if(Count > 0)
		{
			while(currentEvent != nullptr)
			{
				sb->Append(currentEvent->ToString());
				currentEvent = currentEvent->getNextEvent();
			}
			bOK = true;
		}
		return bOK;
	}

	// save current sequence
	bool MouseLL::Save_Sequence(void)
	{
		bool bSuccess = false;

		// create file dialog params
		// Microsoft::Win32::SaveFileDialog^ saveFileDialogSeq = gcnew Microsoft::Win32::SaveFileDialog();
		System::Windows::Forms::SaveFileDialog^ saveFileDialogSeq;
		saveFileDialogSeq = gcnew System::Windows::Forms::SaveFileDialog();

		saveFileDialogSeq->Filter = 
			"Comma Separated (.csv)|*.csv|All files (*.*)|*.*";	// comma seperated or all
		saveFileDialogSeq->DefaultExt = ".csv";
		saveFileDialogSeq->Title = "Save Event Sequence";
		saveFileDialogSeq->CheckPathExists = true;
		saveFileDialogSeq->FileName = seqName;

		saveFileDialogSeq->ShowDialog();
		// If the file name is not an empty string, open it for saving.
		if(saveFileDialogSeq->FileName != "")
		{

			// DO SAVE HERE RETURN WHATEVER IS TRUE
			bSuccess = _SaveSequenceData(saveFileDialogSeq->FileName);
		}

		// remove dirty flag
		if(bSuccess)
			IsDirty = false;

		// return if everything is OK
		return bSuccess;
	}
	// save actual data
	bool MouseLL::_SaveSequenceData(System::String^ seqFilePath)
	{
		// data saved as cvs:  time, arm, fed
		// first line is sequence, movie file, events

		System::String^ movieFileName = System::IO::Path::GetFileName(movieURL);
		System::String^ seqFileName = System::IO::Path::GetFileNameWithoutExtension(seqFilePath);

		// create string builder
		System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder();

		// add file information
		sb->AppendFormat("{0},{1},{2}\n",seqFileName,movieFileName,Count);

		// add events
		_EventsToStrings(sb);

		// --- open & write file
		System::IO::StreamWriter^ outfile = gcnew System::IO::StreamWriter(seqFilePath);
		outfile->Write(sb->ToString());

		// --- close file
		outfile->Close();

		return true;
	}
#pragma endregion

#pragma region File IO Input

	// load existing sequence from file
	bool MouseLL::Load_Sequence(System::String^ strSeqLoadFile)
	{
		bool bOK = true;
		// stream reader
		System::IO::StreamReader^ sr;
		// open file
		try
		{
			sr = gcnew System::IO::StreamReader(strSeqLoadFile);
		}
		catch ( System::SystemException::Exception^ e ) 
		{
			System::Windows::Forms::MessageBox::Show( System::String::Format("Exception opening {0}: {1}",strSeqLoadFile, e));
			delete sr;
			return false;		
		}

		// ------LOAD STUFF
		System::String^ newSeqName;
		System::String^ newMovieName;
		
		// -----header
		int iHeaderEvents = _LoadHeader(sr, newSeqName, newMovieName);
		if(iHeaderEvents <= 0)
		{
			sr->Close();
			return false;
		}
		// check header against current movie
		System::String^ currentMovieName = System::IO::Path::GetFileName(movieURL);
		if(System::String::Compare(currentMovieName, newMovieName, false) != 0)
		{
			// movie names not the same
			System::String^ movieError = 
				System::String::Format("Movie name '{0}' in '{1}' not the same as current movie '{2}'", newMovieName, strSeqLoadFile, currentMovieName);
			System::Windows::Forms::MessageBox::Show(movieError);
			sr->Close();
			return false;
		}

		// -------- load events
		int iEvents = _LoadEvents(sr, strSeqLoadFile, iHeaderEvents);
		if(iEvents <=0 )
		{
			sr->Close();
			return false;
		}
		//System::Console::WriteLine("Sequence: <{0}>, Movie: <{1}>, Events<{2}>", newSeqName, newMovieName, iEvents);
		//System::Diagnostics::Debug::WriteLine(System::String::Format("Sequence: <{0}>, Movie: <{1}>, Events<{2}>", newSeqName, newMovieName, iEvents));
		//System::Diagnostics::Trace::WriteLine("test");

		// close
		sr->Close();

		// --- some intialization
		// add name
		seqName = newSeqName;
		// recent event
		recentEvent = nullptr;
		// new events without save
		IsDirty = false;


		return bOK;
	}
	// load header, return number of events listed in *header*, -1 for error
	int MouseLL::_LoadHeader(System::IO::StreamReader^ sr, System::String^% seqName, System::String^% movieName)
	{
		// initialize number of events
		int iEvents = -1;
		System::String^ strHeader;

		try
		{
			strHeader = sr->ReadLine();
		}
		catch ( System::SystemException::Exception^ e ) 
		{
			System::Windows::Forms::MessageBox::Show( System::String::Format("Exception reading header: {0}",e));
			return -1;		
		}
		// ----- check for null (EOF)
		if(strHeader == nullptr)
		{
			System::Windows::Forms::MessageBox::Show("File has no lines");
			return -1;
		}

		// ------ split the line
		array<System::String^>^ splitHeader = _ParseLine(strHeader);

		if(splitHeader->Length !=3)
		{
			System::Windows::Forms::MessageBox::Show( System::String::Format("Header (line 1) split into {0} comma-separated values",splitHeader->Length));
			return -1;
		}

		// --- parse the header
		seqName = splitHeader[0];		// sequence name (not file name)
		movieName = splitHeader[1];	// movie name
		// parse number of events
		try
		{
			iEvents = System::Int32::Parse(splitHeader[2]);			
		}
		catch (System::FormatException^)
		{
			System::Windows::Forms::MessageBox::Show( System::String::Format("Could not convert header item count string {0} into integer",splitHeader[2]));
			iEvents = -1;
		}


		return iEvents;
	}

	// load all events
	int MouseLL::_LoadEvents(System::IO::StreamReader^ sr, System::String^ strSeqLoadFile, int iHeaderEvents)
	{
		//DEBUG
		//System::String^ debugString1;
		//System::String^ debugString2;
		
		MouseLLEvent^ newEvent = nullptr; // place holder for new events
		//MouseLLEvent^ prevEvent = nullptr; // pointer to previous event
		int iLineNumber = 2;
		int iEventCount = 0;
		bool bOK = true;

		// create arrays	
		array<int>^ iArmArray = gcnew array<int>(iHeaderEvents);
		array<double>^ dTimestampArray = gcnew array<double>(iHeaderEvents);
		array<bool>^ bFedArray = gcnew array<bool>(iHeaderEvents);

		// will return false on error and EOF
		while(((bOK = _LoadSingleEvent(sr, &iEventCount, iLineNumber, dTimestampArray, iArmArray, bFedArray)) == true) && 
			  (iEventCount < iHeaderEvents))
		{
			if(bOK)
			{
				//// DEBUG
				//debugString1 = System::String::Format("Event {0}:\n", iEventCount);
				//debugString2 = System::String::Format("\tTime: {0}, Arm = {1}, Fed = {2}\n", dTimestampArray[iEventCount-1], 
				//							iArmArray[iEventCount-1], bFedArray[iEventCount-1]);
				//System::Diagnostics::Debug::WriteLine(System::String::Concat(debugString1,debugString2));
				//// END DEBUG

				iLineNumber++;
			}
		}
		if(!bOK)
			return -1;

		//// DEBUG
		//debugString1 = System::String::Format("Event {0}:\n", iEventCount);
		//debugString2 = System::String::Format("\tTime: {0}, Arm = {1}, Fed = {2}\n", dTimestampArray[iEventCount-1], 
		//							iArmArray[iEventCount-1], bFedArray[iEventCount-1]);
		//System::Diagnostics::Debug::WriteLine(System::String::Concat(debugString1,debugString2));
		//// END DEBUG

		// check correct number of events read
		if(iEventCount < iHeaderEvents)
		{
			System::Windows::Forms::MessageBox::Show( System::String::Format("Read {0} events, expected {1}", iEventCount, iHeaderEvents));
			return -1;
		}

		// read any more lines??
		System::String^ strTestMoreLines;
		// ----- read line (EOF)
		try
		{
			strTestMoreLines = sr->ReadLine();
		}
		catch ( System::SystemException::Exception^ e ) 
		{
			System::Windows::Forms::MessageBox::Show( System::String::Format("Exception test more lines {0}",e));
			return -1;		
		}
		// ----- check for additional lines
		if(strTestMoreLines != nullptr)
		{
			System::Windows::Forms::MessageBox::Show( System::String::Format("Additional lines after reading {0} events",iEventCount));
			return -1;
		}

		
		// create events
		if(!(_Load_CreateLL(dTimestampArray, iArmArray, bFedArray)))
			return -1;

		// success!
		return iEventCount;
	}

	// load individual events
	bool MouseLL::_LoadSingleEvent(System::IO::StreamReader^ sr, int* p_iEventCount, int iLineNumber, 
								   array<double>^% dTimestampArray, array<int>^% iArmArray, array<bool>^% bFedArray)
	{
		// load single event
		// p_iEventCount = *up to this point*, so *p_iEventCount = 0 means no events yet
		// iLineNumber = current read line
		
		double dNewTimestamp;
		int iNewArm;
		bool bNewFed;

		System::String^ strEventLine;
		// ----- read line (EOF)
		try
		{
			strEventLine = sr->ReadLine();
		}
		catch ( System::SystemException::Exception^ e ) 
		{
			System::Windows::Forms::MessageBox::Show( System::String::Format("Exception reading line {0}: {1}",iLineNumber, e));
			return false;		
		}
		// ----- check for null (EOF)
		if(strEventLine == nullptr)
		{
			// EOF
			return false;
		}

		// --- split line
		array<System::String^>^ splitEventLine = _ParseLine(strEventLine);
		if(splitEventLine->Length !=3)
		{
			// could not split line
			System::Windows::Forms::MessageBox::Show( System::String::Format("Line {0}: split into {0} comma-separated values",iLineNumber, splitEventLine->Length));
			return false;
		}

		// --------------------------------- parse values
		// ------ parse timestamp
		// parse
		try
		{
			dNewTimestamp = System::Double::Parse(splitEventLine[0]);			
		}
		catch (System::FormatException^)
		{
			System::String^ parseErrorStr = _ParseError(L"timestamp", L"double", splitEventLine[0], iLineNumber);
			System::Windows::Forms::MessageBox::Show(parseErrorStr);
			return false;
		}
		// check duration
		if(dNewTimestamp < 0)
		{
			// timestamp less than zero
			System::Windows::Forms::MessageBox::Show( System::String::Format("Line {0}: Timestamp '{0}' less than zero",iLineNumber, splitEventLine[0]));
			return false;
		}
		else
		{
			if(dNewTimestamp > dMaxMovieSecs)
			{
				// timestamp greater than max seconds
				System::Windows::Forms::MessageBox::Show( System::String::Format("Line {0}: Timestamp '{1}' greater than current movie duration of {2:F2} seconds", 
														 iLineNumber, splitEventLine[0], dMaxMovieSecs));
				return false;
			}
		}
		// ------ parse arm
		try
		{
			iNewArm = System::Int32::Parse(splitEventLine[1]);			
		}
		catch (System::FormatException^)
		{
			System::String^ parseErrorStr = _ParseError(L"arm", L"integer", splitEventLine[1], iLineNumber);
			System::Windows::Forms::MessageBox::Show(parseErrorStr);
			return false;
		}
		// check arm
		if((iNewArm < 1) || (iNewArm > 9))
		{
			// arm out of range
			System::Windows::Forms::MessageBox::Show( System::String::Format("Line {0}: Arm '{1}' not in range 1 - 9", 
														iLineNumber, splitEventLine[1]));
			return false;
		}
		// ------ parse fed
		try
		{
			bNewFed = System::Boolean::Parse(splitEventLine[2]);			
		}
		catch (System::FormatException^)
		{
			System::String^ parseErrorStr = _ParseError(L"fed", L"boolean", splitEventLine[2], iLineNumber);
			return false;
		}

		// success!
		dTimestampArray[*p_iEventCount] = dNewTimestamp;
		iArmArray[*p_iEventCount] = iNewArm;
		bFedArray[*p_iEventCount] = bNewFed;
		*p_iEventCount = *p_iEventCount + 1;
		return true;
	}

	// parses a line, makes sure there are three elements
	array<System::String^>^  MouseLL::_ParseLine(System::String^ strLoadLine)
	{
		// use comma delimiter
		System::String^ delimStr = ","; 
		array<System::Char>^delimiter = delimStr->ToCharArray();

		// create array to hold header values
		array<System::String^>^ splitString = gcnew array<System::String^>(3);

		splitString = strLoadLine->Split(delimiter,3);

		return splitString;
	}
	
	// general parse error
	System::String^ MouseLL::_ParseError(System::String^ strConvertColumn, System::String^ strConvertType, System::String^ strConvertText, int iLineNumber)
	{
		return System::String::Format("Could not convert {0} text '{1}' into {2}, line {3}", strConvertColumn, strConvertText, 
					strConvertType, iLineNumber);
	}

	// create events based on loaded values
	bool MouseLL::_Load_CreateLL(array<double>^ dTimestampArray, array<int>^ iArmArray, array<bool>^ bFedArray)
	{
		int iEvents = dTimestampArray->Length;
		int iEventIndex;

		//DEBUG
		//System::Diagnostics::Debug::WriteLine(System::String::Format("{0} Events:\n", iEvents));
		//END DEBUG

		MouseLLEvent^ newEvent;  // newly created event
		MouseLLEvent^ prevEvent; // previous event

		// ---- first event
		iEventIndex = 0;
		// DEBUG
		//System::Diagnostics::Debug::WriteLine(L"First:\n");
		//System::Diagnostics::Debug::WriteLine(System::String::Format("iEventIndex={0}, timestamp={1}, arm={2}, fed={3}\n", 
		//								iEventIndex, dTimestampArray[iEventIndex], iArmArray[iEventIndex], bFedArray[iEventIndex]));
		// END DEBUG
		newEvent = gcnew MouseLLEvent(dTimestampArray[iEventIndex], bFedArray[iEventIndex], iArmArray[iEventIndex], nullptr, nullptr, new_EventID);
		new_EventID++;
		firstEvent = newEvent;
		Count++;
		// get ready for remaining events
		prevEvent = newEvent;

		// DEBUG
		//System::Diagnostics::Debug::WriteLine(L"Iterate:\n");
		// END DEBUG
		// ---- iterate through remaining 
		for(iEventIndex++; iEventIndex < iEvents; iEventIndex++)
		{
			//// DEBUG
			//System::Diagnostics::Debug::WriteLine(System::String::Format("iEventIndex={0}, timestamp={1}, arm={2}, fed={3}\n", 
			//								iEventIndex, dTimestampArray[iEventIndex], iArmArray[iEventIndex], bFedArray[iEventIndex]));
			//// END DEBUG
			// create new event, with pointer to previous
			newEvent = gcnew MouseLLEvent(dTimestampArray[iEventIndex], bFedArray[iEventIndex], iArmArray[iEventIndex], prevEvent, nullptr, new_EventID);
			new_EventID++;
			// increment count
			Count++;
			// set previous event "next" pointer to this
			prevEvent->setNextEvent(newEvent);
			// point to this as previous event
			prevEvent = newEvent;
		}

		// --- last
		// set last event
		lastEvent = newEvent;

		MouseLLEvent^ mLL_Check = nullptr;
		if(_CheckIntegrity(mLL_Check, true) != ML_ERR::MLL_OK)
		{
			// some kind of integrity error
			return false;
		}else
			return true; 
	}

#pragma endregion

#pragma region Data Grid
	
	
	//// vector population of data grid
	//int MouseLL::PopulateGridVector(List<MouseLLEvent^>^ grid_row_vector)
	//{
	//	//List<GridRowData^>
	//	int iAdded = 0;
	//	MouseLLEvent^ currentEvent = firstEvent;

	//	for each(MouseLLEvent^%  current_grid_row in grid_row_vector)
	//	{
	//		current_grid_row = currentEvent;
	//		iAdded++;
	//		currentEvent = currentEvent->getNextEvent();
	//	}

	//	return iAdded;

	//}
	//// text population of data grid
	//int MouseLL::PopulateDataGrid(System::Windows::Forms::DataGridView^ dataGridEvents)
	//{
	//	

	//	int iAdded = 0;
	//	array<System::String^>^rowData = gcnew array<System::String^>(3);
	//	MouseLLEvent^ currentEvent = firstEvent;
	//	
	//	// iterate through list
	//	while(currentEvent != nullptr)
	//	{
	//		// add strings to array
	//		currentEvent->PopulateGridRow(rowData);
	//		// add data to row
	//		dataGridEvents->Rows->Add(rowData);
	//		// get pointer to next event
	//		currentEvent = currentEvent->getNextEvent();
	//		// inc count
	//		iAdded++;
	//	}
	//	return iAdded;
	//}

#pragma endregion

#pragma region data access methods
	// get timestamp of first and last time
	bool MouseLL::getFirstLastTimes(double* p_dFirstTimestamp, double* p_dLastTimestamp)
	{
		if(Count > 0)
		{
			*p_dFirstTimestamp = firstEvent->getTimestamp();
			*p_dLastTimestamp = lastEvent->getTimestamp();
			return true;
		}else
			return false;
	}
#pragma endregion

#pragma region find functions
	// find by index
	MouseLLEvent^ MouseLL::FindNodeByIndex(int iZeroIndex)
	{
		// check for empty
		if(Count == 0)
			return nullptr;

		MouseLLEvent^ currentEvent = firstEvent;
		int iCounter = 0;
		// loop through events (if firstEvent, should skip)
		while((currentEvent != nullptr) && (iCounter < iZeroIndex))
		{
			currentEvent = currentEvent->getNextEvent();
			iCounter++;
		}
		
		// returns null if out of range
		return currentEvent;

	}

	// searches for the node with a time the same or after the event, prior to next event
	bool MouseLL::_search_NoPreviousNode(double te_testTime, MouseLLEvent^% ev_test_current_event, MouseLLEvent^% ev_test_next_event)
	{

		// --- check for time before first event
		if(!_test_TimeAfterEvent(te_testTime, firstEvent))
		{
			// time is before first event
			ev_test_current_event = nullptr; ev_test_next_event = firstEvent;
			return true;
		}

		// --- check for time after last event
		if(_test_TimeAfterEvent(te_testTime, lastEvent))
		{
			// time is after last event
			ev_test_current_event = lastEvent; ev_test_next_event = nullptr;
			return true;
		}

		// --- start search

		MouseLLEvent^ ev_node_current = firstEvent;
		MouseLLEvent^ ev_node_next = ev_node_current->getNextEvent();
		bool bTimeAfterNext = _test_TimeAfterEvent(te_testTime, ev_node_next);

		// loop until time is less than next event time - NOTE THIS ONLY WORKS SINCE WE CAUGHT LAST EVENT
		while(bTimeAfterNext)
		{
			ev_node_current = ev_node_next;
			ev_node_next = ev_node_current->getNextEvent();
			bTimeAfterNext = _test_TimeAfterEvent(te_testTime, ev_node_next);			
		}
		ev_test_current_event = ev_node_current;
		ev_test_next_event = ev_node_next; // this is the node the time is not equal or greater than


		return true;
	}

#pragma endregion

#pragma region test events
	// test if event prior to given is the first event
	bool MouseLL::IsPreviousFirst(MouseLLEvent^% testEvent)
	{
		MouseLLEvent^ previousEvent = testEvent->getPreviousEvent();
		if(previousEvent == nullptr)
		{
			testEvent = nullptr;
			return false;
		}
		else
		{
			if(previousEvent == firstEvent)
				return true;
			else
				return false;
		}
	}
	// test if event after to given is the last event
	bool MouseLL::IsNextLast(MouseLLEvent^% testEvent)
	{
		MouseLLEvent^ nextEvent = testEvent->getNextEvent();
		if(nextEvent == nullptr)
		{
			testEvent = nullptr;
			return false;
		}
		else
		{
			if(nextEvent == lastEvent)
				return true;
			else
				return false;
		}
	}
#pragma endregion

#pragma region check integrity
	// check integrity of list, including timestamps
	ML_ERR MouseLL::_CheckIntegrity(MouseLLEvent^ refMouseEvent, bool bCheckNoNodes)
	{
		MouseLLEvent^ currentEvent;		// event currently being checked
		MouseLLEvent^ checkNextEvent;	// event to check next
		bool bBadTime = false;

		// ----- check no node state
		if((firstEvent == nullptr) && (lastEvent == nullptr))
		{
			// no nodes, check if this counts as an error
			if(bCheckNoNodes)
			{
				// counting as error
				refMouseEvent = nullptr;
				return ML_ERR::MLL_ERR_NO_NODES;
			}else
			{
				// not an error, but check count
				if(Count > 0)
				{
					refMouseEvent = nullptr;
					return ML_ERR::MLL_ERR_COUNT_INCORRECT;
				}else
				{
					//no nodes, count OK
					refMouseEvent = nullptr;
					return ML_ERR::MLL_OK;
				}
			}	
		}

		// ---- check for first w/o last and vice versa
		if((firstEvent == nullptr) || (lastEvent == nullptr)) // one has null pointer
		{
			if(firstEvent != nullptr)
			{
				// firstEvent is not null
				refMouseEvent = firstEvent;
				return ML_ERR::MLL_ERR_FIRST_NO_LAST;
			}
			if(lastEvent != nullptr)
			{
				// lastEvent is not null
				refMouseEvent = lastEvent;
				return ML_ERR::MLL_ERR_LAST_NO_FIRST;
			}
		}

		// --- check first event
		if(firstEvent->getPreviousEvent() != nullptr)
		{
			refMouseEvent = firstEvent;
			return ML_ERR::MLL_ERR_FIRST_HAS_PREVIOUS;
		}
		
		// --- check last event
		if(lastEvent->getNextEvent() != nullptr)
		{
			refMouseEvent = lastEvent;
			return ML_ERR::MLL_ERR_LAST_HAS_NEXT;
		}

		// --- start at beginning, go forward
		currentEvent = firstEvent;
		int iNodeCountForward = 1;
		while(!bBadTime && ((checkNextEvent = currentEvent->getNextEvent()) != nullptr))
		{
			// check if next is earlier than or equal to current
			if(currentEvent->getTimestamp() >= checkNextEvent->getTimestamp())
				bBadTime = true;

			// set current to next
			currentEvent = checkNextEvent; 
			iNodeCountForward++;
		}
		// find reason for breaking loop
		if(bBadTime)
		{
			// time stamp problem
			refMouseEvent = currentEvent;
			if(currentEvent->getTimestamp() == checkNextEvent->getTimestamp())
				return ML_ERR::MLL_ERR_NEXT_TIME_EQUAL;   // next event is same time
			else
				return ML_ERR::MLL_ERR_NEXT_TIME_EARLIER; // next event is earlier
		}
		else
		{
			// currentEvent next points to null, make sure it is the last event
			if(currentEvent != lastEvent)
			{
				refMouseEvent = currentEvent;
				return ML_ERR::MLL_ERR_MISSING_NEXT;
			}

		}

		// --- start at end, go back
		bBadTime = false;
		currentEvent = lastEvent;
		int iNodeCountBackward = 1;
		while(!bBadTime && ((checkNextEvent = currentEvent->getPreviousEvent()) != nullptr))
		{
			// check if previous is later than or equal to current
			if(checkNextEvent->getTimestamp() >= currentEvent->getTimestamp())
				bBadTime = true;

			// set current to next
			currentEvent = checkNextEvent; 
			iNodeCountBackward++;
		}
		// find reason for breaking loop
		if(bBadTime)
		{
			// time stamp problem
			refMouseEvent = currentEvent;
			if(checkNextEvent->getTimestamp() == currentEvent->getTimestamp())
				return ML_ERR::MLL_ERR_PREVIOUS_TIME_EQUAL;   // next event is same time
			else
				return ML_ERR::MLL_ERR_PREVIOUS_TIME_LATER; // next event is earlier
		}
		else
		{
			// currentEvent next points to null, make sure it is the last event
			if(currentEvent != firstEvent)
			{
				refMouseEvent = currentEvent;
				return ML_ERR::MLL_ERR_MISSING_PREVIOUS;
			}
		}

		// check node count
		if(iNodeCountForward == iNodeCountBackward)
		{
			// node counts agree
			if(iNodeCountForward != Count)
			{
				// do not agree with Count
				refMouseEvent = nullptr;
				return ML_ERR::MLL_ERR_COUNT_INCORRECT;
			}
		}else
		{
			// node counts disagree
			refMouseEvent = nullptr;

			if(iNodeCountForward == Count)
			{
				// forward count OK
				return ML_ERR::MLL_ERR_BACKWARD_COUNT_INCORRECT;
			}else
			{
				if(iNodeCountBackward == Count)
				{
					// backward count OK
					return ML_ERR::MLL_ERR_FORWARD_COUNT_INCORRECT;
				}else
				{
					// neither OK
					return ML_ERR::MLL_ERR_NO_COUNT_CORRECT;
				}
			}
			
		}
		

		return ML_ERR::MLL_OK;

		//// get next event
		//MouseLLEvent^ getNextEvent(void){return nextEvent;};
		//// get previous event
		//MouseLLEvent^ getPreviousEvent(void){return prevEvent;};

		//MLL_OK,							// no errors
		//MLL_ERR_MISSING_PREVIOUS,		// current node missing previous (and not firstEvent)
		//MLL_ERR_MISSING_NEXT,			// current node missing next (and not lastEvent)
		//MLL_ERR_NEXT_TIME_EARLIER,		// next node timestamp earlier than current
		//MLL_ERR_PREVIOUS_TIME_LATER,	// previous node timestamp later than current
		//MLL_ERR_NEXT_TIME_EQUAL,		// next node timestamp equal to current
		//MLL_ERR_PREVIOUS_TIME_EQUAL,	// previous node timestamp equal to current
		//MLL_ERR_FIRST_HAS_PREVIOUS,		// firstEvent has previous
		//MLL_ERR_LAST_HAS_NEXT,			// lastEvent has next
		//MLL_ERR_COUNT_INCORRECT,		// count incorrect
		//MLL_ERR_FORWARD_COUNT_INCORRECT,// forward count incorrect, back OK
		//MLL_ERR_BACKWARD_COUNT_INCORRECT,// backward count incorrect, forward OK
		//MLL_ERR_NO_COUNT_CORRECT,		// backward, forward different, don't match count
		//MLL_ERR_NO_NODES,				// no nodes
		//MLL_ERR_FIRST_NO_LAST,			// firstEvent points to event, last doesn't
		//MLL_ERR_LAST_NO_FIRST,			// lastEvent points to event, first doesn't

	}
#pragma endregion


#pragma region event changes in player
	// ----- change events ------
	// called without a timer event
	MouseLLEvent^ MouseLL::playEvent_NoTimer(double tm_new_player, bool* p_bNoState)
	{
		QuickMsgBox::QTrace("\n\n--------------------------------------playEvent_NoTimer: time {0}\n",tm_new_player);
		QuickMsgBox::QEvent("ev_currentEvent", ev_currentEvent);
		QuickMsgBox::QEvent("ev_nextEvent", ev_nextEvent);
		QuickMsgBox::QEvent("firstEvent", firstEvent);
		QuickMsgBox::QEvent("lastEvent", lastEvent);


		// currently this is the same code as MouseLL::playEvent_All
		MouseLLEvent^ new_me;
		MouseLLEvent^ new_next_me;		// don't care about this so much

		if(!_search_NoPreviousNode(tm_new_player, new_me, new_next_me))
		{
			// something went wrong with the search
			System::Windows::Forms::MessageBox::Show(System::String::Format("Error in finding event node at time {0} (playEvent_All)", tm_new_player));
			return nullptr;
		}


		// --- new event
		QuickMsgBox::QTrace("\n---AFTER: _search_NoPreviousNode --");

		QuickMsgBox::QEvent("new_me", new_me);
		QuickMsgBox::QEvent("new_next_me", new_next_me);
		QuickMsgBox::QEvent("ev_currentEvent", ev_currentEvent);
		QuickMsgBox::QEvent("ev_nextEvent", ev_nextEvent);
		QuickMsgBox::QEvent("firstEvent", firstEvent);
		QuickMsgBox::QEvent("lastEvent", lastEvent);


		// by default, assume state
		*p_bNoState = false;
		if(new_me == ev_currentEvent)
		{
			return nullptr;
		}
		if(new_me == nullptr)
			*p_bNoState = true; // not in any state

		else
		{
			if(new_me == nullptr)
			{
				// not in state
				*p_bNoState = true;
				// HACK - FIX o

				return nullptr;
			}
		}


		// update event being sent
		_update_playEvent_All(new_me, new_next_me, tm_new_player);
		
		QuickMsgBox::QTrace("\n-- AFTER: _update_playEvent_All --");
		QuickMsgBox::QEvent("ev_currentEvent", ev_currentEvent);
		QuickMsgBox::QEvent("ev_nextEvent", ev_nextEvent);
		QuickMsgBox::QEvent("firstEvent", firstEvent);
		QuickMsgBox::QEvent("lastEvent", lastEvent);


		// return new event - may be nullptr if before events
		return new_me;
	}

	// -------- this will return for ALL player position types
	// -------- slow?
	//	searches each time, does not depend on time change, relative position
	MouseLLEvent^ MouseLL::playEvent_All(double tm_new_player, bool* p_bNoState)
	{

//if(tm_new_player >= te_nextTime)
//{
//	bool bFunk;
//	bFunk = true;
//}

		MouseLLEvent^ new_me;
		MouseLLEvent^ new_next_me;		// don't care about this so much

		if(!_search_NoPreviousNode(tm_new_player, new_me, new_next_me))
		{
			// something went wrong with the search
			System::Windows::Forms::MessageBox::Show(System::String::Format("Error in finding event node at time {0} (playEvent_All)", tm_new_player));
			return nullptr;
		}
		// by default, assume state
		*p_bNoState = false;
		if(new_me == ev_currentEvent)
		{
			return nullptr;
		}
		if(new_me == nullptr)
			*p_bNoState = true; // not in any state
		//else
		//{
		//	if(new_me == nullptr)
		//	{
		//		// not in state
		//		*p_bNoState = true;
		//		return nullptr;
		//	}
		//}

		// --- new event

		// update event being sent
		_update_playEvent_All(new_me, new_next_me, tm_new_player);

		// return new event - may be nullptr if before events
		return new_me;

	}

	// send new event based on normal playing and timer, will not send nullptr if not past next event
	MouseLLEvent^ MouseLL::playEvent_Timer(double tm_new_player, bool* p_bNoState)
	{
		// -------- this will only test whether new time is passed new event
		return nullptr;
	}

	//// event updates
	//double te_currentTime;  // time at or past this, not to next yet
	//double te_nextTime;     // next event time
	//double te_prevTime;		// previous event time, after moving (not from playing forward)

	//double pl_currentTime; // current player time
	//double pl_prevTime;    // previous player time

	//MouseLLEvent^ ev_prevEvent;		// previous event
	//MouseLLEvent^ ev_nextEvent;		// next event
	//MouseLLEvent^ ev_currentEvent;	// last / current event

	// THIS MAY NOT MATTER
	// update tracking state for playing event change
	bool MouseLL::_update_playEvent_All(MouseLLEvent^ event_sent, MouseLLEvent^ event_sent_next, double tm_new_player)
	{
		// change events
		ev_prevEvent = ev_currentEvent;
		ev_currentEvent = event_sent;
		ev_nextEvent = event_sent_next;

		// change player time
		pl_prevTime = pl_currentTime;
		pl_currentTime = tm_new_player;

		// change event times
		te_prevTime = te_currentTime;	// update previous time
		
		if(event_sent != nullptr)		// update current time if not nullptr
			te_currentTime = event_sent->getTimestamp();
		else
			// nullptr, check event next
			te_currentTime = -1.0;

		if(event_sent_next!= nullptr)  // update current next time if not nullptr
			te_nextTime = event_sent_next->getTimestamp();
		else
			te_nextTime = -1.0;

		return true;
	}

#pragma endregion


#pragma region new event and setup tracking variable functions
	// change tracking for new event
	// iLocation: -1 for before firsEvent, 1 for after lastEvent, 0 for between events
	// NOTE: new event may just be update of first or last event
	bool MouseLL::update_newEvent(MouseLLEvent^ newEvent, double tm_new, int iLocation)
	{
		// newEvent == ev_currentEvent then just updated parameters
		if(newEvent != ev_currentEvent)
		{
			// actual new event - firstEvent and lastEvent will already be updated
			switch( iLocation) 
			{
			case	-1:	// new before first
				{
					QuickMsgBox::QTrace("update_newEvent: before first, time {0}\n",tm_new);
					// events
					ev_prevEvent = ev_currentEvent;
					ev_currentEvent = newEvent;
					ev_nextEvent = ev_currentEvent->getNextEvent();
					// event times
					te_prevTime = te_currentTime;
					te_currentTime = tm_new;
					te_nextTime = ev_nextEvent->getTimestamp();
						
				}
			case	 1:	// new after last
				{
					QuickMsgBox::QTrace("update_newEvent: after last, time {0}\n",tm_new);
					// events
					ev_prevEvent = ev_currentEvent;
					ev_currentEvent = newEvent;
					ev_nextEvent = nullptr;  // new last
					// event times
					te_prevTime = te_currentTime;
					te_currentTime = tm_new;
					te_nextTime = -1.0;		// no next time
				}
			case	 0:	// new in the middle of two events
				//System::Diagnostics::Trace::WriteLine( "PLACEHOLDER: Updating for new event between existing");
				QuickMsgBox::QTrace( "PLACEHOLDER: Updating for new event between existing");
				//{
				//	ev_currentEvent = newEvent;
				//}
			default :
				QuickMsgBox::QTrace("update_newEvent: before first, time {0}\n",tm_new);
				//System::Diagnostics::Trace::WriteLine( "Default on location of new event in switch in update_newEvent");
				QuickMsgBox::QTrace( "Default on location of new event in switch in update_newEvent");
			}

			// update time
			pl_prevTime = pl_currentTime;
			pl_currentTime = tm_new;

			//double pl_currentTime; // current player time
			//double pl_prevTime;    // previous player time
			//
			//// event updates
			//double te_currentTime;  // time at or past this, not to next yet
			//double te_nextTime;     // next event time
			//double te_prevTime;		// previous event time, after moving (not from playing forward)
			//
			//double pl_currentTime; // current player time
			//double pl_prevTime;    // previous player time
			//
			//MouseLLEvent^ ev_prevEvent;		// previous event
			//MouseLLEvent^ ev_nextEvent;		// next event
			//MouseLLEvent^ ev_currentEvent;	// last / current event

		}

		return true;
	}

	// set up event tracking variables when new without record
	bool MouseLL::setup_loadNew(void)
	{
		// constructor for new
		// MouseLL(System::String^ newSeqName, System::String^ currentMovieURL, double currentMovieSecs, double currentMoviePosition);
		QuickMsgBox::QTrace("in setup_loadNew\n");	// QQQQQQQ
		// should be no events
		if(Count != 0)
		{
			QuickMsgBox::MBox(L"Started New sequence with {0} existing events", Count);
		}
		DEBUG_TRACKING();
		return true;
	}

	// set up event tracking variables after first event with record
	bool MouseLL::setup_loadRecordStart(void)
	{
		// constructor for start with record
		//	MouseLL(System::String^ newSeqName, System::String^ currentMovieURL, double currentMovieSecs, 
		//		double dNewTime, bool bNewFeed, int iNewArm);

		QuickMsgBox::QTrace("in setup_loadRecordStart\n");	// QQQQQQQ
		if(Count != 1)
		{
			QuickMsgBox::MBox(L"Started New sequence by first recording with {0} existing events (should be 1)", Count);
		}

		// make sure we have events
		if(firstEvent == nullptr)
		{
			QuickMsgBox::MBox(L"Started New Recording sequence with nullptr as firstEvent", Count);
			return false;
		}

		// ------- updating with one event, *should* be at actual time
		// event updates
		// ev_prevEvent;		
		// ev_nextEvent;		// only event so no next
		ev_currentEvent = firstEvent;	// last / current event

		// time updates
		te_currentTime = firstEvent->getTimestamp();  // time at or past this, not to next yet
		//te_nextTime;      // no next event
		//te_prevTime;		// no prior events

		// player time update from constructor


		DEBUG_TRACKING();
		return true;
		
	}

	// set up after load
	bool MouseLL::setup_loadSequence(void)
	{
		// make sure we have events
		if(firstEvent == nullptr)
		{
			System::Windows::Forms::MessageBox::Show("setup_loadSequence: no events (firstEvent==nullptr)");
			return false;
		}

		// ------------------------------ check if only one event
		if(Count == 1)
		{
			// ---- only one event
			// check if current player time is before ot after
			if(_test_TimeAfterEvent(pl_currentTime, firstEvent))
			{
				// after, so set current event, reset next
				ev_currentEvent = firstEvent;
				ev_nextEvent = nullptr;
				// player time is after only event
				te_currentTime = ev_currentEvent->getTimestamp();
				// no next, so not setting ev_nextEvent
				te_nextTime = -1.0;
			}else
			{
				// player time is before only event
				te_nextTime = ev_currentEvent->getTimestamp(); // next event time is firstEvent time
				// set next event to firstEvent, reset current
				ev_nextEvent = firstEvent;
				ev_currentEvent = nullptr;
				// reset current
				te_currentTime = -1.0;
			}
		}else
		{
			// ---- more than one event
			// search by time, no node to guide	
			bool bSearchOK = false;
			if( (bSearchOK = _search_NoPreviousNode(pl_currentTime, ev_currentEvent, ev_nextEvent)) )
			{
				//possible:
				//	time before firstEvent
				//  time after lastEvent
				// time between nodes

				// check if time is before firstEvent
				if(ev_currentEvent == nullptr)
				{
					// set next time, reset current time
					te_nextTime = ev_nextEvent->getTimestamp();
					te_currentTime = -1.0;
				}else
				{
					// check if current is last
					if(ev_nextEvent == nullptr)
					{
						// next time only if not past last, reset next time
						te_currentTime = ev_currentEvent->getTimestamp();
						te_nextTime = -1.0;
					}else
					{
						// after first, before last
						te_currentTime = ev_currentEvent->getTimestamp();
						te_nextTime = ev_nextEvent->getTimestamp();
					}
				}
			}else
				return false; // error searching
			
		}
		
		DEBUG_TRACKING();
		return true;
	}
#pragma endregion

System::Void MouseLL::DEBUG_TRACKING(void)
{
	//// event updates
	//double te_currentTime;  // time at or past this, not to next yet
	//double te_nextTime;     // next event time
	//double te_prevTime;		// previous event time, after moving (not from playing forward)

	//double pl_currentTime; // current player time
	//double pl_prevTime;    // previous player time

	//MouseLLEvent^ ev_prevEvent;		// previous event
	//MouseLLEvent^ ev_nextEvent;		// next event
	//MouseLLEvent^ ev_currentEvent;	// last / current event

	System::String^ strOut;
	System::String^ strAppend;

	strOut = "\n";
	// player times
	strAppend = System::String::Format("pl_currentTime = {0}\n", pl_currentTime);
	strOut = System::String::Concat(strOut, strAppend);
	strAppend = System::String::Format("pl_prevTime = {0}\n\n", pl_prevTime);
	strOut = System::String::Concat(strOut, strAppend);
	
	// tracking times
	strAppend = System::String::Format("te_currentTime = {0}\n", te_currentTime);
	strOut = System::String::Concat(strOut, strAppend);
	strAppend = System::String::Format("te_nextTime = {0}\n", te_nextTime);
	strOut = System::String::Concat(strOut, strAppend);
	strAppend = System::String::Format("te_prevTime = {0}\n\n", te_prevTime);
	strOut = System::String::Concat(strOut, strAppend);

	// --- events
	// ev_currentEvent
	if(ev_currentEvent != nullptr)
	{
		strAppend = System::String::Format("ev_currentEvent: {0} :", ev_currentEvent);
		strOut = System::String::Concat(strOut, strAppend);
		strAppend = System::String::Format("==firstEvent = {0}, ==lastEvent = {1})\n", ev_currentEvent==firstEvent, ev_currentEvent==lastEvent);
		strOut = System::String::Concat(strOut, strAppend);
	}
	else
	{
		strAppend = System::String::Format("ev_currentEvent = NULL\n");
		strOut = System::String::Concat(strOut, strAppend);
	}

	// ev_nextEvent
	if(ev_nextEvent != nullptr)
	{
		strAppend = System::String::Format("ev_nextEvent: {0} :", ev_nextEvent);
		strOut = System::String::Concat(strOut, strAppend);
		strAppend = System::String::Format("==firstEvent = {0}, ==lastEvent = {1})\n", ev_nextEvent==firstEvent, ev_nextEvent==lastEvent);
		strOut = System::String::Concat(strOut, strAppend);
	}
	else
	{
		strAppend = System::String::Format("ev_nextEvent = NULL\n");
		strOut = System::String::Concat(strOut, strAppend);
	}
	// ev_prevEvent
	if(ev_prevEvent != nullptr)
	{
		strAppend = System::String::Format("ev_prevEvent: {0} :", ev_prevEvent);
		strOut = System::String::Concat(strOut, strAppend);
		strAppend = System::String::Format("==firstEvent = {0}, ==lastEvent = {1})\n", ev_prevEvent==firstEvent, ev_prevEvent==lastEvent);
		strOut = System::String::Concat(strOut, strAppend);
	}
	else
	{
		strAppend = System::String::Format("ev_currentEvent = NULL\n");
		strOut = System::String::Concat(strOut, strAppend);
	}

	System::Diagnostics::Trace::WriteLine(strOut);

}