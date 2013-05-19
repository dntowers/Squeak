#include "StdAfx.h"
#include "MouseLL.h"
#include <math.h>

MouseLLEvent::MouseLLEvent(void)
{

}

MouseLLEvent::MouseLLEvent(double dNewTime, bool bNewFeed, int iNewArm, MouseLLEvent^ new_prevEvent, MouseLLEvent^ new_nextEvent)
{
	// values
	dTimestamp = dNewTime;
	iArm = iNewArm;
	bFed = bNewFeed;
	// pointers
	prevEvent = new_prevEvent;
	nextEvent = new_nextEvent;
}
// populate grid row
void MouseLLEvent::PopulateGridRow(array<System::String^>^ rowDataArray)
{
	// convert time to movie stamp
	double dMinutes = floor(dTimestamp / 60);
	double dSeconds = fmod(dTimestamp, 60);
	// add data
	rowDataArray[0] = System::String::Format( "{0}:{1}",dMinutes,dSeconds.ToString("00.00"));
	rowDataArray[1] = System::String::Format("{0}",iArm);
	rowDataArray[2] = System::String::Format("{0}",bFed);
}

#pragma region MouseLL constructors
MouseLL::MouseLL(void)
{
}

MouseLL::MouseLL(System::String^ currentMovieURL, double currentMovieSecs)
{
	// movie name
	movieURL = currentMovieURL;
	// max movie seconds
	dMaxMovieSecs = currentMovieSecs;
}

MouseLL::MouseLL(System::String^ newSeqName, System::String^ currentMovieURL, double currentMovieSecs)
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

}
#pragma endregion
// buttons
#pragma region MouseLL add event
	// add new event
	bool MouseLL::addEvent(double dNewTime, bool bNewFeed, int iNewArm, int newEventID)
	{
		if(Count == 0)
		{
			// create event
			MouseLLEvent^ newLLEvent = _createEvent(dNewTime, bNewFeed, iNewArm, nullptr, nullptr, newEventID);
			// set internal pointers
			firstEvent = newLLEvent;
			lastEvent = newLLEvent;
			// set count
			Count++;
			// set recent
			recentEvent = newLLEvent;
		}else
		{
			// check if time is before or at first
			if(IsBeforeOrAtFirst(dNewTime))
			{
				// inserted before, or if same time replaced arm and fed
				if(addFirstEvent(dNewTime, bNewFeed, iNewArm, newEventID))
					Count++;
			}else
			{
				// check if time is after or at last
				if(IsAfterOrAtLast(dNewTime))
				{
					// inserted after, or if same time replaced arm and fed
					if(addLastEvent(dNewTime, bNewFeed, iNewArm, newEventID))
						Count++;
				}else
				{
					// between current first and last
				}
			}
		}
		// DEBUG
		// System::Diagnostics::Trace::WriteLine( recentEvent->ToString() );
		// END DEBUG 
		IsDirty = true;
		return true;
	}
	// add event to beginning
	bool MouseLL::addFirstEvent(double dNewTime, bool bNewFeed, int iNewArm, int newEventID)
	{
		bool bReturn = false;
		// check if new event time is before first event
		if(firstEvent->IsDifferentTime(dNewTime))
		{
			// not the same time stamp, so create new: _createEvent(...,new_prevEvent, new_nextEvent);
			MouseLLEvent^ newEvent = _createEvent(dNewTime, bNewFeed, iNewArm, nullptr, firstEvent, newEventID); //new next = current first
			if(newEvent != nullptr)
			{
				// success:
				firstEvent->setPrevEvent(newEvent); // set previous event pointer of current first to new first
				firstEvent = newEvent;				// set first event pointer to new event
				// set recent
				recentEvent = newEvent;
				bReturn =  true;
			}
			else
				bReturn = false; // something went wrong
		}else
		{
			// timestamp is the same
			_replaceEventParams(bNewFeed, iNewArm, firstEvent);
			// set recent
			recentEvent = firstEvent;
			bReturn =  true;
		}
		return bReturn;
		
	}
	
	// add last to beginning
	bool MouseLL::addLastEvent(double dNewTime, bool bNewFeed, int iNewArm, int newEventID)
	{
		bool bReturn = false;
		// check if new event time is before first event
		if(lastEvent->IsDifferentTime(dNewTime))
		{
			// not the same time stamp, so create new: _createEvent(...,new_prevEvent, new_nextEvent);
			MouseLLEvent^ newEvent = _createEvent(dNewTime, bNewFeed, iNewArm, lastEvent, nullptr, newEventID); //new previous = current last
			if(newEvent != nullptr)
			{
				// success:
				lastEvent->setNextEvent(newEvent); // set next event pointer of current last to new last
				lastEvent = newEvent;			   // set first event pointer to new event
				// set recent
				recentEvent = newEvent;
				bReturn =  true;
			}
			else
				bReturn = false; // something went wrong
		}else
		{
			// timestamp is the same
			_replaceEventParams(bNewFeed, iNewArm, lastEvent);
			// set recent
			recentEvent = lastEvent;
			bReturn =  true;
		}
		return bReturn;
	}
	
	// create a new event
	MouseLLEvent^ MouseLL::_createEvent(double dNewTime, bool bNewFeed, int iNewArm, MouseLLEvent^ new_prevEvent, MouseLLEvent^ new_nextEvent, int newEventID)
	{
			// create new event
			MouseLLEvent^ newEvent = gcnew MouseLLEvent(dNewTime, bNewFeed, iNewArm, new_prevEvent, new_nextEvent, newEventID);
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
		newEvent = gcnew MouseLLEvent(dTimestampArray[iEventIndex], bFedArray[iEventIndex], iArmArray[iEventIndex], nullptr, nullptr);
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
			newEvent = gcnew MouseLLEvent(dTimestampArray[iEventIndex], bFedArray[iEventIndex], iArmArray[iEventIndex], prevEvent, nullptr);
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
	int MouseLL::PopulateDataGrid(System::Windows::Forms::DataGridView^ dataGridEvents)
	{
		int iAdded = 0;
		array<System::String^>^rowData = gcnew array<System::String^>(3);
		MouseLLEvent^ currentEvent = firstEvent;
		
		// iterate through list
		while(currentEvent != nullptr)
		{
			// add strings to array
			currentEvent->PopulateGridRow(rowData);
			// add data to row
			dataGridEvents->Rows->Add(rowData);
			// get pointer to next event
			currentEvent = currentEvent->getNextEvent();
			// inc count
			iAdded++;
		}
		return iAdded;
	}

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