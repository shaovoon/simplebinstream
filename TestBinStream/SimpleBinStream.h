// The MIT License (MIT)
// Simplistic Binary Streams 0.9
// Copyright (C) 2014 - 2016, by Wong Shao Voon (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT
//
// version 0.9.2   : Optimize mem_istream constructor for const char*
// version 0.9.3   : Optimize mem_ostream vector insert
// version 0.9.4   : New ptr_istream class
// version 0.9.5   : Add Endianness Swap with compile time check
// version 0.9.6   : Using C File APIs, instead of STL file streams
// version 0.9.7   : Add memfile_istream

#ifndef SimpleBinStream_H
#define SimpleBinStream_H

#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <stdint.h>
#include <cstdio>

namespace simple
{
	enum class Endian
	{
		Big,
		Little
	};
	using BigEndian = std::integral_constant<Endian, Endian::Big>;
	using LittleEndian = std::integral_constant<Endian, Endian::Little>;

	template<typename T>
	void swap(T& val, std::true_type)
	{
		// same endian so do nothing.
	}

	template<typename T>
	void swap(T& val, std::false_type)
	{
		std::is_integral<T> is_integral_type;
		swap_if_integral(val, is_integral_type);
	}

	template<typename T>
	void swap_if_integral(T& val, std::false_type)
	{
		// T is not integral so do nothing
	}

	template<typename T>
	void swap_if_integral(T& val, std::true_type)
	{
		swap_endian<T, sizeof(T)>()(val);
	}

	template<typename T, size_t N>
	struct swap_endian
	{
		void operator()(T& ui)
		{
		}
	};

	template<typename T>
	struct swap_endian<T, 8>
	{
		void operator()(T& ui)
		{
			union EightBytes
			{
				T ui;
				uint8_t arr[8];
			};

			EightBytes fb;
			fb.ui = ui;
			// swap the endian
			std::swap(fb.arr[0], fb.arr[7]);
			std::swap(fb.arr[1], fb.arr[6]);
			std::swap(fb.arr[2], fb.arr[5]);
			std::swap(fb.arr[3], fb.arr[4]);

			ui = fb.ui;
		}
	};

	template<typename T>
	struct swap_endian<T, 4>
	{
		void operator()(T& ui)
		{
			union FourBytes
			{
				T ui;
				uint8_t arr[4];
			};

			FourBytes fb;
			fb.ui = ui;
			// swap the endian
			std::swap(fb.arr[0], fb.arr[3]);
			std::swap(fb.arr[1], fb.arr[2]);

			ui = fb.ui;
		}
	};

	template<typename T>
	struct swap_endian<T, 2>
	{
		void operator()(T& ui)
		{
			union TwoBytes
			{
				T ui;
				uint8_t arr[2];
			};

			TwoBytes fb;
			fb.ui = ui;
			// swap the endian
			std::swap(fb.arr[0], fb.arr[1]);

			ui = fb.ui;
		}
	};

template<typename same_endian_type>
class file_istream
{
public:
	file_istream() : input_file_ptr(nullptr), file_size(0L), read_length(0L) {}
	file_istream(const char * file) : input_file_ptr(nullptr), file_size(0L), read_length(0L)
	{
		open(file);
	}
	void open(const char * file)
	{
		input_file_ptr = std::fopen(file, "rb");
		compute_length();
	}
	void close()
	{
		fclose(input_file_ptr);
		input_file_ptr = nullptr;
	}
	bool is_open()
	{
		return (input_file_ptr != nullptr);
	}
	long file_length() const
	{
		return file_size;
	}
	// http://www.cplusplus.com/reference/cstdio/feof/
	// stream's internal position indicator may point to the end-of-file for the 
	// next operation, but still, the end-of-file indicator may not be set until 
	// an operation attempts to read at that point.
	bool eof() const // not using feof(), see above
	{
		return read_length >= file_size;
	}
	long tellg() const
	{
		return std::ftell(input_file_ptr);
	}
	void seekg (long pos)
	{
		std::fseek(input_file_ptr, pos, SEEK_SET);
	}
	void seekg (long offset, int way)
	{
		std::fseek(input_file_ptr, offset, way);
	}

	template<typename T>
	void read(T& t)
	{
		if(std::fread(reinterpret_cast<void*>(&t), sizeof(T), 1, input_file_ptr) != 1)
		{
			throw std::runtime_error("Read Error!");
		}
		read_length += sizeof(T);
		simple::swap(t, m_same_type);
	}
	template<>
	void read(typename std::vector<char>& vec)
	{
		if (std::fread(reinterpret_cast<void*>(&vec[0]), vec.size(), 1, input_file_ptr) != 1)
		{
			throw std::runtime_error("Read Error!");
		}
		read_length += vec.size();
	}
	void read(char* p, size_t size)
	{
		if (std::fread(reinterpret_cast<void*>(p), size, 1, input_file_ptr) != 1)
		{
			throw std::runtime_error("Read Error!");
		}
		read_length += size;
	}
private:
	void compute_length()
	{
		seekg(0, SEEK_END);
		file_size = tellg();
		seekg(0, SEEK_SET);
	}

