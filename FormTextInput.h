#pragma once

namespace Squeak {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for FormTextInput
	/// </summary>
	public ref class FormTextInput : public System::Windows::Forms::Form
	{
	public:
		FormTextInput(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~FormTextInput()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  buttonOK;
	protected: 
	private: System::Windows::Forms::Button^  buttonCancel;
	private: System::Windows::Forms::TextBox^  inputTextBox;
	private: System::Windows::Forms::Label^  msgLabel;



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->buttonOK = (gcnew System::Windows::Forms::Button());
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			this->inputTextBox = (gcnew System::Windows::Forms::TextBox());
			this->msgLabel = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// buttonOK
			// 
			this->buttonOK->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->buttonOK->Location = System::Drawing::Point(341, 50);
			this->buttonOK->Name = L"buttonOK";
			this->buttonOK->Size = System::Drawing::Size(76, 27);
			this->buttonOK->TabIndex = 0;
			this->buttonOK->Text = L"OK";
			this->buttonOK->UseVisualStyleBackColor = true;
			// 
			// buttonCancel
			// 
			this->buttonCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->buttonCancel->Location = System::Drawing::Point(433, 50);
			this->buttonCancel->Name = L"buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(76, 27);
			this->buttonCancel->TabIndex = 1;
			this->buttonCancel->Text = L"Cancel";
			this->buttonCancel->UseVisualStyleBackColor = true;
			// 
			// inputTextBox
			// 
			this->inputTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->inputTextBox->Location = System::Drawing::Point(21, 54);
			this->inputTextBox->Name = L"inputTextBox";
			this->inputTextBox->Size = System::Drawing::Size(289, 20);
			this->inputTextBox->TabIndex = 2;
			// 
			// msgLabel
			// 
			this->msgLabel->AutoSize = true;
			this->msgLabel->Location = System::Drawing::Point(18, 22);
			this->msgLabel->Name = L"msgLabel";
			this->msgLabel->Size = System::Drawing::Size(52, 13);
			this->msgLabel->TabIndex = 3;
			this->msgLabel->Text = L"msgLabel";
			// 
			// FormTextInput
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(521, 89);
			this->Controls->Add(this->msgLabel);
			this->Controls->Add(this->inputTextBox);
			this->Controls->Add(this->buttonCancel);
			this->Controls->Add(this->buttonOK);
			this->Name = L"FormTextInput";
			this->Text = L"FormTextInput";
			this->Activated += gcnew System::EventHandler(this, &FormTextInput::FormTextInput_Load);
			this->Load += gcnew System::EventHandler(this, &FormTextInput::FormTextInput_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	public:
		void SetCaption(System::String^ strCaption)
		{
			this->Text = strCaption;
		}
		void SetPrompt(System::String^ strPrompt)
		{
			msgLabel->Text = strPrompt;
		}
		void SetDefault(System::String^ strDefault)
		{
			inputTextBox->Text = strDefault;
		}

		System::String^ GetInput(void)
		{
			return inputTextBox->Text;
		}

private:
		// set focus to textbox
		System::Void FormTextInput_Load(System::Object^  sender, System::EventArgs^  e)
		{
			inputTextBox->Focus();
		}
};
}
