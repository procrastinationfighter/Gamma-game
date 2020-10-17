# The Gamma game
## Description
The game takes place on a rectangle-shaped board that consists of square fields. 
Two fields are adjacent to each other if they are in the same row or column of the board.
The game is designed for one or more players. Each player can take a field by placing their token on it. At the beginning of the game, all fields are untaken. 
A group of fields that belong to the same player and are adjacent to any other field from this group is called an area. 
Formally, if we consider an undirected graph with vertices that represent every field belonging to the player and edges between adjacent fields, 
every connected component is an area. A single field is also an area.

Players have an upper bound of owned areas. 
Every player can take a field if it is untaken and taking this field wouldn't increase number of areas owned by the player over given bound.

Once per game, every player can make a golden move. The golden move is a move on a taken field. 
A player takes a field that belongs to other player. However, golden move can't cause **any** player to have more areas than the upper bound of areas.

The game can be played in batch mode and interactive mode. 
In batch mode, the game is played by writing commands. 
In interactive mode, players take turns that consist of one move (including the golden move). The game ends when every player is unable to make a move.

More information about batch and interactive modes is in other sections.

## Installation
The game can be installed only on Linux. To install the game, download the repository, then change directory to your local version of the repository. 
Then write in command line:
```
mkdir game
cd game
cmake ..
make
```
You can also use commands `make test` to create a test of game's engine (example tests are shown in `gamma_test.c`, if you want to add your own tests, 
run cmake command with flag `TEST_FILE=<directory>`, where <directory> is a directory to your file with tests) and `make doc` to make documentation for the project.

## Documentation
If you already installed the game, in order to automatically generate documentation you have to change directory to your local version of the repository and type ```make doc```. The documentation is generated using Doxygen.
Unfortunately, the documentation is currently available only in Polish.

## Usage
To launch the game, use command ```./gamma``` in the directory you used command ```make```. 
