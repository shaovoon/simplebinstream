
#include <vector>
#include <string>
#include "SimpleBinStream.h"

struct Product
{
	Product() : product_name(""), price(0.0f), qty(0) {}
	Product(const std::string& name, float _price, int _qty) : product_name(name), price(_price), qty(_qty) {}
	std::string product_name;
	float price;
	int qty;
};

simple::mem_istream& operator >> (simple::mem_istream& istm, Product& val)
{
	return istm >> val.product_name >> val.price >> val.qty;
}

simple::file_istream& operator >> (simple::file_istream& istm, Product& val)
{
	return istm >> val.product_name >> val.price >> val.qty;
}

simple::mem_ostream& operator << (simple::mem_ostream& ostm, const Product& val)
{
	return ostm << val.product_name << val.price << val.qty;
}

simple::file_ostream& operator << (simple::file_ostream& ostm, const Product& val)
{
	return ostm << val.product_name << val.price << val.qty;
}

simple::mem_istream& operator >> (simple::mem_istream& istm, std::vector<Product>& vec)
{
	int size=0;
	istm >> size;

	if(size<=0)
		return istm;

	for(int i=0; i<size; ++i)
	{
		Product product;
		istm >> product;
		vec.push_back(product);
	}

	return istm;
}

simple::file_istream& operator >> (simple::file_istream& istm, std::vector<Product>& vec)
{
	int size=0;
	istm >> size;

	if(size<=0)
		return istm;

	for(int i=0; i<size; ++i)
	{
		Product product;
		istm >> product;
		vec.push_back(product);
	}

	return istm;
}

simple::mem_ostream& operator << (simple::mem_ostream& ostm, const std::vector<Product>& vec)
{
	int size = vec.size();
	ostm << size;
	for(size_t i=0; i<vec.size(); ++i)
	{
		ostm << vec[i];
	}

	return ostm;
}

simple::file_ostream& operator << (simple::file_ostream& ostm, const std::vector<Product>& vec)
{
	int size = vec.size();
	ostm << size;
	for(size_t i=0; i<vec.size(); ++i)
	{
		ostm << vec[i];
	}

	return ostm;
}

void print_product(const Product& product)
{
	using namespace std;
	cout << "Product:" << product.product_name << ", Price:" << product.price << ", Qty:" << product.qty << endl;
}

void print_products(const std::vector<Product>& vec)
{
	for(size_t i=0; i<vec.size() ; ++i)
		print_product(vec[i]);
}
