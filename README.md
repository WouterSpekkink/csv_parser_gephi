# Introduction
This program was built for personal use by Wouter Spekkink (wouter.spekkink@manchester.ac.uk) to convert csv-output from event sequence datasets into nodes and edges files that can be imported in Gephi, and that are more easily parsed by Neo4J csv-import functions. 

An appropriate input-file has the following characteristics:

*	It has a header that indicates different types of entities listed in the file.
*	The first column is a list of events.
*	The other columns are lists of entities linked to the events in the first column.
*	There may be multiple values within a cell (except for events), as long as these are separated with an appropriate delimiter. Make sure that this delimiter is different from the one used to separate the columns of the file.

The program requires the user to select an input file (csv), as well as the symbol that is used as column delimiter in the file. The user also has the option to select a second delimiter, in case the user has input multiple values in certain columns, and wants to separate these. After importing the file, a new nodes list and a new edges list can be written to the disk.
The purpose of this program is highly specific, but there may be things in the code that are useful for others. 

# Building the program
The program was built with C++, and it makes used of the C++ standard library, as well as the Qt4 library. Thus, to build the program, the Qt4 development library must be available on the machine. You should be able to build the platform on Linux, Windows, as well as OSX. To create a Makefile, the user should first run the following two commands:
*	qmake -project
*	qmake -makefile
	
Then the program can be compiled, using the generated makefile. I used gcc compiler for this, with the command:
*	make -k
	
# Basic user instructions
The program is very easy to use. Here are some basic instructions:

First click the *Open File* button to select the file you want to use as input. The program makes a few assumptions about this file:
*	It has a header that indicates different types of entities listed in the file.
*	The first column is a list of events.
*	The other columns are lists of entities linked to the events in the first column.
	
It is allowed to multiple values in one cell (except for multiple events), as long as the multiple values are separated by an appropriate delimiter. The delimiter for multi-value cells has to be different from the delimiter that is used to separate columns. The program won't allow you to use the same delimiter for both columns and multi-value cells (see below).

After selecting an input file, click *-Select a delimiter-* to select the column delimiter used in the file. If your file has multi-value cells, then check the box *Separators within columns* to enabled the combobox *-Select a second delimiter-* and then proceed to select the delimiter that you used. You will notice that the program forces you to select a delimiter that is different from the other one.

Then you can import the file by clicking the *Import* button, which also activates the options to save a nodes file and an edges file. By clicking the *Save Nodes File* button, the user will open a save dialog for the nodes list, and by clicking the *Save Edges File* button, the user will open a save dialog for the edges list. 

The nodes list will list all entities (including events) that are included in the input file provided by the user, and it will indicate their types based on the column that they were found in. The edges list shows to which events the entities are linked. 

These two files can be imported directly into [Gephi](http://www.gephi.org) from the data laboratory.

# Contact details
For any comments, contact me at wouter.spekkink@manchester.ac.uk or wouterspekkink@gmail.com

