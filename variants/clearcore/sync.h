/*
  Copyright (c) 2019 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdint.h>

#ifndef _SYNC_H_
#define _SYNC_H_
/*
 * Synchronization primitives.
 */

class __Guard {
public:
	__Guard() : primask(__get_PRIMASK()), loops(1) {
		__disable_irq();
	}
	~__Guard() {
		if (primask == 0) {
			__enable_irq();
			// http://infocenter.arm.com/help/topic/com.arm.doc.dai0321a/BIHBFEIB.html
			__ISB();
		}
	}
	uint32_t enter() { return loops--; }
private:
	uint32_t primask;
	uint32_t loops;
};

#define synchronized for (__Guard __guard; __guard.enter(); )

#endif