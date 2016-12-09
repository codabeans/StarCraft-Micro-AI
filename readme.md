# Deepcraft

This project is a clone of David Churhill's Sparcraft with the addition of one extra bot, DeepQ. 
As the name suggests, this bot utilizes deep q learning to learn how to play. I am in the process
of training one scenario, and once it is done, then I will upload the .caffemodel for all to enjoy.
Down the line, I would like to have one model capable of doing matches X units vs Y units for any 
unit type. 

## Compiling

Install the required libraries

```bash
sudo apt install libsdl2-dev libsdl2-image-dev
```

Clone the BWAPI github repository somewhere on your system

```bash
git clone https://github.com/bwapi/bwapi.git
```

Edit the Makefile to point to the directory that you cloned BWAPI

```bash
BWAPI=/where_you_cloned_to/bwapi/bwapi'  (yes, bwapi/bwapi)
```

Edit the Makefile to point to the directory that you have Caffe compiled

```bash
CAFFE=/where_you_cloned_to/caffe' 
```

If your BWAPI directory doesn't contain the file svnrev.h, you need to generate it using the vbs script in that dir
   (note: this may have to be done in windows, but you can just copy the generated .h files to Linux and it will work)

Build the project
	
```bash
make all
```

## Usage

The executable will be in bin/, so navigate to that directory, and run 

```bash
./SparCraft ../sample_experiment/sample_exp.txt
```

To change the setting of the battle, explore sample_experiment/sample_exp.txt and pick your bots, type of units,
number of units, and even make custom maps!

Enjoy!

## Credits

This wouldn't have been possible without David Churhill's Sparcraft.
