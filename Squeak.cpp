// Squeak.cpp : main project file.

#include "stdafx.h"
#include <math.h>
#include "TimeState.h"
#include "BtnArray.h"
#include "MouseLL.h"		// hand made linked list
#include "StateChange.h"
#include "StateChangeLL.h"
#include "FormTextInput.h"
#include "Form1.h"
#include "WTypes.h"

#define STATE_CHANGE

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
	// clicked mouse in movie player
	System::Void Form1::WMP_Mouse_Pressed(AxWMPLib::_WMPOCXEvents_MouseDownEvent^  e){
		Point formPos = PointToClient(Control::MousePosition);
		MoveDirectionButton(formPos);
	}
	// clicked outside movie player
	System::Void Form1::Form_Mouse_Pressed(System::Windows::Forms::MouseEventArgs^  e){
		System::String^ strStatus;
		
		if(e->Button == ::MouseButtons::Left)
		{
			strStatus = String::Format("FORM: Mouse Down at [{0},{1}]", e->X, e->Y);
			tbStateChange->Text = strStatus;
			Point formPos = Point(e->X, e->Y);
			MoveDirectionButton(formPos);
		}
	}
	// move the button
	System::Void Form1::MoveDirectionButton(Drawing::Point btnNewLocation){
		// make sure a button is available
		if(btnSelected == nullptr)
		{
			tbStateChange->Text = L"Tried to move button with null pointer";
		}else
		{
			System::String^ strStatus;
			strStatus = String::Format("New Location [{0},{1}]", btnNewLocation.X, btnNewLocation.Y);
			tbStateChange->Text = strStatus;
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
		// check if there is a sequence
		if(mouseLL != nullptr)
		{
			if(mouseLL->get_Count() > 0)
			{
				double dFirstTime = -1;
				double dLastTime = -1;
				if(mouseLL->getFirstLastTimes(&dFirstTime, &dLastTime))
				{
					// update text boxes in MM:SS:ss format
					eventFirstTextBox->Text = MovieTimeToString(dFirstTime);
					eventLastTextBox->Text = MovieTimeToString(dLastTime);
				}
			}
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
			// get new sequence name
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
						mouseLL = gcnew MouseLL(strNewName, axWindowsMediaPlayer1->URL, WMP_Duration(), dTime, btnArray[iCurrentKey]->bFeeding, iCurrentKey);
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
					

					if(bAddedOK)
					{
						UpdateFormEventTimes(); // update form times, this event is the first and last
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
						mouseLL = gcnew MouseLL(strNewName, axWindowsMediaPlayer1->URL, WMP_Duration());
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
	System::Void Form1::Sequence_Load(void)
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
			// path to load
			System::String^ strSeqLoadFile = _LoadSeqDialog();
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
					mouseLL = gcnew MouseLL(axWindowsMediaPlayer1->URL, WMP_Duration());
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
// Form Events
namespace Squeak
{
	// called when timer ticks
	System::Void Form1::TimerTick(System::EventArgs^  e)
	{
		double dNewWMPPosition; // current position

		// get current position
		dNewWMPPosition = WMP_GetPosition();
		// check if different than last position
		if(dNewWMPPosition != dWmpPosition)
		{
			// change in position of WMP
			dWmpPosition = dNewWMPPosition;

			tbStateChange->Text = MovieTimeToString(dWmpPosition);

			// code for updated position
			//TimeSpan movieTime = TimeSpan::FromSeconds(dWmpPosition);

			//tbStateChange->Text = movieTime.ToString("c");

			//tbStateChange->Text = String::Format("{0}",dWmpPosition);
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

	// sets visible button state (bControl == feeding), e = nullptr if not user generated
	System::Void Form1::_SetButtonState(int iArm, bool bFeeding, bool bRecording)
	{
		Color newColor;
		bool bIsFeeding;
		int iButtonIndex;
		// check if this is a state change
		if((iCurrentKey != iArm) || (bFeeding != btnArray[iArm]->bFeeding))
		{
			// change from current state
			// iterate through buttons
			for(iButtonIndex = 0; iButtonIndex < 10; iButtonIndex++)
			{
				// check if this matches arm
				if(iButtonIndex == iArm)
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
		if(bRecording)
		{
			// addEvent(double dNewTime, bool bNewFed, int iNewArm)
			if(mouseLL != nullptr)
			{
					
				// add new event - ADD BOOLEANS BEFORE UPDATE
				mouseLL->addEvent(WMP_GetPosition(), btnArray[iCurrentKey]->bFeeding, iCurrentKey);

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
	// main update
	System::Void Form1::UpdateGridData(void)
	{
		if(mouseLL != nullptr)
		{
			if(mouseLL->get_Count() > 0)
			{
				// there are events
				// dataGridViewEvents
				dataGridViewEvents->Rows->Clear();

				int iAdded = mouseLL->PopulateDataGrid(dataGridViewEvents); 
				//FillRows();
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
				dataGridViewEvents->Enabled = true;
		}else
			dataGridViewEvents->Enabled = false; // always disable
	}
#pragma endregion

#pragma region data grid events

		// clicked right button on row header - go to event
		System::Void Form1::RowClicked_Right(int zeroRowIndex)
		{
			if(mouseLL != nullptr)
			{
				MouseLLEvent^ rowEvent = mouseLL->FindNodeByIndex(zeroRowIndex);
				if(rowEvent != nullptr)
				{
					WMP_SetPosition(rowEvent->getTimestamp());
				}
			}
		}
		// clicked left button on row header
		System::Void Form1::RowClicked_Left(int zeroRowIndex)
		{

		}
#pragma endregion

}
// convert movie time in double to MM:SS:ss format
System::String^ Squeak::Form1::MovieTimeToString(double dMovieTime)
{
	double dMinutes = floor(dMovieTime / 60);
	double dSeconds = fmod(dMovieTime, 60);

	return String::Format( "{0}:{1}",dMinutes,dSeconds.ToString("00.00"));
}



