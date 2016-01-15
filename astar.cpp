#include <iostream>
#include <fstream>
#include <utility>
#include <list>
#include <algorithm>
#include <cmath>
#include <string>


//Indicates whether the tile is a wall or a floor. Path can't cross walls
enum TileType {FLOOR = 0, WALL = 1, INSPECTING = 2, CLOSED = 3, OPEN = 4};




//Point structure to indicate position within the map
struct Point{

	bool operator !=(const Point& rightArg){

	  return (x != rightArg.x || y != rightArg.y);
	
	}

	int x;
	int y;

	Point(int xCoord, int yCoord) : x(xCoord), y(yCoord) {};

};




//Node structure with an f, g, h value for A* search
struct Node{
	
	int f;
	int g;
	int h;
	Point position;
	Node* parent;
	TileType tile;
	bool isGoal;
	char status;


	Node(): f(0), g(1000), h(0), position(0, 0), parent(NULL), tile(FLOOR), isGoal(false), status(0) {};


};


void printMap(Node** map, int rows, int cols){

	std::cout<< std::string(100, '\n');

	for(int y = 0; y < rows; y++){
	  for(int x = 0; x < cols; x++){

    	switch(map[y][x].tile){

				case FLOOR:
					std::cout << " 0 ";
					break;

				case WALL:
					std::cout << " 1 ";
					break;

				case INSPECTING:
					std::cout << " * ";
					break;

				case CLOSED:
					std::cout << " X ";
					break;

				case OPEN:
					std::cout << " + ";
			}

			

		}

		std::cout << std::endl;
	}

	std::cin.ignore();


}





//#define TEST




int main(int argc, char* argv[] ){

	//Rows and Columns for the 2D Node array
	int rows(0);
	int cols(0);




	//File IO and Memory allocation
	std::ifstream fin;

	fin.open("map.txt");

	if(fin.fail()){

		std::cout<<"ERROR OPENING FILE"<<std::endl;

	}

	fin >> rows;
	fin >> cols;




	//Initialize array of nodes
	Node** map = new Node*[rows];


	for(int y = 0; y < rows; y++){

		map[y] = new Node[cols];


	}



	//User input for start and finish node
	int x(0);
	int y(0);

	std::cout<<"Input starting x and y: ";
	std::cin >> x >> y;

	Point start(x,y);

	std::cout<<"Input goal x and y: ";
	std::cin >> x >> y;

	Point goal(x,y);

	map[goal.x][goal.y].isGoal = true;





	//Read in the map from map.txt and set the coordinates for each node and precompute the h values
	for(int y = 0; y < rows; y++){
		for(int x = 0; x < cols; x++){

			char temp;

			fin >> temp; 

			if(temp - 48)
		  	map[y][x].tile = WALL;
			else
		  	map[y][x].tile = FLOOR;

			map[y][x].position.x = x;
			map[y][x].position.y = y;

			int xDiffGoal = abs(goal.x - map[y][x].position.x);
			int yDiffGoal = abs(goal.y - map[y][x].position.y);


			map[y][x].h = xDiffGoal + yDiffGoal; 

		}
	}




	



	//TEST//

	#ifdef TEST

	

	//Print the map
	printMap(map, rows, cols);

	#endif




	//Define lists containers of node pointers for open and closed nodes
	typedef std::list<Node*> NodeList;

	NodeList openList;
	NodeList closedList;



	// to indicate whether the goal has been found
	bool goalNotFound(true);




	//set the current node to the starting position
	Node* currentNode = &map[start.y][start.x]; 

	currentNode->g = 0;


	int count(0);

	while (goalNotFound){

		#ifdef TEST

		std::cout<< "Iteration" << std::endl;		
		count++;

		if (count > 1000)
			break;

		std::cout << "(" << currentNode->position.x << " ," << currentNode->position.y << ")"; 


		#endif



		//Put the current node on the closed list
		closedList.push_back(currentNode);
		currentNode->tile = CLOSED;





		//Start checking surrounding nodes
		for(int b = -1; b < 2; b++){
			for(int a = 1; a > -2; a--){




				//Skip the current node
				if(0 == a && 0 == b)
					continue;




				//X and Y coord of the current neighbor
				int neighborNodeX = currentNode->position.x + a;
				int neighborNodeY = currentNode->position.y + b;




	
				//Bounds check for neighboring node
				if(neighborNodeX < cols && neighborNodeX >= 0 &&
					neighborNodeY < rows && neighborNodeY >= 0){




					//Pointer to the current neighboring node
					Node* currentNeighborNode = &map[neighborNodeX][neighborNodeY];





					//Check that the current neighbor node is not on the closed list
					NodeList::iterator it = find(closedList.begin(), closedList.end(), currentNeighborNode);
					if(it == closedList.end()){


					


						//Ignore if tile is a wall
						if(currentNeighborNode->tile != WALL){




							//Check for goal node
							if(currentNeighborNode->isGoal){
								goalNotFound = false;
								


								#ifdef TEST

									std::cout<<"Goal found!"<<std::endl;

								#endif

								break;


							}




							//If the neighboring node is not on the open list then put it there.  
							//Otherwise check to see if the current node has a lower cost path
							int xDiffNeighbor = abs(currentNeighborNode->position.x - currentNode->position.x);
							int yDiffNeighbor = abs(currentNeighborNode->position.y - currentNode->position.y);



							//Put the neighbor node on the open list if it's not there already.
							NodeList::iterator it = find(openList.begin(), openList.end(), currentNeighborNode);

							if(openList.end() == it){

								openList.push_back(currentNeighborNode);
								currentNeighborNode->tile = OPEN;

								}



							//Check for lower g value
							int tempGValue = sqrt(pow(xDiffNeighbor*10, 2) + pow(yDiffNeighbor*10, 2)) + currentNode->g;



							//If the new g value is lower than the current g value then re-parent that node to the current node and set g, and f
							if(tempGValue < currentNeighborNode->g){

								
								currentNeighborNode->g = tempGValue;
								currentNeighborNode->f = currentNeighborNode->h + currentNeighborNode->g;

								currentNeighborNode->parent = currentNode;

							}


						}

					}

				}


				//Refresh the onsceen view
				printMap(map, rows, cols);




			}//end for (a < 2)

		}// end for (b > -2)


		//Scan the open list for the node with the lowest f value and move to that node.

		int lowestF(1000);


		for(Node* tmp : openList){			

			if(tmp->f < lowestF){

				lowestF = tmp->f;
				currentNode = tmp;

			}
		}



		//Take the node that we've moved to off the open list
		openList.remove(currentNode);





	}//end while (goalNotFound)


	std::cout << "Path from goal to start" << std::endl << std::endl;

	//Move back through the parents to output the path the goal
	for(Node* n = currentNode; n->position != start; n = n->parent){

		std::cout << "(" << n->position.x << ", " << n->position.y << ")" << std::endl;
		


	}




	//Free dynamic memory
	for(int y = 0; y < rows; y++){

		delete[] map[y];
	}

	delete[] map;



	return 0;
}
