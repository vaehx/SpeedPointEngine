////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine Source Code
//
//	written by iSmokiieZz
//	(c) 2011 - 2014, All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <Util\SAPI.h>

namespace SpeedPoint
{
	// SpeedPoint efficient Queue implementation
	template<class T>
	class S_API SQueue
	{
	private:


	public:
		struct Slot
		{
			T* pObject;
		};

		// Get ptr to the front item (pop_front)
		T* Pop();

		// Get ptr to the front slot
		Slot* PopSlot();

		// Push copy of the object to the back of the queue (push_back)
		// returns slot ptr
		Slot* Push(const T& obj);

		// Push ptr of the object to the back of the queue
		// return slot ptr
		Slot* Push(T* pObj);
	};
}