# Visual-Nonogram-Solver
A Nonogram solver with visuals and a multithreaded solver which permits the user to see how the solving algorithms works

## NonoGram

Nonograms, also known as Hanjie, Paint by Numbers, Picross, Griddlers, Pic-a-Pix, and various other names, are picture logic puzzles in which cells in a grid must be colored or left blank according to numbers at the side of the grid to reveal a hidden picture. In this puzzle type, the numbers are a form of discrete tomography that measures how many unbroken lines of filled-in squares there are in any given row or column. For example, a clue of "4 8 3" would mean there are sets of four, eight, and three filled squares, in that order, with at least one blank square between successive sets.

![10x10 nonogram example](http://1.bp.blogspot.com/-XucdN4XEdys/V0uzHm-R7lI/AAAAAAAAAJ8/gQdSWv9bnjcrMmq0d7bb5NlaUKjXP4PjgCK4B/s1600/19%2B%253F.PNG)

## Recursive Backtracking
The first implemented solving algorithm is called [Recursive Backtracking](https://algodaily.com/lessons/recursive-backtracking-for-combinatorial-path-finding-and-sudoku-solver-algorithms).

To understand Recursive Backtracking in a nonogram puzzle you can imagine a binary tree where the first level of nodes is the first square and the levels beneath are the subsequent squares.

![recursive backtracking 1](https://user-images.githubusercontent.com/68373215/148685623-16f2b988-17ea-4b51-ab1d-7e8f52d3d576.png)

The algorithm will go over follow each path until it reaches either a non valid square that doesn't work with the puzzle or the end, which finishes the algorithm.
Every time a new square is filled in or left empty, we check the hints on the left and top sides to check if there is still a possible solution. Once it finds a square that doesn't fit, it will 'prune' the remaining possible paths following that square. Then it will do the same with the other value (filled/empty) and if that one also doesn't fit it means that one of the previous squares must be wrong and the one before it must change. 

![recursive backtracking 2](https://user-images.githubusercontent.com/68373215/148685627-dbd2dfe1-a70a-444a-81c6-27a78f42e869.png)

## Filling in free squares at the start

Before we start using an algorithm we can check if there are any squares in the puzzle that will always be either filled or empty.

My implementation uses 3 of these methods:

1. It will mark all squares in an empty row/columns as _impossible_ squares. Which means that they must always be empty.

2. If you look at all possible combinations of a row/column you can take the intersection of those combinations and receive a list of squares that will always be filled.

![FreeSquares2](https://user-images.githubusercontent.com/68373215/148686484-0bebe769-5fe8-4773-b8b7-5204a6aefcb7.png)

3. If there is a filled in square somewhere and the distance between that square and a side is smaller than a first/last hint depending on the side, you can continue those squares like in the image below. If there is only 1 hint in that row/column you can mark the other squares as _impossible_ squares.

![FreeSquares3](https://user-images.githubusercontent.com/68373215/148687117-3d6c871e-8cd5-4a98-ba21-3189f06f3c3e.png)

