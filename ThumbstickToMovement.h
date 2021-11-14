#pragma once
#include "stdafx.h"


namespace sds
{
	/// <summary>
	/// Basic logic for mapping thumbstick values to SendInput mouse movement values.
	/// A single instance for a single thumbstick is to be used.
	/// </summary>
	class ThumbstickToMovement
	{
		bool isDeadzoneActivated;
		bool altDeadzoneConfig;

		float altDeadzoneMultiplier;
		int playerDeadzone;
		int mouseSensitivity;

		const int SENSITIVITY_MIN = 0;
		const int SENSITIVITY_MAX = 100;
		const int MOUSE_SMOOTHING = 2;//2

	public:
		//Ctor for not using the alternate deadzone configuration
		ThumbstickToMovement(int sensitivityMouse, int deadzone)
			: mouseSensitivity(sensitivityMouse), playerDeadzone(deadzone), altDeadzoneConfig(false), altDeadzoneMultiplier(1.0f), isDeadzoneActivated(false)
		{
		}

		//Ctor for using the alternate deadzone configuration
		ThumbstickToMovement(int sensitivityMouse, int deadzone, float multiplier)
			: mouseSensitivity(sensitivityMouse), playerDeadzone(deadzone), altDeadzoneConfig(false), altDeadzoneMultiplier(multiplier), isDeadzoneActivated(false)
		{
		}

		/// <summary>
		/// Determines if the X or Y values are greater than the deadzone values and would
		/// thus require movement from the mouse.
		/// </summary>
		/// <param name="x">X value</param>
		/// <param name="y">Y value</param>
		/// <returns>true if requires moving the mouse, false otherwise</returns>
		bool DoesRequireMove(int x, int y)
		{
			bool xMove, yMove;
			xMove = (x > playerDeadzone || x < -playerDeadzone);
			yMove = (y > playerDeadzone || y < -playerDeadzone);

			if (altDeadzoneConfig && isDeadzoneActivated)
			{
				//initial test that will toggle off the isDeadzoneActivated
				//if neither passes the full deadzone value
				if (!xMove && !yMove)
				{
					isDeadzoneActivated = false;
					return false;
				}
				return true;
			}
			else if (xMove || yMove)
			{
				isDeadzoneActivated = true;
				return true;
			}
			else
				return false;
		}

		/// <summary>
		/// Normalize axis value from the thumbstick with regard to deadzone,
		/// and after that the sensitivity value via NormalizeRange.
		/// </summary>
		/// <param name="x"></param>
		/// <returns>An adjusted number of pixels to move, to be used with the getFunctionalValue
		/// sensitivity function.</returns>
		LONG NormalizeAxis(long long x)
		{
			//Copy some values
			int t_sens = mouseSensitivity;
			bool localDeadzoneConfig = this->altDeadzoneConfig;
			bool localIsDeadzoneActivated = this->isDeadzoneActivated;
			float localMultiplier = this->altDeadzoneMultiplier;
			int deadzoneVal = altDeadzoneConfig ? playerDeadzone * localMultiplier : playerDeadzone;

			auto reduceToLongLimit = [](long long valx)
			{
				if (valx <= std::numeric_limits<LONG>::min())
					return static_cast<LONG>(std::numeric_limits<LONG>::min() + 1);
				else if (valx >= std::numeric_limits<SHORT>::max())
					return static_cast<LONG>(std::numeric_limits<LONG>::max() - 1);
				else
					return static_cast<LONG>(valx);
			};
			auto deadzoneMultiplierFunc = [&t_sens, &localDeadzoneConfig, &localIsDeadzoneActivated, &deadzoneVal, &reduceToLongLimit](long long dzv, double dzm, long long val, bool isPositiveVal)
			{
				if (isPositiveVal)
				{
					val -= (dzv);
					if (val <= 0)
						return 0l;
				}
				else
				{
					val += (dzv);
					if (val >= 0)
						return 0l;
				}
				return static_cast<LONG>(reduceToLongLimit(val));
			};


			if (x > 0)
			{
				x = deadzoneMultiplierFunc(playerDeadzone, altDeadzoneMultiplier, x, true);
				if (x != 0)
				{
					if (x > std::numeric_limits<SHORT>::max())
						x = static_cast<long long>(std::numeric_limits<SHORT>::max())-1;
					x = NormalizeRange(0, std::numeric_limits<SHORT>::max() - 1, static_cast<LONG>(t_sens), reduceToLongLimit(x));
				}
				
			}
			else if (x < 0)
			{
				x = deadzoneMultiplierFunc(playerDeadzone, altDeadzoneMultiplier, x, false);
				if (x != 0)
				{
					if (x < std::numeric_limits<SHORT>::min())
						x = static_cast<long long>(std::numeric_limits<SHORT>::min())+1;
					x = -static_cast<long long>(NormalizeRange(0, std::numeric_limits<SHORT>::max() - 1, static_cast<LONG>(t_sens), -reduceToLongLimit(x)));
				}
				
			}
			return static_cast<LONG>(x);
		}