	std::FILE* input_file_ptr;
	long file_size;
	long read_length;
	same_endian_type m_same_type;
};

template<typename same_endian_type, typename T>
typename file_istream<same_endian_type>& operator >> (typename file_istream<same_endian_type>& istm, T& val)
{
	istm.read(val);

	return istm;
}

template<typename same_endian_type>
typename file_istream<same_endian_type>& operator >> (typename file_istream<same_endian_type>& istm, std::string& val)
{
	int size = 0;
	istm.read(size);

	if(size<=0)
		return istm;

	std::vector<char> vec((size_t)size);
	istm.read(vec);
	val.assign(&vec[0], (size_t)size);

	return istm;
}

template<typename same_endian_type>
class mem_istream
{
public:
	mem_istream() : m_index(0) {}
	mem_istream(const char * mem, size_t size) 
	{
		open(mem, size);
	}
	mem_istream(const std::vector<char>& vec) 
	{
		m_index = 0;
		m_vec.reserve(vec.size());
		m_vec.assign(vec.begin(), vec.end());
	}
	void open(const char * mem, size_t size)
	{
		m_index = 0;
		m_vec.clear();
		m_vec.reserve(size);
		m_vec.assign(mem, mem + size);
	}
	void close()
	{
		m_vec.clear();
	}
	bool eof() const
	{
		return m_index >= m_vec.size();
	}
	std::ifstream::pos_type tellg()
	{
		return m_index;
	}
	bool seekg (size_t pos)
	{
		if(pos<m_vec.size())
			m_index = pos;
		else 
			return false;

		return true;
	}
	bool seekg (std::streamoff offset, std::ios_base::seekdir way)
	{
		if(way==std::ios_base::beg && offset < m_vec.size())
			m_index = offset;
		else if(way==std::ios_base::cur && (m_index + offset) < m_vec.size())
			m_index += offset;
		else if(way==std::ios_base::end && (m_vec.size() + offset) < m_vec.size())
			m_index = m_vec.size() + offset;
		else
			return false;

		return true;
	}

	const std::vector<char>& get_internal_vec()
	{
		return m_vec;
	}

	template<typename T>
	void read(T& t)
	{
		if(eof())
			throw std::runtime_error("Premature end of array!");

		if((m_index + sizeof(T)) > m_vec.size())
			throw std::runtime_error("Premature end of array!");

		std::memcpy(reinterpret_cast<void*>(&t), &m_vec[m_index], sizeof(T));

		simple::swap(t, m_same_type);

		m_index += sizeof(T);
	}

	template<>
	void read(typename std::vector<char>& vec)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + vec.size()) > m_vec.size())
			throw std::runtime_error("Premature end of array!");

		std::memcpy(reinterpret_cast<void*>(&vec[0]), &m_vec[m_index], vec.size());

		m_index += vec.size();
	}

	void read(char* p, size_t size)
	{
		if(eof())
			throw std::runtime_error("Premature end of array!");

		if((m_index + size) > m_vec.size())
			throw std::runtime_error("Premature end of array!");

		std::memcpy(reinterpret_cast<void*>(p), &m_vec[m_index], size);

		m_index += size;
	}

	void read(std::string& str, const unsigned int size)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + str.size()) > m_vec.size())
			throw std::runtime_error("Premature end of array!");

		str.assign(&m_vec[m_index], size);

		m_index += str.size();
	}

private:
	std::vector<char> m_vec;
	size_t m_index;
	same_endian_type m_same_type;
};

template<typename same_endian_type, typename T>
typename mem_istream<same_endian_type>& operator >> (typename mem_istream<same_endian_type>& istm, T& val)
{
	istm.read(val);

	return istm;
}

template<typename same_endian_type>
mem_istream<same_endian_type>& operator >> (mem_istream<same_endian_type>& istm, std::string& val)
{
	int size = 0;
	istm.read(size);

	if(size<=0)
		return istm;

	istm.read(val, size);

	return istm;
}

