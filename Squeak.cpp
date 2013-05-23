// Squeak.cpp : main project file.


#include "stdafx.h"
#include "QuickMsgBox.h"
#include <math.h>
#include "TimeState.h"
#include "BtnArray.h"
#include "GridRowData.h"
#include "MouseLL.h"		// hand made linked list
#include "StateChange.h"
#include "StateChangeLL.h"
#include "FormTextInput.h"
#include "Form1.h"
#include "WTypes.h"

//#define STATE_CHANGE

using namespace Squeak;



[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 
	
	Form1^ mainWindow = gcnew Form1;
	Application::Idle += gcnew System::EventHandler(mainWindow, &Form1::MemberIdle);
	Application::Run(mainWindow);
	// Create the main window and run it
	//Application::Run(gcnew Form1());
	return 0;
}
////System::Void btPlay_Click(System::Object^  sender, System::EventArgs^  e) {
//System::Void Squeak::btPlay_Click(System::Object^  sender, System::EventArgs^  e) {
//	axWindowsMediaPlayer1->Ctlcontrols->play();
//}

// buttons
namespace Squeak
{
	// general text input box
	System::String^ Form1::TextInputBox(System::String^ strPrompt, System::String^ strCaption, System::String^ strDefault)
	{
		System::String^ strOuput;
		// strPrompt = actual prompt, strCaption = displayed at top of box
		FormTextInput^ inBox = gcnew FormTextInput;


		inBox->SetCaption(strCaption);
		inBox->SetPrompt(strPrompt);
		inBox->SetDefault(strDefault);

		if ( inBox->ShowDialog( this ) == ::DialogResult::OK )
		{
	        // Read the contents of testDialog's TextBox.
			strOuput = inBox->GetInput();
		}
		else
		{
			strOuput = L"";
		}
		return strOuput;
	    delete inBox; inBox = nullptr;
	}

#pragma region WMP control
	// pause movie
	void Form1::Pause_Movie(void)
	{
		axWindowsMediaPlayer1->Ctlcontrols->pause();
		SetPlayingControls();
	}
	// play movie
	void Form1::Play_Movie(void)
	{
		axWindowsMediaPlayer1->Ctlcontrols->play();
		SetPlayingControls();
	}
	// change play state
	void Form1::TogglePlayState(void)
	{
		if(WMP_IsPlaying())
			Pause_Movie();
		else
			Play_Movie();
		}
		
	}
	// Set playing controls
	void Form1::SetPlayingControls(void)
	{
		
		if(WMP_IsPlaying())
		{
			btPlay->Text = L"Pause (0)";
			EnableGrid(false);
		} else
		{
			btPlay->Text = L"Play (0)";
			EnableGrid(true);
		}
	}
#pragma endregion

#pragma region Button movement
	// -- handle button indicator movement
	// pressed an indicator button
	System::Void Form1::DirectionButtonPressed(System::Windows::Forms::Button^ btnPressed)
	{
		if(!(cb_lockButtons->Checked)) // make sure buttons are not locked
		{
			// not currently in button move mode
			if(btnSelected == nullptr)
			{
				btnSelected = btnPressed;
				// button has been selected
				if(WMP_IsPlaying())
				{ 			
					// playing, so stop
					Pause_Movie();
				}
				//TogglePlayState();
				//tbStateChange->Text = btnSelected->Text;

				// change background color
				btnPressed->BackColor = colorButtonMoving;
			}else
			{
				if(btnPressed == btnSelected) // check if same button was pressed
				{	
					// get out of button move mode
					btnSelected = nullptr;
					btnPressed->BackColor = colorButtonNormal;
				}
			}
		}
	}
	// clicked mouse in movie player
	System::Void Form1::WMP_Mouse_Pressed(AxWMPLib::_WMPOCXEvents_MouseDownEvent^  e){
		Point formPos = PointToClient(Control::MousePosition);
		MoveDirectionButton(formPos);
	}
	// clicked outside movie player
	System::Void Form1::Form_Mouse_Pressed(System::Windows::Forms::MouseEventArgs^  e){
		//System::String^ strStatus;
		if(e->Button == ::MouseButtons::Left)
		{
			//strStatus = String::Format("FORM: Mouse Down at [{0},{1}]", e->X, e->Y);
			//tbStateChange->Text = strStatus;
			Point formPos = Point(e->X, e->Y);
			MoveDirectionButton(formPos);
		}
	}
	// move the button
	System::Void Form1::MoveDirectionButton(Drawing::Point btnNewLocation){
		// make sure a button is available
		if(btnSelected == nullptr)
		{
			//tbStateChange->Text = L"Tried to move button with null pointer";
		}else
		{
			//System::String^ strStatus;
			//strStatus = String::Format("New Location [{0},{1}]", btnNewLocation.X, btnNewLocation.Y);
			//tbStateChange->Text = strStatus;
			btnSelected->Location = btnNewLocation;
		}
	}
