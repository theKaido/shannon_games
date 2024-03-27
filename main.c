#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct link {

	char p1;
	char p2;
	int status; // 0 normal, 1 uncuttable, 2 cut
	struct link * suivant;
};
typedef struct link link_t;
typedef struct link * link_list;
// point structure, name, position in the grid, link to the other points
struct point {

	char name;
	int x;
	int y;
	struct point* suivant;
	link_list links;
};
typedef struct point point_t;
typedef struct point * point_list;



// get name of the configuration
char * getConfigName(int argc, char ** argv);


int getIntValue(char * s);
char getCharValue(char * s);
void getPositionValue(char * s, char * c, int * x, int * y);


point_list cons (char name, int x, int y, point_list l);
point_t * searchPoint(point_list l, char name);


void addLink(point_t * point, char p1, char p2);
link_list consLink (char p1, char p2, link_list l);
link_t * searchLink(link_list l, char name);

char getPointName(int x, int y, point_list list);
void printGrid(int gridSize, char painter, char cutter, point_list list);
int cutterPositionReachable(char painter, char cutter, link_list links, point_list list, int status, char done[100]);

void startGame(int gridSize, char painter, char cutter, point_list pointList);
void cutterTurn(int computer, point_list pointList);
void painterTurn(int computer, point_list pointList);
void getComputerMove(point_list pointList, char * p1, char * p2, int status);

// main with arguments, argc contains the number of arguments, (always 1 the name of the program), argv is a table of string, a string is defined by char *
int main(int argc, char** argv)
{

	char * name = "";

	// read config name in parameters
	name = getConfigName(argc, argv);

	// ensure that config name is valid, otherwise exit
	// can't compare string with == we have to use dedicated function
	if(strcmp(name, "") == 0)
	{
		printf("######### SHANNON GAME ############\n");
		printf("-> Fatal error\n");
		printf("-> Exit, invalid config name\n");
		printf("-> ./a.out config/config1.conf\n");
		return 0;
	}
	else
	{
		printf("######### SHANNON GAME ############\n");
		printf("-> Launch configuration : %s\n", name);
	}








	// read the configuration, warning if file is not existing, file is equal to NULL
	FILE * file = fopen(name, "r");

	if(file == NULL)
	{
		printf("######### SHANNON GAME ############\n");
		printf("-> Fatal error\n");
		printf("-> Exit, invalid config name\n");
		printf("-> ./a.out config/config1.conf\n");
		return 0;
	}
	









	// read the config file
	// declare a string to receive each line
	char buff[255];

	// get grid size
	fgets(buff, 255, file);
	if(buff == NULL)
	{
		printf("-> Fatal error\n");
		return 0;
	}
	int gridSize = getIntValue(buff);

	// get number of point
	fgets(buff, 255, file);
	if(buff == NULL)
	{
		printf("-> Fatal error\n");
		return 0;
	}
	int numberOfPoint = getIntValue(buff);


	// get starting point for painter
	fgets(buff, 255, file);
	if(buff == NULL)
	{
		printf("-> Fatal error\n");
		return 0;
	}
	char painter = getCharValue(buff);



	// get starting point for cutter
	fgets(buff, 255, file);
	if(buff == NULL)
	{
		printf("-> Fatal error\n");
		return 0;
	}
	char cutter = getCharValue(buff);

	printf("-> Grid size : %d\n", gridSize);
	printf("-> Number of point : %d\n", numberOfPoint);
	printf("-> Painter starting point : %c\n", painter);
	printf("-> Cutter starting point : %c\n", cutter);



	// at each node to the list
	point_list pointList = NULL;

	int i = 0;
	for(i = 0; i < numberOfPoint; i++)
	{
		char pointName = 'A';
		int x = 0;
		int y = 0;

		fgets(buff, 255, file);
		if(buff == NULL)
		{
			printf("-> Fatal error\n");
			return 0;
		}

		getPositionValue(buff, &pointName, &x, &y);
		printf("-> Point %c, Position %d:%d\n", pointName, x, y);


		// add point to the list
		pointList = cons(pointName, x, y, pointList);
	}


	// read each links
	while(fgets(buff, 255, file))
	{
		if(strcmp(buff, "") != 0)
		{
			// link of type A-B, p1 at position 0, p2 at position 2
			char p1 = buff[0];
			char p2 = buff[2];
			printf("--> Link between %c and %c\n", p1, p2);

			point_t * firstPoint = searchPoint(pointList, p1);
			addLink(firstPoint, p1, p2);
			point_t * secondPoint = searchPoint(pointList, p2);
			addLink(secondPoint, p1, p2);
		}
	}


	printf("-> Ok configuration loaded \n");

	printf("-> Points : \n");
	point_t * head = pointList;
	while(head != NULL)
	{
		printf("--> Point %c : [", head->name);

		link_t * headLink = head->links;
		while(headLink != NULL)
		{
			printf(" %c-%c ", headLink->p1, headLink->p2);
			headLink = headLink->suivant;
		}
		printf("]\n");
		head  = head->suivant;
	}
	//////////////////////////////////////////////////////////////////////////

	printf("-> Grid :\n");
	printGrid(gridSize, painter, cutter, pointList);

	point_t * painterPoints = searchPoint(pointList, painter);
	char done[100];
	done[0] = '\0';
	int exists =  cutterPositionReachable(painter, cutter, painterPoints->links, pointList, 0, done);
	
	// ensure that a solution exist between cutter and painter
	if(exists == 0)
	{
		printf("-> Fatal error, no solutions.\n");
		return 0;
	}


	startGame(gridSize, painter, cutter, pointList);

	return 0;
}

