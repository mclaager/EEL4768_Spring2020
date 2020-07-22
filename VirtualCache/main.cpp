#include <iostream>
#include <string>
#include <fstream>

using namespace std;


void err (string str)
{
	cout << "Error: " << str << endl;
	exit(EXIT_FAILURE);
}

void virtual_cache (int cache_size, int block_size, int assoc, int repl_pol, int wb, string file_loc)
{
	int i, j;
	int val;
	int set;
	bool hit;

	// Display information
	int misses = 0;
	int hits = 0;
	int mem_reads = 0;
	int mem_writes = 0;
	double miss_ratio = 0.0;

	// Calculates the total number of sets
	int num_sets = cache_size/(assoc*block_size);

	// Cache allocation (and related values)
	string tmpadd;
	int lru_pos;
	char op;
	long long unsigned int address;
	int ** fifo = new int *[num_sets];
	int ** lru = new int *[num_sets];
	int ** db = new int *[num_sets];
	long long unsigned int ** tags = new long long unsigned int*[num_sets];
	if (db == NULL or lru == NULL or fifo == NULL or tags == NULL)
		throw "Cache Memory could not be allocated.";

	// Opens the trace file
	ifstream trace(file_loc);
	if (!trace.is_open())
		throw "File could not be opened.";

	// Allocates the rest of the cache 2D arrays
	for (i = 0; i < num_sets; i++)
	{
		db[i] = new int [assoc];
		lru[i] = new int [assoc];
		fifo[i] = new int [assoc];
		tags[i] = new long long unsigned int [assoc];
		if (db[i] == NULL or lru[i] == NULL or fifo[i] == NULL or tags[i] == NULL)
			throw "Cache Memory could not be allocated.";
	}

	// Runs the cache on the trace file
	while (!trace.eof())
	{
		// Gets the operations and the address
		trace >> op >> tmpadd;
		address = stoull(tmpadd, 0, 16);
		// Calculates the set that will be worked with
		set = (address / block_size) % num_sets;

		hit = false;
		// Tests for cache hits
		for (i = 0; i < assoc; i++)
		{
			if (address/block_size == tags[set][i])
			{
				hit = true;
				hits++;

				// Cache hits on write operations
				if (op == 'W')
				{
					// Write-through
					if (wb == 0)
						mem_writes++;
					// Write-back
					else if (wb == 1)
						db[set][i] = 1;
				}

				// Cache hit on LRU Replacement policy
				if (repl_pol == 0)
				{
					lru_pos = lru[set][i];
					for (j = 0; j < assoc; j++)
					{
						if (lru[set][j] <= lru_pos)
							lru[set][j]++;
					}
					lru[set][i] = 0;
				}

				// Fifo replacement policy... do nothing
				else if (repl_pol == 1);
			}
		}

		// Cache Miss
		if (!hit)
		{
			misses++;

			// A read from memory will occur regardless of operation
			mem_reads++;

			// Tests for miss on write-through policy
			if (op == 'W' && wb == 0)
				mem_writes++;

			// Cache miss on LRU Replacement policy
			if (repl_pol == 0)
			{
				// Gets the index of the max value (least recently used)
				lru_pos = 0;
				for (j = 0; j < assoc; j++)
				{
					if (lru[set][j] >= lru[set][lru_pos])
						lru_pos = j;
				}

				// Checks dirty bit to see if a memory write needs to occur
				// This only occurs on write-back policy
				if (db[set][lru_pos] == 1)
				{
					mem_writes++;
					db[set][lru_pos] = 0;
				}

				// Tests if to replace the dirty bit (a new write operation is taking place) 
				if (op == 'W' && wb == 1)
					db[set][lru_pos] = 1;

				// Replaces the cache tag with new tag
				tags[set][lru_pos] = address/block_size;

				// changes lru to match current state
				for (j = 0; j < assoc; j++)
					lru[set][j]++;
				lru[set][lru_pos] = 0;

			}

			else if (repl_pol == 1)
			{
				// Gets the position of the earliest element to be placed
				val = 0;
				for (j = 0; j < assoc; j++)
				{
					if (fifo[set][j] >= fifo[set][val])
						val = j;
				}

				if (db[set][val] == 1)
				{
					mem_writes++;
					db[set][val] = 0;
				}

				if (op == 'W' && wb == 1)
					db[set][val] = 1;

				tags[set][val] = address/block_size;

				// change fifo to match current state
				for (j = 0; j < assoc; j++)
					fifo[set][j]++;
				fifo[set][val] = 0;
			}

		}
	}


	miss_ratio = double(misses) / (hits + misses);

	cout << cache_size << "\t\t" << assoc << "\t";
	cout << (repl_pol == 0 ? "LRU" : "FIFO") << "\t\t";
	cout << (wb == 0 ? "Write-through" : "Write-back") << "\t";
	cout << file_loc << "\t" << miss_ratio << "\t";
	cout << mem_reads << "\t\t" << mem_writes << endl;
	trace.close();
	return;
}

int main (int argc, char ** argv)
{
	// Block size is a constant and is not affected by CL arguments
	int block_size = 64;
	int cache_size, assoc, repl_pol, wb;
	string file_loc;

	if (argc < 6)
		err("Not enough arguments.");

	// Sets lines to command args (no error checking, not industry worthy)
	cache_size = atoi(argv[1]);
	assoc = atoi(argv[2]);
	repl_pol = atoi(argv[3]);
	wb = atoi(argv[4]);
	file_loc = argv[5];

	try
	{
		cout << "Cache_Size\tAssoc\tReplacement\tWrite_Policy\tFile\t\t";
		cout <<	"Miss_Ratio\tMemory_Reads\tMemory_Writes" << endl;
		virtual_cache(cache_size, block_size, assoc, repl_pol, wb, file_loc);
	}
	catch (const char *e)
	{
		err(e);
	}

	return 0;
}
