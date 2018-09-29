/*
 * Product.hpp
 *
 *  Created on: Sep 29, 2018
 *      Author: suoalex
 */

#ifndef LIB_PRODUCT_SRC_PRODUCT_HPP_
#define LIB_PRODUCT_SRC_PRODUCT_HPP_

#include <memory>
#include <vector>

#include <ProductMessage.hpp>

namespace mm
{
	//
	// This class defines an instrument.
	//
	class Product
	{
	public:

		//
		// Default constructor.
		//
		Product();

		//
		// Constructor.
		//
		// content : The product content itself.
		// underlying : The underlying product pointer.
		//
		Product(const ProductMessage& content, const std::shared_ptr<const Product>& underlying);

		//
		// Constructor.
		//
		// content : The product content itself.
		// underlying : The underlying product pointer.
		// constituents : The constituents.
		//
		Product(const ProductMessage& content,
				const std::shared_ptr<const Product>& underlying,
				const std::vector<std::shared_ptr<const Product> >& constituents);

		//
		// Get the content of the product.
		//
		// return : The content details.
		//
		inline const ProductMessage& getContent() const
		{
			return content;
		}

		//
		// Get the underlying.
		//
		// return : Pointer to the underlying; or nullptr if an underlying cannot be found.
		//
		inline const std::shared_ptr<const Product>& getUnderlying() const
		{
			return underlying;
		}

		//
		// Get the constituents.
		//
		// return : The constituents vector. Empty if this product don't have constituents.
		//
		inline const std::vector<std::shared_ptr<const Product> >& getConstituents() const
		{
			return constituents;
		}

	private:

		friend class ProductService;

		// The content of the product itself.
		ProductMessage content;

		// The underlying if any - we don't handle multiple underlyings now
		std::shared_ptr<const Product> underlying;

		// The constituents if any
		std::vector<std::shared_ptr<const Product> > constituents;
	};
}



#endif /* LIB_PRODUCT_SRC_PRODUCT_HPP_ */
