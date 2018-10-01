/*
 * Trader.cpp
 *
 *  Created on: Oct 1, 2018
 *      Author: suoalex
 */

#include <iostream>
#include <thread>

#include <ServiceContextManager.hpp>

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: <Trader> <config file path>" << std::endl;
		return -1;
	}

	mm::ServiceContextManager manager;
	manager.createContextAndStart(argv[1]);
}



