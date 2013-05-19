#pragma once

using namespace System::Collections::Generic;

ref class StateChangeStack : public System::Object
{
public:
	StateChangeStack(void);

	// stack for StateChange
	Stack<StateChange^>^ stateChanges;
};