template<typename same_endian_type>
class ptr_istream
{
public:
	ptr_istream() : m_arr(NULL), m_size(0), m_index(0) {}
	ptr_istream(const char * mem, size_t size)
	{
		open(mem, size);
	}
	ptr_istream(const std::vector<char>& vec)
	{
		m_index = 0;
		m_arr = vec.data();
		m_size = vec.size();
	}
	void open(const char * mem, size_t size)
	{
		m_index = 0;
		m_arr = mem;
		m_size = size;
	}
	void close()
	{
		m_arr = NULL; m_size = 0; m_index = 0;
	}
	bool eof() const
	{
		return m_index >= m_size;
	}
	std::ifstream::pos_type tellg()
	{
		return m_index;
	}
	bool seekg(size_t pos)
	{
		if (pos<m_size)
			m_index = pos;
		else
			return false;

		return true;
	}
	bool seekg(std::streamoff offset, std::ios_base::seekdir way)
	{
		if (way == std::ios_base::beg && offset < m_size)
			m_index = offset;
		else if (way == std::ios_base::cur && (m_index + offset) < m_size)
			m_index += offset;
		else if (way == std::ios_base::end && (m_size + offset) < m_size)
			m_index = m_size + offset;
		else
			return false;

		return true;
	}

	template<typename T>
	void read(T& t)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + sizeof(T)) > m_size)
			throw std::runtime_error("Premature end of array!");

		std::memcpy(reinterpret_cast<void*>(&t), &m_arr[m_index], sizeof(T));

		simple::swap(t, m_same_type);

		m_index += sizeof(T);
	}

	template<>
	void read(typename std::vector<char>& vec)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + vec.size()) > m_size)
			throw std::runtime_error("Premature end of array!");

		std::memcpy(reinterpret_cast<void*>(&vec[0]), &m_arr[m_index], vec.size());

		m_index += vec.size();
	}

	void read(char* p, size_t size)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + size) > m_size)
			throw std::runtime_error("Premature end of array!");

		std::memcpy(reinterpret_cast<void*>(p), &m_arr[m_index], size);

		m_index += size;
	}

	void read(std::string& str, const unsigned int size)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + str.size()) > m_size)
			throw std::runtime_error("Premature end of array!");

		str.assign(&m_arr[m_index], size);

		m_index += str.size();
	}

private:
	const char* m_arr;
	size_t m_size;
	size_t m_index;
	same_endian_type m_same_type;
};


template<typename same_endian_type, typename T>
typename ptr_istream<same_endian_type>& operator >> (typename ptr_istream<same_endian_type>& istm, T& val)
{
	istm.read(val);

	return istm;
}

template<typename same_endian_type>
typename ptr_istream<same_endian_type>& operator >> (typename ptr_istream<same_endian_type>& istm, std::string& val)
{
	int size = 0;
	istm.read(size);

	if (size <= 0)
		return istm;

	istm.read(val, size);

	return istm;
}

template<typename same_endian_type>
class memfile_istream
{
public:
	memfile_istream() : m_arr(NULL), m_size(0), m_index(0) {}
	memfile_istream(const char * file) : m_arr(NULL), m_size(0), m_index(0)
	{
		open(file);
	}
	~memfile_istream()
	{
		close();
	}
	void open(const char * file)
	{
		close();
		std::FILE* input_file_ptr = std::fopen(file, "rb");
		compute_length(input_file_ptr);
		m_arr = new char[m_size];
		std::fread(m_arr, m_size, 1, input_file_ptr);
		fclose(input_file_ptr);
	}
	void close()
	{
		delete[] m_arr;
		m_arr = NULL; m_size = 0; m_index = 0;
	}
	bool is_open()
	{
		return (m_arr != nullptr);
	}
	long file_length() const
	{
		return m_size;
	}
	bool eof() const
	{
		return m_index >= m_size;
	}
	long tellg(std::FILE* input_file_ptr) const
	{
		return std::ftell(input_file_ptr);
	}
	void seekg(std::FILE* input_file_ptr, long pos)
	{
		std::fseek(input_file_ptr, pos, SEEK_SET);
	}
	void seekg(std::FILE* input_file_ptr, long offset, int way)
	{
		std::fseek(input_file_ptr, offset, way);
	}

	template<typename T>
	void read(T& t)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + sizeof(T)) > m_size)
			throw std::runtime_error("Premature end of array!");

		std::memcpy(reinterpret_cast<void*>(&t), &m_arr[m_index], sizeof(T));

		simple::swap(t, m_same_type);

		m_index += sizeof(T);
	}

	template<>
	void read(typename std::vector<char>& vec)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + vec.size()) > m_size)
			throw std::runtime_error("Premature end of array!");

		std::memcpy(reinterpret_cast<void*>(&vec[0]), &m_arr[m_index], vec.size());

		m_index += vec.size();
	}

	void read(char* p, size_t size)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + size) > m_size)
			throw std::runtime_error("Premature end of array!");

		std::memcpy(reinterpret_cast<void*>(p), &m_arr[m_index], size);

		m_index += size;
	}

	void read(std::string& str, const unsigned int size)
	{
		if (eof())
			throw std::runtime_error("Premature end of array!");

		if ((m_index + str.size()) > m_size)
			throw std::runtime_error("Premature end of array!");

		str.assign(&m_arr[m_index], size);

		m_index += str.size();
	}

