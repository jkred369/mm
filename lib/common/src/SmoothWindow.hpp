/*
 * SmoothWindow.hpp
 *
 *  Created on: Mar 3, 2022
 *      Author: alexsuo
 */

#ifndef LIB_COMMON_SRC_SMOOTHWINDOW_HPP_
#define LIB_COMMON_SRC_SMOOTHWINDOW_HPP_

extern "C"
{
#include <wt.h>
}

#include <boost/circular_buffer.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>

namespace mm
{
	template<typename ValueType> class DwtWindow
	{
	public:

		DwtWindow(size_t level = 8, size_t delay = 6, bool fillBuffer = true) :
			level(level),
			inputCount(1 << level),
			delay(std::min(delay, inputCount - 1)),
			average(ValueType()),
			buffer(inputCount),
			wavelet(discrete_wavelet(WAVELET_NAME::DB, 3))
		{
			if (fillBuffer)
			{
				while (!buffer.full())
				{
					buffer.push_back(ValueType());
				}
			}
		}

		~DwtWindow()
		{
			if (wavelet)
			{
				free_discrete_wavelet(wavelet);
			}
		}

		void add(const ValueType& value)
		{
			buffer.push_back(value);
			if (buffer.full())
			{
				compute();
			}
		}

		inline ValueType getLast() const
		{
			return !buffer.empty() ? buffer.back() : average;
		}

		inline ValueType getAverage() const
		{
			return average;
		}

		inline size_t getLevel() const
		{
			return level;
		}

		inline size_t getDelay() const
		{
			return delay;
		}

		inline size_t size() const
		{
			return buffer.capacity();
		}

		void setLevelAndDelay(size_t level, size_t delay)
		{
			if (level > 20)
			{
				return;
			}

			if (this->level == level && this->delay == delay)
			{
				return;
			}

			if (level > 0 && level != this->level)
			{
				this->level = level;
				this->inputCount = 1 << level;
				this->delay = std::min(delay, inputCount - 1);
				buffer.rset_capacity(inputCount);
			}

			this->delay = std::min(delay, inputCount - 1);
		}

		void setSize(size_t size)
		{
			if (size > 0 && size != buffer.capacity())
			{
				buffer.rset_capacity(size);
			}
		}

	private:

		void compute()
		{
			static constexpr MODE mode = MODE::MODE_PERIODIC;

			ValueType data[inputCount];
			for (size_t i = 0; i < inputCount; ++i)
			{
				data[i] = buffer[i];
			}

			size_t dataCount = inputCount;
			size_t counts[level];

			// forward transform
			{
				for (int i = 0; i < level; ++i)
				{
					const size_t bufferSize = dwt_buffer_length(dataCount, wavelet->dec_len, mode);
					counts[i] = dataCount;

					ValueType cA[bufferSize];
					fill(cA, ValueType(), bufferSize);

					if (double_dec_a(data, dataCount, wavelet, cA, bufferSize, mode) < 0)
					{
						return;
					}

					// manipulate data
					for (int j = 0; j < bufferSize; j++)
					{
						data[j] = cA[j];
					}

					dataCount = bufferSize;
				}
			}

			// clean up buffer
			for (int i = dataCount; i < inputCount; ++i)
			{
				data[i] = ValueType();
			}

			// backward construction
			{
				for (int i = 0; i < level; ++i)
				{
					const size_t invertBufferSize = idwt_buffer_length(dataCount, wavelet->rec_len, mode);

					ValueType cD[dataCount];
					fill(cD, ValueType(), dataCount);

					ValueType rec[invertBufferSize];
					fill(rec, ValueType(), invertBufferSize);

					if (double_idwt(data, dataCount, cD, dataCount, rec, invertBufferSize, wavelet, mode) < 0)
					{
						return;
					}

					for (int j = 0; j < counts[level - i - 1]; ++j)
					{
						data[j] = rec[j];
					}

					dataCount = counts[level - i - 1];
				}
			}

			// take the value
			average = data[inputCount - 1 - delay];
		}

		inline void fill(ValueType* array, ValueType value, size_t size)
		{
			for (size_t i = 0; i < size; ++i)
			{
				*(array + i) = value;
			}
		}

		size_t level;

		size_t inputCount;

		size_t delay;

		ValueType average;

		boost::circular_buffer<ValueType> buffer;

		DiscreteWavelet* wavelet;
	};
}



#endif /* LIB_COMMON_SRC_SMOOTHWINDOW_HPP_ */
