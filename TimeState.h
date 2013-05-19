#pragma once
ref class TimeState
{
public:
	TimeState(void);
	 
	int iButton;
	bool bFeeding;

private: 
		System::Windows::Forms::Button^ btnActive;

public:
	System::Windows::Forms::Button^ getButton(){
		return btnActive;
	}
	void setButton(System::Windows::Forms::Button^ btnNewActive){
		btnActive = btnNewActive;
	}
};

