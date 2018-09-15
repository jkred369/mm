/*
 * IService.hpp
 *
 *  Created on: Sep 15, 2018
 *      Author: suoalex
 */

#ifndef LIB_SERVICE_SRC_ISERVICE_HPP_
#define LIB_SERVICE_SRC_ISERVICE_HPP_

namespace mm
{
	//
	// This class defines the service interface.
	//
	//
	//
	class IService
	{
	public:

		virtual ~IService() {}

		//
		// Start the service.
		//
		// return : true if the service started successfully.
		//
		virtual bool start() = 0;

		//
		// Stop a service.
		//
		virtual void stop() = 0;
	};
}



#endif /* LIB_SERVICE_SRC_ISERVICE_HPP_ */
