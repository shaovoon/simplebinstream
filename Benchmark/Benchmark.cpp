// Benchmark.cpp : Defines the entry point for the console application.
//

#include <string>
#include <iomanip>
#include <iostream>
#include <chrono>
#include <vector>
#include "../TestBinStream/SimpleBinStream.h"

#ifdef WIN32

#pragma optimize("", off)
template <class T>
void do_not_optimize_away(T&& datum) {
	datum = datum;
}
#pragma optimize("", on)

#else
static void do_not_optimize_away(void* p) {
	asm volatile("" : : "g"(p) : "memory");
}
#endif

class timer
{
public:
	timer() = default;
	void start_timing(const std::string& text_)
	{
		text = text_;
		begin = std::chrono::high_resolution_clock::now();
	}
	void stop_timing()
	{
		auto end = std::chrono::high_resolution_clock::now();
		auto dur = end - begin;
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
		std::cout << std::setw(30) << text << ":" << std::setw(5) << ms << "ms" << std::endl;
	}

private:
	std::string text;
	std::chrono::steady_clock::time_point begin;
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
	const std::string old_file = "F:\\mini_products.txt";
	const std::string new_file = "F:\\blazing_products.txt";

	const size_t MAX_LOOP = (argc == 2) ? atoi(argv[1]) : 10000;

	std::vector<Product> vec;
	init(vec);

	std::string result = "";
	const std::string trimChars = "\"";
	timer stopwatch;

	// file stream benchmark
	//=========================
	{
		simple::file_ostream<std::true_type> os(old_file.c_str(), std::ios_base::out | std::ios_base::binary);

		if (os.is_open())
		{
			stopwatch.start_timing("simple::file_ostream");
			for (size_t k = 0; k < MAX_LOOP; ++k)
			{
				for (size_t i = 0; i < vec.size(); ++i)
				{
					const Product& product = vec[i];
					os << product.name << product.qty << product.price;
					do_not_optimize_away(result.c_str());
				}
			}
			stopwatch.stop_timing();
		}
		os.flush();
		os.close();

		simple::file_istream<std::true_type> is(old_file.c_str(), std::ios_base::in | std::ios_base::binary);

		if (is.is_open())
		{
			Product product;
			stopwatch.start_timing("simple::file_istream");
			try
			{
				is >> product.name >> product.qty >> product.price;
			}
			catch (std::runtime_error& e)
			{
				fprintf(stderr, "%s\n", e.what());
			}
			stopwatch.stop_timing();
		}
	}
	{
		using namespace simple;

		simple::file_ostream<std::true_type> os(new_file.c_str(), std::ios_base::out | std::ios_base::binary);

		if (os.is_open())
		{
			stopwatch.start_timing("simple::file_ostream");
			for (size_t k = 0; k < MAX_LOOP; ++k)
			{
				for (size_t i = 0; i < vec.size(); ++i)
				{
					const Product& product = vec[i];
					os << product.name << product.qty << product.price;
					do_not_optimize_away(result.c_str());
				}
			}
			stopwatch.stop_timing();
		}
		os.flush();
		os.close();

		simple::file_istream<std::true_type> is(new_file.c_str(), std::ios_base::in | std::ios_base::binary);

		if (is.is_open())
		{
			Product product;
			stopwatch.start_timing("simple::file_istream");
			try
			{
				is >> product.name >> product.qty >> product.price;
			}
			catch (std::runtime_error& e)
			{
				fprintf(stderr, "%s\n", e.what());
			}
			stopwatch.stop_timing();
		}
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
