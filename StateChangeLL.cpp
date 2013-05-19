#include "StdAfx.h"
#include "MouseLL.h"
#include "StateChange.h"
#include "StateChangeLL.h"


StateChangeLL::StateChangeLL(void)
{
	stateChanges = gcnew LinkedList<StateChange^>;
	next_StateID = 0;
	

	hashEventIDs = gcnew HashSet<int>;
}


// Sequence: Load
#pragma region Event Adds
// add an event
System::Void StateChangeLL::add_event(MouseLLEvent^ new_Event, EV_TYPE evType, SC_STATE prState, SC_REASON reason, MouseLLEvent^ firstEvent, MouseLLEvent^ lastEvent)
{
	if(new_Event == nullptr)
	{
		System::Windows::Forms::MessageBox::Show("Warning - New Event sent with null pointer");
	}else
	{
		MouseLLEvent^ prev_Event = new_Event->getPreviousEvent();
		MouseLLEvent^ next_Event = new_Event->getNextEvent();


		// make new state change object
		StateChange^ newChange = gcnew StateChange(next_StateID);

		// ---------- add conditions
		newChange->seqX	=	 SC_SEQ::SCQ_ADDED;
		newChange->reasonX = reason;
		newChange->stateX  = prState;

		// ----- add current event
		// event
		newChange->set_scEvent(StateChange::SCI_ORIGIN, evType, new_Event, new_Event->getTimestamp());
		// time
		newChange->scTimes[StateChange::SCI_ORIGIN] = new_Event->getTimestamp();
		// has
		_AddEventToHash(new_Event);

		
		// ----- check additional
		if(evType != EV_TYPE::EV_LAST)
		{
			// -- has a next event
			// time
			newChange->scTimes[StateChange::SCI_NEXT] = next_Event->getTimestamp();
			// has
			_AddEventToHash(next_Event);

			if(next_Event == lastEvent)
			{
				// add last as next
				newChange->set_scEvent(StateChange::SCI_NEXT, EV_TYPE::EV_LAST, next_Event, next_Event->getTimestamp());
			}else
			{
				// add middle as next
				newChange->set_scEvent(StateChange::SCI_NEXT, EV_TYPE::EV_MIDDLE, next_Event, next_Event->getTimestamp());			
			}
		}
		
		// ADD HERE
		stateChanges->AddFirst(newChange);
		next_StateID++;

	}
}

// occurs with new sequence, events may be empty
System::Void StateChangeLL::add_event_newSequence(MouseLLEvent^ new_firstEvent)
{
	if(new_firstEvent == nullptr)
	{
		System::Windows::Forms::MessageBox::Show("Warning - New Sequence Event sent to State Change stack with null first event pointer");
	}else
	{
		// make new state change object
		StateChange^ newChange = gcnew StateChange(next_StateID);

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


		// ADD HERE
		stateChanges->AddFirst(newChange);
		next_StateID++;
	}
	return System::Void();
}
// occurs with load sequence
System::Void StateChangeLL::add_event_loadSequence(MouseLLEvent^ new_firstEvent, MouseLLEvent^ new_lastEvent, MouseLLEvent^ new_nextEvent)
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
		StateChange^ newChange = gcnew StateChange(next_StateID);
	
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

		// ADD HERE
		stateChanges->AddFirst(newChange);
		next_StateID++;

	}

	return System::Void();
}

#pragma endregion