#pragma endregion
#pragma region Key press
	// Keypress
	System::Void Form1::Form1_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
	{
		// check if form handled it
		if ( bKeyHandled == true )
        {         
  
           e->Handled = true;
        }

	}
	System::Void Form1::Form1_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
	{

		// set flag
		bKeyHandled = false;

	    switch (e->KeyCode)
        {
			case Keys::Enter:		// trap enter
				bKeyHandled = true;
				break;
			case Keys::NumPad0:
				TogglePlayState();	// toggle WMP play state
				bKeyHandled = true;
				break;
			case Keys::Divide:		// toggle event recording
				ToggleRecording();
				bKeyHandled = true;
				break;
			case Keys::Multiply:
				if(e->Shift)
				{
					bool bClearOK = false;
					// pressed Shift-Multiply to clear all buttons
					if(mouseLL == nullptr)
					{
						bClearOK = true;
					}else
					{
						if(mouseLL->get_Count() == 0)
							bClearOK = true;
					}
					if(bClearOK)
						_StateSetButtonStateNone();
					else
						QuickMsgBox::MBox("Cannot switch to no event state when events exist - try removing events");
				}
				break;
			default:
				// check if number pad
				if ( e->KeyCode >= Keys::NumPad1 && e->KeyCode <= Keys::NumPad9 )
				{
					_KeyboardSetButtonState(e);
					//NumPadDown(e);
				}else
				{
					bKeyHandled = true;
				}
				break;
        }

	}
#pragma endregion
#pragma region new video
	// load video file
	System::Void Form1::LoadVideoFile(System::String^ strFileName)
	{
		// load video file
		axWindowsMediaPlayer1->URL = strFileName;
		// set controls
		SetPlayingControls();
		// add name to text box
		textBoxVideoFile->Text = openFileDialogVideo->FileName;
		// get position
		dWmpPosition = WMP_GetPosition();
	}
#pragma endregion


#pragma region Sequence Interaction
	// update the first, last event times on form based on sequence
	System::Void Form1::UpdateFormEventTimes(void)
	{
		bool bHaveEvents = false;
		// check if there is a sequence
		if(mouseLL != nullptr)
		{
			if(mouseLL->get_Count() > 0)
			{
				double mov_firstTime = -1;
				double mov_lastTime = -1;
				if(mouseLL->getFirstLastTimes(&mov_firstTime, &mov_lastTime))
				{
					// update text boxes in MM:SS:ss format
					eventFirstTextBox->Text = MovieTimeToString(mov_firstTime);
					eventLastTextBox->Text = MovieTimeToString(mov_lastTime);
					bHaveEvents = true;
				}
			}
		}
		if(!bHaveEvents) // clear the boxes
		{
			eventFirstTextBox->Text = "";
			eventLastTextBox->Text = "";
		}
	}
#pragma endregion

