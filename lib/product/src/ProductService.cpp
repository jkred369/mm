/*
 * ProductService.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: suoalex
 */

#include <NativeDefinition.hpp>

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
		const std::int64_t id = message->id;

		std::shared_ptr<Product> product;
		{
			auto it = productMap.find(id);
			if (it == productMap.end())
			{
				product = std::make_shared<Product> ();
				productMap[id] = product;

				LOGINFO("New product {} added to cache", id);
			}
			else
			{
				product = it->second;
			}
		}

		// update product content
		// the ID of the product must match the ID from the message here.
		if (!(product->getContent() == *message))
		{
			LOGINFO("Updating product {}", id);

			const std::int64_t curUnderlyerId = product->getContent().underlyerId;
			const std::int64_t newUnderlyerId = message->underlyerId;
			product->content = *message;

			// manage underlyer dependencies.
			if (UNLIKELY(curUnderlyerId != newUnderlyerId))
			{
				if (curUnderlyerId != 0)
				{
					std::vector<std::int64_t>& dependents = dependencyMap[curUnderlyerId];
					dependents.erase(std::remove_if(dependents.begin(), dependents.end(), [&id] (const std::int64_t& value) {
						return id == value;
					}));
				}

				if (newUnderlyerId != 0)
				{
					dependencyMap[newUnderlyerId].push_back(id);
				}
			}

			const Subscription subscription = {SourceType::PRODUCT_SERVICE, DataType::PRODUCT, id};
			publish(subscription, product);
		}
	}

	void ProductService::consume(const std::shared_ptr<const ProductConstituentMessage>& message)
	{
		// TODO: Provide support for constituent products like basket or indices.
	}

}


