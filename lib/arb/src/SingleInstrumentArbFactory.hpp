/*
 * SingleInstrumentArbFactory.hpp
 *
 *  Created on: Oct 14, 2018
 *      Author: suoalex
 */

#ifndef LIB_ARB_SRC_SINGLEINSTRUMENTARBFACTORY_HPP_
#define LIB_ARB_SRC_SINGLEINSTRUMENTARBFACTORY_HPP_

#include <string>

#include <IServiceFactory.hpp>
#include <Logger.hpp>

namespace mm
{
	struct SingleInstrumentArbConfig
	{
		// The class name for sanity check.
		static const std::string CLASS_NAME;

		// The dispatch key for product service.
		static const std::string DISPATCH_KEY;

		// The strategy ID for dispatching.
		static const std::string STRATEGY_ID;

		// The instrument ID being arbed.
		static const std::string INSTRUMENT_ID;

		// The sample count used.
		static const std::string SAMPLE_COUNT;
	};

	//
	// The factory for single instrument arb.
	//
	class SingleInstrumentArbFactory : public IServiceFactory
	{
	public:

		// Flag if the factory is registered.
		static const bool registered;

		// virtual destructor.
		virtual ~SingleInstrumentArbFactory();

		//
		// Implement the service factory.
		//
		virtual std::shared_ptr<IService> createService(
				const std::string serviceClass,
				const std::string serviceName,
				const std::shared_ptr<IConfig> config,
				ServiceContext& context) override;

	private:

		// Logger of the class.
		static Logger logger;
	};
}



#endif /* LIB_ARB_SRC_SINGLEINSTRUMENTARBFACTORY_HPP_ */
