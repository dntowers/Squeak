#pragma once

	public ref class QuickMsgBox : public System::Object
	{
	public:
		QuickMsgBox(void);

		// quick message box with NO objects
		static void MBox(System::String^ msg);


		// quick message box with ONE object
		static void MBox(System::String^ msg, Object^ obj1);

		// quick message box with TWO objects
		static void MBox(System::String^ msg, Object^ obj1, Object^ obj2);

		// quick trace
		static void QTrace(System::String^ msg);
		// quick trace
		static void QTrace(System::String^ msg, Object^ obj1);
		// quick trace
		static void QTrace(System::String^ msg, Object^ obj1, Object^ obj2);
		// quick trace
		static void QTrace(System::String^ msg, Object^ obj1, Object^ obj2, Object^ obj3);

		// trace of event contents
		static void QEvent(System::String^ e_name, Object^ obj1);
	};
