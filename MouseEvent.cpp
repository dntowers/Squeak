#include "StdAfx.h"
//#include <windows.h>
#include "MouseEvent.h"

MouseEvent::MouseEvent(void)
{
	// initialize
	dTimestamp = 0;
	iArm = 0;
	bFed = false;
}


MouseEvent::MouseEvent(double dNewTime, bool bFeedState, int iNewArm)
{
	// initialize with parameters
	dTimestamp = dNewTime;
	iArm = iNewArm;
	bFed = bFeedState;
}



#pragma region MouseEvents constructors
// contructors
MouseEvents::MouseEvents(void)
{
	try
	{
		mouseEvents = gcnew LinkedList<MouseEvent^>;		    // create list
	}
	catch (System::Exception ^e)
	{
		System::Windows::Forms::MessageBox::Show( System::String::Format("Exception at new event linked list: {0}", e) );
	}

	IsDirty = false;
	sequenceName = "";
	eventRecentAdd = nullptr;
}

MouseEvents::MouseEvents(System::String^ newSequenceName)
{
	try
	{
		mouseEvents = gcnew LinkedList<MouseEvent^>;		    // create list
	}
	catch (System::Exception ^e)
	{
		System::Windows::Forms::MessageBox::Show( System::String::Format("Exception at new event linked list: {0}", e) );
	}
	sequenceName = newSequenceName;
	IsDirty = false;
	eventRecentAdd = nullptr;
}

MouseEvents::MouseEvents(System::String^ newSequenceName, double dInitialTime, bool bInitialFed, int iInitialArm)
{
	// MouseEvents::MouseEvents(newSequenceName);
	try
	{
		mouseEvents = gcnew LinkedList<MouseEvent^>;		    // create list
	}
	catch (System::Exception ^e)
	{
		System::Windows::Forms::MessageBox::Show( System::String::Format("Exception at new event linked list: {0}", e) );
	}

	sequenceName = newSequenceName;
	IsDirty = false;
	eventRecentAdd = nullptr;

	// add first event
	eventRecentAdd = addEvent(dInitialTime, bInitialFed, iInitialArm);

}
#pragma endregion


// ----------------------------------------------------------- save, load
#pragma region MouseEvents save and load

// save existing sequence
bool MouseEvents::Save_Sequence(System::String^ strMovieFileName)
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
	saveFileDialogSeq->FileName = sequenceName;

	saveFileDialogSeq->ShowDialog();
	// If the file name is not an empty string, open it for saving.
	if(saveFileDialogSeq->FileName != "")
	{

		// DO SAVE HERE RETURN WHATEVER IS TRUE
		bSuccess = _SaveSequenceData(saveFileDialogSeq->FileName, strMovieFileName);
	}

	// remove dirty flag
	if(bSuccess)
		IsDirty = false;

	// return if everything is OK
	return bSuccess;
}
bool MouseEvents::_SaveSequenceData(System::String^ seqFilePath, System::String^ movieFileURL)
{
	// data saved as cvs:  time, arm, fed
	// first line is sequence, movie file, events

	System::String^ movieFileName = System::IO::Path::GetFileName(movieFileURL);
	System::String^ seqFileName = System::IO::Path::GetFileName(seqFilePath);

	// create string builder
    System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder();

	// add file information
	sb->AppendFormat("{0},{1},{2}\n",seqFileName,movieFileName,mouseEvents->Count);
	// add headers
	sb->Append("Time,Arm,Fed\n");

	// iterate through event nodes
	for each (MouseEvent^ me in mouseEvents)
    {
		sb->Append(me->ToString());
    }


	// --- open & write file
	System::IO::StreamWriter^ outfile = gcnew System::IO::StreamWriter(seqFilePath);
    outfile->Write(sb->ToString());

	// --- close file
	outfile->Close();

	return true;
}
#pragma endregion

// -----------------------------------------------------------------------event change
#pragma region MouseEvents event changes