// Load, Save, New sequence
namespace Squeak
{
	// create new sequence
	bool Form1::Sequence_New(bool bFromRecordButton)
	{
		// check if movie is "connected"
		if(WMP_Duration() == 0)
		{
			MessageBox::Show("Movie must be loaded before new sequence can be loaded - try playing movie briefly");
			return false;
		}
		Pause_Movie();
		bool bAddedOK = false;
		bool bCancel = false;
		if(Sequence_OK_New(&bCancel))
		{
			// create a default name
			System::String^ strDefaultName = DefaultSeriesName();
			System::String^ strNewName = TextInputBox(L"Enter New Sequence Name", "Squeak", strDefaultName);

			if(!(String::IsNullOrEmpty(strNewName)))
			{

				// no sequence - create list, check recording or if started by record button
				if(bRecording || bFromRecordButton)
				{
					// IMPLEMENT NEW SEQUENCE - RECORDING
					double dTime = WMP_GetPosition();
					bAddedOK = true;
					try
					{
						// make sure we are in an event state
						if(iCurrentKey > 0)
							mouseLL = gcnew MouseLL(strNewName, axWindowsMediaPlayer1->URL, WMP_Duration(), dTime, btnArray[iCurrentKey]->bFeeding, iCurrentKey);
						else
							mouseLL = gcnew MouseLL(strNewName, axWindowsMediaPlayer1->URL, WMP_Duration(), WMP_GetPosition());
					}
					catch (OutOfMemoryException ^e)
					{
						MessageBox::Show( String::Format("New MouseLL: OutOfMemoryException Handler: {0}", e) );
						bAddedOK = false;
					}
					// set up tracking for new events with first record 
					// *** we will only be here when record is started without a sequence!!!
					// check if we actually have an event
					if(iCurrentKey > 0)	// check
					{
						// there is an event - ADD HERE instead of constructor, so event tracking not updated by addEvent in constructor, then AGAIN
						//		by setup_loadRecordStart
						//QuickMsgBox::QTrace("sending addEvent: record on with event\n");	// QQQQQQQ
						mouseLL->addEvent(dTime, btnArray[iCurrentKey]->bFeeding, iCurrentKey, true); // true = start from record

						//QuickMsgBox::QTrace("sending setup_loadRecordStart: record on with event\n");	// QQQQQQQ
						mouseLL->setup_loadRecordStart();
					}
					else
					{
						// no event, treat as new
						mouseLL->setup_loadNew();
					}
					// IMPLEMENT NEW STATE CHANCE STACK ???
					#ifdef STATE_CHANGE
						try
						{
							if(iCurrentKey > 0)
								stateLL = gcnew StateChangeLL();
						}
						catch (OutOfMemoryException ^e)
						{
							MessageBox::Show( String::Format("New StateChangeLL: OutOfMemoryException Handler: {0}", e) );
							bAddedOK = false;
						}
					#endif
					
					// check both added OK and there was an event	
					if(bAddedOK && (iCurrentKey > 0))
					{
						UpdateFormEventTimes(); // update form times, this event is the first and last
						UpdateGridData();

						// add the new single event
						#ifdef STATE_CHANGE
								// add load sequence state change
								stateLL->add_event_newSequence(mouseLL->getFirstEvent());
						#endif
					}
				}else
				{
					// IMPLEMENT NEW SEQUENCE - NOT RECORDING
					bAddedOK = true;
					try
					{
						mouseLL = gcnew MouseLL(strNewName, axWindowsMediaPlayer1->URL, WMP_Duration(), WMP_GetPosition());
					}
					catch (OutOfMemoryException ^e)
					{
						MessageBox::Show( String::Format("New MouseLL: OutOfMemoryException Handler: {0}", e) );
						bAddedOK = false;
					}
					// set up for new events
					mouseLL->setup_loadNew();

					UpdateFormEventTimes(); // update form times, this event is the first and last
					UpdateGridData();

					// IMPLEMENT NEW STATE CHANCE STACK ???
					#ifdef STATE_CHANGE
						try
						{
							stateLL = gcnew StateChangeLL();
						}
						catch (OutOfMemoryException ^e)
						{
							MessageBox::Show( String::Format("New StateChangeLL: OutOfMemoryException Handler: {0}", e) );
							bAddedOK = false;
						}
					#endif

					
				}
				// add to sequence name text box
				if(bAddedOK)
				{
					textBoxSeqName->Text = strNewName;
					btnSequenceSave->Enabled = true;
				}
			}else
			{
				// empty string name
				bAddedOK = false;
			}
		}
		return bAddedOK;
	}

	// load sequence
	System::Void Form1::Sequence_Load(System::String^ strSeqLoadFile)
	{
		bool bCancel = false;
		// check if movie is "connected"
		if(WMP_Duration() == 0)
		{
			MessageBox::Show("Movie must be loaded before new sequence can be loaded - try playing movie briefly");
			return;
		}
		// pause before loading
		Pause_Movie();
		// check OK for new sequenc
		if(Sequence_OK_New(&bCancel))
		{
			// DEVELOP - autload sequence name
			#ifdef AUTOSEQ 
				strSeqLoadFile = AUTOSEQ;
			#endif

			// path to load - NOTE: Included for autorun
			if(String::IsNullOrEmpty(strSeqLoadFile))
			{
				System::String^ strSeqLoadFile = _LoadSeqDialog();
			}else
				

			if(strSeqLoadFile->Length > 0)
			{
				bool bAddedOK = true;

				// ---- have file name!	
				// need new sequence
				if(mouseLL != nullptr)
				{
					delete mouseLL;
				}
				try
				{
					mouseLL = gcnew MouseLL(axWindowsMediaPlayer1->URL, WMP_Duration(), WMP_GetPosition());
				}
				catch (OutOfMemoryException ^e)
				{
					MessageBox::Show( String::Format("New MouseLL: OutOfMemoryException Handler: {0}", e) );
					bAddedOK = false;
				}
				// IMPLEMENT NEW STATE CHANCE STACK ???
				#ifdef STATE_CHANGE
					try
					{
						stateLL = gcnew StateChangeLL();
					}
					catch (OutOfMemoryException ^e)
					{
						MessageBox::Show( String::Format("New StateChangeLL: OutOfMemoryException Handler: {0}", e) );
						bAddedOK = false;
					}
				#endif

				// IMPLEMENT LOAD SEQUENCE
				if(bAddedOK)
				{
					if(mouseLL->Load_Sequence(strSeqLoadFile))
					{
						// set up tracking
						mouseLL->setup_loadSequence();
						// update grid data
						UpdateGridData();
						// update first, last events
						UpdateFormEventTimes();
						// update save button?									
						btnSequenceSave->Enabled = true;
						// update series name box
						textBoxSeqName->Text = mouseLL->getSequenceName();
						#ifdef STATE_CHANGE
							// add load sequence state change
							MouseLLEvent^ firstEvent = mouseLL->getFirstEvent();
							MouseLLEvent^ lastEvent = mouseLL->getLastEvent();
							stateLL->add_event_loadSequence(firstEvent, lastEvent, firstEvent->getNextEvent());
						#endif
					}else
					{
						// fail, destroy sequence
						MessageBox::Show("DELETING");
						delete mouseLL;
						mouseLL = nullptr;
						#ifdef STATE_CHANGE
							delete stateLL;
							stateLL = nullptr;
						#endif

					}
				}
			}
		}
	}
	// save sequence
	System::Void Form1::Sequence_Save(void)
	{
		// save sequence data
		bool bSaveOK = mouseLL->Save_Sequence();
	}

