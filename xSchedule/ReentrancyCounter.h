#pragma once

/***************************************************************
 * This source files comes from the xLights project
 * https://www.xlights.org
 * https://github.com/smeighan/xLights
 * See the github commit history for a record of contributing
 * developers.
 * Copyright claimed based on commit dates recorded in Github
 * License: https://github.com/smeighan/xLights/blob/master/License.txt
 **************************************************************/

#include <atomic>

class ReentrancyCounter
{
	std::atomic<int>& _ref;

	public:
		ReentrancyCounter(std::atomic<int>& ref) : _ref(ref)
		{
		    ++_ref;
		}
		virtual ~ReentrancyCounter()
		{
            --_ref;
			//wxASSERT(_ref >= 0);
            if (_ref < 0) _ref = 0;
		}
		bool SoleReference()
		{
            return _ref == 1;
		}
};

