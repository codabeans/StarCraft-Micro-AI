Compilation Instructions:

1. Install the required libraries

```bash
sudo apt install libsdl2-dev libsdl2-image-dev
```

2. Clone the BWAPI github repository somewhere on your system

```bash
git clone https://github.com/bwapi/bwapi.git
```

3. Edit the Makefile to point to the directory that you cloned BWAPI

```bash
BWAPI=/where_you_cloned_to/bwapi/bwapi'  (yes, bwapi/bwapi)
```

4. If your BWAPI directory doesn't contain the file svnrev.h, you need to generate it using the vbs script in that dir
   (note: this may have to be done in windows, but you can just copy the generated .h files to Linux and it will work)

5. Build the project
	
```bash
make all
```

6. The executable will be in bin/, so navigate to that directory, and run 

```bash
./SparCraft ../sample_experiment/sample_exp.txt
```

Enjoy!
