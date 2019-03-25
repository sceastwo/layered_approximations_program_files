
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include "multi_array.h"
#include "data_quantization.h"

//#define __TESTING__
//#define __TESTING_MULTI_ARRAY__
//#define __TESTING_BINARY_OPERATORS__
//#define __TESTING_ENTROPY__
//#define __TESTING_SPECTRUM_GENERATION__
//#define __TESTING_DATA_QUANTIZATION__
#define __NO_TESTING__
//#define __WITH_INPUT_DATA__ // Is the input data being used?

int main()
{

#ifdef __TESTING__
	ifstream multi_array_input("Input and output files/testing_multi_array.txt");
	int num_of_input_arrays;
	multi_array* arr_x = NULL;
	try
	{
		multi_array_input >> num_of_input_arrays;
		arr_x = new multi_array[num_of_input_arrays];
		for (int i = 0; i < num_of_input_arrays; i++)
		{
			arr_x[i] = multi_array(multi_array_input);
			cout << "Array " << (i+1) << ":\n" << arr_x[i].formatted_print() << '\n';
		}
		cout << "************************\n";
	}
	catch (char* error_msg)
	{
		cerr << error_msg << '\n';
	}
#endif

#ifdef __TESTING_MULTI_ARRAY__

	cout << "Testing the 0 dimensional array:\n";
	multi_array().print(cout);
	cout << multi_array().formatted_print() << '\n';

	cout << "Read and print array 1:\n";
	arr_x[0].print(cout);
	cout << arr_x[0].formatted_print() << '\n';

	cout << "\ncopy array 1:\n";
	multi_array arr_1_copy(arr_x[0]);
	cout << arr_1_copy.formatted_print() << '\n';

	cout << "\nmarginalize array 1:\n";
	bool marg_flags[3] = {false, false, false};
	arr_1_copy.marginalize(marg_flags);
	cout << arr_1_copy.formatted_print() << '\n';

	arr_1_copy = multi_array(arr_x[0]);
	marg_flags[0] = false;
	marg_flags[1] = true;
	marg_flags[2] = false;
	arr_1_copy.marginalize(marg_flags);
	cout << arr_1_copy.formatted_print() << '\n';

	arr_1_copy = multi_array(arr_x[0]);
	marg_flags[0] = true;
	marg_flags[1] = true;
	marg_flags[2] = false;
	arr_1_copy.marginalize(marg_flags);
	cout << arr_1_copy.formatted_print() << '\n';

	arr_1_copy = multi_array(arr_x[0]);
	marg_flags[0] = true;
	marg_flags[1] = true;
	marg_flags[2] = true;
	arr_1_copy.marginalize(marg_flags);
	cout << arr_1_copy.formatted_print() << '\n';

	cout << "\ncondition array 1:\n";
	multi_array arr_1_copy_2(arr_x + 0);
	int cond_var_values[3] = {-1, 1,-1};
	arr_1_copy_2.condition(cond_var_values);
	arr_1_copy_2.print(cout);

	cout << "\nRead and print array 2:\n";
	arr_x[1].print(cout); 
	cout << arr_x[1].formatted_print() << '\n';
#endif

#ifdef __TESTING_BINARY_OPERATORS__
	cout << "\nTesting addition, subtraction, multiplication, and division:\n";
	cout << "The operands are:\n";
	cout << arr_x[1].formatted_print() << '\n'
		 << arr_x[2].formatted_print() << '\n';

	cout << "The sum is:\n"
		 << (arr_x[1] + arr_x[2]).formatted_print() << '\n';

	cout << "The difference is:\n"
		 << (arr_x[1] - arr_x[2]).formatted_print() << '\n';

	cout << "The product is:\n"
		 << (arr_x[1] * arr_x[2]).formatted_print() << '\n';
	
	cout << "The quotient is:\n"
		 << (arr_x[1] / arr_x[2]).formatted_print() << '\n';
#endif

#ifdef __TESTING_ENTROPY__
	cout << "Testing Shannon Entropy:\n";
	multi_array Pr1(arr_x[2]);
	Pr1.scale_normalize();
	cout << Pr1.formatted_print() << '\n';
	multi_array Pr2(arr_x[6]);
	Pr2.scale_normalize();
	cout << Pr2.formatted_print() << '\n';
	cout << Pr1.Shannon_Entropy() << ' ' << Pr2.Shannon_Entropy() << '\n';
	cout << multi_array::KL_distance(Pr1,Pr2) << ' ' << multi_array::KL_distance(Pr2,Pr1) << '\n';

#endif

#ifdef __TESTING_SPECTRUM_GENERATION__
	cout << "\nTesting the spectrum generation:\n";
	cout << arr_x[0].formatted_print() << '\n';
	multi_array** arr_spectrum = arr_x[0].generate_spectrum();
	multi_array total;
	for (int i = 0; i < 8; i++)
	{
		cout << arr_spectrum[i]->formatted_print() << '\n';
		if (i == 0)
		{
			total = *(arr_spectrum[i]);
		}
		else
		{
			total = total + *(arr_spectrum[i]);
		}
	}
	cout << total.formatted_print() << '\n';

	try
	{
		cout << "\nTesting the generation of correlation terms:\n";
		multi_array correlation_term_test_array(arr_x[4]);
		correlation_term_test_array.scale_normalize();
		cout << correlation_term_test_array.formatted_print() << '\n';
		multi_array** correlation_term_test_array_correlation_terms = correlation_term_test_array.generate_correlation_terms(0.05);
		for (int i = 0; i < pow(2,correlation_term_test_array.get_num_of_variables()); i++)
		{
			if (correlation_term_test_array_correlation_terms[i] != NULL)
			{
				cout << correlation_term_test_array_correlation_terms[i]->formatted_print() << '\n';
			}
			else
			{
				cout << "NULL\n";
			}
		}
		multi_array correlation_term_test_array_restored = multi_array::generate_probability_distribution(
			correlation_term_test_array.get_num_of_variables(),
			correlation_term_test_array.get_var_names(),
			correlation_term_test_array.get_var_dom_sizes(), 
			(int) pow(2,correlation_term_test_array.get_num_of_variables()),
			correlation_term_test_array_correlation_terms);
		cout << correlation_term_test_array_restored.formatted_print() << '\n';
	}
	catch (char* error_msg)
	{
		cout << error_msg << '\n';
		return EXIT_FAILURE;
	}

#endif


#ifdef __TESTING_DATA_QUANTIZATION__
	cout << "Testing data quantization:\n";

	int num_of_data_points = 452;
	double* the_data = new double[num_of_data_points];
	int num_of_categories = 50;
	double* category_dividers = NULL;
	double* category_points = NULL;

	for (int i = 0; i < num_of_data_points; i++)
	{
		the_data[i] = 1000.0 * ((double) rand()) / ((double) RAND_MAX);
		cout << the_data[i] << ' ';
	}

	data_quantize(num_of_data_points,the_data,num_of_categories,
		category_dividers,category_points);

	cout << "\n\n";
	for (int i = 0; i < num_of_categories; i++)
	{
		if (i == 0)
		{
			cout << "(-inf," << category_points[i] << "," << category_dividers[i] << ") ";
		}
		else if (i == num_of_categories-1)
		{
			cout << "(" << category_dividers[i-1] << "," << category_points[i] << ",+inf)"; 
		}
		else
		{
			cout << "(" << category_dividers[i-1] << "," << category_points[i] << "," << category_dividers[i] << ") ";
		}
	}

	cout << "\n\n";

#endif

#ifdef __NO_TESTING__


#ifdef __WITH_INPUT_DATA__
	// MAIN: Extract the raw data:
	ifstream data_input("Input and output files/input_data_1.csv");
	ofstream data_output("Input and output files/output_data_1.txt");

	const int max_num_of_lines = 5000;
	const int max_line_length = 50;
	int num_of_lines = 0;
	int* line_lengths = new int[max_num_of_lines];
	string** token_array = new string*[max_num_of_lines];

	char char_buffer = 'a';

	int line_number = 0;
	while (!data_input.eof() && line_number < max_num_of_lines)
	{ // MAIN: read all input
		line_lengths[line_number] = 0;
		token_array[line_number] = new string[max_line_length];
		while (!data_input.eof() && line_lengths[line_number] < max_line_length)
		{ // MAIN: read a line
			token_array[line_number][line_lengths[line_number]] = "";
			while (!data_input.eof())
			{ // MAIN: read a token
				data_input.read(&char_buffer,1);
				if (char_buffer != ',' && char_buffer != '\n')
				{
					token_array[line_number][line_lengths[line_number]] += char_buffer;
				}
				else 
				{
					break;
				}
			}

			line_lengths[line_number]++;
			if (char_buffer == '\n')
			{
				break;
			}
		}
		line_number++;
	}

	// ***************************** TESTING *****************************
	/*for (int i = 0; i < line_number; i++)
	{
		cout << i << ": " << line_lengths[i] << " : ";
		for (int j = 0; j < line_lengths[i]; j++)
		{
			cout << token_array[i][j] << ",";
		}
		cout << '\n';
	}*/
	// ************************* END TESTING *****************************

	// MAIN: Get the variables:
	const int var_column_offset = 0;
	int num_of_variables = line_lengths[0] - var_column_offset;
	string* var_names = new string[num_of_variables];
	int* var_domain_sizes = new int[num_of_variables];
	for (int i = 0; i < num_of_variables; i++)
	{ // MAIN
		var_names[i] = token_array[0][var_column_offset+i];
		var_domain_sizes[i] = 3;
	}
	var_domain_sizes[0] = 3;
	var_domain_sizes[1] = 2;
	var_domain_sizes[2] = 3;
	var_domain_sizes[3] = 2;
	int* index_step_sizes = new int[num_of_variables+1];
	index_step_sizes[num_of_variables] = 1;
	for (int i = num_of_variables-1; i >= 0; i--)
	{
		index_step_sizes[i] = index_step_sizes[i+1]*var_domain_sizes[i];
	}
	
	// MAIN: collate the data:
	double** data_points = new double*[line_number];
	for (int i = 0; i < line_number; i++)
	{
		data_points[i] = new double[num_of_variables];
	}
	double** domain_points = new double*[num_of_variables];
	for (int i = 0; i < num_of_variables; i++)
	{
		domain_points[i] = new double[line_number];
	}

	int num_of_data_points = 0;
	for (int i = 1; i < line_number; i++) // Start at i=1, since the first row is the variable names.
	{ // MAIN
		// If any of the fields are blank (or unreadable), then the data point should be skipped.
		bool skip_flag = false;
		for (int j = 0; (j < num_of_variables) && !skip_flag; j++)
		{
			if (token_array[i][var_column_offset+j] == "NA") // Is the field blank?
			{
				skip_flag = true;
				break;
			}
			else
			{
				try // Is the field unreadable?
				{
					stod(token_array[i][var_column_offset+j]);
				}
				catch (const invalid_argument& the_error)
				{
					skip_flag = true;
					break;
				}
			}
		}
		if (skip_flag)
		{
			continue;
		}

		// Extract the data point:
		for (int j = 0; j < num_of_variables; j++)
		{ // MAIN
			try
			{
				data_points[num_of_data_points][j] = stod(token_array[i][var_column_offset+j]);
			}
			catch (const invalid_argument& the_error)
			{
				cerr << "Invalid argument: " << the_error.what() << " " << token_array[i][var_column_offset+j] << "\n";
				system("PAUSE");
				return EXIT_FAILURE;
			}
			domain_points[j][num_of_data_points] = data_points[num_of_data_points][j];
		}

		num_of_data_points++;
	}

	// ***************************** TESTING *****************************
	/*for (int i = 0; i < num_of_data_points; i++)
	{
		cout << i << "(";
		for (int j = 0; j < num_of_variables; j++)
		{
			cout << data_points[i][j];
			if (j < num_of_variables-1)
			{
				cout << ",";
			}
			else
			{
				cout << ")";
			}
		}
		cout << "\n";
	}*/
	// ************************* END TESTING *****************************

	// MAIN: Quantize the variable domains:
	double** category_dividers = new double*[num_of_variables]; // There are separate category dividers for each variable.
	double** category_points = new double*[num_of_variables]; // There are separate category points for each variable.
	cout << "\n";
	data_output << "\n";
	for (int i = 0; i < num_of_variables; i++)
	{ // MAIN
		data_quantize(num_of_data_points,domain_points[i],var_domain_sizes[i],
			category_dividers[i],category_points[i]);

		cout << "\nThe intervals for variable " << var_names[i] << " are:\n";
		data_output << "\nThe intervals for variable " << var_names[i] << " are:\n";
		for (int j = 0; j < var_domain_sizes[i]; j++)
		{
			if (j == 0)
			{
				cout << "(-inf," << category_points[i][j] << "," << category_dividers[i][j] << ") ";
				data_output << "(-inf," << category_points[i][j] << "," << category_dividers[i][j] << ") "; 
			}
			else if (j == var_domain_sizes[i]-1)
			{
				cout << "(" << category_dividers[i][j-1] << "," << category_points[i][j] << ",+inf)"; 
				data_output << "(" << category_dividers[i][j-1] << "," << category_points[i][j] << ",+inf)";
			}
			else
			{
				cout << "(" << category_dividers[i][j-1] << "," << category_points[i][j] << "," << category_dividers[i][j] << ") ";
				data_output << "(" << category_dividers[i][j-1] << "," << category_points[i][j] << "," << category_dividers[i][j] << ") ";
			}
		}

		cout << "\n";
		data_output << "\n";
	}
	cout << "\n";
	data_output << "\n";

	// MAIN: Fill the array with counts of each possible assignment:
	double* array_entries = new double[index_step_sizes[0]];
	for (int i = 0; i < index_step_sizes[0]; i++)
	{
		array_entries[i] = 0.0;
	}
	int* curr_assignment = new int[num_of_variables];
	for (int i = 0; i < num_of_data_points; i++) // For each data point, pigeon-hole it into the correct assignment.
	{ // MAIN
		// Get the current assignment:
		int array_index = 0;
		for (int j = 0; j < num_of_variables; j++)
		{
			double the_data = data_points[i][j];
			curr_assignment[j] = 0;
			while ((curr_assignment[j] < var_domain_sizes[j]-1) && (the_data >= category_dividers[j][curr_assignment[j]]))
			{
				curr_assignment[j]++;
				array_index += index_step_sizes[j+1];
			}
		}

		// Add 1 to the count at "array_index":
		array_entries[array_index] += 1.0;
	}
	delete[] curr_assignment;
	
	// MAIN: Build the initial array:
	multi_array the_prime_array(num_of_variables,var_names,var_domain_sizes,array_entries);

	the_prime_array.scale_normalize();

#else

	ofstream data_output("Input and output files/output_data_1.txt");
	multi_array the_prime_array;

	bool use_old_flag = true;
	//cout << "Use existing distribution? 0/1: ";
	//cin >> use_old_flag;
	if (!use_old_flag)
	{
		int num_of_variables = 4;
		string* var_names = new string[num_of_variables];
		var_names[0] = "A";
		var_names[1] = "B";
		var_names[2] = "C";
		var_names[3] = "D";
		int* var_domain_sizes = new int[num_of_variables];
		var_domain_sizes[0] = 3;
		var_domain_sizes[1] = 2;
		var_domain_sizes[2] = 3;
		var_domain_sizes[3] = 2;
		int* index_step_sizes = new int[num_of_variables+1];
		index_step_sizes[num_of_variables] = 1;
		for (int i = num_of_variables-1; i >= 0; i--)
		{
			index_step_sizes[i] = index_step_sizes[i+1]*var_domain_sizes[i];
		}
		double* array_entries = new double[index_step_sizes[0]];
		for (int i = 0; i < index_step_sizes[0]; i++)
		{
			array_entries[i] = (double) rand();
		}


		the_prime_array = multi_array(num_of_variables,var_names,var_domain_sizes,array_entries);
		the_prime_array.scale_normalize();

		ofstream array_output("Input and output files/true_probability_array_buffer.txt");
		the_prime_array.print(array_output);
	}
	else
	{
		ifstream array_input("Input and output files/true_probability_array_buffer.txt");
		the_prime_array = multi_array(array_input);
	}


#endif

	// MAIN: Universal commands:
	the_prime_array.scale_normalize();
	cout << the_prime_array.formatted_print() << '\n';
	data_output << the_prime_array.formatted_print() << '\n';
	data_output << the_prime_array.LaTeX_print() << '\n';
	
	// MAIN: Generate the complete spectrum of correlation terms:
	multi_array** the_correlation_terms = the_prime_array.generate_correlation_terms(-1.0);
	cout << "\nThe correlation terms are:\n";
	data_output << "The correlation terms are:\n";
	for (int i = 0; i < pow(2,the_prime_array.get_num_of_variables()); i++)
	{ // MAIN
		if (the_correlation_terms[i] != NULL)
		{
			cout << the_correlation_terms[i]->formatted_print() << '\n';
			data_output << the_correlation_terms[i]->formatted_print() << '\n';
			data_output << the_correlation_terms[i]->LaTeX_print() << '\n';
		}
		else
		{
			cout << "NULL\n\n";
			data_output << "NULL\n\n";
		}
	}

	// Tests for different values of epsilon:
	int num_of_epsilons = 9;
	double epsilon_values[] = {-1.0, 0.0001, 0.001, 0.01, 0.02, 0.03, 0.04, 0.05, 0.1};
	multi_array*** the_correlation_term_sets = new multi_array**[num_of_epsilons];
	for (int i = 0; i < num_of_epsilons; i++)
	{
		cout << "For epsilon: " << epsilon_values[i] << "\n";
		data_output << "For epsilon: " << epsilon_values[i] << "\n";

		the_correlation_term_sets[i] = the_prime_array.generate_correlation_terms(epsilon_values[i]);
		multi_array the_prime_array_restored = multi_array::generate_probability_distribution(
			the_prime_array.get_num_of_variables(),
			the_prime_array.get_var_names(),
			the_prime_array.get_var_dom_sizes(), 
			(int) pow(2,the_prime_array.get_num_of_variables()),
			the_correlation_term_sets[i]);
		//cout << "The restored distribution is:\n" << the_prime_array_restored.formatted_print() << "\n\n";
		//data_output << "The restored distribution is:\n" << the_prime_array_restored.formatted_print() << "\n\n";

		//multi_array diff = the_prime_array_restored - the_prime_array;
		//cout << "The difference between the restored and original probability distribution is:\n" << diff.formatted_print() << "\n\n";
		//data_output << "The difference between the restored and original probability distribution is:\n" << diff.formatted_print() << "\n\n";

		cout << "\nThe correlation term inclusion flags are: ";
		data_output << "\nThe correlation term inclusion flags are: ";
		for (int j = 1; j < pow(2,the_prime_array.get_num_of_variables()); j++)
		{ // MAIN
			cout << (the_correlation_term_sets[i][j] != NULL);
			data_output << (the_correlation_term_sets[i][j] != NULL);
		}
		cout << '\n';
		data_output << '\n';

		/*cout << "The KL-divergences are: " << multi_array::KL_distance(the_prime_array,the_prime_array_restored) 
			<< ' ' << multi_array::KL_distance(the_prime_array_restored,the_prime_array) << '\n';
		data_output << "The KL-divergences are: " << multi_array::KL_distance(the_prime_array,the_prime_array_restored) 
			<< ' ' << multi_array::KL_distance(the_prime_array_restored,the_prime_array) << '\n';*/
		cout << "The L1 distance is: " << multi_array::L1_distance(the_prime_array,the_prime_array_restored) << '\n';
		data_output << "The L1-distance is: " << multi_array::L1_distance(the_prime_array,the_prime_array_restored) << '\n';


		// Probabilistic inference example:
		multi_array posterior_probability_distribution(the_prime_array_restored);
		bool marg_flags[4] = {false, true, true, false};
		posterior_probability_distribution.marginalize(marg_flags);
		int var_values[4] = {1, -1, -1, -1};
		posterior_probability_distribution.condition(var_values);
		posterior_probability_distribution.scale_normalize();
		cout << "\nThe probabilistic inference results are:\n"
			<< posterior_probability_distribution.formatted_print() << "\n\n";
		data_output << "\nThe probabilistic inference results are:\n"
			<< posterior_probability_distribution.formatted_print() << "\n\n";
	}

	data_output.close();

#endif


	system("PAUSE");
	return EXIT_SUCCESS;
}





