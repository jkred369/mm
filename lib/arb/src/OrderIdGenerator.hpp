/*
 * OrderIdGenerator.hpp
 *
 *  Created on: Oct 16, 2018
 *      Author: suoalex
 */

#ifndef LIB_ARB_SRC_ORDERIDGENERATOR_HPP_
#define LIB_ARB_SRC_ORDERIDGENERATOR_HPP_

#include <atomic>
#include <cmath>
#include <cstdint>
#include <ctime>

#include <NativeDefinition.hpp>
#include <TimeUtil.hpp>

namespace mm
{
	//
	// Create an order ID generator following ID generator concept. The generated ID satisfies:
	//
	// - always positive
	// - uniqueness within a day
	// - distinguishable across strategy
	// - ASCII increment and numerical increment
	// - thread safety
	//
	// For the above reasons and especially performance its written separate from common IdGenerator.
	//
	class OrderIdGenerator
	{
	public:

		// The invalid order ID.
		static constexpr std::int64_t invalidOrderId = -1;

		// The default shift factor.
		static constexpr std::int64_t defaultShiftDigit = 6;

		// The default strategy shift digit.
		static constexpr std::int64_t defaultStrategyDigit = 2;

		//
		// Constructor.
		//
		// shiftFactor : Max amount of order ID to generate in 10 ^ N.
		// strategyDigit : Max strategy digit.
		//
		OrderIdGenerator(
				const std::int64_t shiftDigit = defaultShiftDigit,
				const std::int64_t strategyDigit = defaultStrategyDigit) :
			timestamp(getTimeAsInt(std::time(0))),
			shiftFactor(std::pow(10, shiftDigit)),
			strategyShiftFactor(std::pow(10, strategyDigit)),
			counter(0)
		{
			if (timestamp <= 0)
			{
				throw std::invalid_argument("Timestamp must be positive");
			}

			if (shiftFactor < 1)
			{
				throw std::invalid_argument("Invalid shift factor");
			}
		}

		//
		// Generate the ID.
		//
		// strategy ID: The strategy ID requesting for next ID.
		//
		// return : The new generated ID as timestamp * shiftFacor + incremented counter * strategy shift + strategy ID.
		// Or -1 if an ID cannot be generated.
		//
		inline std::int64_t generate(std::int64_t strategyId)
		{
			if (strategyId >= 0 && strategyId < strategyShiftFactor)
			{
				return generate() + strategyId;
			}

			return invalidOrderId;
		}

	protected:

		//
		// Generate the ID as an ID generator to implement IdGenerator concept.
		//
		// return : The new generated ID as timestamp * shiftFacor + incremented counter * strategy shift.
		//
		inline std::int64_t generate()
		{
			return timestamp * shiftFactor + (++counter) * strategyShiftFactor;
		}

	private:

		// Prefix timestamp of the ID.
		const std::int64_t timestamp;

		// Shift factor aka the max of ID generated.
		const std::int64_t shiftFactor;

		// The strategy ID shift factor.
		const std::int64_t strategyShiftFactor;

		// Counter for increment.
		std::atomic<std::int64_t> counter;
	};
}



#endif /* LIB_ARB_SRC_ORDERIDGENERATOR_HPP_ */
