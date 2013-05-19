#pragma once

//// array for buttons down
//public ref struct buttonArray
//{
//	System::Windows::Forms::Button^  btn;
//	bool bOn;
//	System::Windows::Forms::Keys btnKeyCode;
//};

namespace Squeak {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	//using namespace System::Collections::Generic;
	using namespace System::Windows::Forms;
	using namespace System::Windows;
	using namespace System::Data;
	using namespace System::Drawing;


	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			// --------------------  Media player settings
			// rate
			dWMP_Rate = WMP_GetRate();
			rateUpDown->Value = Decimal(dWMP_Rate);
			axWindowsMediaPlayer1->settings->autoStart = false; // prevent automatically starting on load
			// position
			dWmpPosition = WMP_GetPosition();
			tbStateChange->Text = String::Format("{0}",dWmpPosition);

			//strRate = System::String::Format("{0:F}",dWMP_Rate);
			//this->tbRate->Text = strRate;
			
			// initialize selected button to null
			btnSelected = nullptr;

			// set colors
			colorButtonNormal = System::Drawing::SystemColors::InactiveCaptionText;
			colorButtonMoving = System::Drawing::SystemColors::GradientActiveCaption;
			colorButtonOn = System::Drawing::SystemColors::ButtonHighlight;
			colorButtonFeeding = System::Drawing::Color::Yellow;

			// TEMP - KEEP FROM NEEDING TO LOAD
			axWindowsMediaPlayer1->URL = "C:\\Users\\David\\Desktop\\CH1 1.wmv";
			textBoxVideoFile->Text = axWindowsMediaPlayer1->URL;

			// set dialog box properties
			openFileDialogVideo->Filter = "WMV files (*.wmv)|*.wmv|All files (*.*)|*.*";

			// input
			bKeyHandled = false;
			iCurrentKey = 5;		// init to no arm state
			//bFeeding = false;		// init to not feeding

			// --- events
			bRecording = false;

			// mouse events linked list
			mouseLL = nullptr;
			// state change stack
			stateStack = nullptr;
			



#pragma region button array
			// -- active state array
			// init array
			btnArray = gcnew array<buttonArray ^>(10);
			// 0 = pause/play - will never be a button
			// set flag to false
			for (int i = 0 ; i < 10; i++) {
				btnArray[i] = gcnew buttonArray;
				btnArray[i]->bOn = false;
				btnArray[i]->btn = nullptr;
				btnArray[i]->bFeeding = false;
				//btnArray[i]->btnKeyCode = -1;
			}
			// add buttons
			//btnArray[0]->btn = nullptr; 
			btnArray[1]->btn = btn1; 
			btnArray[2]->btn = btn2; 
			btnArray[3]->btn = btn3; 
			btnArray[4]->btn = btn4; 
			//btnArray[5]->btn = nullptr; 
			btnArray[6]->btn = btn6; 
			btnArray[7]->btn = btn7; 
			btnArray[8]->btn = btn8; 
			btnArray[9]->btn = btn9;
			// add keys
			btnArray[0]->btnKeyCode = Keys::NumPad0;
			btnArray[1]->btnKeyCode = Keys::NumPad1;
			btnArray[2]->btnKeyCode = Keys::NumPad2;
			btnArray[3]->btnKeyCode = Keys::NumPad3;
			btnArray[4]->btnKeyCode = Keys::NumPad4;
			btnArray[5]->btnKeyCode = Keys::NumPad5;
			btnArray[6]->btnKeyCode = Keys::NumPad6;
			btnArray[7]->btnKeyCode = Keys::NumPad7;
			btnArray[8]->btnKeyCode = Keys::NumPad8;
			btnArray[9]->btnKeyCode = Keys::NumPad9;
			// add keys

#pragma endregion

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

	// control variables
	
	private: AxWMPLib::AxWindowsMediaPlayer^  axWindowsMediaPlayer1;
	private: System::Windows::Forms::Button^  btPlay;

	

#pragma region My variables
	private:
		// Windows Media Player
		bool bWMPPlaying;
		double dWMP_Rate;
		System::String^ strRate;
		double dWmpPosition;

		// pointer to selected button
		System::Windows::Forms::Button^  btnSelected;

		// colors for button indicators
		Color colorButtonNormal;
		Color colorButtonMoving;
		Color colorButtonOn;
		Color colorButtonFeeding;

		// keyboard stuff
		bool bKeyHandled;
		array<buttonArray ^> ^ btnArray;
		int iCurrentKey;

		//LinkedList<MouseEvent^>^ mouseEvents;
		// events
		bool bRecording;
		// MouseEvents^ mouseEvents; OLD SEQUENCE
		MouseLL^ mouseLL;
		// state change stack
		StateChangeStack^ stateStack;

#pragma endregion
	
