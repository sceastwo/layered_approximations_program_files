
#include "multi_array.h"


void multi_array::generate_from_data(int input_num_of_variables, const string* input_var_names, const int* input_var_domain_sizes, const double* input_array_entries)
{
	if (input_num_of_variables < 0)
	{
		throw "multi_array::generate_from_data: the number of input variables cannot be negative.\n";
	}

	// The variable names can be provided in any order.
	// The variable names will be shorted into lexicographic order.

	bool* used_flags = new bool[input_num_of_variables]; // These flags will mark variables that have been "used".
	for (int i = 0; i < input_num_of_variables; i++)
	{
		used_flags[i] = false;
	}

	// *************** Count the new number of variables ************************
	num_of_variables = 0;
	for (int i = 0; i < input_num_of_variables; i++)
	{ // CONSTRUCTOR 
		bool found_flag = false;
		for (int j = 0; j < i; j++)
		{
			if (input_var_names[j] == input_var_names[i])
			{
				found_flag = true;
				break;
			}
		}
		if (!found_flag)
		{
			num_of_variables++;
		}
		else
		{
			used_flags[i] = true;
		}
	}

	// *************** Generate the sorted list of variable names: *******************
	var_names = new string[num_of_variables];
	var_domain_sizes = new int[num_of_variables];

	int* var_index_map = new int[num_of_variables]; // A temporary array to map the sorted list to the old list.
	
	for (int i = 0; i < num_of_variables; i++)
	{ // CONSTRUCTOR 
		// Find the smallest unused variable name:
		int smallest_var_name_index = -1;
		for (int j = 0; j < input_num_of_variables; j++)
		{
			if (!used_flags[j])
			{
				if (smallest_var_name_index < 0)
				{
					smallest_var_name_index = j;
				}
				else if (input_var_names[j] < input_var_names[smallest_var_name_index])
				{
					smallest_var_name_index = j;
				}
			}
		}

		var_index_map[i] = smallest_var_name_index;

		var_names[i] = input_var_names[smallest_var_name_index];
		var_domain_sizes[i] = input_var_domain_sizes[smallest_var_name_index];

		used_flags[smallest_var_name_index] = true;
	}


	// *************** Generate the index step sizes: ******************
	index_step_sizes = new int[num_of_variables+1];
	index_step_sizes[num_of_variables] = 1;
	for (int i = num_of_variables-1; i >= 0; i--)
	{ // CONSTRUCTOR 
		index_step_sizes[i] = index_step_sizes[i+1]*var_domain_sizes[i];
	}

	int* input_index_step_sizes = new int[input_num_of_variables+1];
	input_index_step_sizes[input_num_of_variables] = 1;
	for (int i = input_num_of_variables-1; i >= 0 ; i--)
	{ // CONSTRUCTOR 
		input_index_step_sizes[i] = input_index_step_sizes[i+1]*input_var_domain_sizes[i];
	}


	// *************** Fill the array: *******************
	array_entries = new double[index_step_sizes[0]];

	int array_index = 0;
	int old_array_index = 0;
	int* array_coord = new int[num_of_variables]; // Sweeping the domain of the sorted variables.
	for (int i = 0; i < num_of_variables; i++)
	{
		array_coord[i] = 0;
	}
	while (true)
	{ // CONSTRUCTOR 
		array_entries[array_index] = input_array_entries[old_array_index];

		// Move to the next array coordinate:
		bool carry_flag = true;
		int var_index = num_of_variables-1;
		while (carry_flag && var_index >= 0)
		{
			if (array_coord[var_index] < var_domain_sizes[var_index]-1)
			{
				array_coord[var_index]++;
				old_array_index += input_index_step_sizes[var_index_map[var_index]+1];

				carry_flag = false;
			}
			else // propagate the carry:
			{
				array_coord[var_index] = 0;
				old_array_index -= input_index_step_sizes[var_index_map[var_index]+1]*(input_var_domain_sizes[var_index_map[var_index]] - 1);

				var_index--;
			}
		}

		array_index++;
		if (carry_flag) // An overflow indicates the end of the array sweep:
		{ // CONSTRUCTOR 
			break;
		}
	}

	// Clean up memory:
	delete[] used_flags;
	delete[] var_index_map;
	delete[] input_index_step_sizes;
	delete[] array_coord;
}


void multi_array::read_from_stream(istream& input)
{
	string string_buffer;

	int input_num_of_variables;
	input >> input_num_of_variables;
	string* input_var_names = new string[input_num_of_variables];
	int* input_var_domain_sizes = new int[input_num_of_variables];
	int total_size = 1;
	for (int i = 0; i < input_num_of_variables; i++)
	{
		input >> input_var_names[i];
		input >> input_var_domain_sizes[i];
		total_size *= input_var_domain_sizes[i];
		if (input_var_domain_sizes[i] < 1)
		{
			throw "multi_array::read_from_stream: variable domain size cannot be less than 1.\n";
		}
	}
	double* input_array_entries = new double[total_size];
	for (int i = 0; i < total_size; i++)
	{
		input >> input_array_entries[i];
	}

	generate_from_data(input_num_of_variables, input_var_names, input_var_domain_sizes, input_array_entries);

	delete[] input_var_names;
	delete[] input_var_domain_sizes;
	delete[] input_array_entries;
}


