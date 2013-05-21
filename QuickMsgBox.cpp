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
	
