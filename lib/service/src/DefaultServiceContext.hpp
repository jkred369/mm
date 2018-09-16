/*
 * DefaultServiceContext.hpp
 *
 *  Created on: Sep 16, 2018
 *      Author: suoalex
 */

#ifndef LIB_SERVICE_SRC_DEFAULTSERVICECONTEXT_HPP_
#define LIB_SERVICE_SRC_DEFAULTSERVICECONTEXT_HPP_

#include <memory>
#include <string>
#include <unordered_map>

#include <IServiceContext.hpp>

namespace mm
{
	//
	// A default service context.
	//
	class DefaultServiceContext : public IServiceContext
	{
	public:

		virtual ~DefaultServiceContext();

	protected:

		// IServiceContext interface.
		virtual IService* getService(const std::string& serviceName) override;

	private:

		// The service map. Key is the service name and value is the service shared pointer.
		std::unordered_map<std::string, std::shared_ptr<IService> > serviceMap;
	};
}



#endif /* LIB_SERVICE_SRC_DEFAULTSERVICECONTEXT_HPP_ */