	private: System::Windows::Forms::TextBox^  tblabelRate;
	private: System::Diagnostics::EventLog^  eventLog1;
	private: System::Windows::Forms::NumericUpDown^  rateUpDown;
	private: System::Windows::Forms::TextBox^  tbStateChange;
	private: System::Windows::Forms::Button^  btn9;
	private: System::Windows::Forms::Button^  btn8;
	private: System::Windows::Forms::Button^  btn6;
	private: System::Windows::Forms::Button^  btn3;
	private: System::Windows::Forms::Button^  btn2;
	private: System::Windows::Forms::Button^  btn1;
	private: System::Windows::Forms::Button^  btn4;
	private: System::Windows::Forms::Button^  btn7;
	private: System::Windows::Forms::TextBox^  textBoxVideoFile;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialogVideo;
	private: System::Windows::Forms::Button^  btnGetVideoFileName;
	private: System::Windows::Forms::Timer^  timerControl;
	private: System::Windows::Forms::TextBox^  textBoxSeqName;
	private: System::Windows::Forms::Button^  btnRecordEvents;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  labelRecordStatus;
	private: Microsoft::VisualBasic::PowerPacks::ShapeContainer^  shapeContainer1;
	private: Microsoft::VisualBasic::PowerPacks::OvalShape^  shapeRecording;
	private: System::Windows::Forms::Button^  btnSequenceLoad;
	private: System::Windows::Forms::Button^  btnSequenceNew;
	private: System::Windows::Forms::Button^  btnSequenceSave;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  eventLastTextBox;
	private: System::Windows::Forms::TextBox^  eventFirstTextBox;
private: System::Windows::Forms::DataGridView^  dataGridViewEvents;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  gchTime;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  gchArm;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  gchFed;
private: System::Windows::Forms::Button^  btnStepForward;
private: System::Windows::Forms::Button^  btnStepBack;






