// Benchmark.cpp : Defines the entry point for the console application.
//

#include <string>
#include <iomanip>
#include <iostream>
#include <chrono>
#include <vector>
#include "../TestBinStream/SimpleBinStream.h"
#include "OldSimpleBinStream.h"

#ifdef WIN32

#pragma optimize("", off)
template <class T>
void do_not_optimize_away(T&& datum) {
	datum = datum;
}
#pragma optimize("", on)

#else
static void do_not_optimize_away(const char* p) {
	asm volatile("" : : "g"(p) : "memory");
}
#endif

class timer
{
public:
	timer() = default;
	void start(const std::string& text_)
	{
		text = text_;
		begin = std::chrono::high_resolution_clock::now();
	}
	void stop()
	{
		auto end = std::chrono::high_resolution_clock::now();
		auto dur = end - begin;
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
		std::cout << std::setw(30) << text << ":" << std::setw(5) << ms << "ms" << std::endl;
	}

private:
	std::string text;
	std::chrono::high_resolution_clock::time_point begin;
};

struct Product
{
	Product() : name(""), qty(0), price(0.0f) {}
	Product(std::string name_, int qty_, float price_) : name(name_), qty(qty_), price(price_) {}
	std::string name;
	int qty;
	float price;
};

void init(std::vector<Product>& vec);

int main(int argc, char *argv[])
{
	const std::string old_file = "F:\\old_products.txt";
	const std::string new_file = "F:\\new_products.txt";
	const std::string mem_file = "F:\\mem_products.txt";

	const size_t MAX_LOOP = (argc == 2) ? atoi(argv[1]) : 100000;

	std::vector<Product> vec;
	init(vec);

	std::string result = "";
	const std::string trimChars = "\"";
	timer stopwatch;

	// file stream benchmark
	//=========================
	{
		using namespace old;

		file_ostream<std::true_type> os(old_file.c_str());

		if (os.is_open())
		{
			stopwatch.start("old::file_ostream");
			for (size_t k = 0; k < MAX_LOOP; ++k)
			{
				for (size_t i = 0; i < vec.size(); ++i)
				{
					const Product& product = vec[i];
					os << product.name << product.qty << product.price;
					do_not_optimize_away(result.c_str());
				}
			}
			stopwatch.stop();
		}
		os.flush();
		os.close();

		file_istream<std::true_type> is(old_file.c_str());

		if (is.is_open())
		{
			Product product;
			stopwatch.start("old::file_istream");
			try
			{
				while (!is.eof())
				{
					is >> product.name >> product.qty >> product.price;
				}
			}
			catch (std::runtime_error& e)
			{
				fprintf(stderr, "%s\n", e.what());
			}
			stopwatch.stop();
		}
	}
	{
		using namespace simple;

		file_ostream<std::true_type> os(new_file.c_str());

		if (os.is_open())
		{
			stopwatch.start("new::file_ostream");
			for (size_t k = 0; k < MAX_LOOP; ++k)
			{
				for (size_t i = 0; i < vec.size(); ++i)
				{
					const Product& product = vec[i];
					os << product.name << product.qty << product.price;
					do_not_optimize_away(result.c_str());
				}
			}
			stopwatch.stop();
		}
		os.flush();
		os.close();

		file_istream<std::true_type> is(new_file.c_str());

		if (is.is_open())
		{
			Product product;
			stopwatch.start("new::file_istream");
			try
			{
				while (!is.eof())
				{
					is >> product.name >> product.qty >> product.price;
				}
			}
			catch (std::runtime_error& e)
			{
				fprintf(stderr, "%s\n", e.what());
			}
			stopwatch.stop();
		}

		is.close();
	}
	{
		using namespace simple;

		memfile_ostream<std::true_type> os;

		stopwatch.start("new::memfile_ostream");
		for (size_t k = 0; k < MAX_LOOP; ++k)
		{
			for (size_t i = 0; i < vec.size(); ++i)
			{
				const Product& product = vec[i];
				os << product.name << product.qty << product.price;
				do_not_optimize_away(result.c_str());
			}
		}
		os.write_to_file(mem_file.c_str());
		stopwatch.stop();

		memfile_istream<std::true_type> is_copy(mem_file.c_str());

		if (is_copy.is_open())
		{
			Product product;
			stopwatch.start("new::memfile_istream");
			try
			{
				while (!is_copy.eof())
				{
					is_copy >> product.name >> product.qty >> product.price;
				}
			}
			catch (std::runtime_error& e)
			{
				fprintf(stderr, "%s\n", e.what());
			}
			stopwatch.stop();
		}
		is_copy.close();
	}

	{
		using namespace simple;

		mem_ostream<std::true_type> os;

		stopwatch.start("new::mem_ostream");
		for (size_t k = 0; k < MAX_LOOP; ++k)
		{
			for (size_t i = 0; i < vec.size(); ++i)
			{
				const Product& product = vec[i];
				os << product.name << product.qty << product.price;
				do_not_optimize_away(result.c_str());
			}
		}
		stopwatch.stop();

		mem_istream<std::true_type> is(os.get_internal_vec());

		Product product;
		stopwatch.start("new::mem_istream");
		try
		{
			while (!is.eof())
			{
				is >> product.name >> product.qty >> product.price;
			}
		}
		catch (std::runtime_error& e)
		{
			fprintf(stderr, "%s\n", e.what());
		}
		stopwatch.stop();

		ptr_istream<std::true_type> ptr_is(os.get_internal_vec());

		stopwatch.start("new::ptr_istream");
		try
		{
			while (!ptr_is.eof())
			{
				ptr_is >> product.name >> product.qty >> product.price;
			}
		}
		catch (std::runtime_error& e)
		{
			fprintf(stderr, "%s\n", e.what());
		}
		stopwatch.stop();
	}

	return 0;
}

void init(std::vector<Product>& vec)
{
	vec.push_back(Product("Apples", 5, 2.5f));
	vec.push_back(Product("Shampoo", 125, 12.5f));
	vec.push_back(Product("Soap", 250, 30.25f));
	vec.push_back(Product("Instant Ramen", 125, 12.5f));
	vec.push_back(Product("Canned Sardines", 20, 50.7f));
	vec.push_back(Product("Color Book", 10, 10.5f));
	vec.push_back(Product("Speakers", 40, 1089.75f));
	vec.push_back(Product("Photo Holder", 34, 56.85f));
	vec.push_back(Product("Microphone", 12, 572.43f));
	vec.push_back(Product("Sound card", 8, 1250.62f));

	/*
	// mostly long strings
	vec.push_back(Product("Apples and Oranges", 5, 2.5f));
	vec.push_back(Product("Shampoo and Conditioners", 125, 12.5f));
	vec.push_back(Product("Soap / body foam", 250, 30.25f));
	vec.push_back(Product("Instant Ramen / Udon", 125, 12.5f));
	vec.push_back(Product("Canned Sardines", 20, 50.7f));
	vec.push_back(Product("Color Book / Color pencils / Drawing Paper", 10, 10.5f));
	vec.push_back(Product("Audio Speakers / Subwoofer", 40, 1089.75f));
	vec.push_back(Product("Camera and Photo Holder", 34, 56.85f));
	vec.push_back(Product("Classic Microphone", 12, 572.43f));
	vec.push_back(Product("Sound card", 8, 1250.62f));
	*/
}