// get name of the configuration to launch, if one parameters is filled, argc must be equal to 2, argv[1] must contains the name
char * getConfigName(int argc, char ** argv)
{
	char * name = "";
	// ensure that there's enough argument
	if(argc == 2)
	{
		name = argv[1];
	}
	return name;
}



// get the integer value in a string of type abc=55
int getIntValue(char * s)
{
	int i = 0;

	char *token;
   
	token = strtok(s, "=");

	while(token != NULL )
	{
		i = atoi(token);
		token = strtok(NULL, s);
	}


	return i;
}

// get the char value in a string of type abc=A
char getCharValue(char * s)
{
	char c = 'A';

	char *token;
   
	token = strtok(s, "=");

	while(token != NULL )
	{
		c = token[0];
		token = strtok(NULL, s);
	}

	return c;
}

// get the position value in a string of type abc=0,0, put result in x and y
void getPositionValue(char * s, char * p, int * x, int * y)
{
	int i = 0;

	char *token;
   	char * position;
	token = strtok(s, "=");

	while(token != NULL )
	{
		if(i == 0)
		{
			// read point
			*p = token[0];
		}
		else if(i == 1)
		{
			// convert char to int
			*x = token[0] - '0';
			*y = token[2] - '0';
		}
		token = strtok(NULL, s);
		i++;
	}
}




// add a point of name name at position x, y to the list, return the new list
point_list cons (char name, int x, int y, point_list l)
{
  point_list newl;
  newl=(point_list)malloc(sizeof (point_t));

   (* newl).name  = name;
   (* newl).x  = x;
   (* newl).y  = y;
   (* newl).links = NULL;
   (* newl).suivant = NULL;
  	if(l == NULL)
  	{
		 
		  return newl;
	}
	else
	{
		point_t * head = l;
		while(head->suivant != NULL)
		{

			head = head->suivant;
		}
		head->suivant = newl;
		return l;
	}
  
}





// add a point of name name at position x, y to the list, return the new list
link_list consLink (char p1, char p2, link_list l)
{
  link_list newl;
  newl=(link_list)malloc(sizeof (link_t));

   (* newl).p1  = p1;
   (* newl).p2  = p2;
   (* newl).suivant = NULL;
  	if(l == NULL)
  	{
		 
		  return newl;
	}
	else
	{
		link_t * head = l;
		while(head->suivant != NULL)
		{

			head = head->suivant;
		}
		head->suivant = newl;
		return l;
	}
  
}