	// check if dirty, save or overwrite
	bool Form1::Sequence_OK_New(bool* p_bCancel)
	{
		bool bOK_New = true;
		bool bCancel = false;	// this is here for cancelling action
		// check if there is video
		if(WMP_HasVideo())
		{
			// check if there is a sequence
			if(mouseLL != nullptr)
			{
				// sequence exists - check if sequence is dirty
				if(mouseLL->get_IsDirty())
				{
					// sequence is dirty, check for save
					bOK_New = Sequence_CheckOverwrite(p_bCancel);
				}
			}
		}else
		{
			// no video
			MessageBox::Show("Cannot have sequence without video loaded");
			bOK_New = false;
		}
		// return status
		return bOK_New;
	}
	// ask user to save, overwrite, or cancel
	bool Form1::Sequence_CheckOverwrite(bool* p_bCancel)
	{
		bool bOverwriteOK = false;
	       String^ message = "There are unsaved events. Save the current sequence?";
           String^ caption = "Squeak";
           System::Windows::Forms::DialogResult result;
  
           // Displays the MessageBox.
  
           result = MessageBox::Show(message, caption, MessageBoxButtons::YesNoCancel, MessageBoxIcon::Question );
           if( result == ::DialogResult::No )
           {
			   bOverwriteOK = true;  // overwrite without saving
		   }else
		   {
			   if( result == ::DialogResult::Yes )
			   {
				   // SAVE CURRENT STATE, IF SUCCESSFUL THEN OK OVERWRITE
				   //bOverwriteOK = mouseEvents->Save_Sequence(axWindowsMediaPlayer1->URL); // OLD SEQUENCE
				   bOverwriteOK = mouseLL->Save_Sequence();
			   }else
			   {
				   if( result == ::DialogResult::Cancel )
				   {
					   // do not overwrite, and if it applies, cancel action
					   *p_bCancel = true;
					   bOverwriteOK = false;
				   }
			   }
		   }

		   return bOverwriteOK;
	}
	// create default series name out of movie file name
	System::String^ Form1::DefaultSeriesName()
	{
		// get movie file name no extension
		System::String^ movieName = System::IO::Path::GetFileNameWithoutExtension(axWindowsMediaPlayer1->URL);
		// replace blanks with underscore
		return movieName->Replace(' ','_');		
	}
	
	// dialog to load sequence
	System::String^ Form1::_LoadSeqDialog()
	{
		System::String^ strSeqLoadFile = L"";
		//System::Windows::Forms::OpenFileDialog^ openFileDialogSeq = gcnew System::Windows::Forms::OpenFileDialog;
		OpenFileDialog^ openFileDialogSeq = gcnew OpenFileDialog;
        openFileDialogSeq->InitialDirectory = System::Environment::GetFolderPath( System::Environment::SpecialFolder::Desktop );
        openFileDialogSeq->Filter = "Comma Separated (.csv)|*.csv|All files (*.*)|*.*";
		openFileDialogSeq->Title = "Load Event Sequence";
        openFileDialogSeq->FilterIndex = 1;
        openFileDialogSeq->RestoreDirectory = true;
  

        if ( openFileDialogSeq->ShowDialog() == ::DialogResult::OK )
			strSeqLoadFile = openFileDialogSeq->FileName;

		return strSeqLoadFile;
	}
}

