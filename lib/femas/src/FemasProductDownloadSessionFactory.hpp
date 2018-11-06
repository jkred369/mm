/*
 * FemasProductDownloadSessionFactory.hpp
 *
 *  Created on: 6 Nov 2018
 *      Author: alexsuo
 */

#ifndef LIB_FEMAS_SRC_FEMASPRODUCTDOWNLOADSESSIONFACTORY_HPP_
#define LIB_FEMAS_SRC_FEMASPRODUCTDOWNLOADSESSIONFACTORY_HPP_

#include <memory>
#include <string>

#include <IServiceFactory.hpp>
#include <Logger.hpp>

namespace mm
{
	struct FemasProductDownloadSessionConfig
	{
		// The class name for sanity check.
		static const std::string CLASS_NAME;

		// The dispatch key for product service.
		static const std::string DISPATCH_KEY;

		// The service name for product service to use.
		static const std::string PRODUCT_SERVICE_NAME;

		// The output path for the product definitions.
		static const std::string OUTPUT_PATH;
	};

	//
	// The factory for product service.
	//
	class FemasProductDownloadSessionFactory : public IServiceFactory
	{
	public:

		// Flag if the factory is registered.
		static const bool registered;

		// virtual destructor.
		virtual ~FemasProductDownloadSessionFactory();

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




#endif /* LIB_FEMAS_SRC_FEMASPRODUCTDOWNLOADSESSIONFACTORY_HPP_ */
