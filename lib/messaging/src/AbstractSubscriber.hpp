/*
 * DefaultSubscriber.hpp
 *
 *  Created on: Sep 14, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_ABSTRACTSUBSCRIBER_HPP_
#define LIB_MESSAGING_SRC_ABSTRACTSUBSCRIBER_HPP_

#include <mutex>

#include <ISubscriber.hpp>

namespace mm
{
	//
	// The class provides a default implementation of the ISubscriber interface.
	//
	class AbstractSubscriber : public ISubscriber
	{
	public:

		virtual void subscribe(const ISubscription& subscription) override;

		virtual void addConsumer(IConsumer& consumer) override;

		virtual void removeConsumer(IConsumer& consumer) override;

		virtual size_t getConsumerCount() const override;

		virtual void removeAll() override;

	private:

		// The consumers.
		std::vector<IConsumer*> consumers;

		// The mutex for accessing the consumer.
		std::mutex mutex;
	};
}

#endif /* LIB_MESSAGING_SRC_ABSTRACTSUBSCRIBER_HPP_ */
