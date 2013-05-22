#include "StdAfx.h"
#include "QuickMsgBox.h"


QuickMsgBox::QuickMsgBox(void)
{
}


// quick message box with NO objects
void QuickMsgBox::MBox(System::String^ msg)
{
	System::Windows::Forms::MessageBox::Show(msg);
	
}

// quick message box with ONE object
void QuickMsgBox::MBox(System::String^ msg, Object^ obj1)
{
	System::String^ strFormat;
	strFormat = System::String::Format(msg, obj1);
	System::Windows::Forms::MessageBox::Show(strFormat);
}

// quick message box with TWO objects
void QuickMsgBox::MBox(System::String^ msg, Object^ obj1, Object^ obj2)
{
	System::String^ strFormat;
	strFormat = System::String::Format(msg, obj1, obj2);
	System::Windows::Forms::MessageBox::Show(strFormat);
}

// quick trace
void QuickMsgBox::QTrace(System::String^ msg)
{
	System::Diagnostics::Trace::WriteLine( msg );
}
// quick trace
void QuickMsgBox::QTrace(System::String^ msg, Object^ obj1)
{

	System::String^ strFormat;
	strFormat = System::String::Format(msg, obj1);
	System::Diagnostics::Trace::WriteLine( strFormat );

}
// quick trace
void QuickMsgBox::QTrace(System::String^ msg, Object^ obj1, Object^ obj2)
{
	System::String^ strFormat;
	strFormat = System::String::Format(msg, obj1, obj2);
	System::Diagnostics::Trace::WriteLine( strFormat );
}

// quick trace
void QuickMsgBox::QTrace(System::String^ msg, Object^ obj1, Object^ obj2, Object^ obj3)
{
	System::String^ strFormat;
	strFormat = System::String::Format(msg, obj1, obj2, obj3);
	System::Diagnostics::Trace::WriteLine( strFormat );
}

// trace event
void QuickMsgBox::QEvent(System::String^ e_name, Object^ obj1)
{
	System::String^ strFormat;
	if(obj1 == nullptr)
		strFormat = System::String::Format("{0}:\t nullptr", e_name);
	else
		strFormat = System::String::Format("{0}:\t {1}", e_name, obj1->ToString());

	System::Diagnostics::Trace::WriteLine(strFormat);
}