		/// <summary>
		/// WARNING: Function only works with positive values!
		/// If you need a negative value, pass a positive value and negate the result.
		/// This function takes two range values (begin, end) and a sensitivity value (rp),
		/// it also accepts the current value that the thumbstick is reporting (val) minus the deadzone value.
		/// It returns a sensitivity "normalized" value that is eventually translated into the number of
		/// pixels to move the mouse pointer after a "functional value" is obtained via "getFunctionalValue".
		/// </summary>
		/// <param name="begin"> is the range begin</param>
		/// <param name="end"> is the range end</param>
		/// <param name="rp"> is the sensitivity value to use in the calculation</param>
		/// <param name="val"> is the thumbstick value</param>
		/// <returns> a value which becomes a number of pixels to move, based on the sensitivity</returns>
		long long NormalizeRange(LONG begin, LONG end, LONG rp, LONG val)
		{
			if (val == 0)
				return 0ll;
			long long step = (end - begin) / rp;
			for (long long i = begin, j = 1; i <= end; i += step, ++j)
			{
				if (val >= i && val < i + step)
				{
					if (val == end)
						return static_cast<long long>(j) - 1;
					return static_cast<long long>(j);
				}
			}
			return 0ll;
		}

		/// <summary>
		/// Sensitivity function is the graph of x*x/mouseSensitivity + 1
		/// returns a sensitivity normalized value; or 1 if the result is 0
		/// </summary>
		/// <param name="x">normalized value to adjust for sensitivity</param>
		/// <returns> a sensitivity normalized value or 1 if the result is 0</returns>
		size_t getFunctionalValue(size_t x) const
		{
			x = static_cast<size_t>((x * x) / mouseSensitivity + 1);
			return (x == 0) ? 1 : x;
		}

		/// <summary>
		/// Runs through the gamut of utility functions to get the number of pixels to move
		/// before moving the mouse.
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		std::vector<std::tuple<int,int>> GetFinalInput(LONG x, LONG y)
		{
			//NormalizeAxis will subtract (or add) the deadzone value from the numbers.
			long long x_value = NormalizeAxis(x);
			long long y_value = 0;

			//Have to be careful inverting signs,
			//can overflow easily!
			if (y <= std::numeric_limits<SHORT>::min())
			{
				y = std::numeric_limits<SHORT>::min() + 1;
			}

			//here y is inverted, to invert the y axis.
			//this is because the coordinate plane for pixels starts at the top-left on windows
			y_value = -NormalizeAxis(y);

			if (x_value != 0)
			{
				if (x_value > 0)
				{
					x_value = getFunctionalValue(x_value);
				}
				else
				{
					x_value = -(LONG)getFunctionalValue(abs(x_value));
				}
			}

			if (y_value != 0)
			{
				if (y_value > 0)
				{
					y_value = getFunctionalValue(y_value);
				}
				else
				{
					y_value = -(LONG)getFunctionalValue(abs(y_value));
				}
			}

			return GetPackagedInput(x_value, y_value);
		}

		std::vector<std::tuple<int,int>> GetPackagedInput(LONG x, LONG y)
		{
			std::vector<std::tuple<int, int>> ret;
			int abX = std::abs(x);
			int abY = std::abs(y);

			bool xGood = (abX <= MOUSE_SMOOTHING);
			bool yGood = (abY <= MOUSE_SMOOTHING);
			if(xGood && yGood)
			{
				ret.push_back(std::make_tuple(x, y));
				return ret;
			}

			int countX = abX / MOUSE_SMOOTHING;
			int countY = abY / MOUSE_SMOOTHING;
			//int remainX = x % MOUSE_SMOOTHING;
			//int remainY = y % MOUSE_SMOOTHING;
			int remainX = 0;
			int remainY = 0;
			if (abX > MOUSE_SMOOTHING)
				remainX = x % MOUSE_SMOOTHING;
			else
				remainX = x;
			if (abY > MOUSE_SMOOTHING)
				remainY = y % MOUSE_SMOOTHING;
			else
				remainY = y;

			//send smooth step amounts for x and y at the same time
			while ((countX > 0) && (countY > 0))
			{
				int xv = x > 0 ? MOUSE_SMOOTHING : -MOUSE_SMOOTHING;
				int yv = y > 0 ? MOUSE_SMOOTHING : -MOUSE_SMOOTHING;
				ret.push_back(std::make_tuple(xv, yv));
				countX--;
				countY--;
			}
			//one or the other is now zero
			while (countX > 0)
			{
				if (x > 0)
				{
					ret.push_back(std::make_tuple(MOUSE_SMOOTHING, remainY));
					remainY = 0;
				}
				else
				{
					ret.push_back(std::make_tuple(-MOUSE_SMOOTHING, remainY));
					remainY = 0;
				}
				countX--;
			}
			while (countY > 0)
			{
				if (y > 0)
				{
					ret.push_back(std::make_tuple(remainX, MOUSE_SMOOTHING));
					remainX = 0;
				}
				else
				{
					ret.push_back(std::make_tuple(remainX, -MOUSE_SMOOTHING));
					remainX = 0;
				}
				countY--;
			}
			if (remainX || remainY)
			{
				ret.push_back(std::make_tuple(remainX, remainY));
			}
			return ret;
		}

	};
}