MouseEvent^ MouseEvents::addEvent(double dNewTime, bool bNewFed, int iNewArm)
{
	

	// create new event
	MouseEvent^ newEvent = gcnew MouseEvent(dNewTime, bNewFed, iNewArm);

	
	// determine if list is empty
	if(mouseEvents->Count == 0)
	{
		// only event
		mouseEvents->AddFirst(newEvent);
		// set to most recent
		eventRecentAdd = newEvent;
		// flag dirty
		IsDirty = true;
		// return event
		return eventRecentAdd;
	}else
	{
		
		// -------------------------------------------------------------------------compare to last event
		int iCompareLast = compareTimeToEvent(dNewTime, mouseEvents->Last->Value);
		if(iCompareLast > 0)
		{
			// new event is *after last* event
			mouseEvents->AddLast(newEvent);
			eventRecentAdd = newEvent;
			IsDirty = true;
			return eventRecentAdd;
		}else
		{
			if(iCompareLast == 0)
			{
				// new event is *at the same time as last*
				delete newEvent; newEvent = nullptr;
				if(setEvent(mouseEvents->Last->Value, dNewTime, bNewFed, iNewArm))
					return mouseEvents->Last->Value;
				else
				{
					// - error -
					System::Windows::Forms::MessageBox::Show( System::String::Format("Error: new event = last event, could not set values"));
					return nullptr;
				}
			}else
			{
				// -------------------------------------------------------------------------compare to first event
				int iCompareFirst = compareTimeToEvent(dNewTime, mouseEvents->Last->Value);
				if(iCompareFirst < 0)
				{
					// new event is *before first*
					mouseEvents->AddFirst(newEvent); 
					eventRecentAdd = newEvent;
					IsDirty = true;
					return eventRecentAdd;
				}else
				{
					if(iCompareFirst == 0)
					{
						// new event is *at the same time as first*
						delete newEvent; newEvent = nullptr;
						if(setEvent(mouseEvents->First->Value, dNewTime, bNewFed, iNewArm))
							return mouseEvents->First->Value;
						else
						{
							// - error -
							System::Windows::Forms::MessageBox::Show( System::String::Format("Error: new event = last event, could not set values"));
							return nullptr;
						}
					}else
					{
						// new event is *in between first and last*
						return nullptr;
					}
				}
			}
		}
			
			//return nullptr;
	}

}
// assigns values to given event
bool MouseEvents::setEvent(MouseEvent^ mEvent, double dNewTime, bool bNewFed, int iNewArm)
{
	if(mEvent != nullptr)
	{
		mEvent->setAll(dNewTime, bNewFed, iNewArm);
		return true;
	}else
		return false;

}

#pragma endregion


#pragma region list information
// first and last event times
bool MouseEvents::getFirstLastTimes(double* p_dFirstTime, double* p_dLastTime)
{
	if(mouseEvents->Count == 0)
		return false;
	else
	{
		*p_dFirstTime = getEventTime(mouseEvents->First->Value);
		*p_dLastTime = getEventTime(mouseEvents->Last->Value);
		return true;
	}
}
// first event time
bool MouseEvents::getFirstTime(double* p_dFirstTime)
{
	if(mouseEvents->Count == 0)
		return false;
	else
	{
		*p_dFirstTime = getEventTime(mouseEvents->First->Value);
		return true;
	}
}
// last event time
bool MouseEvents::getLastTime(double* p_dLastTime)
{
	if(mouseEvents->Count == 0)
		return false;
	else
	{
		*p_dLastTime = getEventTime(mouseEvents->Last->Value);
		return true;
	}
}
// fill arrays with all data in list
bool MouseEvents::getAllEventData(array<double>^ arr_dTimeStamp, array<bool>^ arr_bFed, array<int>^ arr_iArm)
{
			
	int iArraySize = arr_dTimeStamp->Length;

	//array<int>^ iArmArray = gcnew array<int>(iSize);
	//	array<double>^ dTimestampArray = gcnew array<double>(iSize);
	//	array<bool>^ bFedArray = gcnew array<bool>(iSize);

	 //for(int i=0; i<strarr->Length; i++)
  //    Console::WriteLine(strarr[i]);

	if(iArraySize != mouseEvents->Count)
		return false;
	else
	{
		double dCurrentTime;
		bool bCurrentFed;
		int iCurrentArm;
		int iCount = 0;
		for each (MouseEvent^ me in mouseEvents)
		{
			me->getAll(&dCurrentTime, &bCurrentFed, &iCurrentArm);
			arr_dTimeStamp[iCount] = dCurrentTime;
			arr_bFed[iCount] = bCurrentFed;
			arr_iArm[iCount] = iCurrentArm;
			iCount++;
		}
	}
	return true;
}
//bool MouseEvents::getEventDataAtIndex(int iIndex, double* p_dTimeStamp, bool* p_bFed, int* p_iArm)
//{
//	if((iIndex < 0) || (iIndex >= mouseEvents->Count))
//		return false;
//	else
//	{
//		getEventData(mouseEvents->ElementAt(iIndex), p_dTimeStamp, p_bFed, p_iArm);
//	}
//	return true;
//}

#pragma endregion