	private: System::ComponentModel::IContainer^  components;

	
	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->btPlay = (gcnew System::Windows::Forms::Button());
			this->axWindowsMediaPlayer1 = (gcnew AxWMPLib::AxWindowsMediaPlayer());
			this->tblabelRate = (gcnew System::Windows::Forms::TextBox());
			this->eventLog1 = (gcnew System::Diagnostics::EventLog());
			this->rateUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->tbStateChange = (gcnew System::Windows::Forms::TextBox());
			this->btn9 = (gcnew System::Windows::Forms::Button());
			this->btn8 = (gcnew System::Windows::Forms::Button());
			this->btn7 = (gcnew System::Windows::Forms::Button());
			this->btn4 = (gcnew System::Windows::Forms::Button());
			this->btn1 = (gcnew System::Windows::Forms::Button());
			this->btn2 = (gcnew System::Windows::Forms::Button());
			this->btn3 = (gcnew System::Windows::Forms::Button());
			this->btn6 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textBoxVideoFile = (gcnew System::Windows::Forms::TextBox());
			this->openFileDialogVideo = (gcnew System::Windows::Forms::OpenFileDialog());
			this->btnGetVideoFileName = (gcnew System::Windows::Forms::Button());
			this->timerControl = (gcnew System::Windows::Forms::Timer(this->components));
			this->textBoxSeqName = (gcnew System::Windows::Forms::TextBox());
			this->btnRecordEvents = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->labelRecordStatus = (gcnew System::Windows::Forms::Label());
			this->shapeContainer1 = (gcnew Microsoft::VisualBasic::PowerPacks::ShapeContainer());
			this->shapeRecording = (gcnew Microsoft::VisualBasic::PowerPacks::OvalShape());
			this->btnSequenceSave = (gcnew System::Windows::Forms::Button());
			this->btnSequenceNew = (gcnew System::Windows::Forms::Button());
			this->btnSequenceLoad = (gcnew System::Windows::Forms::Button());
			this->eventFirstTextBox = (gcnew System::Windows::Forms::TextBox());
			this->eventLastTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->dataGridViewEvents = (gcnew System::Windows::Forms::DataGridView());
			this->gchTime = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->gchArm = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->gchFed = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->btnStepForward = (gcnew System::Windows::Forms::Button());
			this->btnStepBack = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->axWindowsMediaPlayer1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->eventLog1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->rateUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridViewEvents))->BeginInit();
			this->SuspendLayout();
			// 
			// btPlay
			// 
			this->btPlay->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btPlay->Location = System::Drawing::Point(744, 112);
			this->btPlay->Name = L"btPlay";
			this->btPlay->Size = System::Drawing::Size(115, 30);
			this->btPlay->TabIndex = 1;
			this->btPlay->Text = L"Play (0)";
			this->btPlay->UseVisualStyleBackColor = true;
			this->btPlay->Click += gcnew System::EventHandler(this, &Form1::btPlay_Click);
			// 
			// axWindowsMediaPlayer1
			// 
			this->axWindowsMediaPlayer1->Enabled = true;
			this->axWindowsMediaPlayer1->Location = System::Drawing::Point(41, 31);
			this->axWindowsMediaPlayer1->Name = L"axWindowsMediaPlayer1";
			this->axWindowsMediaPlayer1->OcxState = (cli::safe_cast<System::Windows::Forms::AxHost::State^  >(resources->GetObject(L"axWindowsMediaPlayer1.OcxState")));
			this->axWindowsMediaPlayer1->Size = System::Drawing::Size(694, 597);
			this->axWindowsMediaPlayer1->TabIndex = 0;
			this->axWindowsMediaPlayer1->MouseDownEvent += gcnew AxWMPLib::_WMPOCXEvents_MouseDownEventHandler(this, &Form1::axWindowsMediaPlayer1_MouseDownEvent);
			this->axWindowsMediaPlayer1->PlayStateChange += gcnew AxWMPLib::_WMPOCXEvents_PlayStateChangeEventHandler(this, &Form1::axWindowsMediaPlayer1_PlayStateChange);
			this->axWindowsMediaPlayer1->PositionChange += gcnew AxWMPLib::_WMPOCXEvents_PositionChangeEventHandler(this, &Form1::axWindowsMediaPlayer1_PositionChange);
			// 
			// tblabelRate
			// 
			this->tblabelRate->BackColor = System::Drawing::SystemColors::Control;
			this->tblabelRate->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->tblabelRate->Enabled = false;
			this->tblabelRate->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tblabelRate->Location = System::Drawing::Point(880, 103);
			this->tblabelRate->Name = L"tblabelRate";
			this->tblabelRate->Size = System::Drawing::Size(76, 15);
			this->tblabelRate->TabIndex = 2;
			this->tblabelRate->Text = L"Play Rate";
			this->tblabelRate->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// eventLog1
			// 
			this->eventLog1->SynchronizingObject = this;
			// 
			// rateUpDown
			// 
			this->rateUpDown->DecimalPlaces = 1;
			this->rateUpDown->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->rateUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->rateUpDown->Location = System::Drawing::Point(880, 124);
			this->rateUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {2, 0, 0, 0});
			this->rateUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 65536});
			this->rateUpDown->Name = L"rateUpDown";
			this->rateUpDown->Size = System::Drawing::Size(76, 22);
			this->rateUpDown->TabIndex = 4;
			this->rateUpDown->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->rateUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 65536});
			this->rateUpDown->ValueChanged += gcnew System::EventHandler(this, &Form1::rateUpDown_ValueChanged);
			// 
			// tbStateChange
			// 
			this->tbStateChange->Location = System::Drawing::Point(17, 663);
			this->tbStateChange->Name = L"tbStateChange";
			this->tbStateChange->Size = System::Drawing::Size(106, 20);
			this->tbStateChange->TabIndex = 5;
			// 
			// btn9
			// 
			this->btn9->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->btn9->FlatAppearance->BorderColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->btn9->FlatAppearance->BorderSize = 2;
			this->btn9->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btn9->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btn9->Location = System::Drawing::Point(549, 94);
			this->btn9->Name = L"btn9";
			this->btn9->Size = System::Drawing::Size(36, 33);
			this->btn9->TabIndex = 6;
			this->btn9->Tag = L"Button9NE";
			this->btn9->Text = L"9";
			this->btn9->UseVisualStyleBackColor = false;
			this->btn9->Click += gcnew System::EventHandler(this, &Form1::btn9_Click);
			// 
			// btn8
			// 
			this->btn8->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->btn8->FlatAppearance->BorderColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->btn8->FlatAppearance->BorderSize = 2;
			this->btn8->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btn8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btn8->Location = System::Drawing::Point(353, 12);
			this->btn8->Name = L"btn8";
			this->btn8->Size = System::Drawing::Size(36, 33);
			this->btn8->TabIndex = 7;
			this->btn8->Tag = L"Button8N";
			this->btn8->Text = L"8";
			this->btn8->UseVisualStyleBackColor = false;
			this->btn8->Click += gcnew System::EventHandler(this, &Form1::btn8_Click);
			// 
			// btn7
			// 
			this->btn7->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->btn7->FlatAppearance->BorderColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->btn7->FlatAppearance->BorderSize = 2;
			this->btn7->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btn7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btn7->Location = System::Drawing::Point(147, 94);
			this->btn7->Name = L"btn7";
			this->btn7->Size = System::Drawing::Size(36, 33);
			this->btn7->TabIndex = 8;
			this->btn7->Tag = L"Button7NW";
			this->btn7->Text = L"7";
			this->btn7->UseVisualStyleBackColor = false;
			this->btn7->Click += gcnew System::EventHandler(this, &Form1::btn7_Click);
			// 
			// btn4
			// 
			this->btn4->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->btn4->FlatAppearance->BorderColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->btn4->FlatAppearance->BorderSize = 2;
			this->btn4->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btn4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btn4->Location = System::Drawing::Point(70, 273);
			this->btn4->Name = L"btn4";
			this->btn4->Size = System::Drawing::Size(36, 33);
			this->btn4->TabIndex = 9;
			this->btn4->Tag = L"Button4W";
			this->btn4->Text = L"4";
			this->btn4->UseVisualStyleBackColor = false;
			this->btn4->Click += gcnew System::EventHandler(this, &Form1::btn4_Click);
			// 
			// btn1
			// 
			this->btn1->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->btn1->FlatAppearance->BorderColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->btn1->FlatAppearance->BorderSize = 2;
			this->btn1->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btn1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btn1->Location = System::Drawing::Point(147, 461);
			this->btn1->Name = L"btn1";
			this->btn1->Size = System::Drawing::Size(36, 33);
			this->btn1->TabIndex = 10;
			this->btn1->Tag = L"Button1SW";
			this->btn1->Text = L"1";
			this->btn1->UseVisualStyleBackColor = false;
			this->btn1->Click += gcnew System::EventHandler(this, &Form1::btn1_Click);
			// 
			// btn2
			// 
			this->btn2->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->btn2->FlatAppearance->BorderColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->btn2->FlatAppearance->BorderSize = 2;
			this->btn2->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btn2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btn2->Location = System::Drawing::Point(353, 545);
			this->btn2->Name = L"btn2";
			this->btn2->Size = System::Drawing::Size(36, 33);
			this->btn2->TabIndex = 11;
			this->btn2->Tag = L"Button2S";
			this->btn2->Text = L"2";
			this->btn2->UseVisualStyleBackColor = false;
			this->btn2->Click += gcnew System::EventHandler(this, &Form1::btn2_Click);
			// 
			// btn3
			// 
			this->btn3->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->btn3->FlatAppearance->BorderColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->btn3->FlatAppearance->BorderSize = 2;
			this->btn3->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btn3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btn3->Location = System::Drawing::Point(549, 461);
			this->btn3->Name = L"btn3";
			this->btn3->Size = System::Drawing::Size(36, 33);
			this->btn3->TabIndex = 12;
			this->btn3->Tag = L"Button3SE";
			this->btn3->Text = L"3";
			this->btn3->UseVisualStyleBackColor = false;
			this->btn3->Click += gcnew System::EventHandler(this, &Form1::btn3_Click);
			// 
			// btn6
			// 
			this->btn6->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->btn6->FlatAppearance->BorderColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->btn6->FlatAppearance->BorderSize = 2;
			this->btn6->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btn6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btn6->Location = System::Drawing::Point(620, 273);
			this->btn6->Name = L"btn6";
			this->btn6->Size = System::Drawing::Size(36, 33);
			this->btn6->TabIndex = 13;
			this->btn6->Tag = L"Button6E";
			this->btn6->Text = L"6";
			this->btn6->UseVisualStyleBackColor = false;
			this->btn6->Click += gcnew System::EventHandler(this, &Form1::btn6_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(14, 736);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(69, 16);
			this->label1->TabIndex = 14;
			this->label1->Text = L"Video File";
			// 
			// textBoxVideoFile
			// 
			this->textBoxVideoFile->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->textBoxVideoFile->Location = System::Drawing::Point(89, 735);
			this->textBoxVideoFile->Name = L"textBoxVideoFile";
			this->textBoxVideoFile->Size = System::Drawing::Size(515, 22);
			this->textBoxVideoFile->TabIndex = 15;
			// 
			// openFileDialogVideo
			// 
			this->openFileDialogVideo->Title = L"Select Video File";
			// 
			// btnGetVideoFileName
			// 
			this->btnGetVideoFileName->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnGetVideoFileName->Location = System::Drawing::Point(620, 729);
			this->btnGetVideoFileName->Name = L"btnGetVideoFileName";
			this->btnGetVideoFileName->Size = System::Drawing::Size(36, 31);
			this->btnGetVideoFileName->TabIndex = 16;
			this->btnGetVideoFileName->Text = L"...";
			this->btnGetVideoFileName->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			this->btnGetVideoFileName->UseVisualStyleBackColor = true;
			this->btnGetVideoFileName->Click += gcnew System::EventHandler(this, &Form1::btnGetVideoFileName_Click);
			// 
			// timerControl
			// 
			this->timerControl->Enabled = true;
			this->timerControl->Tick += gcnew System::EventHandler(this, &Form1::timerControl_Tick);
			// 
			// textBoxSeqName
			// 
			this->textBoxSeqName->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->textBoxSeqName->Location = System::Drawing::Point(130, 702);
			this->textBoxSeqName->Name = L"textBoxSeqName";
			this->textBoxSeqName->Size = System::Drawing::Size(234, 22);
			this->textBoxSeqName->TabIndex = 18;
			// 
			// btnRecordEvents
			// 
			this->btnRecordEvents->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnRecordEvents->Location = System::Drawing::Point(850, 23);
			this->btnRecordEvents->Name = L"btnRecordEvents";
			this->btnRecordEvents->Size = System::Drawing::Size(74, 47);
			this->btnRecordEvents->TabIndex = 19;
			this->btnRecordEvents->Text = L"Record\r\nEvents (/)";
			this->btnRecordEvents->UseVisualStyleBackColor = true;
			this->btnRecordEvents->Click += gcnew System::EventHandler(this, &Form1::btnRecordEvents_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(14, 705);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(110, 16);
			this->label2->TabIndex = 20;
			this->label2->Text = L"Sequence Name";
			// 
			// labelRecordStatus
			// 
			this->labelRecordStatus->AutoSize = true;
			this->labelRecordStatus->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->labelRecordStatus->ForeColor = System::Drawing::Color::Black;
			this->labelRecordStatus->Location = System::Drawing::Point(763, 21);
			this->labelRecordStatus->Name = L"labelRecordStatus";
			this->labelRecordStatus->Size = System::Drawing::Size(60, 16);
			this->labelRecordStatus->TabIndex = 21;
			this->labelRecordStatus->Text = L"Playing";
			this->labelRecordStatus->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// shapeContainer1
			// 
			this->shapeContainer1->Location = System::Drawing::Point(0, 0);
			this->shapeContainer1->Margin = System::Windows::Forms::Padding(0);
			this->shapeContainer1->Name = L"shapeContainer1";
			this->shapeContainer1->Shapes->AddRange(gcnew cli::array< Microsoft::VisualBasic::PowerPacks::Shape^  >(1) {this->shapeRecording});
			this->shapeContainer1->Size = System::Drawing::Size(1024, 762);
			this->shapeContainer1->TabIndex = 22;
			this->shapeContainer1->TabStop = false;
			// 
			// shapeRecording
			// 
			this->shapeRecording->BackColor = System::Drawing::Color::Black;
			this->shapeRecording->BorderColor = System::Drawing::SystemColors::ButtonShadow;
			this->shapeRecording->BorderWidth = 2;
			this->shapeRecording->FillColor = System::Drawing::Color::Black;
			this->shapeRecording->FillStyle = Microsoft::VisualBasic::PowerPacks::FillStyle::Solid;
			this->shapeRecording->Location = System::Drawing::Point(775, 43);
			this->shapeRecording->Name = L"shapeRecording";
			this->shapeRecording->Size = System::Drawing::Size(33, 34);
			// 
			// btnSequenceSave
			// 
			this->btnSequenceSave->Enabled = false;
			this->btnSequenceSave->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnSequenceSave->Location = System::Drawing::Point(370, 700);
			this->btnSequenceSave->Name = L"btnSequenceSave";
			this->btnSequenceSave->Size = System::Drawing::Size(55, 27);
			this->btnSequenceSave->TabIndex = 23;
			this->btnSequenceSave->Text = L"Save";
			this->btnSequenceSave->UseVisualStyleBackColor = true;
			this->btnSequenceSave->Click += gcnew System::EventHandler(this, &Form1::btnSequenceSave_Click);
			// 
			// btnSequenceNew
			// 
			this->btnSequenceNew->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnSequenceNew->Location = System::Drawing::Point(499, 700);
			this->btnSequenceNew->Name = L"btnSequenceNew";
			this->btnSequenceNew->Size = System::Drawing::Size(55, 27);
			this->btnSequenceNew->TabIndex = 24;
			this->btnSequenceNew->Text = L"New";
			this->btnSequenceNew->UseVisualStyleBackColor = true;
			this->btnSequenceNew->Click += gcnew System::EventHandler(this, &Form1::btnSequenceNew_Click);
			// 
			// btnSequenceLoad
			// 
			this->btnSequenceLoad->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnSequenceLoad->Location = System::Drawing::Point(435, 700);
			this->btnSequenceLoad->Name = L"btnSequenceLoad";
			this->btnSequenceLoad->Size = System::Drawing::Size(55, 27);
			this->btnSequenceLoad->TabIndex = 25;
			this->btnSequenceLoad->Text = L"Load";
			this->btnSequenceLoad->UseVisualStyleBackColor = true;
			this->btnSequenceLoad->Click += gcnew System::EventHandler(this, &Form1::btnSequenceLoad_Click);
			// 
			// eventFirstTextBox
			// 
			this->eventFirstTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->eventFirstTextBox->Location = System::Drawing::Point(462, 663);
			this->eventFirstTextBox->Name = L"eventFirstTextBox";
			this->eventFirstTextBox->Size = System::Drawing::Size(128, 26);
			this->eventFirstTextBox->TabIndex = 26;
			// 
			// eventLastTextBox
			// 
			this->eventLastTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->eventLastTextBox->Location = System::Drawing::Point(607, 663);
			this->eventLastTextBox->Name = L"eventLastTextBox";
			this->eventLastTextBox->Size = System::Drawing::Size(128, 26);
			this->eventLastTextBox->TabIndex = 27;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(458, 640);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(85, 20);
			this->label3->TabIndex = 28;
			this->label3->Text = L"First Event";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(603, 640);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(85, 20);
			this->label4->TabIndex = 29;
			this->label4->Text = L"Last Event";
			// 
			// dataGridViewEvents
			// 
			this->dataGridViewEvents->AllowUserToAddRows = false;
			this->dataGridViewEvents->AllowUserToDeleteRows = false;
			this->dataGridViewEvents->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridViewEvents->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(3) {this->gchTime, 
				this->gchArm, this->gchFed});
			this->dataGridViewEvents->Location = System::Drawing::Point(753, 182);
			this->dataGridViewEvents->Name = L"dataGridViewEvents";
			this->dataGridViewEvents->ReadOnly = true;
			this->dataGridViewEvents->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->dataGridViewEvents->Size = System::Drawing::Size(245, 539);
			this->dataGridViewEvents->TabIndex = 30;
			this->dataGridViewEvents->RowHeaderMouseClick += gcnew System::Windows::Forms::DataGridViewCellMouseEventHandler(this, &Form1::dataGridViewEvents_RowHeaderMouseClick);
			this->dataGridViewEvents->RowHeaderMouseDoubleClick += gcnew System::Windows::Forms::DataGridViewCellMouseEventHandler(this, &Form1::dataGridViewEvents_RowHeaderMouseDoubleClick);
			// 
			// gchTime
			// 
			this->gchTime->HeaderText = L"Time";
			this->gchTime->Name = L"gchTime";
			this->gchTime->ReadOnly = true;
			this->gchTime->Width = 80;
			// 
			// gchArm
			// 
			this->gchArm->HeaderText = L"Arm";
			this->gchArm->Name = L"gchArm";
			this->gchArm->ReadOnly = true;
			this->gchArm->Width = 40;
			// 
			// gchFed
			// 
			this->gchFed->HeaderText = L"Fed";
			this->gchFed->Name = L"gchFed";
			this->gchFed->ReadOnly = true;
			this->gchFed->Width = 80;
			// 
			// btnStepForward
			// 
			this->btnStepForward->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnStepForward.BackgroundImage")));
			this->btnStepForward->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Center;
			this->btnStepForward->Location = System::Drawing::Point(369, 644);
			this->btnStepForward->Name = L"btnStepForward";
			this->btnStepForward->Size = System::Drawing::Size(32, 32);
			this->btnStepForward->TabIndex = 31;
			this->btnStepForward->UseVisualStyleBackColor = true;
			this->btnStepForward->Click += gcnew System::EventHandler(this, &Form1::btnStepForward_Click);
			// 
			// btnStepBack
			// 
			this->btnStepBack->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnStepBack.BackgroundImage")));
			this->btnStepBack->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Center;
			this->btnStepBack->Location = System::Drawing::Point(331, 644);
			this->btnStepBack->Name = L"btnStepBack";
			this->btnStepBack->Size = System::Drawing::Size(32, 32);
			this->btnStepBack->TabIndex = 32;
			this->btnStepBack->UseVisualStyleBackColor = true;
			this->btnStepBack->Click += gcnew System::EventHandler(this, &Form1::btnStepBack_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1024, 762);
			this->Controls->Add(this->btnStepBack);
			this->Controls->Add(this->btnStepForward);
			this->Controls->Add(this->dataGridViewEvents);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->eventLastTextBox);
			this->Controls->Add(this->eventFirstTextBox);
			this->Controls->Add(this->btnSequenceLoad);
			this->Controls->Add(this->btnSequenceNew);
			this->Controls->Add(this->btnSequenceSave);
			this->Controls->Add(this->labelRecordStatus);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->btnRecordEvents);
			this->Controls->Add(this->textBoxSeqName);
			this->Controls->Add(this->btnGetVideoFileName);
			this->Controls->Add(this->textBoxVideoFile);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->btn6);
			this->Controls->Add(this->btn3);
			this->Controls->Add(this->btn2);
			this->Controls->Add(this->btn1);
			this->Controls->Add(this->btn4);
			this->Controls->Add(this->btn7);
			this->Controls->Add(this->btn8);
			this->Controls->Add(this->btn9);
			this->Controls->Add(this->tbStateChange);
			this->Controls->Add(this->rateUpDown);
			this->Controls->Add(this->tblabelRate);
			this->Controls->Add(this->btPlay);
			this->Controls->Add(this->axWindowsMediaPlayer1);
			this->Controls->Add(this->shapeContainer1);
			this->KeyPreview = true;
			this->Name = L"Form1";
			this->Text = L"Squeak";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::Form1_KeyDown);
			this->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::Form1_KeyPress);
			this->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::Form1_MouseClick);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->axWindowsMediaPlayer1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->eventLog1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->rateUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridViewEvents))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