// Mouse Events
namespace Squeak
{
	// toggle record/play events mode
	System::Void Form1::ToggleRecording(void)
	{
		// toggle recording state
		if(!bRecording) // check if playing
		{
			// playing, check if mouse event structur
			if(mouseLL == nullptr)
			{
				// create new sequence
				if(Sequence_New(true))
				{					
					bRecording = true;
					EnableGrid(false);
				}
			}else
			{
				bRecording = true;
				EnableGrid(false);
			}
		}else
		{
			// recording, change to playing
			Pause_Movie();
			bRecording = false;
			UpdateGridData();
			EnableGrid(true);
		}

		// change appearance
		if(bRecording)
		{
			btnRecordEvents->Text = L"Play\r\nEvents (/)";
			shapeRecording->FillColor = System::Drawing::Color::Red;
			labelRecordStatus->Text = L"Recording";
			labelRecordStatus->ForeColor = System::Drawing::Color::Red;
		}else
		{
			btnRecordEvents->Text = L"Record\r\nEvents (/)";
			shapeRecording->FillColor = System::Drawing::Color::Black;
			labelRecordStatus->Text = L"Playing";
			labelRecordStatus->ForeColor = System::Drawing::Color::Black;
		}
	}
}
// Time Event
namespace Squeak
{
	// called when timer ticks
	System::Void Form1::TimerTick(System::EventArgs^  e)
	{
		// ------ check position
		double dNewWMPPosition; // current position

		// get current position
		dNewWMPPosition = WMP_GetPosition();
		// check if different than last position
		if(dNewWMPPosition != dWmpPosition)
		{
			// -----------------------------------change in position of WMP
			// update time string
			dWmpPosition = dNewWMPPosition;
			if(WMP_IsPlaying()) //tbStateChange->Text = MovieTimeToString(dWmpPosition);
				Update_Time_Box(dWmpPosition, "Form1::TimerTick");

			// --- update events
			if(!bRecording)
			{
				if(mouseLL != nullptr)
				{
					bool bNoState = true;
					// call only when player is running
					if(WMP_IsPlaying())
					{
						MouseLLEvent^ me_sent = mouseLL->playEvent_All(dWmpPosition, &bNoState);
						_UpdateFrom_PlayEvent(me_sent, bNoState);
					}
				}
			}
		}

	}

}

// called after checking event change state in MouseLL
System::Void Form1::_UpdateFrom_PlayEvent(MouseLLEvent^ m_event, bool bNoState)
{
	if(m_event != nullptr)
	{
		// new event - update buttons
		_StateSetButtonState(m_event->getArm(), m_event->getFed());
		// update grdi selection
		UpdateGridData_SelectionFromEvent(m_event);
	}else
	{
		// sent null pointer, either not new state or not state at all
		if(bNoState)
		{
			// no current state!
			_StateSetButtonStateNone();
		}
	}
}

// arm indicator change events
namespace Squeak
{
	// set button from key events
	System::Void Form1::_KeyboardSetButtonState(System::Windows::Forms::KeyEventArgs^  e)
	{
		// check whether control was pressed (feeding)
		bool bControl = e->Control;

		// iterate through buttons
		int iButtonIndex = 0;
		int iButtonOn = -1;

		while((iButtonIndex < 10) && (iButtonOn < 0))
		{
			if(e->KeyCode == btnArray[iButtonIndex]->btnKeyCode)
				iButtonOn = iButtonIndex;
			// iterate index
			iButtonIndex++;
		}

		// set the button state, pass whether recording
		_SetButtonState(iButtonOn, bControl, bRecording);
	}

	// set button from forced state
	System::Void Form1::_StateSetButtonState(int iArm, bool bFeeding)
	{
		// set the button state, do not allow recording when passed state
		_SetButtonState(iArm, bFeeding, false);


	}
	// currently in no state (e.g. no events, or before first)
	System::Void Form1::_StateSetButtonStateNone(void)
	{
		_SetButtonState(-1, false, false); // reset all buttons
	}
	// sets visible button state (bControl == feeding), e = nullptr if not user generated
	System::Void Form1::_SetButtonState(int iArm, bool bFeeding, bool bRecording)
	{
		Color newColor;
		bool bIsFeeding;
		int iButtonIndex;
		// check if this is a state change
		bool bNewFeeding;	// need to do this in case no arm (no event)
		if(iArm > 0)
			bNewFeeding = btnArray[iArm]->bFeeding;
		else
			bNewFeeding = false;
		if((iCurrentKey != iArm) || (bFeeding != bNewFeeding))
		{
			// change from current state
			// iterate through buttons
			for(iButtonIndex = 0; iButtonIndex < 10; iButtonIndex++)
			{
				// check if this matches arm
				if(iButtonIndex == iArm)	// should nevet match if no event
				{
					// button matches arm
					btnArray[iButtonIndex]->bOn = true;
					// check if actual button
					if(btnArray[iButtonIndex]->btn != nullptr)
					{
						// represents actual button - turn on
						if(bFeeding)
						{
							// feeding state
							newColor = colorButtonFeeding;
							bIsFeeding = true;
						}else
						{
							// not feeding state
							newColor = colorButtonOn;
							bIsFeeding = false;
						}
						// set button params
						btnArray[iButtonIndex]->btn->BackColor = newColor;
						btnArray[iButtonIndex]->bFeeding = bIsFeeding;
					}
				}else
				{
					// no match - turn off
					btnArray[iButtonIndex]->bOn = false; // turn off
					btnArray[iButtonIndex]->bFeeding = false;
					if(btnArray[iButtonIndex]->btn != nullptr)
						btnArray[iButtonIndex]->btn->BackColor = colorButtonNormal;

				} // end if/then/else for button - arm
				
			} // end iteration through buttons

		} // end check for state change	

		// ----- set as current key
		iCurrentKey = iArm;


		//  ---------------- IF Recording, Add Event --------------------
		if(bRecording && (iArm>0))
		{
			// addEvent(double dNewTime, bool bNewFed, int iNewArm)
			if(mouseLL != nullptr)
			{
					
				// add new event - ADD BOOLEANS BEFORE UPDATE
				//QuickMsgBox::QTrace("sending addEvent: keystroke\n");
				mouseLL->addEvent(WMP_GetPosition(), btnArray[iCurrentKey]->bFeeding, iCurrentKey, false); // false = not added with start of new sequence, recording, event

				// update form controls
				UpdateFormEventTimes();
			}
					
		}

	}
	