void multi_array::copy(const multi_array& existing_array)
{
	num_of_variables = existing_array.num_of_variables;
	var_names = new string[num_of_variables];
	var_domain_sizes = new int[num_of_variables];
	index_step_sizes = new int[num_of_variables+1];
	for (int i = 0; i < num_of_variables; i++)
	{
		var_names[i] = existing_array.var_names[i];
		var_domain_sizes[i] = existing_array.var_domain_sizes[i];
	}
	for (int i = 0; i < num_of_variables+1; i++)
	{
		index_step_sizes[i] = existing_array.index_step_sizes[i];
	}
	array_entries = new double[index_step_sizes[0]];
	for (int i = 0; i < index_step_sizes[0]; i++)
	{
		array_entries[i] = existing_array.array_entries[i];
	}
}

void multi_array::clear()
{
	delete[] var_names;
	delete[] var_domain_sizes;
	delete[] index_step_sizes;
	delete[] array_entries;
}

string multi_array::print()
{
	string output_string = "";
	output_string += (to_string(num_of_variables) + "\n");
	for (int i = 0; i < num_of_variables; i++)
	{
		output_string += (var_names[i] + " " + to_string(var_domain_sizes[i]) + "\n");
	}
	for (int i = 0; i < index_step_sizes[0]; i++)
	{
		output_string += (to_string(array_entries[i]) + " ");
	}
	output_string += "\n";

	return output_string;
}



multi_array::multi_array()
{
	num_of_variables = 0;
	var_names = new string[0];
	var_domain_sizes = new int[0];
	index_step_sizes = new int[1];
	index_step_sizes[0] = 1;
	array_entries = new double[1];
	array_entries[0] = 1.0;
}

multi_array::multi_array(const multi_array& existing_array)
{
	copy(existing_array);
}

multi_array::multi_array(const multi_array* existing_array)
{
	copy(*existing_array);
}

multi_array& multi_array::operator=(const multi_array& existing_array)
{
	clear();
	copy(existing_array);
	return *this;
}

multi_array::multi_array(int input_num_of_variables, const string* input_var_names, const int* input_var_domain_sizes, const double* input_array_entries)
{
	generate_from_data(input_num_of_variables, input_var_names, input_var_domain_sizes, input_array_entries);
}

multi_array::multi_array(istream& input)
{
	read_from_stream(input);
}

void multi_array::print(ostream& output)
{
	output << print();
}

multi_array::~multi_array()
{
	clear();
}



int multi_array::get_num_of_variables() const
{
	return num_of_variables;
}

string multi_array::get_var_name(int var_index) const
{
	return var_names[var_index];
}

const string* multi_array::get_var_names() const
{
	return var_names;
}

int multi_array::get_var_dom_size(int var_index) const
{
	return var_domain_sizes[var_index];
}

const int* multi_array::get_var_dom_sizes() const
{
	return var_domain_sizes;
}

double multi_array::get_array_entry(const int* var_values) const
{
	int array_index = 0;
	for (int i = num_of_variables-1; i >= 0; i--)
	{
		if (i == num_of_variables-1)
		{
			array_index += var_values[i];
		}
		else
		{
			array_index += index_step_sizes[i+1]*var_values[i];
		}
	}
	return array_entries[array_index];
}

const double* multi_array::get_array_entries() const
{
	return array_entries;
}

void multi_array::write_entries(double* dest_pointer) const
{
	for (int i = 0; i < index_step_sizes[0]; i++)
	{
		dest_pointer[i] = array_entries[i];
	}
}

string multi_array::formatted_print() const
{
	string output_string = "";
	output_string += "(";
	for (int i = 0; i < num_of_variables; i++)
	{ // FORMATTED PRINT
		output_string += ("[" + var_names[i] + ";" + to_string(var_domain_sizes[i]) + "]");
		if (i < num_of_variables-1)
		{
			output_string += ",";
		}
	}
	output_string += ")\n";

	int array_index = 0;
	int* coord = new int[num_of_variables];
	for (int i = 0; i < num_of_variables; i++)
	{
		coord[i] = 0;
	}
	while (true)
	{ // FORMATTED PRINT
		output_string += "(";
		for (int i = 0; i < num_of_variables; i++)
		{ // FORMATTED PRINT
			output_string += to_string(coord[i]);
			if (i < num_of_variables-1)
			{
				output_string += ",";
			}
		}
		output_string += (") => " + to_string(array_entries[array_index]) + "\n");

		// Move to the next array coordinate:
		bool carry_flag = true;
		int var_index = num_of_variables-1;
		while (carry_flag && (var_index >= 0))
		{ // FORMATTED PRINT
			if (coord[var_index] < var_domain_sizes[var_index]-1)
			{
				coord[var_index]++;

				carry_flag = false;
			}
			else // propagate the carry:
			{
				coord[var_index] = 0;

				var_index--;
			}
		}

		array_index++;
		if (carry_flag) // An overflow indicates the end of the array sweep:
		{ // FORMATTED PRINT
			break;
		}
	}

	delete[] coord;

	return output_string;
}