point_t * searchPoint(point_list l, char name)
{
	point_t * head = l;

	while(head != NULL)
	{
		if(head->name == name)
		{
			return head;
		}
		head = head->suivant;
	}
	return NULL;
}


link_t * searchLink(link_list l, char name)
{
	link_t * head = l;

	while(head != NULL)
	{
		if(head->p2 == name)
		{
			return head;
		}
		head = head->suivant;
	}
	return NULL;
}



void addLink(point_t * point, char p1, char p2)
{
	char p3;
	if(p2 == point->name)
	{
		p3 = p2;
		p2 = p1;
		p1 = p3;
	}
	point->links = consLink(p1, p2, point->links);
}





void printGrid(int gridSize, char painter, char cutter, point_list list)
{
	int i = 0;
	int j = 0;
	for(j = 0; j < gridSize; j++)
	{
		for(i = 0; i < gridSize; i++)
		{

			char pointName = getPointName(i, j, list);

			if(pointName != '\0')
			{
				if(painter == pointName)
				{
					printf(" \x1B[32m%c\x1B[0m ", pointName);
				}
				else if(cutter == pointName)
				{
					printf(" \x1B[31m%c\x1B[0m ", pointName);
				}
				else
				{
					printf(" %c ", pointName);
				}
			}
			else
			{
				int topLeftLink = 0;
				int topLink = 0;
				int topRightLink = 0;
				int rightLink = 0;
				int bottomRightLink = 0;
				int bottomLink = 0;
				int bottomLeftLink = 0;
				int leftLink = 0;

				int numberOfLink = 0;
				int status = 0;



				int ci = i;
				int cj = j;

				// search a top left point of this position
				while(ci >= 0 && cj >= 0 && ci < gridSize && cj < gridSize)
				{
					if(ci != i || cj != j)
					{
						char p = getPointName(ci, cj, list);
						if(p != '\0')
						{
							topLeftLink = p;
							break;
							
						}
					}
					ci--;
					cj--;
				}

				ci = i;
				cj = j;
				// search a top point of this position
				while(ci >= 0 && cj >= 0 && ci < gridSize && cj < gridSize)
				{

					if(ci != i || cj != j)
					{
						char p = getPointName(ci, cj, list);
						if(p != '\0')
						{
							topLink = p;
							break;
						}
					}
					cj--;
				}

				ci = i;
				cj = j;
				// search a top right point of this position
				while(ci >= 0 && cj >= 0 && ci < gridSize && cj < gridSize)
				{
					if(ci != i || cj != j)
					{
						char p = getPointName(ci, cj, list);
						if(p != '\0')
						{
							topRightLink = p;
							break;
						}
					}
					ci++;
					cj--;
				}


				ci = i;
				cj = j;
				// search a  right point of this position
				while(ci >= 0 && cj >= 0 && ci < gridSize && cj < gridSize)
				{
					if(ci != i || cj != j)
					{
						char p = getPointName(ci, cj, list);
						if(p != '\0')
						{
							rightLink = p;
							break;
						}
					}
					ci++;
				}


				ci = i;
				cj = j;
				// search a bottom right point of this position
				while(ci >= 0 && cj >= 0 && ci < gridSize && cj < gridSize)
				{
					if(ci != i || cj != j)
					{
						char p = getPointName(ci, cj, list);
						if(p != '\0')
						{
							bottomRightLink = p;
							break;
						}
					}
					ci++;
					cj++;
				}

				ci = i;
				cj = j;
				// search a bottom point of this position
				while(ci >= 0 && cj >= 0 && ci < gridSize && cj < gridSize)
				{
					if(ci != i || cj != j)
					{
						char p = getPointName(ci, cj, list);
						if(p != '\0')
						{
							bottomLink = p;
							break;
						}
					}
					cj++;
				}

				ci = i;
				cj = j;
				// search a bottom left point of this position
				while(ci >= 0 && cj >= 0 && ci < gridSize && cj < gridSize)
				{
					if(ci != i || cj != j)
					{
						char p = getPointName(ci, cj, list);
						if(p != '\0')
						{
							bottomLeftLink =p;
							break;
						}
					}
					cj++;
					ci--;
				}

				ci = i;
				cj = j;
				// search a left point of this position
				while(ci >= 0 && cj >= 0 && ci < gridSize && cj < gridSize)
				{
					if(ci != i || cj != j)
					{
						char p = getPointName(ci, cj, list);
						if(p != '\0')
						{
							leftLink = p;
							break;
						}
					}
					ci--;
				}

				
				
				int rightLeftLink = 0;
				int topBottomLink = 0;
				int topLeftRightBottomLink = 0;
				int topRightLeftBottomLink = 0;


				if(leftLink != '\0' && rightLink != '\0')
				{
					// search a link between the two point
					point_t * firstPoint = searchPoint(list, leftLink);
					if(firstPoint)
					{
						link_t * link = searchLink(firstPoint->links, rightLink);
						if(link)
						{
							
							status = link->status;
							rightLeftLink = 1;

						}
					}
				}
				else if(topLink != '\0' && bottomLink != '\0')
				{
					// search a link between the two point
					point_t * firstPoint = searchPoint(list, topLink);
					if(firstPoint)
					{
						link_t * link = searchLink(firstPoint->links, bottomLink);
						if(link)
						{
							
							status = link->status;
							topBottomLink = 1;

						}
					}
				}
				else if(topLeftLink != '\0' && bottomRightLink != '\0')
				{
					// search a link between the two point
					point_t * firstPoint = searchPoint(list, topLeftLink);
					if(firstPoint)
					{
						link_t * link = searchLink(firstPoint->links, bottomRightLink);
						if(link)
						{
							
							status = link->status;
							topLeftRightBottomLink = 1;

						}
					}
				}
				else if(topRightLink != '\0' && bottomLeftLink != '\0')
				{
					// search a link between the two point
					point_t * firstPoint = searchPoint(list, topRightLink);
					if(firstPoint)
					{
						link_t * link = searchLink(firstPoint->links, bottomLeftLink);
						if(link)
						{
							
							status = link->status;
							topRightLeftBottomLink = 1;

						}
					}
				}

				numberOfLink = rightLeftLink + topBottomLink + topLeftRightBottomLink + topRightLeftBottomLink;


				if(numberOfLink == 1)
				{
					if(topLeftRightBottomLink == 1)
					{
						if(status == 0)
						{
							printf("`-,");
						}
						else if(status == 1)
						{
							printf("\x1B[32m`-,\x1B[0m");
						}
						else
						{
							printf("\x1B[31m`-,\x1B[0m");
						}
						
					}
					else if(topBottomLink == 1)
					{
						if(status == 0)
						{
							printf(" | ");
						}
						else if(status == 1)
						{
							printf(" \x1B[32m|\x1B[0m ");
						}
						else
						{
							printf(" \x1B[31m|\x1B[0m ");
						}
						
					}
					else if(rightLeftLink == 1)
					{
						if(status == 0)
						{
							printf("---");
						}
						else if(status == 1)
						{
							printf("\x1B[32m---\x1B[0m");
						}
						else
						{
							printf("\x1B[31m---\x1B[0m");
						}
						
					}
					else if(topRightLeftBottomLink == 1)
					{
						if(status == 0)
						{
							printf(",-'");
						}
						else if(status == 1)
						{
							printf("\x1B[32m,-'\x1B[0m");
						}
						else
						{
							printf("\x1B[31m,-'\x1B[0m");
						}
						
					}
				}
				else if(numberOfLink > 1)
				{
					if(status == 0)
					{
						printf(" X ");
					}
					else if(status == 1)
					{
						printf(" \x1B[32mX\x1B[0m ");
					}
					else
					{
						printf(" \x1B[31mX\x1B[0m ");
					}
					
				}
				else
				{

					printf("   ");
					
				}

			}
		}
		printf("\n");
	}

};