	// use to ket gey codes from button down - REMOVE
	//int _GetButtonStateFromKeyboard(System::Windows::Forms::KeyEventArgs^  e)
	//{
	//	// iterate through buttons
	//	int iButtonIndex = 0;
	//	int iButton = -1;

	//	while((iButtonIndex < 10) && (iButton < 0))
	//	{
	//		if(e->KeyCode == btnArray[iButtonIndex]->btnKeyCode)
	//			iButton = iButtonIndex
	//		// iterate index
	//		iButtonIndex++;
	//	}
	//}

}

// grid functions
namespace Squeak {

#pragma region data grid methods
	// update row
	System::Void Form1::UpdateGridData_FillRow(array<System::String^>^ row_string_array, 
											   double row_timestamp, int row_arm, bool row_fed)
	{
		// -- fill out grid with test	
		System::String^ str_movieTime;		// convert to mm::ss.ss

		// format time
		str_movieTime = MovieTimeToString(row_timestamp);
		row_string_array[0] = str_movieTime;

		// format arm and fed
		row_string_array[1] = System::String::Format("{0}",row_arm);
		row_string_array[2] = System::String::Format("{0}",row_fed);

	}

	// main update
	System::Void Form1::UpdateGridData(void)
	{
		dataGridViewEvents->Rows->Clear();
		if(mouseLL != nullptr)
		{
			int event_count = mouseLL->get_Count();
			if( event_count > 0)
			{
				// there are events
				// dataGridViewEvents

				// SHOULD ALWAYS CLEAR, e.g. in case of new sequence?
				//dataGridViewEvents->Rows->Clear();

	
				// ----- fill out list
				MouseLLEvent^ grid_row_event;

				// create vector of mouse event
				grid_row_event_vector = gcnew List<MouseLLEvent^>();
				// create dictionaries
				grid_row_to_event = gcnew Dictionary<int,MouseLLEvent^>(); // dictionary linking rows to events
				grid_event_to_row = gcnew Dictionary<MouseLLEvent^,int>() ;// dictionary linking events to rows				

				// add events by iterating through list of events	
				int iCount = 0;
				grid_row_event = mouseLL->getFirstEvent();
				while(grid_row_event != nullptr)
				{
					grid_row_event_vector->Add(grid_row_event);
					iCount++;	
					// new event added, use count for dictionary
					grid_row_to_event->Add(iCount, grid_row_event);
					grid_event_to_row->Add(grid_row_event, iCount);

					// get next event
					grid_row_event = grid_row_event->getNextEvent();
				}

	

				// -- fill out grid with test	
				// System::String^ str_movieTime;		// convert to mm::ss.ss
				
				// holds row string for data grid dataGridViewEvents
				array<System::String^>^ rowData = gcnew array<System::String^>(3);

				for each(grid_row_event in grid_row_event_vector)
				{
					UpdateGridData_FillRow(rowData, grid_row_event->getTimestamp(), grid_row_event->getArm(), 
													grid_row_event->getFed());

					//// format time
					//str_movieTime = MovieTimeToString(grid_row_event->getTimestamp());
					//rowData[0] = str_movieTime;

					//// format arm and fed
					//rowData[1] = System::String::Format("{0}",grid_row_event->getArm());
					//rowData[2] = System::String::Format("{0}",grid_row_event->getFed());

					// add data to row
					dataGridViewEvents->Rows->Add(rowData);
				}

				// try to set correct row
				MouseLLEvent^ me_find = mouseLL->FindNodeByTime(WMP_GetPosition());
				// this wil check for a null pointer, otherwise set grid row
				UpdateGridData_SelectionFromEvent(me_find);
			}
		}
	}