void multi_array::expand(int additional_num_of_variables, string* additional_var_names, int* additional_var_domain_sizes)
{
	// This routine will work by crudely assembling new data and then effectively calling the primary constructor.

	// *************** EXPAND: crudely create the new variable list ***********
	int input_num_of_variables = num_of_variables +  additional_num_of_variables;
	string* input_var_names = new string[num_of_variables + additional_num_of_variables];
	int* input_var_domain_sizes = new int[num_of_variables + additional_num_of_variables];
	for (int i = 0; i < num_of_variables; i++)
	{ // EXPAND
		input_var_names[i] = var_names[i];
		input_var_domain_sizes[i] = var_domain_sizes[i];
	}
	int additional_var_domain_sizes_product = 1;
	for (int i = 0; i < additional_num_of_variables; i++)
	{ // EXPAND
		input_var_names[num_of_variables+i] = additional_var_names[i];
		input_var_domain_sizes[num_of_variables+i] = additional_var_domain_sizes[i];
		additional_var_domain_sizes_product *= additional_var_domain_sizes[i];
	}

	// *************** EXPAND: crudely create the new data array ***************
	double* input_array_entries = new double[index_step_sizes[0]*additional_var_domain_sizes_product];
	for (int i = 0; i < index_step_sizes[0]; i++)
	{ // EXPAND
		int offset = additional_var_domain_sizes_product*i;
		for (int j = 0; j < additional_var_domain_sizes_product; j++)
		{ // EXPAND
			input_array_entries[offset+j] = array_entries[i];
		}
	}

	// **************** EXPAND: effectively call the primary constructor. ************
	clear();
	generate_from_data(input_num_of_variables, input_var_names, input_var_domain_sizes, input_array_entries);

	// ***************** EXPAND: clean up. ***************
	delete[] input_var_names;
	delete[] input_var_domain_sizes;
	delete[] input_array_entries;
}


void multi_array::expand_2(int additional_num_of_variables, string* additional_var_names, int* additional_var_domain_sizes, int* additional_var_assignment, double default_value)
{
	// This routine will work by crudely assembling new data and then effectively calling the primary constructor.

	// *************** EXPAND 2: crudely create the new variable list ***********
	int input_num_of_variables = num_of_variables +  additional_num_of_variables;
	string* input_var_names = new string[num_of_variables + additional_num_of_variables];
	int* input_var_domain_sizes = new int[num_of_variables + additional_num_of_variables];
	for (int i = 0; i < num_of_variables; i++)
	{ // EXPAND 2
		input_var_names[i] = var_names[i];
		input_var_domain_sizes[i] = var_domain_sizes[i];
	}
	for (int i = 0; i < additional_num_of_variables; i++)
	{ // EXPAND 2
		input_var_names[num_of_variables+i] = additional_var_names[i];
		input_var_domain_sizes[num_of_variables+i] = additional_var_domain_sizes[i];
	}

	int additional_var_domain_sizes_product = 1;
	int special_index = 0;
	for (int i = additional_num_of_variables-1; i >= 0; i--)
	{
		special_index += additional_var_domain_sizes_product*additional_var_assignment[i];
		additional_var_domain_sizes_product *= additional_var_domain_sizes[i];
	}


	// *************** EXPAND 2: crudely create the new data array ***************
	double* input_array_entries = new double[index_step_sizes[0]*additional_var_domain_sizes_product];
	for (int i = 0; i < index_step_sizes[0]; i++)
	{ // EXPAND 2
		int offset = additional_var_domain_sizes_product*i;
		for (int j = 0; j < additional_var_domain_sizes_product; j++)
		{ // EXPAND 2
			if (j == special_index)
			{
				input_array_entries[offset+j] = array_entries[i];
			}
			else
			{
				input_array_entries[offset+j] = default_value;
			}
		}
	}

	// **************** EXPAND 2: effectively call the primary constructor. ************
	clear();
	generate_from_data(input_num_of_variables, input_var_names, input_var_domain_sizes, input_array_entries);

	// ***************** EXPAND 2: clean up. ***************
	delete[] input_var_names;
	delete[] input_var_domain_sizes;
	delete[] input_array_entries;
}


void multi_array::condition(const int* var_values)
{
	// ************ CONDITION: count the remaining variables **************
	int new_num_of_variables = 0; 
	for (int i = 0; i < num_of_variables; i++)
	{ // CONDITION
		if (var_values[i] == -1)
		{
			new_num_of_variables++;
		}
	}

	// ************ CONDITION: generate the new variable lists ***************
	string* new_var_names = new string[new_num_of_variables];
	int* new_var_domain_sizes = new int[new_num_of_variables];
	int* new_to_old_var_index_map = new int[new_num_of_variables];

	int old_var_array_counter = 0;
	for (int i = 0; i < new_num_of_variables; i++)
	{ // CONDITION
		while (var_values[old_var_array_counter] != -1)
		{
			old_var_array_counter++;
		}
		new_var_names[i] = var_names[old_var_array_counter];
		new_var_domain_sizes[i] = var_domain_sizes[old_var_array_counter];
		new_to_old_var_index_map[i] = old_var_array_counter;
		old_var_array_counter++;
	}

	// ************** CONDITION: generate the new index step sizes *******************
	int* new_index_step_sizes = new int[new_num_of_variables+1];
	new_index_step_sizes[new_num_of_variables] = 1;
	for (int i = new_num_of_variables-1; i >= 0; i--)
	{ // CONDITION
		new_index_step_sizes[i] = new_index_step_sizes[i+1]*new_var_domain_sizes[i];
	}

	// **************** CONDITION: fill the array *****************
	double* new_array_entries = new double[new_index_step_sizes[0]];

	int new_array_index = 0;
	int old_array_index = 0;
	for (int i = 0; i < num_of_variables; i++) // Generate the offset of the old_array_index.
	{ // CONDITION
		if (var_values[i] != -1)
		{
			old_array_index += var_values[i]*index_step_sizes[i+1];
		}
	}
	int* coord = new int[new_num_of_variables]; // Sweeping the domain of the NEW variables.
	for (int i = 0; i < new_num_of_variables; i++)
	{ // CONDITION
		coord[i] = 0;
	}
	while (true)
	{ // CONDITION
		new_array_entries[new_array_index] = array_entries[old_array_index];

		// Move to the next array coordinate:
		bool carry_flag = true;
		int var_index = new_num_of_variables-1;
		while (carry_flag && (var_index >= 0))
		{ // CONDITION
			if (coord[var_index] < new_var_domain_sizes[var_index] - 1)
			{
				coord[var_index]++;
				old_array_index += index_step_sizes[new_to_old_var_index_map[var_index]+1];

				carry_flag = false;
			}
			else
			{
				coord[var_index] = 0;
				old_array_index -= index_step_sizes[new_to_old_var_index_map[var_index]+1]*(var_domain_sizes[new_to_old_var_index_map[var_index]]-1);

				var_index--;
			}
		}

		new_array_index++;
		if (carry_flag) // An overflow indicates the end of the array sweep:
		{ // CONDITION
			break;
		}
	}

	// ***************** CONDITION: update the fields ***********************
	delete[] new_to_old_var_index_map;
	delete[] coord;

	delete[] var_names;
	delete[] var_domain_sizes;
	delete[] index_step_sizes;
	delete[] array_entries;

	num_of_variables = new_num_of_variables;
	var_names = new_var_names;
	var_domain_sizes = new_var_domain_sizes;
	index_step_sizes = new_index_step_sizes;
	array_entries = new_array_entries;
}


