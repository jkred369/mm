/*
 * ProductService.hpp
 *
 *  Created on: Sep 28, 2018
 *      Author: suoalex
 */

#ifndef LIB_PRODUCT_SRC_PRODUCTSERVICE_HPP_
#define LIB_PRODUCT_SRC_PRODUCTSERVICE_HPP_

#include <istream>
#include <unordered_map>

#include <DispatchableService.hpp>
#include <Logger.hpp>
#include <PublisherAdapter.hpp>
#include <ProductMessage.hpp>
#include <ProductConstituentMessage.hpp>

#include "Product.hpp"

namespace mm
{
	//
	// Implement a product service aiming at providing all product information and update.
	// The service provide product message in a single way. Product relationship should be constructed by ProductCache.
	//
	class ProductService :
			public DispatchableService,
			public IConsumer<ProductMessage>,
			public IConsumer<ProductConstituentMessage>,
			public PublisherAdapter<Product>
	{
	public:

		//
		// Constructor.
		//
		// dispatchKey : The dispatch key.
		// serviceName : The name of the service in context.
		// dispatcher : The dispatcher used.
		// serviceContext : The service context for subscription and source other service.
		// is : The input stream for the product definitions.
		//
		ProductService(
				const KeyType dispatchKey,
				const std::string serviceName,
				ServiceContext& serviceContext,
				std::istream& is);

		virtual ~ProductService();

		//
		// The service interface.
		//
		virtual bool start() override;
		virtual void stop() override;

		//
		// override to publish product after subscription when its available.
		//
		virtual bool subscribe(const Subscription& subscription, IConsumer<Product>* consumer) override;

		//
		// The consumer interface.
		//
		virtual void consume(const std::shared_ptr<const ProductMessage>& message) override;
		virtual void consume(const std::shared_ptr<const ProductConstituentMessage>& message) override;

	private:

		// Logger of the class.
		static Logger logger;

		// The product map where key is the instrument ID and value is the product object.
		std::unordered_map<std::int64_t, std::shared_ptr<Product> > productMap;

		// The map where key is the instrument ID and value are the instruments depending on it.
		std::unordered_map<std::int64_t, std::vector<std::int64_t> > dependencyMap;
	};
}



#endif /* LIB_PRODUCT_SRC_PRODUCTSERVICE_HPP_ */