	// will update selected row when new event occurs
	System::Void Form1::UpdateGridData_SelectionFromEvent(MouseLLEvent^ new_event)
	{
		// get row from dictionary
		if(grid_event_to_row != nullptr)
		{
			if(new_event != nullptr)
			{
				// get row index from event to row dictionary
				int rowIndex = grid_event_to_row[new_event];
				
				// update selected row
				dataGridViewEvents->Rows[rowIndex-1]->Selected = true;
			}
		}
	}

	// enable/disable grid, enable only when not playing and/or recording
	System::Void Form1::EnableGrid(bool bEnable)
	{
		if(bEnable)
		{
			// only enable if not playing and/or recording
			if(!bRecording && !WMP_IsPlaying())
			{
				dataGridViewEvents->Enabled = true;
				dataGridViewEvents->DefaultCellStyle->BackColor = colorNormalDataGrid;

				dataGridViewEvents->DefaultCellStyle->SelectionForeColor = colorNormalDataGridSelectionForeColor;
				dataGridViewEvents->DefaultCellStyle->SelectionBackColor = colorNormalDataGridSelectionBackColor;
			}
		}else
		{
			dataGridViewEvents->Enabled = false; // always disable
			dataGridViewEvents->DefaultCellStyle->BackColor = colorDisabledDataGrid;
	
			dataGridViewEvents->DefaultCellStyle->SelectionForeColor = colorDisabledDataGridSelectionFore;
			dataGridViewEvents->DefaultCellStyle->SelectionBackColor = colorDisabledDataGridSelectionBack;
		}
	}


#pragma endregion

#pragma region data grid event - add or remove
		System::Void Form1::_Grid_AddRemoveEvents(int iType)
		{
			// iType = -1 for add before, 0 for remove , 1 for add after
			// check if there is a selection!!!
			// QuickMsgBox::QTrace("Selected Rows = {0}", dataGridViewEvents->SelectedRows->Count);

			bool bOK = false;

			// check for selection
			DataGridViewRow^ dataGridViewRow = dataGridViewEvents->CurrentRow;
			if(dataGridViewRow == nullptr)
			{
				QuickMsgBox::MBox("Event must be selected on the data grid");
			}
			//if(dataGridViewEvents->SelectedRows->Count != 0)
			//{
			//	QuickMsgBox::MBox("Event must be selected on the data grid");
			//}
			// get row
			int iRow = dataGridViewRow->Index;

			// do add or remove
			if(iType == 0)
			{
				bOK = _Grid_RemoveEvent(iRow+1); // send non-zero index of row
			}else
			{
				// need to check if there is a state
				bOK = _Grid_AddEvent(iRow + 1, iType);
				//AddEventAt(MouseLLEvent^ addAtNode, double dNewTime, int iNewArm, bool bNewFed, int iDirection);
			}
			// if either returns true, update:
			if(bOK)
			{
				// MAKE SURE THESE CAN HANDLE NO EVENTS
				UpdateGridData();
				UpdateFormEventTimes();
				mouseLL->set_IsDirty();
			}
		}
		// remove event at selection
		bool Form1::_Grid_RemoveEvent(int iRow_NZ)
		{
			// get the current node
			MouseLLEvent^ current_node = grid_row_to_event[iRow_NZ]; // get event, zero index row

			return mouseLL->RemoveEventAt(current_node);
		}
		
		// add before or after selection
		bool Form1::_Grid_AddEvent(int iRow_NZ, int iType)
		{
			// check there is a state!!
			if(iCurrentKey < 1)
			{
				QuickMsgBox::MBox("Need current event state to add another event (check whether an arm indicator is on)");
				return false;
			}

			// get the current node
			MouseLLEvent^ current_node = grid_row_to_event[iRow_NZ]; // get event, zero index row

			// add the event
			return mouseLL->AddEventAt(current_node, WMP_GetPosition(), iCurrentKey, btnArray[iCurrentKey]->bFeeding, iType);
		}
#pragma endregion

#pragma region data grid event - change existing data