char getPointName(int x, int y, point_list list)
{
	point_t * head;

	head = list;
	while(head != NULL)
	{
		if(head->x == x && head->y == y)
		{
			return head->name;
		}
		head=head->suivant;
	}
	return '\0';
};


// search if a destination exists between painter and cutter
int cutterPositionReachable(char painter, char cutter, link_list links, point_list list, int status, char done[100])
{

	point_t * painterPoint = searchPoint(list, painter);
	if(painterPoint == NULL)
	{
		return 0;
	}
	else if(painter == cutter)
	{
		return 1;
	}
	else
	{
		int k = 0;
		for(k = 0; k < 100; k++)
		{
			if(done[k] == '\0')
			{
				done[k] = painter;
				done[k+1] = '\0';
				break;
			}
		}

		link_t * head = links;
		while(head != NULL)
		{
			int statusCheck = 0;
			if(status == 0)
			{
				if(head->status == 0 || head->status == 1)
				{
					statusCheck = 1;
				}
			}
			else
			{
				if(head->status == status)
				{
					statusCheck = 1;
				}
			}

			int inarray = 0;
			int j = 0;
			while(done[j] != '\0')
			{
				if(done[j] == head->p2)
				{
					inarray = 1;
					break;
				}
				j++;
			}

			//printf("Painter %c - %c, %s - %d\n", painter, head->p2, done, inarray);

			// don't proceed already proceed node, name has to be in ordered order
			if(inarray == 0 && statusCheck == 1)
			{
				int subLinkExist = 0;
				point_t * currentPont = searchPoint(list, head->p2);
				subLinkExist = cutterPositionReachable(head->p2, cutter, currentPont->links, list, status, done);
				if(subLinkExist == 1)
				{
					return 1;
				}
				
			}
			head = head->suivant;
		}
		return 0;
	}
}


