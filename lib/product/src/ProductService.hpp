/*
 * ProductService.hpp
 *
 *  Created on: Sep 28, 2018
 *      Author: suoalex
 */

#ifndef LIB_PRODUCT_SRC_PRODUCTSERVICE_HPP_
#define LIB_PRODUCT_SRC_PRODUCTSERVICE_HPP_

#include <istream>
#include <unordered_map>

#include <IService.hpp>
#include <PublisherAdapter.hpp>
#include <ProductMessage.hpp>

namespace mm
{
	//
	// Implement a product service aiming at providing all product information and update.
	// The service provide product message in a single way. Product relationship should be constructed by ProductCache.
	//
	class ProductService :
			public IService,
			public PublisherAdapter<ProductMessage>
	{
	public:

		ProductService(std::istream& is);

		ProductService(std::istream&& is);

		virtual ~ProductService();

	private:

		std::unordered_map<std::int64_t, >
	};
}



#endif /* LIB_PRODUCT_SRC_PRODUCTSERVICE_HPP_ */
