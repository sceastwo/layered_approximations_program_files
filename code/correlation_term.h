


#ifndef __CORRELATION_TERM_H__
#define __CORRELATION_TERM_H__

#include <math.h>

#include "multi_array.h"

class correlation_term : public multi_array
{

public:
	correlation_term();
	correlation_term(const correlation_term& existing_term);
	correlation_term(const correlation_term* existing_term);
	correlation_term& operator=(const correlation_term& existing_term);
	correlation_term(int input_num_of_variables, string* input_var_names, int* input_var_domain_sizes, double* input_array_entries);
	correlation_term(istream& input); 
	void print(ostream& output);
	~correlation_term();

	void normalize();
};

#endif

