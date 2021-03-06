# Introduction
This program was built for personal use by Wouter Spekkink (wouter.spekkink@manchester.ac.uk) to convert csv-output from event sequence datasets into nodes and edges files that can be imported in Gephi, and that are more easily parsed by Neo4J csv-import functions. 

An appropriate input-file has the following characteristics:

*	It has a header that indicates different types of entities listed in the file.
*	There may be multiple values within a cell, as long as these are separated with an appropriate delimiter. Make sure that this delimiter is different from the one used to separate the columns of the file.

The program requires the user to select an input file (csv), as well as the symbol that is used as column delimiter in the file. The user also has the option to select a second delimiter, in case the user has put multiple values in certain columns, and wants to separate these. After importing the file, the program can be configured to export a nodes list and edge list (see basic user instructions).

The purpose of this program is highly specific, but there may be things in the code that are useful for others. 

# Downloading the programming
The program can be downloaded for free from [my website](http://www.wouterspekkink.org/?page_id=392). There is a version for Linux as well as for Windows. I do not plan to build a version of Mac OSX, but feel free to try yourself (see section on building the program yourself). There might be unexpected behaviour due to the different standard that Mac OSX uses for csv-files, although you should als be able to save csv-files in the format that is more commonly used.

# Building the program yourself
The program was built with C++, and it makes used of the C++ standard library, as well as the Qt4 library. Thus, to build the program, the Qt4 development library must be available on the machine. You should be able to build the platform on Linux, Windows, as well as OSX. What should be kept in mind is that I used C++11 functionality in the program, which requires the project file to be slight altered before building the program. First, you should generate the project file:
  * qmake -project
	
Then add the following line to the project file (csv-parser-gephi.pro) manually:
  * QMAKE_CXXFLAGS += -std=c++11
	
The user should also generate a makefile:
  * qmake -makefile
	
Then the program can be compiled, using the generated makefile. I used gcc compiler for this, with the command:
  * make -k
	
# Basic user instructions
The program is very easy to use. Here are some basic instructions:
 

First click the *Open File* button to select the file you want to use as input. The program makes a few assumptions about this file:
  * It has a header that indicates different types of entities listed in the file.
  * The first column is a list of events.
  * The other columns are lists of entities linked to the events in the first column.
	
It is allowed to multiple values in one cell (except for multiple events), as long as the multiple values are separated by an appropriate delimiter. The delimiter for multi-value cells has to be different from the delimiter that is used to separate columns. The program won't allow you to use the same delimiter for both columns and multi-value cells (see below).

After selecting an input file, click *-Select a delimiter-* to select the column delimiter used in the file. If your file has multi-value cells, then check the box *Separators within columns* to enabled the combobox *-Select a second delimiter-* and then proceed to select the delimiter that you used. You will notice that the program forces you to select a delimiter that is different from the other one.

Then you can import the file by clicking the *Import* button. This will active the next part of the interface, where you set up the source and target nodes. With *-Select Source Node-* the user can select the entity that is to serve as the source node, and with *-Select Target Node-* the user selects a target node. If valid options have been set, then the results can already be exported in the form of a nodes list and edges list, which can be imported in [Gephi](http://www.gephi.org via the data laboratory. However, it is also possible to first set properties. For this, the user clicks the *Set Properties* button, and then indicates the elements that are to serve as properties by checking the appropriate boxes.

Note that you can also select the same entity as a source node and a target node. This may be useful, for example, in the reconstruction of co-authorship networks from csv-files exported from databases such as Scopus or Web of Knowledge. The relationship is always set to undirected when the source and target node are the same.

Before exporting the data, the user can also set a few options that will affect the exported files. First, with the option *Exclude sources from nodes list*, the user can exclude the source nodes from the nodes list, and with the option *Exclude targets from nodes list*, the user can exclude target nodes from the nodes list. I added these options because I usually indicate linkages between events in a separate column (which I then select as the source column), while the events themselves are already in another column (which I select as the target column), and don't have to be included in the nodes list a second time. Second, the user can indicate that the *Relationships are undirected* or that the *Relationships are directed*. Third, the user can assign a label to the relationships by typing something in the field *Set label for relationship type*. Unless the field is empty, this label will be added as a variable to the exported edge list.

By clicking the *Save Nodes File* button, the user will open a save dialog for the nodes list, and by clicking the *Save Edges File* button, the user will open a save dialog for the edges list. 

# Example file
In the download folders (see section on Downloading the program) I also included an example file, called *Example_ESD.csv* (ESD is short for Event Sequence Dataset). It is based on a small subset of an event sequence dataset that I once built for my PhD project. It lists a number of incidents that have been grouped in events, together with a number of properties, such as descriptions, dates, as well as codes for actors that were involved in the events. The example file uses commas as its main delimiter, and it uses semicolons to separate multiple values in a single cell. After importing the file, you can export nodes and edges lists that represent different relationships between the entities included in the dataset. For example, by setting *Actors* as the source, and *Incidents* as the target, you can create files for a network that describes the involvement of actors in incidents. If you select *Incidents* as the source, and *Events* as the target, then you can create files for a network that describes which incidents are part of which events. 

# Contact details
For any comments, contact me at wouter.spekkink@manchester.ac.uk or wouterspekkink@gmail.com