void multi_array::marginalize(const bool* marg_flags)
{
	// ************ MARGINALIZE: count the remaining variables **************
	int new_num_of_variables = 0; 
	for (int i = 0; i < num_of_variables; i++)
	{ // MARGINALIZE
		if (!marg_flags[i])
		{
			new_num_of_variables++;
		}
	}

	// ************ MARGINALIZE: generate the new variable lists ***************
	string* new_var_names = new string[new_num_of_variables];
	int* new_var_domain_sizes = new int[new_num_of_variables];
	int* new_to_old_var_index_map = new int[new_num_of_variables];
	int* old_to_new_var_index_map = new int[num_of_variables];

	int old_var_array_counter = 0;
	for (int i = 0; i < new_num_of_variables; i++)
	{ // MARGINALIZE
		while (marg_flags[old_var_array_counter])
		{
			old_to_new_var_index_map[old_var_array_counter] = -1;
			old_var_array_counter++;
		}
		new_var_names[i] = var_names[old_var_array_counter];
		new_var_domain_sizes[i] = var_domain_sizes[old_var_array_counter];
		new_to_old_var_index_map[i] = old_var_array_counter;
		old_to_new_var_index_map[old_var_array_counter] = i;
		old_var_array_counter++;
	}
	while (marg_flags[old_var_array_counter] && old_var_array_counter < num_of_variables)
	{
		old_to_new_var_index_map[old_var_array_counter] = -1;
		old_var_array_counter++;
	}

	// ************** MARGINALIZE: generate the new index step sizes *******************
	int* new_index_step_sizes = new int[new_num_of_variables+1];
	new_index_step_sizes[new_num_of_variables] = 1;
	for (int i = new_num_of_variables-1; i >= 0; i--)
	{ // MARGINALIZE
		new_index_step_sizes[i] = new_index_step_sizes[i+1]*new_var_domain_sizes[i];
	}

	// **************** MARGINALIZE: fill the array *****************
	double* new_array_entries = new double[new_index_step_sizes[0]];
	for (int i = 0; i < new_index_step_sizes[0]; i++)
	{
		new_array_entries[i] = 0.0;
	}

	int new_array_index = 0;
	int old_array_index = 0;
	int* coord = new int[num_of_variables]; // Sweeping the domain of the OLD variables.
	for (int i = 0; i < num_of_variables; i++)
	{ // MARGINALIZE
		coord[i] = 0;
	}
	while (true)
	{ // MARGINALIZE
		new_array_entries[new_array_index] += array_entries[old_array_index];

		// Move to the next array coordinate:
		bool carry_flag = true;
		int var_index = num_of_variables-1;
		while (carry_flag && (var_index >= 0))
		{ // MARGINALIZE
			if (coord[var_index] < var_domain_sizes[var_index] - 1)
			{
				coord[var_index]++;
				if (old_to_new_var_index_map[var_index] != -1)
				{
					new_array_index += new_index_step_sizes[old_to_new_var_index_map[var_index]+1];
				}

				carry_flag = false;
			}
			else
			{
				coord[var_index] = 0;
				if (old_to_new_var_index_map[var_index] != -1)
				{
					new_array_index -= new_index_step_sizes[old_to_new_var_index_map[var_index]+1]*(new_var_domain_sizes[old_to_new_var_index_map[var_index]]-1);
				}

				var_index--;
			}
		}

		old_array_index++;
		if (carry_flag) // An overflow indicates the end of the array sweep:
		{ // MARGINALIZE
			break;
		}
	}

	// ***************** MARGINALIZE: update the fields ***********************
	delete[] new_to_old_var_index_map;
	delete[] old_to_new_var_index_map;
	delete[] coord;

	delete[] var_names;
	delete[] var_domain_sizes;
	delete[] index_step_sizes;
	delete[] array_entries;

	num_of_variables = new_num_of_variables;
	var_names = new_var_names;
	var_domain_sizes = new_var_domain_sizes;
	index_step_sizes = new_index_step_sizes;
	array_entries = new_array_entries;
}


