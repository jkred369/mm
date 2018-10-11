/*
 * ProductServiceFactory.hpp
 *
 *  Created on: Oct 11, 2018
 *      Author: suoalex
 */

#ifndef LIB_PRODUCT_SRC_PRODUCTSERVICEFACTORY_HPP_
#define LIB_PRODUCT_SRC_PRODUCTSERVICEFACTORY_HPP_

#include <string>

#include <IServiceFactory.hpp>
#include <Logger.hpp>

namespace mm
{
	struct ProductServiceConfig
	{
		// The class name for sanity check.
		static const std::string CLASS_NAME;

		// The dispatch key for product service.
		static const std::string DISPATCH_KEY;

		// The data file path for the product service.
		static const std::string DATA_FILE;

	};

	//
	// The factory for product service.
	//
	class ProductServiceFactory : public IServiceFactory
	{
	public:

		// Flag if the factory is registered.
		static const bool registered;

		// virtual destructor.
		virtual ~ProductServiceFactory();

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



#endif /* LIB_PRODUCT_SRC_PRODUCTSERVICEFACTORY_HPP_ */
