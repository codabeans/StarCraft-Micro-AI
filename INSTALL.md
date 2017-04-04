# Installation

## Dependencies

Install the required libraries for the bwapi

```bash
sudo apt install libsdl2-dev libsdl2-image-dev
```

## Setting up

Clone the BWAPI github repository somewhere on your system

```bash
git clone https://github.com/bwapi/bwapi.git
```

Edit the Makefile to point to the directory that you cloned BWAPI

```bash
BWAPI=/where_you_cloned_to/bwapi
```

If your BWAPI directory doesn't contain the file svnrev.h, you need to generate it using the vbs script in that dir
   (note: this may have to be done in windows, but you can just copy the generated .h files to Linux and it will work)

## Compiling

```bash
make
```

To speed up compilation time, pass the flag -jX where X is your number of cores.

## Usage

The executable will be in bin/, so navigate to that directory, and run

```bash
./StarCraft-Micro-AI ../sample_experiment/sample_exp.txt
```

To change the setting of the battle, explore sample_experiment/sample_exp.txt and pick your bots, type of units,
number of units, and even make custom maps!

For GPU acceleration
```bash
Player 1 DeepQ true
```

Enjoy!
