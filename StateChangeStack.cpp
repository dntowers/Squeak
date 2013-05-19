#include "StdAfx.h"
#include "MouseLL.h"
#include "StateChange.h"
#include "StateChangeStack.h"


StateChangeStack::StateChangeStack(void)
{
	stateChanges = gcnew Stack<StateChange^>;
	stateChanges->Push(nullptr);

	hashEventIDs = gcnew HashSet<int>;
}


// Sequence: Load
#pragma region Sequence Adds
// occurs with new sequence, events may be empty
System::Void StateChangeStack::add_event_newSequence(MouseLLEvent^ new_firstEvent)
{
	if(new_firstEvent == nullptr)
	{
		System::Windows::Forms::MessageBox::Show("Warning - New Sequence Event sent to State Change stack with null first event pointer");
	}else
	{
		// make new state change object
		StateChange^ newChange = gcnew StateChange();

		// ---------- add times
		newChange->scTimes[StateChange::SCI_ORIGIN] = new_firstEvent->getTimestamp();
		// ---------- add conditions
		newChange->seqX	=	 SC_SEQ::SCQ_NEW;
		newChange->reasonX = SC_REASON::SCR_NEWSEQ;
		// --------- add pointers
		newChange->set_scEvent(StateChange::SCI_ORIGIN, EV_TYPE::EV_FIRST, new_firstEvent, new_firstEvent->getTimestamp());
		newChange->set_scEvent(StateChange::SCI_DEST,   EV_TYPE::EV_FIRST, new_firstEvent, new_firstEvent->getTimestamp());
		newChange->set_scEvent(StateChange::SCI_NEXT,   EV_TYPE::EV_FIRST, new_firstEvent, new_firstEvent->getTimestamp());
		// add event IDs to hash (won't if nullptr)
		_AddEventToHash(new_firstEvent); 
	}
	return System::Void();
}
// occurs with load sequence
System::Void StateChangeStack::add_event_loadSequence(MouseLLEvent^ new_firstEvent, MouseLLEvent^ new_lastEvent, MouseLLEvent^ new_nextEvent)
{
	if((new_firstEvent == nullptr) || (new_lastEvent == nullptr))
	{
		if(new_firstEvent == nullptr)
			System::Windows::Forms::MessageBox::Show("Warning - Load Sequence Event sent to State Change stack with null first event pointer");
		else
			System::Windows::Forms::MessageBox::Show("Warning - Load Sequence Event sent to State Change stack with null last event pointer");
	}else
	{
		// make new state change object
		StateChange^ newChange = gcnew StateChange();
	
		// ---------- add conditions
		newChange->seqX	=	 SC_SEQ::SCQ_LOADED;
		newChange->reasonX = SC_REASON::SCR_LOADSEQ;
		// ---------- add times
		newChange->scTimes[StateChange::SCI_ORIGIN] = new_firstEvent->getTimestamp();
		newChange->scTimes[StateChange::SCI_DEST] = new_lastEvent->getTimestamp();
		newChange->scTimes[StateChange::SCI_NEXT] = new_nextEvent->getTimestamp();
		
		// --------- add pointers
		newChange->set_scEvent(StateChange::SCI_ORIGIN, EV_TYPE::EV_FIRST, new_firstEvent, new_firstEvent->getTimestamp());
		if(new_lastEvent == new_firstEvent)
		{
			// only one event, set last and next to the same
			newChange->set_scEvent(StateChange::SCI_DEST,   EV_TYPE::EV_FIRST,  new_firstEvent, new_firstEvent->getTimestamp());
			newChange->set_scEvent(StateChange::SCI_NEXT,   EV_TYPE::EV_FIRST,  new_firstEvent, new_firstEvent->getTimestamp());
		}else
		{
			// at least two events
			newChange->set_scEvent(StateChange::SCI_DEST,   EV_TYPE::EV_LAST,  new_lastEvent, new_firstEvent->getTimestamp());
			// check for middle pointer
			if(new_nextEvent == new_lastEvent)
			{
				// only first and last event
				newChange->set_scEvent(StateChange::SCI_NEXT,   EV_TYPE::EV_LAST,  new_lastEvent, new_lastEvent->getTimestamp());
			}else
			{
				// event in the middle
				newChange->set_scEvent(StateChange::SCI_NEXT,   EV_TYPE::EV_MIDDLE,  new_nextEvent, new_nextEvent->getTimestamp());
			}
		}
		// add event IDs to hash (won't if nullptr)
		_AddEventToHash(new_firstEvent); 
		_AddEventToHash(new_lastEvent);
		_AddEventToHash(new_nextEvent);
	}

	return System::Void();
}

#pragma endregion