void multi_array::scale(double scale_factor)
{
	for (int i = 0; i < index_step_sizes[0]; i++)
	{
		array_entries[i] *= scale_factor;
	}
}


void multi_array::scale_normalize()
{
	double total = 0.0;
	for (int i = 0; i < index_step_sizes[0]; i++)
	{ // SCALE NORMALIZE
		total += array_entries[i];
	}
	if (total == 0.0)
	{
		throw "multi_array::scale_normalize: the array total is 0.0.";
	}
	for (int i = 0; i < index_step_sizes[0]; i++)
	{ // SCALE NORMALIZE
		array_entries[i] /= total;
	}
}


void multi_array::sum_normalize()
{
	multi_array remainder(*this);

	bool* marg_flags = new bool[num_of_variables];
	for (int i = 0; i < num_of_variables; i++)
	{ // SUM NORMALIZE    
		multi_array sums(remainder);
		for (int j = 0; j < num_of_variables; j++)
		{ // SUM NORMALIZE 
			marg_flags[j] = (j == i);
		}
		sums.marginalize(marg_flags);
		sums.scale(1.0/var_domain_sizes[i]);
		remainder = remainder - sums;
	}

	delete[] marg_flags;

	operator=(remainder);
}



multi_array** multi_array::generate_spectrum() const
{
	int* powers_of_2 = new int[num_of_variables+1];
	powers_of_2[0] = 1;
	for (int i = 1; i <= num_of_variables; i++)
	{
		powers_of_2[i] = powers_of_2[i-1]*2;
	}

	multi_array remainder(*this);

	multi_array** the_array_spectrum = new multi_array*[powers_of_2[num_of_variables]];

	bool* inclusion_flags = new bool[num_of_variables];
	int array_spectrum_index = 0;
	bool* marg_flags = new bool[num_of_variables];

	for (int i = 0; i < num_of_variables; i++)
	{ // GENERATE SPECTRUM
		inclusion_flags[i] = false;
	}
	while (true)
	{ // GENERATE SPECTRUM 
		multi_array sums(remainder);
		int marg_domain_size = 1;
		for (int i = 0; i < num_of_variables; i++)
		{
			marg_flags[i] = !inclusion_flags[i];
			if (marg_flags[i])
			{
				marg_domain_size *= var_domain_sizes[i];
			}
		}
		sums.marginalize(marg_flags);
		sums.scale(1.0/marg_domain_size);
		remainder = remainder - sums;

		the_array_spectrum[array_spectrum_index] = new multi_array(sums);

		bool carry_flag = true;
		int var_index = num_of_variables-1;
		while (carry_flag && (var_index >= 0))
		{ // GENERATE SPECTRUM 
			if (!inclusion_flags[var_index])
			{
				inclusion_flags[var_index] = true;

				carry_flag = false;
			}
			else
			{
				inclusion_flags[var_index] = false;

				var_index--;
			}
		}

		array_spectrum_index++;
		if (carry_flag)
		{
			break;
		}
	}

	delete[] powers_of_2;
	delete[] inclusion_flags;
	delete[] marg_flags;

	return the_array_spectrum;
}


multi_array multi_array::generate_probability_distribution(int input_num_of_variables, const string* input_var_names, const int* input_var_domain_sizes, 
		int num_of_terms, multi_array** the_correlation_terms)
{
	// Generate the initial model w/o any correlation terms:
	int* input_index_step_sizes = new int[input_num_of_variables+1];
	input_index_step_sizes[input_num_of_variables] = 1;
	for (int i = input_num_of_variables-1; i >= 0; i--)
	{ // GENERATE PROBABILITIES
		input_index_step_sizes[i] = input_index_step_sizes[i+1]*input_var_domain_sizes[i];
	}
	double* initial_model_probabilities = new double[input_index_step_sizes[0]];
	for (int i = 0; i < input_index_step_sizes[0]; i++)
	{ // GENERATE PROBABILITIES
		initial_model_probabilities[i] = 1.0/input_index_step_sizes[0];
	}
	multi_array model_probabilities(input_num_of_variables,input_var_names,input_var_domain_sizes,initial_model_probabilities);

	// Sequentially apply the correlation terms:
	bool* marg_flags = new bool[input_num_of_variables];
	for (int i = 0; i < num_of_terms; i++)
	{ // GENERATE PROBABILITIES
		if (the_correlation_terms[i] == NULL) // Skip non-existant correlation terms.
		{
			continue;
		}

		for (int j = 0; j < input_num_of_variables; j++)
		{
			marg_flags[j] = false;
			for (int k = 0; k < the_correlation_terms[i]->num_of_variables; k++)
			{ // GENERATE PROBABILITIES
				if (the_correlation_terms[i]->var_names[k] == input_var_names[j])
				{
					marg_flags[j] = true;
					break;
				}
			}
		}

		multi_array model_marginal_probabilities(model_probabilities);
		model_marginal_probabilities.marginalize(marg_flags);

		model_probabilities = model_probabilities + model_marginal_probabilities * *(the_correlation_terms[i]);

	}

	delete[] input_index_step_sizes;
	delete[] initial_model_probabilities;
	delete[] marg_flags;

	return model_probabilities;
}


