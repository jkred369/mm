/*
 * FemasOrderSessionFactory.hpp
 *
 *  Created on: Oct 12, 2018
 *      Author: suoalex
 */

#ifndef LIB_FEMAS_SRC_FEMASORDERSESSIONFACTORY_HPP_
#define LIB_FEMAS_SRC_FEMASORDERSESSIONFACTORY_HPP_

#include <string>

#include <IServiceFactory.hpp>
#include <Logger.hpp>

namespace mm
{
	struct FemasOrderSessionConfig
	{
		// The class name for sanity check.
		static const std::string CLASS_NAME;

		// The dispatch key for product service.
		static const std::string DISPATCH_KEY;

		// The service name for product service to use.
		static const std::string PRODUCT_SERVICE_NAME;

		// The CPU affinity for threads.
		static const std::string CPU_AFFINITY;
	};

	//
	// The factory for product service.
	//
	class FemasOrderSessionFactory : public IServiceFactory
	{
	public:

		// Flag if the factory is registered.
		static const bool registered;

		// virtual destructor.
		virtual ~FemasOrderSessionFactory();

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




#endif /* LIB_FEMAS_SRC_FEMASORDERSESSIONFACTORY_HPP_ */