#pragma region Button state change

		// changes controls based on whether player is going
		void SetPlayingControls(void);

#pragma endregion

#pragma region WMP Control
	private: 
		// respond to play button
		System::Void btPlay_Click(System::Object^  sender, System::EventArgs^  e) {
			TogglePlayState();
		}
		// set play state
		void TogglePlayState(void);
		void Pause_Movie(void);
		void Play_Movie(void);
		// set new time
		void WMP_SetPosition(double dPositionSecs)
			{axWindowsMediaPlayer1->Ctlcontrols->currentPosition = dPositionSecs;}
#pragma endregion

private:
#pragma region Form Events: sequence buttons
		// sequence load, new, save
		System::Void btnSequenceSave_Click(System::Object^  sender, System::EventArgs^  e) {
			if(mouseLL != nullptr)
			{
				if(mouseLL->get_Count() > 0)
					Sequence_Save();
			}
			// OLD SEQUENCE
			//if(mouseEvents != nullptr)
			//{
			//	if(mouseEvents->get_NumberEvents() > 0)
			//		Sequence_Save();
			//}
		 }
		System::Void btnSequenceLoad_Click(System::Object^  sender, System::EventArgs^  e) {
			Sequence_Load();
		 }
		System::Void btnSequenceNew_Click(System::Object^  sender, System::EventArgs^  e) {
			Sequence_New(false);
		 }