		// update cell on click
		System::Void Form1::update_cell_on_click(int cell_row, int cell_col)
		{
			// * cell_row, cell_col ZERO INDEXED
			// get event			
			MouseLLEvent^ row_event = grid_row_to_event[cell_row+1];

			if(row_event == nullptr)
			{
				QuickMsgBox::QTrace("cell_double_click_DataGrid:mouse_event for row {0} is nullptr", cell_row+1);
			}else
			{

				//Stringgrid_row_to_event value = "";

				if (grid_row_to_event->TryGetValue(cell_row+1, row_event))
				{
					// call appropriate cell handler
					switch(cell_col) 
					{
					case	0:	// time
						{
							//QuickMsgBox::QTrace("Clicked on row {0} (not zero index):", cell_row+1);
							//QuickMsgBox::QEvent("row_event:", row_event);
							double_click_DataGrid_Time(cell_row, row_event); 
							break;
						}
					case	1:	// arm
						{
							double_click_DataGrid_Arm(cell_row, row_event);	break;				
						}
					case	2:	// fed
						{
							double_click_DataGrid_Feed(cell_row, row_event); break;					
						}
					default :
						QuickMsgBox::QTrace("ERROR: hit default switch on double click cell column {0}, \n",cell_col);
					}
				}
			}
		}
		// double clicked time cell
		System::Void Form1::cell_click_DataGrid_Right(int cell_row, int cell_col)
		{
			update_cell_on_click(cell_row, cell_col);
		}
		// double clicked time cell
		System::Void Form1::cell_double_click_DataGrid(int cell_row, int cell_col)
		{
		}	
		// double clicked time cell
		bool Form1::double_click_DataGrid_Time(int cell_row, MouseLLEvent^ row_event)
		{
			// need to check if change is OK: not before or after current event (unless last), 
			//		or at beginning of movie (time = 0)
			// will return error message if invalid time
			// will update the event time in row_event
					

			String^ str_msg = L"";
			double new_time = WMP_GetPosition();
			bool okChange = mouseLL->ChangeGridTimeChange(new_time, row_event, str_msg);
			if(okChange)
			{
				// ok, change cell
				str_msg = MovieTimeToString(row_event->getTimestamp()); // convert new time to movie time
				dataGridViewEvents->Rows[cell_row]->Cells[0]->Value = str_msg;
				UpdateFormEventTimes();
				mouseLL->set_IsDirty();
				return true;	
			}else
			{
				QuickMsgBox::MBox(str_msg);
				return false;
			}
			
		}
		// -- double clicked arm cell
		bool Form1::double_click_DataGrid_Arm(int cell_row, MouseLLEvent^ row_event)
		{
			if( iCurrentKey > 0 )
			{
				// set event arm
				row_event->setArm(iCurrentKey);
				// get event arm, update grid
				System::String^ str_arm = System::String::Format("{0}",row_event->getArm());
				dataGridViewEvents->Rows[cell_row]->Cells[1]->Value = str_arm;
				mouseLL->set_IsDirty();
			}
			return true;
		}

		// double clicked feed cell
		bool Form1::double_click_DataGrid_Feed(int cell_row, MouseLLEvent^ row_event)
		{
			if( iCurrentKey > 0 )
			{
				// get current state from keys
				bool bNewFeeding = btnArray[iCurrentKey]->bFeeding;
				// change event
				row_event->setFed(bNewFeeding);
				// update grid
				System::String^ str_fed = System::String::Format("{0}",row_event->getFed());
				dataGridViewEvents->Rows[cell_row]->Cells[2]->Value = str_fed;
				mouseLL->set_IsDirty();
			}
			return true;
		}
#pragma endregion

#pragma region data grid - click row
		// clicked right button on row header - go to event
		System::Void Form1::RowClicked_Right(int zeroRowIndex)
		{
		}
		// clicked left button on row header
		System::Void Form1::RowClicked_Left(int zeroRowIndex)
		{
			if(mouseLL != nullptr)
			{
				MouseLLEvent^ rowEvent = mouseLL->FindNodeByIndex(zeroRowIndex);
				if(rowEvent != nullptr)
				{
					WMP_SetPosition(rowEvent->getTimestamp());
					// iCurrentKey = rowEvent->getArm();
					// bFeeding = rowEvent->getFed();
				}
			}

		}
#pragma endregion


}
// convert movie time in double to MM:SS:ss format
System::String^ Squeak::Form1::MovieTimeToString(double dMovieTime)
{
	System::String^ strTime;

	double dMinutes = floor(dMovieTime / 60);
	double dSeconds = fmod(dMovieTime, 60);

	strTime = String::Format( "{0}:{1}",dMinutes,dSeconds.ToString("00.00"));
		
	//QuickMsgBox::QTrace("\nMOVIE_TIME:  Seconds: {0}, String: [{1}]\n", dMovieTime, strTime); 
	return strTime;
}

// update "status" textbok with time, caller
System::Void Squeak::Form1::Update_Time_Box(double dMovieTime, System::String^ strCaller)
{
	System::String^  strTime = MovieTimeToString(dMovieTime);
	if(String::IsNullOrEmpty(strCaller))
		tbStateChange->Text = strTime;
	else
	{
		tbStateChange->Text = String::Format("{0} [Caller='{1}']", strTime, strCaller);
	}
}