///////////////////////////////////// GAME ///////////////////////////////:


void startGame(int gridSize, char painter, char cutter, point_list pointList)
{
	printf("\n\n\n\n");
	printf("######################################\n");
	printf("###########   Start game   ###########\n");
	printf("######################################\n");
	printf("# Link to play must be select like this : A-D\n");
	printf("# \x1B[32mPainter\x1B[0m\n");
	printf("# \x1B[31mCutter\x1B[0m\n");

	int computer = 0;
	int firstPlayerAsPainter = 0;

	printf("-> Play with a computer (1 yes, 0 no) ? : ");
	scanf("%d", &computer);
	printf("-> First player as painter (1 yes, 0 no) ? : ");
	scanf("%d", &firstPlayerAsPainter);

	if(computer == 1)
	{
		printf("-> Start human vs computer\n");
	}
	else
	{
		printf("-> Start human vs human\n");
	}
	
	printf("\n\n");
	printGrid(gridSize, painter, cutter, pointList);

	point_t * painterPoints = searchPoint(pointList, painter);

	char done[100];
	done[0] = '\0';

	int exists 	=  cutterPositionReachable(painter, cutter, painterPoints->links, pointList, 0, done);
	int win 	= 0;

	int turn  	= 0;
	if(firstPlayerAsPainter == 0)
	{
		turn = 1;
	}
	while(exists && !win)
	{
		if(turn == 0)
		{
			// painter turn
			if(firstPlayerAsPainter == 1)
			{
				painterTurn(0, pointList);
			}
			else
			{
				painterTurn(computer, pointList);
			}
		}
		else
		{
			// painter turn
			if(firstPlayerAsPainter == 0)
			{
				cutterTurn(0, pointList);
			}
			else
			{
				cutterTurn(computer, pointList);
			}

		}





		if(turn == 0)
		{
			turn = 1;
		}
		else
		{
			turn = 0;
		}

		printf("\n\n");
		printGrid(gridSize, painter, cutter, pointList);

		char done2[100];
		done2[0] = '\0';

		char done3[100];
		done3[0] = '\0';

		exists 	= cutterPositionReachable(painter, cutter, painterPoints->links, pointList, 0, done2);
		win 	= cutterPositionReachable(painter, cutter, painterPoints->links, pointList, 1, done3);
	}

	if(win)
	{
		printf("\n\n--> Painter win !\n");
	}
	else
	{
		printf("\n\n--> Cutter win !\n");
	}


}


