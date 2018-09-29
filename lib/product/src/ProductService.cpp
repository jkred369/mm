/*
 * ProductService.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: suoalex
 */

#include "ProductService.hpp"

mm::Logger mm::ProductService::logger;

namespace mm
{
	ProductService::ProductService(
			const KeyType dispatchKey,
			const std::string serviceName,
			Dispatcher& dispatcher,
			ServiceContext& serviceContext,
			std::istream& is) :
		DispatchableService(dispatchKey, serviceName, dispatcher, serviceContext),
		PublisherAdapter<Product>(dispatcher)
	{
	}

	ProductService::ProductService(
			const KeyType dispatchKey,
			const std::string serviceName,
			Dispatcher& dispatcher,
			ServiceContext& serviceContext,
			std::istream&& is) :
		DispatchableService(dispatchKey, serviceName, dispatcher, serviceContext),
		PublisherAdapter<Product>(dispatcher)
	{
	}

	ProductService::~ProductService()
	{
	}

	bool ProductService::start()
	{
		if (!DispatchableService::start())
		{
			return false;
		}

		if (!serviceContext.subscribe(Subscription(SourceType::ALL, DataType::PRODUCT, ALL_ID), dynamic_cast<IConsumer<ProductMessage>*> (this)))
		{
			return false;
		}

		return true;
	}

	void ProductService::stop()
	{
		DispatchableService::stop();
	}

	void ProductService::consume(const std::shared_ptr<const ProductMessage>& message)
	{
		std::shared_ptr<Product> product;
		{
			auto it = productMap.find(message->id);
			if (it == productMap.end())
			{
				product = std::make_shared<Product> ();
				productMap[message->id] = product;

				LOGINFO("New product {} added to cache", message->id);
			}
			else
			{
				product = it->second;
			}
		}

		// update product content
		if (!(product->getContent() == *message))
		{
			LOGINFO("Updating product {}", message->id);

			// firstly remove all the existing dependencies, if any
			{
				if (product->getContent().underlyerId != 0)
				{

				}
			}

			product->content = *message;
		}
	}

	void ProductService::consume(const std::shared_ptr<const ProductConstituentMessage>& message)
	{
		// TODO: Provide support for constituent products like basket or indices.
	}

}