multi_array** multi_array::generate_correlation_terms(double minimum_entropy_decrease) const
{
	int* powers_of_2 = new int[num_of_variables+1];
	powers_of_2[0] = 1;
	for (int i = 1; i <= num_of_variables; i++)
	{
		powers_of_2[i] = powers_of_2[i-1]*2;
	}

	// Generate the initial model w/o any correlation terms:
	double* initial_model_probabilities = new double[index_step_sizes[0]];
	for (int i = 0; i < index_step_sizes[0]; i++)
	{ // GENERATE CORRELATION TERMS
		initial_model_probabilities[i] = 1.0/index_step_sizes[0];
	}
	multi_array model_probabilities(num_of_variables,var_names,var_domain_sizes,initial_model_probabilities);
	double model_entropy = log((double) index_step_sizes[0]) / log(2.0);

	multi_array** the_correlation_terms = new multi_array*[powers_of_2[num_of_variables]];

	for (int n = 0; n <= num_of_variables; n++) // Step through the variable set sizes:
	{ // GENERATE CORRELATION TERMS
		bool* inclusion_flags = new bool[num_of_variables];
		int array_spectrum_index = 0;
		bool* marg_flags = new bool[num_of_variables];
		for (int i = 0; i < num_of_variables; i++)
		{ // GENERATE CORRELATION TERMS
			inclusion_flags[i] = false;
		}

		while (true)
		{ // GENERATE CORRELATION TERMS 
			int set_size = 0; // Compute the size of the current variable set:
			for (int i = 0; i < num_of_variables; i++)
			{
				if (inclusion_flags[i])
				{
					set_size++;
				}
			}
			if (set_size == n) // Only generate a correlation term for variable sets of the correct size:
			{
				int marg_domain_size = 1;
				for (int i = 0; i < num_of_variables; i++)
				{ // GENERATE CORRELATION TERMS
					marg_flags[i] = !inclusion_flags[i];
					if (marg_flags[i])
					{
						marg_domain_size *= var_domain_sizes[i];
					}
				}

				multi_array true_marginal_probabilities(*this);
				true_marginal_probabilities.marginalize(marg_flags);
				multi_array model_marginal_probabilities(model_probabilities);
				model_marginal_probabilities.marginalize(marg_flags);

				multi_array new_term = true_marginal_probabilities - model_marginal_probabilities;
				new_term.sum_normalize();

				// Compute the potential new model:
				multi_array model_complement_marginal_probabilities(model_probabilities); // Marginalize out the variables from the current variable set.
				model_complement_marginal_probabilities.marginalize(inclusion_flags);
				multi_array new_model_probabilities = model_probabilities + model_complement_marginal_probabilities*new_term;
				double new_model_entropy = 0.0;
				for (int i = 0; i < index_step_sizes[0]; i++)
				{ // GENERATE CORRELATION TERMS
					if (new_model_probabilities.array_entries[i] > 0)
					{
						new_model_entropy -= new_model_probabilities.array_entries[i] * log(new_model_probabilities.array_entries[i]) / log(2.0);
					}
				}
				if (new_model_entropy < model_entropy - minimum_entropy_decrease)
				{ // GENERATE CORRELATION TERMS
					the_correlation_terms[array_spectrum_index] = new multi_array(new_term);

					model_probabilities = new_model_probabilities;
					model_entropy = new_model_entropy;
				}
				else
				{
					the_correlation_terms[array_spectrum_index] = NULL;
				}
			}

			// Move to the next variable set:
			bool carry_flag = true;
			int var_index = num_of_variables-1;
			while (carry_flag && (var_index >= 0)) 
			{ // GENERATE CORRELATION TERMS
				if (!inclusion_flags[var_index])
				{
					inclusion_flags[var_index] = true;

					carry_flag = false;
				}
				else
				{
					inclusion_flags[var_index] = false;
	
					var_index--;
				}
			}

			array_spectrum_index++;
			if (carry_flag)
			{ // GENERATE CORRELATION TERMS
				break;
			}
		}

		delete[] inclusion_flags;
		delete[] marg_flags;
	}

	delete[] powers_of_2;
	delete[] initial_model_probabilities;

	return the_correlation_terms;
}


multi_array multi_array::binary_operator(const multi_array& the_array_2, binary_op_code the_op_code) const
{
	multi_array operand_1(*this);
	multi_array operand_2(the_array_2);

	operand_1.expand(the_array_2.num_of_variables,the_array_2.var_names,the_array_2.var_domain_sizes);
	operand_2.expand(num_of_variables,var_names,var_domain_sizes);

	double* new_array_entries = new double[operand_1.index_step_sizes[0]];
	for (int i = 0; i < operand_1.index_step_sizes[0]; i++)
	{ // BINARY OPERATOR
		if (the_op_code == OP_ADDITION)
		{
			new_array_entries[i] = operand_1.array_entries[i] + operand_2.array_entries[i];
		}
		else if (the_op_code == OP_SUBTRACTION)
		{
			new_array_entries[i] = operand_1.array_entries[i] - operand_2.array_entries[i];
		}
		else if (the_op_code == OP_MULTIPLICATION)
		{
			new_array_entries[i] = operand_1.array_entries[i] * operand_2.array_entries[i];
		}
		else if (the_op_code == OP_DIVISION)
		{
			new_array_entries[i] = operand_1.array_entries[i] / operand_2.array_entries[i];
		}
		else if (the_op_code == OP_MAX)
		{
			new_array_entries[i] = max(operand_1.array_entries[i],operand_2.array_entries[i]);
		}
		else if (the_op_code == OP_MIN)
		{
			new_array_entries[i] = min(operand_1.array_entries[i],operand_2.array_entries[i]);
		}
		else
		{
			throw "multi_array::binary_operator: Invalid op code.";
		}
	}

	multi_array resultant_array(operand_1.num_of_variables,operand_1.var_names,operand_1.var_domain_sizes,new_array_entries);

	delete[] new_array_entries;

	return resultant_array;
}

