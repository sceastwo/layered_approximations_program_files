

#include "correlation_term.h"


correlation_term::correlation_term() : multi_array()
{
}

correlation_term::correlation_term(const correlation_term& existing_term) : multi_array(existing_term)
{
}

correlation_term::correlation_term(const correlation_term* existing_term) : multi_array(existing_term)
{
}

/*correlation_term& correlation_term::operator=(const correlation_term& existing_term)
{
	return multi_array::operator=(existing_term);
}

	correlation_term(int input_num_of_variables, string* input_var_names, int* input_var_domain_sizes, double* input_array_entries);
	correlation_term(istream& input); 
	void print(ostream& output);
	~correlation_term();

	void normalize();*/

