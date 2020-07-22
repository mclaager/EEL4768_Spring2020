#include <iostream>
#include <string>
#include <cmath>
#include <fstream>

using namespace std;

void err (string str)
{
	cout << "Error: " << str << endl;
	exit(1);
}


// Performs G-Share on bits to store Global Buffer Table index (M),
// Global History Buffer size (N), and the trace filename.
// Assumes two-bit predictors.
void gshare (int m, int n, string filename)
{
	int i;
	unsigned int indx;

	int ghb = 0;
	int gbt_size = (int)(pow(2,m));
	int *gbt;
	bool *gbt_marked;

	string tmppc;
	long long unsigned int pc;
	// Creates a mask m bits from the right
	int mask = (~0) & (int)(pow(2,m)-1);
	char true_result;
	char pred_result;
	int num_correct = 0;
	int num_incorrect = 0;

	// Initializes GBT
	gbt = new int[gbt_size];
	gbt_marked = new bool[gbt_size];
	if (gbt == NULL or gbt_marked == NULL)
		throw "Memory could not be allocated for GBT.";
	for (i = 0; i < gbt_size; i++)
		gbt[i] = 2;

	// Opens trace file
	ifstream trace(filename);
	if (!trace.is_open())
		throw "File could not be opened.";

	// Performs G-Share on trace file
	while (true)
	{
		trace >> tmppc >> true_result;
		// Tests for EOF
		if (trace.eof())
			break;
		pc = stoull(tmppc, 0, 16);

		// Moves ghb to left, PC 2 to the right, XOR and then applies mask
		indx = ( (ghb << (m-n)) ^ (int)(pc >> 2) ) & mask; 

		if (gbt[indx] >= 2)
			pred_result = 'T';
		else
			pred_result = 'N';

		if (pred_result == true_result)
			num_correct++;
		else
			num_incorrect++;

		// Alters GHB and value at the index of GBT depending on the actual outcome
		if (true_result == 'T')
		{
			if (gbt[indx] < 3)
				gbt[indx] += 1;
			ghb = (ghb >> 1) + (int)(pow(2,n-1));
		}
		else
		{
			if (gbt[indx] > 0)
				gbt[indx] -= 1;
			ghb = ghb >> 1;
		}

		// Marks the index
		gbt_marked[indx] = true;
	}

	// Prints out the ratio of correct predictions
	cout << "Correct Branch Prediction Ratio: " << num_correct << "/" <<  num_correct + num_incorrect << endl << endl;

	// Prints out all GBT values that aren't the initial
	cout << "GBT Status (Only Non-Initial Entries)" << endl;
	for (i = 0; i < gbt_size; i++)
	{
		if (gbt_marked[i])
			cout << "GBT[" << i << "] = " << gbt[i] << endl;
	}

	trace.close();
	return;
}

// M (log2(GBT_SIZE)), N (GHB), Input File
int main (int argc, char **argv)
{
	int m, n;
	string filename;

	if (argc < 4)
		err("Not enough arguments.");

	// Gets command-line arguments
	m = atoi(argv[1]);
	n = atoi(argv[2]);
	filename = argv[3];

	// Runs the gshare algorithm
	try
	{
		gshare(m, n, filename);
	}
	catch (const char *err_str)
	{
		err(err_str);
	}

}