void painterTurn(int computer, point_list pointList)
{
	if(computer == 1)
	{
		char p1;
		char p2;

		getComputerMove(pointList, &p1, &p2, 1);
		printf("-> Painter (computer) play : %c-%c\n", p1, p2);
	}
	else
	{
		char p1 ='\0';
		char p2  ='\0';
		char sep;

		int valid = 0;
		while(!valid)
		{
			char str[255];
			printf("-> Painter : ");
			scanf("%s", str);
			
			p1 = str[0];
			p2 = str[2];

			char p3;

			if(p2 < p1)
			{
				p3 = p2;
				p2 = p1;
				p1 = p3;
			}

			point_t *pp1 = searchPoint(pointList, p1);
			point_t *pp2 = searchPoint(pointList, p2);

			if(pp1 == NULL || pp2 == NULL)
			{
				printf("-> Points aren't valid, try again\n");
				valid = 0;
			}
			else
			{
				link_t * link =  searchLink(pp1->links, p2);
				if(link == NULL)
				{
					printf("-> Link isn't valid, try again\n");
					valid = 0;
				}
				else if(link->status == 1)
				{
					printf("-> Link already painted\n");
					valid = 0;
				}
				else if(link->status == 2)
				{
					printf("-> Link already cut\n");
					valid = 0;
				}
				else
				{
					link_t * link2 =  searchLink(pp2->links, p1);
					if(link2)
					{
						link2->status = 1;
					}
					link->status = 1;
					valid = 1;
				}
			}
		}
	}
};

void cutterTurn(int computer, point_list pointList)
{
	if(computer == 1)
	{
		char p1;
		char p2;

		getComputerMove(pointList, &p1, &p2, 2);
		printf("-> Cutter (computer) play : %c-%c\n", p1, p2);
	}
	else
	{
		char p1;
		char p2;
		char sep;

		int valid = 0;
		while(!valid)
		{
			char str[255];
			printf("-> Cutter : ");
			scanf("%s", str);
			
			p1 = str[0];
			p2 = str[2];
			char p3;

			if(p2 < p1)
			{
				p3 = p2;
				p2 = p1;
				p1 = p3;
			}

			point_t *pp1 = searchPoint(pointList, p1);
			point_t *pp2 = searchPoint(pointList, p2);

			if(pp1 == NULL || pp2 == NULL)
			{
				printf("-> Points aren't valid, try again\n");
				valid = 0;
			}
			else
			{
				link_t * link =  searchLink(pp1->links, p2);
				if(link == NULL)
				{
					printf("-> Link not exist, try again\n");
					valid = 0;
				}
				else if(link->status == 1)
				{
					printf("-> Link already painted\n");
					valid = 0;
				}
				else if(link->status == 2)
				{
					printf("-> Link already cut\n");
					valid = 0;
				}
				else
				{
					link_t * link2 =  searchLink(pp2->links, p1);
					if(link2)
					{
						link2->status = 2;
					}
					link->status = 2;
					valid = 1;
				}
			}
		}
	}
};


void getComputerMove(point_list pointList, char * p1, char * p2, int status)
{
	// 
	point_t * headpoint = pointList;

	while(headpoint != NULL)
	{

		link_t * headLink = headpoint->links;
		while(headLink != NULL)
		{
			if(headLink->status == 0)
			{
				headLink->status = status;

				point_t *pp2 = searchPoint(pointList, headLink->p2);
				link_t * link2 =  searchLink(pp2->links,  headLink->p1);
				if(link2)
				{
					link2->status = status;
				}

				*p1 = headLink->p1;
				*p2 = headLink->p2;
				return;
			}
			headLink = headLink->suivant;
		}
		headpoint = headpoint->suivant;
	}
}