multi_array multi_array::operator+(const multi_array& the_array_2) const
{
	return binary_operator(the_array_2, OP_ADDITION);
}

multi_array multi_array::operator-(const multi_array& the_array_2) const
{
	return binary_operator(the_array_2, OP_SUBTRACTION);
}

multi_array multi_array::operator*(const multi_array& the_array_2) const
{
	return binary_operator(the_array_2, OP_MULTIPLICATION);
}

multi_array multi_array::operator/(const multi_array& the_array_2) const
{
	return binary_operator(the_array_2, OP_DIVISION);
}

multi_array max(const multi_array& the_array_1, const multi_array& the_array_2)
{
	return the_array_1.binary_operator(the_array_2,multi_array::OP_MAX);
}

multi_array min(const multi_array& the_array_1, const multi_array& the_array_2)
{
	return the_array_1.binary_operator(the_array_2,multi_array::OP_MIN);
}


double multi_array::Shannon_Entropy() const
{
	double entropy = 0.0;
	for (int i = 0; i < index_step_sizes[0]; i++)
	{ // SHANNON ENTROPY
		if (array_entries[i] > 0)
		{
			entropy -= array_entries[i] * log(array_entries[i]) / log(2.0);
		}
	}
	return entropy;
}


double multi_array::KL_distance(const multi_array& the_array_1, const multi_array& the_array_2)
{
	double dist = 0.0;

	// It will be assumed that both arrays have the same variable sets.
	for (int i = 0; i < the_array_1.index_step_sizes[0]; i++)
	{
		if ((the_array_1.array_entries[i] > 0) && (the_array_2.array_entries[i] > 0))
		{
			dist += the_array_1.array_entries[i] * log(the_array_1.array_entries[i]/the_array_2.array_entries[i]) / log(2.0);
		}
		else if (the_array_1.array_entries[i] > 0) // If the element in the 2nd array is non-positive, but not the element from the first array, then return a large value.
		{
			return 10000.00;
		}
	}

	return dist;
}

double multi_array::L1_distance(const multi_array& the_array_1, const multi_array& the_array_2)
{
	double dist = 0.0;

	// It will be assumed that both arrays have the same variable sets.
	for (int i = 0; i < the_array_1.index_step_sizes[0]; i++)
	{
		dist += abs(the_array_1.array_entries[i] - the_array_2.array_entries[i]);
	}

	dist /= the_array_1.index_step_sizes[0];
	return dist;
}



string lowercase(string input_string)
{
	string output_string = input_string;

	for (int i = 0; i < (int) output_string.length(); i++)
	{
		output_string[i] = tolower(output_string[i]);
	}

	return output_string;
}

