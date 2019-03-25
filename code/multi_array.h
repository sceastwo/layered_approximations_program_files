

#ifndef __MULTI_ARRAY_H__
#define __MULTI_ARRAY_H__

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;

class multi_array
{
private:
	int num_of_variables;
	string* var_names; // The variable names will be sorted in lexicographic order.
	int* var_domain_sizes;
	int* index_step_sizes;
	double* array_entries;

	void generate_from_data(int input_num_of_variables, const string* input_var_names, const int* input_var_domain_sizes, const double* input_array_entries);
	void read_from_stream(istream& input);
	void copy(const multi_array& existing_array);
	void clear();
	string print(); // Prints the multi_array using the same format that is expected to be read. 
public:
	multi_array();
	multi_array(const multi_array& existing_array);
	multi_array(const multi_array* existing_array);
	multi_array& operator=(const multi_array& existing_array);
	multi_array(int input_num_of_variables, const string* input_var_names, const int* input_var_domain_sizes, const double* input_array_entries);
	multi_array(istream& input); 
	void print(ostream& output); // A basic print to an output stream.
	~multi_array();

	int get_num_of_variables() const;
	string get_var_name(int var_index) const;
	const string* get_var_names() const;
	int get_var_dom_size(int var_index) const;
	const int* get_var_dom_sizes() const;
	double get_array_entry(const int* var_values) const;
	const double* get_array_entries() const;
	void write_entries(double* dest_pointer) const; // Fill the array located at "dest_pointer" with the entries of "array_entries". 
	string formatted_print() const; // Prints the multi_array in a more explicit format. However, this explicit format cannot be read.

	void expand(int additional_num_of_variables, string* additional_var_names, int* additional_var_domain_sizes);
	void expand_2(int additional_num_of_variables, string* additional_var_names, int* additional_var_domain_sizes, int* additional_var_assignment, double default_value);
	void condition(const int* var_values); // -1 is an unassigned value.
	void marginalize(const bool* marg_flags);
	void scale(double scale_factor); 
	void scale_normalize();
	void sum_normalize();

	multi_array** generate_spectrum() const; // Create a spectrum of correlation terms that SUM to the current multi-array.
	static multi_array generate_probability_distribution(int input_num_of_variables, const string* input_var_names, const int* input_var_domain_sizes, 
		int num_of_terms, multi_array** the_correlation_terms);
	multi_array** generate_correlation_terms(double minimum_entropy_decrease) const; // Create a spectrum of correlation terms that generate the current probability distribution.

	enum binary_op_code {OP_ADDITION, OP_SUBTRACTION, OP_MULTIPLICATION, OP_DIVISION, OP_MAX, OP_MIN};
	multi_array binary_operator(const multi_array& the_array_2, binary_op_code the_op_code) const;
	multi_array operator+(const multi_array& the_array_2) const;
	multi_array operator-(const multi_array& the_array_2) const;
	multi_array operator*(const multi_array& the_array_2) const;
	multi_array operator/(const multi_array& the_array_2) const;

	double Shannon_Entropy() const; 
	static double KL_distance(const multi_array& the_array_1, const multi_array& the_array_2);
	static double L1_distance(const multi_array& the_array_1, const multi_array& the_array_2);
	string LaTeX_print() const;
};
multi_array max(const multi_array& the_array_1, const multi_array& the_array_2);
multi_array min(const multi_array& the_array_1, const multi_array& the_array_2);


#endif

