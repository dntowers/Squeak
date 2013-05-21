#pragma once

//class MouseLL;
#include "MouseLL.h"

//#ifndef __GRIDROWDATA_H_INCLUDED__   // if x.h hasn't been included yet...
//#define __GRIDROWDATA_H_INCLUDED__   //   #define this so the compiler knows it has been included


//using namespace System::Collections::Generic;

ref class GridRowData : public System::Object
{
public:
	GridRowData(void);

	double t_event;
	bool   feed;
	int	   arm;

	MouseLLEvent^ m_row_event;
};


//ref class GridRowDataVector 
//{
//	List<GridRowData^>^ grid_row_list
//
//};

//#endif 