#pragma endregion
#pragma region Form Events: button click
		// click indicator buttons
		System::Void btn9_Click(System::Object^  sender, System::EventArgs^  e) {
			DirectionButtonPressed(btn9);
		}
		System::Void btn8_Click(System::Object^  sender, System::EventArgs^  e) {
			DirectionButtonPressed(btn8);
		}
		System::Void btn7_Click(System::Object^  sender, System::EventArgs^  e) {
			DirectionButtonPressed(btn7);
		 }
		System::Void btn4_Click(System::Object^  sender, System::EventArgs^  e) {
			DirectionButtonPressed(btn4);
		 }
		System::Void btn1_Click(System::Object^  sender, System::EventArgs^  e) {
			DirectionButtonPressed(btn1);
		 }
		System::Void btn2_Click(System::Object^  sender, System::EventArgs^  e) {
			DirectionButtonPressed(btn2);
		 }
		System::Void btn3_Click(System::Object^  sender, System::EventArgs^  e) {
			DirectionButtonPressed(btn3);
		 }
		System::Void btn6_Click(System::Object^  sender, System::EventArgs^  e) {
			DirectionButtonPressed(btn6);
		 }
#pragma endregion

#pragma region Form Events: data grid
		// single click row
		System::Void dataGridViewEvents_RowHeaderMouseClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellMouseEventArgs^  e) {
			if(e->Button == System::Windows::Forms::MouseButtons::Right)
			{
				RowClicked_Right(e->RowIndex);
			}
			if(e->Button == System::Windows::Forms::MouseButtons::Left)
			{
				RowClicked_Left(e->RowIndex);
			}
		}
		// double click row
		System::Void dataGridViewEvents_RowHeaderMouseDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellMouseEventArgs^  e) {
		}
