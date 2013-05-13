// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__STANDARD_MENU_HPP
#define XPCC__STANDARD_MENU_HPP

#include "abstract_menu.hpp"
#include "scrollable_text.hpp"
#include "menu_entry_callback.hpp"
#include <xpcc/container/doubly_linked_list.hpp>
#include <xpcc/workflow/periodic_timer.hpp>

#define DISPLAY_UPDATE_TIME 500 //after this time the display is redrawn

namespace xpcc
{
	struct MenuEntry
	{
		MenuEntry(const char* text, uint16_t space, MenuEntryCallback func);
		
		ScrollableText text;
		/*const char* text;*/
		MenuEntryCallback callback;
	};
	
	class StandardMenu : public AbstractMenu
	{
	public:

		StandardMenu(xpcc::ViewStack* stack, uint8_t identifier);
		
		virtual ~StandardMenu() = 0;

		StandardMenu(xpcc::ViewStack* stack, uint8_t identifier, const char* title);

		void
		addEntry(const char* text, MenuEntryCallback func);

		void
		setTitle(const char* text);

		virtual void
		shortButtonPress(xpcc::MenuButtons::Button button);

		virtual bool
		hasChanged();

		virtual void
		draw();

		virtual void
		selectedEntryFunction(uint8_t selected);

	private:

		xpcc::PeriodicTimer<> timer;
		bool buttonAction;
		bool newOnStack;

		const char* title;

		uint8_t maximalDrawnEntrys;

		uint8_t homePosition; // first entry drawn
		uint8_t position; // selected entry

	protected:

		typedef xpcc::DoublyLinkedList<MenuEntry> EntryList;
		EntryList entries;

	};
}

#endif // XPCC__STANDARD_MENU_HPP
