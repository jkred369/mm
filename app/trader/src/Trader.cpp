/*
 * Trader.cpp
 *
 *  Created on: Oct 1, 2018
 *      Author: suoalex
 */

#include <iostream>
#include <thread>

//TODO: Fix the path.
//#include "FemasMarketDataSessionFactory.hpp"
//#include "FemasOrderSessionFactory.hpp"
#include <FemasMarketDataSessionFactory.hpp>
#include <FemasOrderSessionFactory.hpp>
#include <ProductServiceFactory.hpp>
#include <ServiceContextManager.hpp>

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: <Trader> <config file path>" << std::endl;
		return -1;
	}

	// make sure factory is properly registered
	if (
		!mm::FemasMarketDataSessionFactory::registered ||
		!mm::FemasOrderSessionFactory::registered ||
		!mm::ProductServiceFactory::registered)
	{
		std::cerr << "Not all required service factory registered." << std::endl;
		return -1;
	}

	mm::ServiceContextManager manager;
	manager.createContextAndStart(argv[1]);
}



