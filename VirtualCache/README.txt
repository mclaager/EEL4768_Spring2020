========================
Flexible Cache Simulator
========================


Compilation
-----------
	
	To properly compile the program in Linux (if 'SIM' does not already exist),
	you must either run the makefile OR perform the following command:
	
		g++ -o SIM main.cpp


Running the Simulator
---------------------

	The simulator takes in 5 command arguments, which are the following:
	
		./SIM <CACHE_SIZE> <ASSOC> <REPL> <WB> <TRACE_FILE>
		
	<CACHE_SIZE>is the size of the simulated cache in bytes
	<ASSOC> is the associativity
	<REPLACEMENT> replacement policy: 0 means LRU, 1 means FIFO
	<WB> Write-back policy: 0 means write-through, 1 means write-back
	<TRACE_FILE> trace file name with full path
	
Example
-------

		./SIM 32768 8 1 1 /home/TRACES/MCF.t
		
	This will simulate a 32KB write-back cache with 8-way associativity and FIFO replacement
	policy. The memory trace will be read from /home/TRACES/MCF.t