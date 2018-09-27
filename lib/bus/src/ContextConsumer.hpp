/*
 * ContextConsumer.hpp
 *
 *  Created on: Sep 15, 2018
 *      Author: suoalex
 */

#ifndef LIB_BUS_SRC_CONTEXTCONSUMER_HPP_
#define LIB_BUS_SRC_CONTEXTCONSUMER_HPP_

#include <cstdint>
#include <memory>
#include <stdexcept>

#include <IConsumer.hpp>

namespace mm
{
	//
	// This class performs a context switching for consuming a message.
	//
	template<typename Message, typename Key = std::int32_t, typename Dispatcher> class ContextConsumer : public IConsumer<Message>
	{
	public:

		ContextConsumer(
				Dispatcher& dispatcher,
				IConsumer<Message>& consumer) :
			key(consumer->getKey()),
			dispatcher(dispatcher),
			consumer(consumer)
		{
		}

		virtual ~ContextConsumer() {}

		virtual void consume(const std::shared_ptr<const Message>& message) override
		{
			dispatcher.submit(key, [message]() {
				consumer.consume(message);
			});
		}

	private:

		// The key to assign task on.
		const Key key;

		// The dispatcher.
		Dispatcher& dispatcher;

		// The wrapped consumer.
		IConsumer<Message>& consumer;
	};
}


#endif /* LIB_BUS_SRC_CONTEXTCONSUMER_HPP_ */
