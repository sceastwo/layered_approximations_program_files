

#include "data_quantization.h"


void data_quantize(int num_of_data_points, const double* the_data, int num_of_categories, 
				   double*& category_dividers, double*& category_points)
{
	// This function will work via the process of "Histogram normalization". 

	if (num_of_data_points <= 0)
	{
		throw "data_quantize: there must be at least 1 data point.";
	}
	if (num_of_categories <= 0)
	{
		throw "data_quantize: there must be at least 1 category.";
	}
	if (num_of_categories > num_of_data_points)
	{
		throw "data_quantize: there are not enough data points for the requested number of categories.";
	}

	category_dividers = new double[num_of_categories-1];
	category_points = new double[num_of_categories];

	// Sort the list of data points, preserving duplicates:
	double* sorted_data_points = new double[num_of_data_points];
	bool* used_flags = new bool[num_of_data_points];
	for (int i = 0; i < num_of_data_points; i++)
	{ // DATA QUANTIZE 
		used_flags[i] = false;
	}
	for (int i = 0; i < num_of_data_points; i++)
	{ // DATA QUANTIZE
		int min_element_index = -1;
		for (int j = 0; j < num_of_data_points; j++)
		{
			if (!used_flags[j])
			{
				if (min_element_index == -1)
				{
					min_element_index = j;
				}
				else if (the_data[j] < the_data[min_element_index])
				{
					min_element_index = j;
				}
			}
		}

		sorted_data_points[i] = the_data[min_element_index];
		used_flags[min_element_index] = true;
	}

	int min_points_per_category = num_of_data_points / num_of_categories;
	int remaining_points = num_of_data_points - min_points_per_category*num_of_categories;

	// Generate the category separators:
	for (int i = 0; i < remaining_points; i++)
	{ // DATA QUANTIZE 
		category_dividers[i] = (sorted_data_points[(min_points_per_category+1)*(i+1)-1]
			+ sorted_data_points[(min_points_per_category+1)*(i+1)]) / 2;
	}
	for (int i = remaining_points; i < num_of_categories-1; i++)
	{ // DATA QUANTIZE 
		category_dividers[i] = (sorted_data_points[remaining_points + min_points_per_category*(i+1) - 1] 
			+ sorted_data_points[remaining_points + min_points_per_category*(i+1)]) / 2;
	}

	// Generate the category points:
	for (int i = 0; i < remaining_points; i++)
	{ // DATA QUANTIZE 
		category_points[i] = 0.0; 
		for (int j = (min_points_per_category+1)*i; j < (min_points_per_category+1)*(i+1); j++)
		{
			category_points[i] += sorted_data_points[j];
		}
		category_points[i] /= (min_points_per_category + 1);
	}
	for (int i = remaining_points; i < num_of_categories; i++)
	{
		category_points[i] = 0.0; 
		for (int j = remaining_points + min_points_per_category*i; j < remaining_points + min_points_per_category*(i+1); j++)
		{
			category_points[i] += sorted_data_points[j];
		}
		category_points[i] /= min_points_per_category;
	}

	delete[] sorted_data_points;
	delete[] used_flags;

}