string multi_array::LaTeX_print() const
{
	string LaTeX_string = "";

	// LATEX PRINT : determine the dimensions that will be indexed by the rows and columns.
	int row_dimensions = num_of_variables;
	int column_dimensions = 0;
	while ((row_dimensions > 0) && (index_step_sizes[row_dimensions-1] <= 8))
	{
		row_dimensions--;
		column_dimensions++;
	}
	int table_width = index_step_sizes[row_dimensions];
	int table_height = index_step_sizes[0] / table_width;
	
	// LATEX PRINT : the table top-matter:
	LaTeX_string += "\\(t(";
	for (int i = 0; i < num_of_variables; i++)
	{ // LATEX PRINT 
		LaTeX_string += var_names[i];
		if (i < num_of_variables-1)
		{
			LaTeX_string += ",";
		}
		else
		{
			LaTeX_string += ")\\) \\\\\n";
		}
	}
	LaTeX_string += "\\begin{small}\n";
	LaTeX_string += "\\begin{tabular}{|";
	if (row_dimensions == 0)
	{
		LaTeX_string += "c||";
	}
	else
	{
		for (int i = 0; i < row_dimensions; i++)
		{
			LaTeX_string += "c|";
		}
		LaTeX_string += "|";
	}
	for (int i = 0; i < table_width; i++)
	{
		LaTeX_string += "c|";
	}
	LaTeX_string += "}\n";
	
	// LATEX PRINT : the table body: 3 cases
	if (row_dimensions == 0)
	{
		if (column_dimensions > 0)
		{
			// LATEX PRINT : CASE #1 row_dimensions == 0 && column_dimensions > 0
			LaTeX_string += "\\hline\n";
			for (int i = 0; i < column_dimensions; i++)
			{
				int var_index = row_dimensions + i;
				LaTeX_string += ("\\(" + var_names[var_index] + "\\)");
				for (int j = 0; j < (table_width / index_step_sizes[var_index+1]); j++)
				{
					if (var_index < num_of_variables - 1)
					{
						LaTeX_string += (" & \\multicolumn{" + to_string(index_step_sizes[var_index+1]) + "}{|c|}{\\(" 
							+ lowercase(var_names[var_index]) + "_" + to_string((j % var_domain_sizes[var_index]) + 1) + "\\)}");
					}
					else
					{
						LaTeX_string += (" & \\(" + lowercase(var_names[num_of_variables-1]) + "_" + to_string((j % var_domain_sizes[num_of_variables-1]) + 1) + "\\)");
					}
				}
				LaTeX_string += " \\\\\n";
				LaTeX_string += "\\hline\n";
			}
			LaTeX_string += "\\hline\n";
			LaTeX_string += "\\(t(";
			for (int i = 0; i < num_of_variables; i++)
			{ // LATEX PRINT : CASE #1 
				LaTeX_string += var_names[i];
				if (i < num_of_variables-1)
				{
					LaTeX_string += ",";
				}
				else
				{
					LaTeX_string += ")\\)";
				}
			}
			for (int i = 0; i < table_width; i++)
			{
				LaTeX_string += (" & " + to_string(array_entries[i]));
			}
			LaTeX_string += " \\\\\n";
			LaTeX_string += "\\hline\n";
		}
	}
	else
	{
		if (column_dimensions == 0)
		{
			// LATEX PRINT : CASE #2 row_dimensions > 0 && column_dimensions == 0
			LaTeX_string += "\\hline\n";
			for (int i = 0; i < row_dimensions; i++)
			{ // LATEX PRINT : CASE #2
				LaTeX_string += ("\\(" + var_names[i] + "\\) & ");
			}
			LaTeX_string += "\\(t(";
			for (int i = 0; i < num_of_variables; i++)
			{ // LATEX PRINT : CASE #2
				LaTeX_string += var_names[i];
				if (i < num_of_variables-1)
				{
					LaTeX_string += ",";
				}
				else
				{
					LaTeX_string += ")\\)";
				}
			}
			LaTeX_string += " \\\\\n";
			LaTeX_string += "\\hline\n";
			LaTeX_string += "\\hline\n";

			for (int i = 0; i < table_height; i++)
			{ // LATEX PRINT : CASE #2
				int index_offset = table_width * i;
				for (int j = 0; j < row_dimensions; j++)
				{ // LATEX PRINT : CASE #2
					int var_assignment = (index_offset / index_step_sizes[j+1]) % var_domain_sizes[j];
					LaTeX_string += ("\\(" + lowercase(var_names[j]) + "_" + to_string(var_assignment + 1) + "\\) & ");
				}
				LaTeX_string += (to_string(array_entries[index_offset]) + " \\\\\n");
				LaTeX_string += "\\hline\n";
			}
		}
		else
		{
			// LATEX PRINT : CASE #3 row_dimensions > 0 && column_dimensions > 0
			LaTeX_string += "\\hline\n";
			for (int i = 0; i < column_dimensions; i++)
			{ // LATEX PRINT : CASE #3 
				int col_var_index = row_dimensions + i;
				if (row_dimensions <= 1)
				{
					LaTeX_string += ("\\(" + var_names[col_var_index] + "\\)");
				}
				else
				{
					LaTeX_string += ("\\multicolumn{" + to_string(row_dimensions) + "}{|c||}{\\(" + var_names[col_var_index] + "\\)}");
				}
				for (int j = 0; j < (table_width / index_step_sizes[col_var_index+1]); j++)
				{ // LATEX PRINT : CASE #3 
					if (col_var_index < num_of_variables - 1)
					{
						LaTeX_string += (" & \\multicolumn{" + to_string(index_step_sizes[col_var_index+1]) + "}{|c|}{\\(" 
							+ lowercase(var_names[col_var_index]) + "_" + to_string((j % var_domain_sizes[col_var_index]) + 1) + "\\)}");
					}
					else
					{
						LaTeX_string += (" & \\(" + lowercase(var_names[num_of_variables-1]) + "_" + to_string((j % var_domain_sizes[num_of_variables-1]) + 1) + "\\)");
					}
				}
				LaTeX_string += " \\\\\n";
				LaTeX_string += "\\hline\n";
			}
			LaTeX_string += "\\hline\n";
			for (int i = 0; i < row_dimensions; i++)
			{ // LATEX PRINT : CASE #3
				LaTeX_string += ("\\(" + var_names[i] + "\\) & ");
			}
			LaTeX_string += "\\multicolumn{" + to_string(table_width) + "}{|c|}{\\(t(";
			for (int i = 0; i < num_of_variables; i++)
			{ // LATEX PRINT : CASE #3  
				LaTeX_string += var_names[i];
				if (i < num_of_variables-1)
				{
					LaTeX_string += ",";
				}
				else
				{
					LaTeX_string += ")\\)}";
				}
			}
			LaTeX_string += " \\\\\n";
			LaTeX_string += "\\hline\n";
			LaTeX_string += "\\hline\n";

			for (int i = 0; i < table_height; i++)
			{
				int index_offset = table_width * i;
				for (int j = 0; j < row_dimensions; j++)
				{ // LATEX PRINT : CASE #3
					int var_assignment = (index_offset / index_step_sizes[j+1]) % var_domain_sizes[j];
					LaTeX_string += ("\\(" + lowercase(var_names[j]) + "_" + to_string(var_assignment + 1) + "\\) & ");
				}
				for (int j = 0; j < table_width; j++)
				{ // LATEX PRINT : CASE #3
					LaTeX_string += to_string(array_entries[index_offset + j]);
					if (j < table_width-1)
					{
						LaTeX_string += " & ";
					}
					else
					{
						LaTeX_string += " \\\\\n";
					}
				}
				LaTeX_string += "\\hline\n";
			}
		}
	}
	
	// LATEX PRINT : the table bottom matter:
	LaTeX_string += "\\end{tabular}\n";
	LaTeX_string += "\\end{small}\n";
	
	return LaTeX_string;
}