private:
	void compute_length(std::FILE* input_file_ptr)
	{
		seekg(input_file_ptr, 0, SEEK_END);
		m_size = tellg(input_file_ptr);
		seekg(input_file_ptr, 0, SEEK_SET);
	}

	char* m_arr;
	size_t m_size;
	size_t m_index;
	same_endian_type m_same_type;
};


template<typename same_endian_type, typename T>
typename memfile_istream<same_endian_type>& operator >> (typename memfile_istream<same_endian_type>& istm, T& val)
{
	istm.read(val);

	return istm;
}

template<typename same_endian_type>
typename memfile_istream<same_endian_type>& operator >> (typename memfile_istream<same_endian_type>& istm, std::string& val)
{
	int size = 0;
	istm.read(size);

	if (size <= 0)
		return istm;

	istm.read(val, size);

	return istm;
}

template<typename same_endian_type>
class file_ostream
{
public:
	file_ostream() : output_file_ptr(nullptr) {}
	file_ostream(const char * file) : output_file_ptr(nullptr)
	{
		open(file);
	}
	void open(const char * file)
	{
		output_file_ptr = std::fopen(file, "wb");
	}
	void flush()
	{
		std::fflush(output_file_ptr);
	}
	void close()
	{
		std::fclose(output_file_ptr);
	}
	bool is_open()
	{
		return output_file_ptr != nullptr;
	}
	template<typename T>
	void write(const T& t)
	{
		T t2 = t;
		simple::swap(t2, m_same_type);
		std::fwrite(reinterpret_cast<const void*>(&t2), sizeof(T), 1, output_file_ptr);
	}
	template<>
	void write(const std::vector<char>& vec)
	{
		std::fwrite(reinterpret_cast<const void*>(&vec[0]), vec.size(), 1, output_file_ptr);
	}
	void write(const char* p, size_t size)
	{
		std::fwrite(reinterpret_cast<const void*>(p), size, 1, output_file_ptr);
	}

private:
	std::FILE* output_file_ptr;
	same_endian_type m_same_type;
};

template<typename same_endian_type, typename T>
file_ostream<same_endian_type>& operator << (file_ostream<same_endian_type>& ostm, const T& val)
{
	ostm.write(val);

	return ostm;
}

template<typename same_endian_type>
typename file_ostream<same_endian_type>& operator << (typename file_ostream<same_endian_type>& ostm, const std::string& val)
{
	int size = val.size();
	ostm.write(size);

	if(val.size()<=0)
		return ostm;

	ostm.write(val.c_str(), val.size());

	return ostm;
}

template<typename same_endian_type>
typename file_ostream<same_endian_type>& operator << (typename file_ostream<same_endian_type>& ostm, const char* val)
{
	int size = std::strlen(val);
	ostm.write(size);

	if(size<=0)
		return ostm;

	ostm.write(val, size);

	return ostm;
}

template<typename same_endian_type>
class mem_ostream
{
public:
	mem_ostream() {}
	void close()
	{
		m_vec.clear();
	}
	const std::vector<char>& get_internal_vec()
	{
		return m_vec;
	}
	template<typename T>
	void write(const T& t)
	{
		std::vector<char> vec(sizeof(T));
		T t2 = t;
		simple::swap(t2, m_same_type);
		std::memcpy(reinterpret_cast<void*>(&vec[0]), reinterpret_cast<const void*>(&t2), sizeof(T));
		write(vec);
	}
	template<>
	void write(const std::vector<char>& vec)
	{
		m_vec.insert(m_vec.end(), vec.begin(), vec.end());
	}
	void write(const char* p, size_t size)
	{
		for(size_t i=0; i<size; ++i)
			m_vec.push_back(p[i]);
	}

private:
	std::vector<char> m_vec;
	same_endian_type m_same_type;
};

template<typename same_endian_type, typename T>
typename mem_ostream<same_endian_type>& operator << (typename mem_ostream<same_endian_type>& ostm, const T& val)
{
	ostm.write(val);

	return ostm;
}

template<typename same_endian_type>
typename mem_ostream<same_endian_type>& operator << (typename mem_ostream<same_endian_type>& ostm, const std::string& val)
{
	int size = val.size();
	ostm.write(size);

	if(val.size()<=0)
		return ostm;

	ostm.write(val.c_str(), val.size());

	return ostm;
}

template<typename same_endian_type>
typename mem_ostream<same_endian_type>& operator << (typename mem_ostream<same_endian_type>& ostm, const char* val)
{
	int size = std::strlen(val);
	ostm.write(size);

	if(size<=0)
		return ostm;

	ostm.write(val, size);

	return ostm;
}

} // ns simple

#endif // SimpleBinStream_H