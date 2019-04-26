
On line 453 of the file "main.cpp", set "use_old_flag" to false to generate a fresh joint probability distribution. The number of variables, their names and domain sizes can be set inside the if-block starting at line 457. Set "use_old_flag" to true to reload a previously generated probability distribution from the file "Input and output files/true_probability_array_buffer.txt". 

The output is printed to both the screen and to the file "Input and output files/output_data_1.txt". In addition, a string that can be copied into a LaTeX file to generate a table with the formatting already handled is printed into the output file. This string can then be copied into a LaTeX source file. The format of the output is:

<the total probability distribution>

<the total list of correlation terms> 

For each epsilon = -1.0, 0.0001, 0.001, 0.01, 0.02, 0.03, 0.04, 0.05, 0.1;

	<the current value of epsilon> 

	<the correlation term inclusion flags> 

	<the L1 distance from the true probability distribution> 

	<the result of probabilistic inference on the recovered probability distribution> 

end loop



