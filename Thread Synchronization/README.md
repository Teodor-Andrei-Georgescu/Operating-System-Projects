This project used thread synchronization to control the creation of Kosmos-Ethynyl radicals. These radicals require two carbons and one hydrogen to be created. These atoms are being simulated through threads, with some threads being randomly created as "carbons" and "hydrogens." This program controls the threads so that whenever there are enough atoms, a reaction occurs, and it uses the earliest created atoms in each reaction.

There are two versions of this project: one done through mutexs (Kosmos-mcv) and one doone through semaphores (Kosmos-sem).

Before running the programs, the make file must be run first.

For all exact specifications and details, as well as the expected outcome, please read the PDF.
