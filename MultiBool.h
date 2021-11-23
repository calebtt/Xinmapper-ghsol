#pragma once

namespace sds
{
	/// <summary>
	/// Probably a "finite state machine", used to track the progress of key presses.
	/// </summary>
	struct MultiBool
	{
		/// <summary>
		/// C++11 style "enum class" that adds type safety and strong scoping.
		/// used by MultiBool as a type of finite state machine helper to keep track of the current state
		/// </summary>
		enum class BUTTONSTATE
		{
			STATE_ONE,
			STATE_TWO,
			STATE_THREE,
			STATE_FOUR,
			STATE_FIVE,
			STATE_SIX
		} current_state;

		MultiBool() : current_state(BUTTONSTATE::STATE_ONE) {}
		MultiBool( const MultiBool &rhs )
		{
			this->current_state = rhs.current_state;
		}
		MultiBool(const BUTTONSTATE s)
		{
			current_state = s;
		}
		void ResetState()
		{
			current_state = BUTTONSTATE::STATE_ONE;
		}
	};

}//end namespace