/*
 * SingleInstrumentArbFactory.cpp
 *
 *  Created on: Oct 14, 2018
 *      Author: suoalex
 */

#include <DispatchKey.hpp>
#include <DelegateServiceFactory.hpp>

#include "SingleInstrumentArb.hpp"
#include "SingleInstrumentArbFactory.hpp"

const std::string mm::SingleInstrumentArbConfig::CLASS_NAME = "SingleInstrumentArb";
const std::string mm::SingleInstrumentArbConfig::DISPATCH_KEY = "DispatchKey";
const std::string mm::SingleInstrumentArbConfig::STRATEGY_ID = "StrategyId";
const std::string mm::SingleInstrumentArbConfig::INSTRUMENT_ID = "InstrumentId";
const std::string mm::SingleInstrumentArbConfig::SAMPLE_COUNT = "SampleCount";

const bool mm::SingleInstrumentArbFactory::registered = mm::DelegateServiceFactory::getFactory().registerFactory(
		SingleInstrumentArbConfig::CLASS_NAME, new mm::SingleInstrumentArbFactory());

mm::Logger mm::SingleInstrumentArbFactory::logger;

namespace mm
{
	SingleInstrumentArbFactory::~SingleInstrumentArbFactory()
	{
	}

	std::shared_ptr<IService> SingleInstrumentArbFactory::createService(
			const std::string serviceClass,
			const std::string serviceName,
			const std::shared_ptr<IConfig> config,
			ServiceContext& context)
	{
		// sanity check
		if (serviceClass != SingleInstrumentArbConfig::CLASS_NAME)
		{
			LOGERR("Cannot create service with class: {}. Only support class: {}", serviceClass, SingleInstrumentArbConfig::CLASS_NAME);
			return std::shared_ptr<IService> ();
		}

		// creation
		const KeyType dispatchKey = config->getInt64(SingleInstrumentArbConfig::DISPATCH_KEY);
		const std::int64_t strategyId = config->getInt64(SingleInstrumentArbConfig::STRATEGY_ID);
		const std::int64_t instrumentId = config->getInt64(SingleInstrumentArbConfig::INSTRUMENT_ID);
		const std::int64_t sampleCount = config->getInt64(SingleInstrumentArbConfig::SAMPLE_COUNT);

		// sanity check
		if (sampleCount <= 0)
		{
			LOGERR("Cannot create arb strategy with non-positive sampleCount: {}", sampleCount);
			return std::shared_ptr<IService> ();
		}

		return std::shared_ptr<IService> (new SingleInstrumentArb(dispatchKey, serviceName, context,
				strategyId, instrumentId, static_cast<std::size_t> (sampleCount)));
	}
}


