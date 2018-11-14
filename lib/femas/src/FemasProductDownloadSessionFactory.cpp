/*
 * FemasProductDownloadSessionFactory.cpp
 *
 *  Created on: 6 Nov 2018
 *      Author: alexsuo
 */

#include <DispatchKey.hpp>
#include <DelegateServiceFactory.hpp>
#include <ProductType.hpp>
#include <ThreadUtil.hpp>

#include "FemasProductDownloadSession.hpp"
#include "FemasProductDownloadSessionFactory.hpp"

const std::string mm::FemasProductDownloadSessionConfig::CLASS_NAME = "FemasProductDownloadSession";
const std::string mm::FemasProductDownloadSessionConfig::DISPATCH_KEY = "DispatchKey";
const std::string mm::FemasProductDownloadSessionConfig::PRODUCT_SERVICE_NAME = "ProductServiceName";
const std::string mm::FemasProductDownloadSessionConfig::OUTPUT_PATH = "OutputPath";
const std::string mm::FemasProductDownloadSessionConfig::INCLUDED_TYPES = "IncludedTypes";

const bool mm::FemasProductDownloadSessionFactory::registered = mm::DelegateServiceFactory::getFactory().registerFactory(
		mm::FemasProductDownloadSessionConfig::CLASS_NAME, new mm::FemasProductDownloadSessionFactory());

mm::Logger mm::FemasProductDownloadSessionFactory::logger;

namespace mm
{
	FemasProductDownloadSessionFactory::~FemasProductDownloadSessionFactory()
	{
	}

	std::shared_ptr<IService> FemasProductDownloadSessionFactory::createService(
			const std::string serviceClass,
			const std::string serviceName,
			const std::shared_ptr<IConfig> config,
			ServiceContext& context)
	{
		// sanity check
		if (serviceClass != FemasProductDownloadSessionConfig::CLASS_NAME)
		{
			LOGERR("Cannot create service with class: {}. Only support class: {}", serviceClass, FemasProductDownloadSessionConfig::CLASS_NAME);
			return std::shared_ptr<IService> ();
		}

		// creation
		const KeyType dispatchKey = config->getInt64(FemasProductDownloadSessionConfig::DISPATCH_KEY);
		const std::string productServiceName = config->getString(FemasProductDownloadSessionConfig::PRODUCT_SERVICE_NAME);
		const std::string outputPath = config->getString(FemasProductDownloadSessionConfig::OUTPUT_PATH);
		const FemasUserDetail userDetail(config);

		std::unordered_set<ProductType> types;
		for (const std::string& typeString : config->getStringList(FemasProductDownloadSessionConfig::INCLUDED_TYPES))
		{
			types.insert(ProductTypeUtil::fromString(typeString));
		}

		return std::shared_ptr<IService> (new FemasProductDownloadSession(
				dispatchKey, serviceName, context, productServiceName, outputPath, userDetail, types));
	}
}


