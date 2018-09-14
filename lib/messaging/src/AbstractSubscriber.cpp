/*
 * AbstractSubscriber.cpp
 *
 *  Created on: Sep 14, 2018
 *      Author: suoalex
 */

#include <algorithm>

#include <AbstractSubscriber.hpp>

namespace mm
{
	void AbstractSubscriber::subscribe(const ISubscription& subscription)
	{
	}

	void AbstractSubscriber::addConsumer(IConsumer& consumer)
	{
		std::unique_lock<std::mutex> lock(mutex);
		consumers.push_back(&consumer);
	}

	void AbstractSubscriber::removeConsumer(IConsumer& consumer)
	{
		std::unique_lock<std::mutex> lock(mutex);
		std::remove(consumers.begin(), consumers.end(), &consumer);
	}

	size_t AbstractSubscriber::getConsumerCount() const
	{
		std::unique_lock<std::mutex> lock(mutex);
		return consumers.size();
	}

	void AbstractSubscriber::removeAll()
	{
		std::unique_lock<std::mutex> lock(mutex);
		consumers.clear();
	}

}