#pragma endregion

#pragma region Form Events: other
		// toggle between play and record events
		System::Void btnRecordEvents_Click(System::Object^  sender, System::EventArgs^  e) {
			ToggleRecording();
		 }

		// timer
		System::Void timerControl_Tick(System::Object^  sender, System::EventArgs^  e) {
			// run code for timer tick
			TimerTick(e);
		 }

		// clicked load button
		System::Void btnGetVideoFileName_Click(System::Object^  sender, System::EventArgs^  e) {
			if ( openFileDialogVideo->ShowDialog() == ::System::Windows::Forms::DialogResult::OK )
			{
				LoadVideoFile(openFileDialogVideo->FileName);
			}

		 }
		// clicked on form
		System::Void Form1_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			if(btnSelected != nullptr)
				Form_Mouse_Pressed(e);
		 }
		// closing form
		System::Void Form1_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
			// check for dirty sequence
			bool bCloseCancel = false;
			bool bOkClose = false;
			// bOkClose = true if no sequence or sequence saved; bCloseCancel will be true of close should be cancelled
			bOkClose = Sequence_OK_New(&bCloseCancel);
			if(bCloseCancel)
				e->Cancel = true;
		}
		// step forward
		System::Void btnStepForward_Click(System::Object^  sender, System::EventArgs^  e) {
			if(!WMP_IsPlaying())
				((WMPLib::IWMPControls2^)axWindowsMediaPlayer1->Ctlcontrols)->step(1);
		}
		// step back
		System::Void btnStepBack_Click(System::Object^  sender, System::EventArgs^  e) {
			if(!WMP_IsPlaying())
				((WMPLib::IWMPControls2^)axWindowsMediaPlayer1->Ctlcontrols)->step(-1);
		}

