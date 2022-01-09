# Visual-Nonogram-Solver
A Nonogram solver with visuals and a multithreaded solver which permits the user to see how the solving algorithms works

## NonoGram

Nonograms, also known as Hanjie, Paint by Numbers, Picross, Griddlers, Pic-a-Pix, and various other names, are picture logic puzzles in which cells in a grid must be colored or left blank according to numbers at the side of the grid to reveal a hidden picture. In this puzzle type, the numbers are a form of discrete tomography that measures how many unbroken lines of filled-in squares there are in any given row or column. For example, a clue of "4 8 3" would mean there are sets of four, eight, and three filled squares, in that order, with at least one blank square between successive sets.

![10x10 nonogram example](http://1.bp.blogspot.com/-XucdN4XEdys/V0uzHm-R7lI/AAAAAAAAAJ8/gQdSWv9bnjcrMmq0d7bb5NlaUKjXP4PjgCK4B/s1600/19%2B%253F.PNG)

## Recursive Backtracking
The first implemented solving algorithm is called [Recursive Backtracking](https://algodaily.com/lessons/recursive-backtracking-for-combinatorial-path-finding-and-sudoku-solver-algorithms).

To understand Recursive Backtracking in a nonogram puzzle you can imagine a binary tree where the first level of nodes is the first square and the levels beneath are the subsequent squares.

![recursive backtracking 1](https://user-images.githubusercontent.com/68373215/148687196-deedc72e-cce3-410c-9c86-3b687a0b141e.png)

The algorithm will go over follow each path until it reaches either a non valid square that doesn't work with the puzzle or the end, which finishes the algorithm.
Every time a new square is filled in or left empty, we check the hints on the left and top sides to check if there is still a possible solution. Once it finds a square that doesn't fit, it will 'prune' the remaining possible paths following that square. Then it will do the same with the other value (filled/empty) and if that one also doesn't fit it means that one of the previous squares must be wrong and the one before it must change. 

![recursive backtracking 2](https://user-images.githubusercontent.com/68373215/148687185-8a85c4c4-fca3-42be-8d95-74b47de2a9ec.png)

#### Complexity

the time complexity of this algorithm is **O(2^n)**. The maximum possible steps this algorithm can take is the amount of possible combinations of a nonogram puzzle.

Nonograms are usually `n x n` grids where `n` is a multiple of 5.

 - 5x5 puzzle is 2^25 combinations = 33'554'432.
 - 10x10 puzzle is 1.267 x 10^30
 - 15x15 puzzle is 5.392 x 10^67

This algorithm's speed drastically changes depending if it can guess the first squares correctly. If it makes a mistake on the first square it will have to redo the whole process again. But when it guesses the first one correctly it throws out 1/2 of the possible path it can go to.

## Filling in free squares at the start

Before we start using an algorithm we can check if there are any squares in the puzzle that will always be either filled or empty.

My implementation uses 3 of these methods:

1. It will mark all squares in an empty row/columns as _impossible_ squares. Which means that they must always be empty.

2. If you look at all possible combinations of a row/column you can take the intersection of those combinations and receive a list of squares that will always be filled.

![FreeSquares2](https://user-images.githubusercontent.com/68373215/148687175-c658623c-1c36-4170-b92c-88c1a02229d2.png)

3. If there is a filled in square somewhere and the distance between that square and a side is smaller than a first/last hint depending on the side, you can continue those squares like in the image below. If there is only 1 hint in that row/column you can mark the other squares as _impossible_ squares.

![FreeSquares3](https://user-images.githubusercontent.com/68373215/148687117-3d6c871e-8cd5-4a98-ba21-3189f06f3c3e.png)

## Comparison

(note: it may seem that the animation suddenly starts and ends midway through the solving. But in reality it solved the first and end segment quickly and got stuck in the middle)

#### Recursive Backtracking

![RecursiveBacktracking](https://user-images.githubusercontent.com/68373215/148688477-45dac260-8999-4736-8f18-6a019d0b3a3d.gif)

#### Improved Recursive Backtracking (with free squares at start)

![ImprovedRecursiveBacktracking](https://user-images.githubusercontent.com/68373215/148688504-46f5762a-5348-4a99-bf8a-905744ef9974.gif)

Recursive backtracking and its improved versions are able to do 40x40 puzzles without much trouble but its usually impossible to do 45x45 puzzles as its complexity becomes too big at that point
