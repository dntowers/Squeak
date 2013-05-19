#include "StdAfx.h"
#include "MouseLL.h"
#include "StateChange.h"
#include "StateChangeStack.h"


StateChangeStack::StateChangeStack(void)
{
	stateChanges = gcnew Stack<StateChange^>;
	stateChanges->Push(nullptr);
}


// IMPLEMENT UNIQUE ID FOR EVENTS!!!

 //try
 //{
	// ATestFunctionThatUseVariableForNothing(a);
 //}
 //catch (System::ObjectDisposedException^ e)
 //{
	// a = nullptr;
 //}
 //catch (System::NullReferenceException^ e)
 //{
	// a = nullptr;
 //}