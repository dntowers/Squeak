#pragma once

using namespace System::Collections::Generic;
//using namespace System::Windows::Forms;
ref class StateChangeLL : public System::Object
{
public:
	StateChangeLL(void);

	// stack for StateChange
	LinkedList<StateChange^>^ stateChanges;

	// hash set for EventIDs
	HashSet<int>^ hashEventIDs;
	
	//state changed IDs
	int next_StateID;

#pragma region Event Checking
private:
	// add event ID to hash table
	void _AddEventToHash(MouseLLEvent^ new_Event){
		if(new_Event != nullptr)
			hashEventIDs->Add(new_Event->getEventID());
		}
	
	// remove event from hash table
	void _RemoveEventFromHash(MouseLLEvent^ remove_Event){
		if(remove_Event != nullptr)
			hashEventIDs->Remove(remove_Event->getEventID());
		}
	
	// check if event is still around
	bool _CheckEventIDInHash(int EventID){ return hashEventIDs->Contains(EventID);}
#pragma endregion

#pragma region Event Adds
public:
	// add an event
	System::Void add_event(MouseLLEvent^ new_Event, EV_TYPE evType, SC_STATE prState, SC_REASON reason, MouseLLEvent^ firstEvent, MouseLLEvent^ lastEvent);
	// occurs with new sequence, events may be empty
	System::Void add_event_newSequence(MouseLLEvent^ new_firstEvent);
	// occurs with load sequence
	System::Void add_event_loadSequence(MouseLLEvent^ new_firstEvent, MouseLLEvent^ new_lastEvent, MouseLLEvent^ new_nextEvent);
#pragma endregion


};

