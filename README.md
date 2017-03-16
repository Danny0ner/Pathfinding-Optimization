Pathfinding is an important resource on many games, the basic algorithms like BFS, DJikstra or A* may work fine on some games, but in some cases, especially on large maps, they can be slow and make your game stops for a moment until it finds the path or slow the game.
That makes its optimization so important, because you want your program or game to be as fast as possible for possible future expansions of the same.

In this document you will find, a quick explanation of A*, some little optimizations for it, an explanation of the most used optimizations derived from the A* algorithm and a guide to implement the HPA algorithm.

# Reminding A* algorithm

The A* algorithm aims to find a path from a single start to a single destination node. The algorithm calculate your path using a simple algorithm of how much far is the destination. Adding the already travelled and the estimated distances together, it expands the most promising paths first. As it checks almost every way to find the path by it distance, we can say that the path that it finds is almost always optimal.

![A-Star example](https://s15.postimg.org/v557wx4u3/image.png)
 
As we can see in the image, starting at the white center square, the A* searches the squares around him and follows the nearest square to the destination. When it finds the destination, it returns the path following the parent of every square, being the parent the square where they have been expanded from.

This is what we do when we implement the A* algorithm:

1. Find node closest to your position and declare it start node and put it on the open list. 
2. While there are nodes in the open list:
  3. Pick the node from the open list having the smallest F score. Put it on 
      the closed list (you don't want to consider it again).
   4. For each neighbour (adjacent cell) which isn't in the closed list:
     5. Set its parent to current node.
     6. Calculate G score (distance from starting node to this neighbour) and 
         add it to the open list
     7. Calculate F score by adding heuristics to the G value.

# Basic Pathfinding Optimizations
We have so many ways to optimize pathfinding making little changes to it. They’re changes that will make it faster, but not as fast as concept optimizations, which will be explained later.
Also, some of this methods can not return the optimal search in some situations, but they’re a good implementation if your map is a spaced map without a lot of obstacles.
If we want to make our pathfinding faster without having to do big changes to our algorithm we can:
•	Use unordered lists. One of the things that we have to do every loop is to find the node with lowest score. Using unordered lists we won’t need to search that node so it will be on the first position. 

•	Using maps or heaps instead of list will also accelerate you algorithm. This data structures are faster while iterating so it will be so helpful when you need to search a node. You can use unordered maps and heaps so the first advice is also useful here.

•	Parting Pathfinding. This means that your units don’t need the complete path in one frame because they won’t go through it in that frame. You can set a limit of loops to the pathfinding so if it don’t find the goal and reaches it, you save the last node it has closed and you continue your path next frame since there. This method can cause errors if your pathfinding loop limit is too low, because it could be searching on a large closed zone if it doesn’t find the way out. But if you are sure that your map doesn’t take that risks and your loop limit is an acceptable number, it will help your pathfinding a lot.

•	Variables is_open or is_close in nodes. You can create a variable for your nodes that is true if the node is on the open or the closed list. Having this, when you need to search for a node in a list, instead of looking at its position on the whole list, you can access it and see if it is on that list only looking to that variable. That reduces your search on the list open, in a map of NxN tiles, by open(N) to open(1).

•	Performing a lower level pathfinding before the real one. You can precompute a map of separated nodes, and when you need to find a path, use that map first. Using this, you’ll get a path of separated nodes to the destination. When you have this path, you can start to use pathfinding from node to node in separated frames, in a similar way that parting pathfinding but without a limit of loops, you will path to the first low level node of the path and then wait for the next frame to continue the path to the next node until you find the destination.






# Conceptual Pathfinding Optimizations
We call conceptual optimizations to optimizations that change the ways that pathfinding search nodes. Every little change on the way the pathfinding search for nodes can optimize your pathfinding a lot based on your map distribution or your way to move. Be aware that some of the conceptual optimizations for pathfinding will work better or worse depending on how much your map changes or if it is static or depending on your obstacles’ positioning.
There are some good conceptual optimizations but we will talk about two of them: the HPA and the JPS.
## HPA
Hierarchical pathfinding is based on pre-processing a lower level pathfinding to the destination, that lower level will be spared in clusters, which its distances and optimal paths to each other are precomputed. 
Once we did the lower level pathfinding, we will cross each cluster using its precomputed optimal path and obtaining a quick path to the destination.

![HPA1](https://s18.postimg.org/7yekww93t/HPA1.png)

As we can see at the image, the grid is divided on clusters, and this clusters has exits that connects them to other clusters. Once we connect them, we will have something like in the right image, where we can see the costs of going to one edge of the cluster to another one. That precalculated costs are so helpful in a way to pick the optimal path.

![HPA2](https://s18.postimg.org/3qjso57o9/HPA2.jpg)
   
Now, let’s see an example where we’re in S and we want to go to G. The algorithm is simple, we just do a normal a* with clusters, once we have the path, we use the costs to go from one cluster to the next one as fast as possible.

Remember that you can create your own grid with your own clusters, it sounds obvious but you can create a grid based on your map’s distribution. With an own grid, you could make some clusters bigger that others to fit rooms or other zones of the map that would prevent you from doing a lower level pathfinding in a room if you can fit a cluster in that room instead of having 2, 3 or more clusters on that room.

### Pros and cons:
Every optimization works better in certain circumstances than others, having HPA to precalculate distances and paths won’t be optimal in games where your map is always being updated and changing will make your HPA recalculate again paths and distances which will take some time if the changes are considerable.
The fact that you can create your own grid is so god so it can help you to optimize concurred zones of your map.
HPA is not the best option for spaced maps where you don’t have much obstacles. This don’t mean that it works worse it just mean that other optimizations like JPS wins in spaced areas. 







## JPS

The base of the Jump Point Search algorithm works on expanding the start node in cardinals to reach zones where it can’t continue expanding. We will explain it now, but if you think about it is pretty simple, why would you add nodes to a list if you can access that position with another node and have less nodes to iterate?

As a difference with HPA, with JPS you don’t need to precalculate nothing, it just spends so much less time iterating open and close list.
We need to know, that JPS exploits the regularity of the grids, this means that if we have tiles or zones of the map with different costs, JPS will take them as a uniform cost grid, because it doesn’t look all nodes so it won’t be relevant.
Also, by exploiting the regularity, there is no need to expand in every direction from every tile, and have a lot of nodes being iterated on the open and closed lists with every cell like A* does. It is sufficient to only scan the tiles to check if there is anything 'interesting' nearby, what we call a jump point.
Now we will process to explain the scanning process, basically how we expand the nodes and how we find jump points.

Before going to the Algorithms of JPS, let’s talk about its base, the neighbour pruning:

![JPS1](https://s18.postimg.org/4qeoxlc7t/JPS1.png)
 
The node x is being expanded by its parent, which position is referenced with the arrow.
As we can see, all the nodes can be checked doing the search in horizontal or diagonal, as the arrow indicates, without having to pass for the node x. This means that we can check all this nodes with the parent, so x is not necessary.
Now let’s talk about what is a forced neighbour:

![JPS2](https://s18.postimg.org/f1r1q93x5/JPS2.png)
 
Forced neighbour are nodes that cannot be expanded from the node X, as we see in the image, if we follow the arrow on grey tiles, we can’t reach the highlighted nodes. Nodes which we can’t reach from another one are called forced neighbours.
Remember that always that a node that is being expanded is next to an obstacle the tile behind it will be a forced neighbour.
### JPS Algorithm explanation

![JPS3](https://s16.postimg.org/bhlyju5h1/JPS3.jpg)

We start at p(x) and start to expand the node in horizontal. We find that the tile above p(x) is blocked, so we find the first forced neighbour, X. we stop expanding p(x) and start expanding x.
We expand horizontally X so it finds Y. Y has a blocked tile above it so we mark Y as a forced neighbour and the tile that we won’t be able to reach Z as a neighbour too.
There are 2 possibilities in this case. When we find that Y has a blocked tile above it, we only get Z as a forced neighbour and continue expanding Z, but if we also add Y we have a direct path, and we don’t have to do a pathfinding between X and Z, because that is what we’re trying not to do.
The algorithm continues this way all the time. You can put in order which search you do first. It is normal to do first horizontal search, then vertical, and then diagonal, that is a mesh of vertical and horizontal in every diagonal tile. 
 
![JPS4](https://s16.postimg.org/t8xkyakvp/JPS4.png) 
 
Let’s see this JPS using diagonal searching. We start at p(x) blocked tile next to it, the next thing we do is to making X a forced neighbour. We expand p(x) in vertical but we don’t find anything interesting so we pass to the next neighbour. We start expanding X, while we expand it we have the same problem as p(x) we have an obstacle down X so we add a forced neighbour, W, and we continue expanding X. If we continue expanding X in diagonal, we will find that when we’re expanding in Y, we find Z, the goal, so we add both as forced neighbour so we can get an easy path and we create the path because we found the goal.

### Pros and Cons:
As we told before, having different costs on grids won’t work on JPS so it you have a grid with different costs, this isn’t your optimal optimization.
As more spaced is your map, less jump points you’ll have and the iteration will be faster.
You don’t need to pre-process nothing and the map can change the way you want, it won’t affect the JPS. 


In the case you didn’t understand well how JPS works here you have a web to see A*, Djikstra, JPS and others working. Is a useful web to understand who they work while you see them doing the searches. [PathFinding.js](https://qiao.github.io/PathFinding.js/visual/)

# Pathfinding optimization implementation
Now, we will explain an easy form to optimize your pathfinding using a node map.
This method is based on getting out of closed list and not iterating open list creating a node map where we will check if the nodes have already been on the open or closed list.
We will start with the normal Pathfinding.
It’s the moment to set the map, now we will create the map of nodes with the size of the map.

![IMP1](https://s13.postimg.org/5tlzuuvef/imp1.png)

Now, with this array of nodes, we can access to every node in the map just by entering its position on the grid, which will be really useful while iterating.
One time we have the map of nodes, we start to work with the A* function.
The first thing we have to do is to fill the map of nodes with nodes out of the map, so they won’t be evaluated, for that we will use std::fill(first item, last item, Node);

![IMP2](https://s13.postimg.org/6keq0mxrr/imp2.png)

*Note: size is the width + height of the map.
Next thing we will do is to create a priority_queue for having our open list. As we said before, having a priority queue will make fast the iteration of the queue and we will always have the node with the lowest score at the top of the list so we don’t need to search for it.

![IMP3](https://s13.postimg.org/obqcf3d6f/imp3.png)

If you don’t know what compare is, it’s an easy struct that has an operator to compare the score of the nodes to organize them.

![IMP4](https://s13.postimg.org/3svga0z93/imp4.png)

The next step is to create a FirstNode pointer node which information will be saved in the map and push it to the open list. I use DistanceTo function that calculates heuristic distance.

![IMP5](https://s13.postimg.org/rxw5rqjjr/imp5.png)

Note* GetPathNode receives a position and returns a pointer of a Pathnode of the map of nodes.

![IMP6](https://s13.postimg.org/k6fft6fef/imp6.png)

At this moment we’re ready to start with the while loop. We will adapt it to use the pointer to the map of nodes and check if they have been already in the open or the closed list.

![IMP7](https://s13.postimg.org/4z41mkcxj/imp7.png)

We set current as the lowest score node in the map, and we do like if we add it to the closed list but we only set its bool on_close true.
Is time to find neighbours and see if they’re already been added to the lists or not.

![IMP8](https://s13.postimg.org/50dzfzerb/imp8.png)

Now we create a temp that points to the item node, that will give us a pointer to the map, and we check its bools to see if it has been settled like in a close list or in the open list. Doing this we get rid of the biggest problem that A* has, iterating lists to find id the node is there.
The rest continues like a normal A*, we just have to set its parent to the current node if we found a shorter path to that node.
*Note: CalculateFopt is a function that calculates g and h and sets it depending if the distance is diagonal or straight.
The last thing we have to do is, when we find the destination, to backtrack the nodes until we find the first one.

![IMP9](https://s21.postimg.org/dm30i0a3b/imp9.png)

We can do this easy by doing a pushback to the position, and updating that position to be its parent until it has no parent.
And we finished optimizing the pathfinding using a map of nodes instead of lists!!

##Conclusions of the optimization
I’ve been trying making the hardest path I could on a 120x120 path and the optimized path did take a max of 20 ms while the normal A* did take ~200-600 ms

You can download the project in github (link at the top), and try yourself!!

# References:

[https://www.gamedev.net/resources/_/technical/artificial-intelligence/jump-point-search-fast-a-pathfinding-for-uniform-cost-grids-r4220](https://www.gamedev.net/resources/_/technical/artificial-intelligence/jump-point-search-fast-a-pathfinding-for-uniform-cost-grids-r4220)

[http://www.gameaipro.com/GameAIPro/GameAIPro_Chapter17_Pathfinding_Architecture_Optimizations.pdf](http://www.gameaipro.com/GameAIPro/GameAIPro_Chapter17_Pathfinding_Architecture_Optimizations.pdf)

[http://www.intelligence.tuc.gr/~robots/ARCHIVE/2015w/Projects/LAB51326924/jps.html](http://www.intelligence.tuc.gr/~robots/ARCHIVE/2015w/Projects/LAB51326924/jps.html)

[https://gamedevelopment.tutsplus.com/tutorials/how-to-speed-up-a-pathfinding-with-the-jump-point-search-algorithm--gamedev-5818](https://gamedevelopment.tutsplus.com/tutorials/how-to-speed-up-a-pathfinding-with-the-jump-point-search-algorithm--gamedev-5818)

[http://aigamedev.com/open/review/near-optimal-hierarchical-pathfinding/](http://aigamedev.com/open/review/near-optimal-hierarchical-pathfinding/)

[https://qiao.github.io/PathFinding.js/visual/](https://qiao.github.io/PathFinding.js/visual/)