#pragma endregion

#pragma region WMP state
	private:
			// set flag for whether WMP is playing
			void WMP_CheckPlaying() {
				bWMPPlaying = (axWindowsMediaPlayer1->playState == WMPLib::WMPPlayState::wmppsPlaying);
				SetPlayingControls();
			}
			// check if playing
			bool WMP_IsPlaying() {
				return (axWindowsMediaPlayer1->playState == WMPLib::WMPPlayState::wmppsPlaying);
			}
			// get rate
			double WMP_GetRate() {
				return axWindowsMediaPlayer1->settings->rate;
				// dWMP_Rate = axWindowsMediaPlayer1->settings->rate;
				//dWMP_Rate = axWindowsMediaPlayer1->settings->rate();
			}
			// get rate
			double WMP_GetPosition() {
				return axWindowsMediaPlayer1->Ctlcontrols->currentPosition;
				// dWMP_Rate = axWindowsMediaPlayer1->settings->rate;
				//dWMP_Rate = axWindowsMediaPlayer1->settings->rate();
			}
			// get frame rate
			System::String^ WMP_FrameRate() {				
				return axWindowsMediaPlayer1->currentMedia->getItemInfo("FrameRate");
				// dWMP_Rate = axWindowsMediaPlayer1->settings->rate;
				//dWMP_Rate = axWindowsMediaPlayer1->settings->rate();
			}
			// rate change in WMP
			System::Void rateUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e){
				dWMP_Rate = double(rateUpDown->Value);
				axWindowsMediaPlayer1->settings->rate = dWMP_Rate;
			}
			// slider position changed
			System::Void axWindowsMediaPlayer1_PositionChange(System::Object^  sender, AxWMPLib::_WMPOCXEvents_PositionChangeEvent^  e) {
				if(!WMP_IsPlaying())
				{
					//WMPLib::IWMPControls2^ ctls2 = axWindowsMediaPlayer1->Ctlcontrols;
					((WMPLib::IWMPControls2^)axWindowsMediaPlayer1->Ctlcontrols)->step(1);
					//CAxWindow                   m_wndView;
					///CComPtr<IWMPPlayer>         m_spWMPPlayer;
					//IWMPControls2 Ctlcontrols2 = (IWMPControls2)axWindowsMediaPlayer1->Ctlcontrols;
				}
			}
			// check if media player has video
			bool WMP_HasVideo(void)
				{ return (axWindowsMediaPlayer1->URL->Length != 0);}
			// check max duration of media
			double WMP_Duration(void)
				{return axWindowsMediaPlayer1->currentMedia->duration;}
				//{ double dDuration; ((WMPLib::IWMPMedia^)axWindowsMediaPlayer1->currentMedia)->get_duration(&dDuration); return dDuration;}
				//  return axWindowsMediaPlayer1->currentMedia->get_Duration;}

