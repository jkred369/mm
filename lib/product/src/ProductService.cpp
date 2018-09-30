/*
 * ProductService.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: suoalex
 */

#include <NativeDefinition.hpp>
#include <StringBuffer.hpp>

#include "ProductService.hpp"

mm::Logger mm::ProductService::logger;

namespace mm
{
	ProductService::ProductService(
			const KeyType dispatchKey,
			const std::string serviceName,
			ServiceContext& serviceContext,
			std::istream& is) :
		DispatchableService(dispatchKey, serviceName, serviceContext),
		PublisherAdapter<Product>(serviceContext.getDispatcher())
	{
		try
		{
			for (std::string line; std::getline(is, line); )
			{
				StringUtil::trim(line);
				if (line.size() == 0 || line[0] == '#')
				{
					continue;
				}

				StringBuffer buffer(line);
				std::shared_ptr<ProductMessage> product = std::make_shared<ProductMessage>();

				if (!product->deserialize(buffer))
				{
					LOGERR("Error loading product from line; skipped: {}", line);
					continue;
				}

				consume(product);
			}
		}
		catch (std::exception& e)
		{
			LOGERR("Failed to read property: {}", e.what());
		}
		catch (...)
		{
			LOGERR("Failed to read property with unknown reason.");
		}
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
			LOGWARN("No other product service to sync with.");
		}

		return true;
	}

	void ProductService::stop()
	{
		DispatchableService::stop();
	}

	bool ProductService::subscribe(const Subscription& subscription, IConsumer<Product>* consumer)
	{
		if (subscription.sourceType != SourceType::PRODUCT_SERVICE && subscription.dataType != DataType::PRODUCT)
		{
			return false;
		}

		if (!PublisherAdapter::subscribe(subscription, consumer))
		{
			return false;
		}

		// check if we have the product - if yes just return it.
		const std::int64_t id = subscription.key;
		auto it = productMap.find(id);

		if (it != productMap.end())
		{
			publishTo(consumer->getKey(), consumer, it->second);
		}

		return true;
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
			if (getConsumerCount(subscription) > 0)
			{
				publish(subscription, product);
			}
		}
	}

	void ProductService::consume(const std::shared_ptr<const ProductConstituentMessage>& message)
	{
		// TODO: Provide support for constituent products like basket or indices.
	}

}


