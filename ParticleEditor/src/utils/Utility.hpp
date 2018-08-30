#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <algorithm>
#include <SFML/System/Vector2.hpp>

namespace px
{
	namespace utils
	{
		inline void clampVec(sf::Vector2f & value, const float & min, const float & max)
		{
			value.x = std::clamp(value.x, min, max);
			value.y = std::clamp(value.y, min, max);
		};

		inline void constrainNegatives(float & value)
		{
			value = std::clamp(value, 0.f, std::numeric_limits<float>::max());
		};

		inline void constrainNegativesVec(sf::Vector2f & value)
		{
			value.x = std::clamp(value.x, 0.f, std::numeric_limits<float>::max());
			value.y = std::clamp(value.y, 0.f, std::numeric_limits<float>::max());
		};

		inline void constrainDistrVec(sf::Vector2f & value)
		{
			value.x = std::clamp(value.x, 0.f, value.y);
			value.y = std::clamp(value.y, value.x, std::numeric_limits<float>::max());
		};

		inline void constrainDistrVec(float & value_1, float & value_2)
		{
			value_1 = std::clamp(value_1, 0.f, value_2);
			value_2 = std::clamp(value_2, value_1, std::numeric_limits<float>::max());
		};
	}
}