#pragma endregion

#pragma region WMP events
	private: 	
		System::Void axWindowsMediaPlayer1_MouseDownEvent(System::Object^  sender, AxWMPLib::_WMPOCXEvents_MouseDownEvent^  e) {
				if(btnSelected != nullptr)
					WMP_Mouse_Pressed(e);
			}

		System::Void axWindowsMediaPlayer1_PlayStateChange(System::Object^  sender, AxWMPLib::_WMPOCXEvents_PlayStateChangeEvent^  e) {
				 //WMP_CheckPlaying();
				 SetPlayingControls();
			 }
#pragma endregion

#pragma region Input events
	private: 
		System::Void Form1_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e);
		System::Void Form1_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);

#pragma endregion

#pragma region timer and idle
	// IDLE event
	public:
		void MemberIdle(Object^ sender, EventArgs^ e) {
			// check rate
			dWMP_Rate = WMP_GetRate();
			if(double(rateUpDown->Value) != dWMP_Rate)
				rateUpDown->Value = Decimal(dWMP_Rate);
			
    }
	void TimerTick(System::EventArgs^  e);
#pragma endregion

#pragma region button selection
private:
		void DirectionButtonPressed(System::Windows::Forms::Button^ btnPressed);
		void WMP_Mouse_Pressed(AxWMPLib::_WMPOCXEvents_MouseDownEvent^  e);
		void Form_Mouse_Pressed(System::Windows::Forms::MouseEventArgs^  e);
		void MoveDirectionButton(Drawing::Point btnNewLocation);
#pragma endregion

#pragma region numpad change
		// number pad button down
		void NumPadDown(System::Windows::Forms::KeyEventArgs^  e);
		//void NumButtonDown(int iButtonDownIndex,System::Windows::Forms::KeyEventArgs^  e); // called if button is down
		//void NumButtonUp(int iButtonUpIndex);	  // called if button is not down
//		void NumPadUp(int iButtonArrayIndex);
#pragma endregion

#pragma region loading video files
		void LoadVideoFile(System::String^ strFileName);
#pragma endregion

#pragma region data grid methods
		// enable/disable grid, enable only when not playing and/or recording
		System::Void EnableGrid(bool bEnable);
		// update grid data
		System::Void UpdateGridData(void);
#pragma endregion
#pragma region data grid events
		// clicked right button on row header
		System::Void RowClicked_Right(int zeroRowIndex);
		// clicked left button on row header
		System::Void RowClicked_Left(int zeroRowIndex);
#pragma endregion

#pragma region arm indicator change events
private:
	// set button from key events
	System::Void _KeyboardSetButtonState(System::Windows::Forms::KeyEventArgs^  e);
	// set button from forced state, prevents recording
	System::Void _StateSetButtonState(int iArm, bool bFeeding);
	// sets visible button state (bControl == feeding)
	System::Void _SetButtonState(int iArm, bool bFeeding, bool bRecording);
	// use to ket gey codes from button down
	// int _GetButtonStateFromKeyboard(System::Windows::Forms::KeyEventArgs^  e);
#pragma endregion

#pragma region recording events
private:
		void ToggleRecording(void);
		// check if OK to create new sequence (e.g. sequence exists & is dirty, checked overwrite or saved)
		bool Sequence_OK_New(bool* p_bCancel); 
		bool Sequence_CheckOverwrite(bool* p_bCancel); // ask user to save, overwrite, or cancel
		bool Sequence_New(bool bFromRecordButton);	// create new sequence
		void Sequence_Load(void);	// load sequence
		void Sequence_Save(void);	// load sequence
		// make default series name
		System::String^ DefaultSeriesName(void);
		// update first, last times on form
		System::Void UpdateFormEventTimes(void);

#pragma endregion

	// generic text box input
	System::String^ TextInputBox(System::String^ strPrompt, System::String^ strCaption, System::String^ strDefault);
	// convert movie time in double to MM:SS:ss format
	System::String^ MovieTimeToString(double dMovieTime);

	// load file dialog
	System::String^ _LoadSeqDialog(void);
	
};
};


