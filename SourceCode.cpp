// CPP program to illustrate
// Scanline Polygon fill Algorithm

#include <stdio.h>
#include <math.h>
#include <vector>
#include <glut.h>
#define maxHt 800
#define maxWd 600
#define maxVer 10000
float r = 0.0;
float g = 0.0;
float b = 0.0;

FILE *stream;
class Point
{
public:
	Point(int, int);
	int x, y;

	int getX() {
		return this->x;
	}
	int getY() {
		return this->y;
	}
};

Point::Point(int x, int y)
{
	this->x = x;
	this->y = y;
}
std::vector<Point> vertexList;


// Start from lower left corner
typedef struct edgelist
{
	float xint; //x-coordinate of lowest edge point updated only in aet
	float slopeinverse;
	int yhigh; //max y-coordinate of edge
}EL;

typedef struct edgetable
{
	// the array will give the scanline number
	// The edge table (ET) with edges entries sorted 
	// in increasing y and x of the lower end

	int ELcount; //no. of edgebuckets
	EL el[maxVer];
}ET;

ET EdgeTable[maxHt], AEL;


// Scanline Function
void initEdgeTable()
{
	int i;
	for (i = 0; i<maxHt; i++)
	{
		EdgeTable[i].ELcount = 0;
	}

	AEL.ELcount = 0;
}


void printTuple(ET *tup)
{
	int j;

	if (tup->ELcount)
		printf("\nCount %d-----\n", tup->ELcount);

	for (j = 0; j<tup->ELcount; j++)
	{
		printf(" %d+%.2f+%.2f",
			tup->el[j].yhigh, tup->el[j].xint, tup->el[j].slopeinverse);
	}
}

void printTable()
{
	int i, j;

	for (i = 0; i<maxHt; i++)
	{
		if (EdgeTable[i].ELcount)
			printf("\nScanline %d", i);

		printTuple(&EdgeTable[i]);
	}
}


/* Function to sort an array using insertion sort*/
void insertionSort(ET *ett)
{
	int i, j;
	EL temp;

	for (i = 1; i < ett->ELcount; i++)
	{
		temp.yhigh = ett->el[i].yhigh;
		temp.xint = ett->el[i].xint;
		temp.slopeinverse = ett->el[i].slopeinverse;
		j = i - 1;

		while ((temp.xint < ett->el[j].xint) && (j >= 0))
		{
			ett->el[j + 1].yhigh = ett->el[j].yhigh;
			ett->el[j + 1].xint = ett->el[j].xint;
			ett->el[j + 1].slopeinverse = ett->el[j].slopeinverse;
			j = j - 1;
		}
		ett->el[j + 1].yhigh = temp.yhigh;
		ett->el[j + 1].xint = temp.xint;
		ett->el[j + 1].slopeinverse = temp.slopeinverse;
	}
}


void storeEdgeInTuple(ET *receiver, int ym, int xm, float slopInv)
{
	// both used for edgetable and active edge table..
	// The edge tuple sorted in increasing ymax and x of the lower end.
	(receiver->el[(receiver)->ELcount]).yhigh = ym;
	(receiver->el[(receiver)->ELcount]).xint = (float)xm;
	(receiver->el[(receiver)->ELcount]).slopeinverse = slopInv;

	// sort the buckets
	insertionSort(receiver);

	(receiver->ELcount)++;


}

void storeEdgeInTable(int x1, int y1, int x2, int y2)
{
	float m, minv;
	int ymaxTS, xwithyminTS, scanline; //ts stands for to store

	if (x2 == x1)
	{
		minv = 0.000000;
	}
	else
	{
		m = ((float)(y2 - y1)) / ((float)(x2 - x1));

		// horizontal lines are not stored in edge table
		if (y2 == y1)
			return;

		minv = (float)1.0 / m;
		printf("\nSlope string for %d %d & %d %d: %f", x1, y1, x2, y2, minv);
	}

	if (y1>y2)
	{
		scanline = y2;
		ymaxTS = y1;
		xwithyminTS = x2;
	}
	else
	{
		scanline = y1;
		ymaxTS = y2;
		xwithyminTS = x1;
	}
	// the assignment part is done..now storage..
	storeEdgeInTuple(&EdgeTable[scanline], ymaxTS, xwithyminTS, minv);


}

void removeEdgeByYmax(ET *Tup, int yy)
{
	int i, c;
	for (i = 0; i< Tup->ELcount; i++)
	{
		if (Tup->el[i].yhigh == yy)
		{
			printf("\nRemoved at %d", yy);

			for (c = i; c < Tup->ELcount - 1; c++)
			{
				Tup->el[c].yhigh = Tup->el[c + 1].yhigh;
				Tup->el[c].xint = Tup->el[c + 1].xint;
				Tup->el[c].slopeinverse = Tup->el[c + 1].slopeinverse;
			}
			Tup->ELcount--;
			i--;
		}
	}
}


void updatexbyslopeinv(ET *Tup)
{
	int i;

	for (i = 0; i<Tup->ELcount; i++)
	{
		(Tup->el[i]).xint = (Tup->el[i]).xint + (Tup->el[i]).slopeinverse;
	}
}


void ScanlineFill()
{
	/* Follow the following rules:
	1. Horizontal edges: Do not include in edge table
	2. Horizontal edges: Drawn either on the bottom or on the top.
	3. Vertices: If local max or min, then count twice, else count
	once.
	4. Either vertices at local minima or at local maxima are drawn.*/


	int i, j, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;

	// we will start from scanline 0; 
	// Repeat until last scanline:
	for (i = 0; i<maxHt; i++)//4. Increment y by 1 (next scan line)
	{

		// 1. Move from ET bucket y to the
		// AET those edges whose ymin = y (entering edges)
		for (j = 0; j<EdgeTable[i].ELcount; j++)
		{
			storeEdgeInTuple(&AEL, EdgeTable[i].el[j].
				yhigh, EdgeTable[i].el[j].xint,
				EdgeTable[i].el[j].slopeinverse);
		}
		printTuple(&AEL);

		// 2. Remove from AET those edges for 
		// which y=ymax (not involved in next scan line)
		removeEdgeByYmax(&AEL, i);

		//sort AET (remember: ET is presorted)
		insertionSort(&AEL);

		printTuple(&AEL);

		//3. Fill lines on scan line y by using pairs of x-coords from AET
		j = 0;
		FillFlag = 0;
		coordCount = 0;
		x1 = 0;
		x2 = 0;
		ymax1 = 0;
		ymax2 = 0;
		while (j<AEL.ELcount)
		{
			if (coordCount % 2 == 0)
			{
				x1 = (int)(AEL.el[j].xint);
				ymax1 = AEL.el[j].yhigh;
				if (x1 == x2)
				{
					/* three cases can arrive-
					1. lines are towards top of the intersection
					2. lines are towards bottom
					3. one line is towards top and other is towards bottom
					*/
					if (((x1 == ymax1) && (x2 != ymax2)) || ((x1 != ymax1) && (x2 == ymax2)))
					{
						x2 = x1;
						ymax2 = ymax1;
					}

					else
					{
						coordCount++;
					}
				}

				else
				{
					coordCount++;
				}
			}
			else
			{
				x2 = (int)AEL.el[j].xint;
				ymax2 = AEL.el[j].yhigh;

				FillFlag = 0;

				// checking for intersection...
				if (x1 == x2)
				{
					/*three cases can arive-
					1. lines are towards top of the intersection
					2. lines are towards bottom
					3. one line is towards top and other is towards bottom
					*/
					if (((x1 == ymax1) && (x2 != ymax2)) || ((x1 != ymax1) && (x2 == ymax2)))
					{
						x1 = x2;
						ymax1 = ymax2;
					}
					else
					{
						coordCount++;
						FillFlag = 1;
					}
				}
				else
				{
					coordCount++;
					FillFlag = 1;
				}


				if (FillFlag)
				{
					//drawing actual lines...
					glColor3f(r, g, b);
					glBegin(GL_LINES);
					glVertex2i(x1, i);
					glVertex2i(x2, i);
					glEnd();
					glFlush();

					// printf("\nLine drawn from %d,%d to %d,%d",x1,i,x2,i);
				}

			}

			j++;
		}


		// 5. For each nonvertical edge remaining in AET, update x for new y
		updatexbyslopeinv(&AEL);
	}


	printf("\nScanline filling complete");

}


void myInit(void)
{

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluOrtho2D(0, maxHt, 0, maxWd);
	glClear(GL_COLOR_BUFFER_BIT);
}

void drawPolyDino()
{
	glColor3f(1.0f, 0.0f, 0.0f);
	int count = 0, x1, y1, x2, y2;
	while (count< vertexList.size())
	{
		count++;
		if (count>2)
		{
			x1 = x2;
			y1 = y2;
			//count = 2;
		}
		if (count == 1)
		{
			fscanf(stream, "%d,%d", &x1, &y1);
		}
		else
		{
			fscanf(stream, "%d,%d", &x2, &y2);
			printf("\n%d,%d", x2, y2);
			glBegin(GL_LINES);
			glVertex2i(x1, y1);
			glVertex2i(x2, y2);
			glEnd();
			storeEdgeInTable(x1, y1, x2, y2);//storage of edges in edge table.


			glFlush();
		}
	}


}

void display(void)
{
	
	glPointSize(10);
	
	glBegin(GL_POINTS);
	glColor3f(0.0, 0.0, 1.0);
	glVertex2f(500,500);
	glEnd();
	
	glBegin(GL_POINTS);
	glColor3f(0.0, 1.0, 0.0);
	glVertex2f(500, 530);
	glEnd();
	
	glBegin(GL_POINTS);
	glColor3f(1.0, 0.0, 0.0);
	glVertex2f(500, 560);
	glEnd();

	glFlush();
}

void drawPoint(int x, int y) {
	x = x ;
	y = maxWd - y;
	glPointSize(2);
	glBegin(GL_POINTS);
	glColor3f(0.0, 0.0, 0.0);
	glVertex2f(x, y);
	glEnd();
	glFlush();
}

void MouseFunc(int button, int state, int x, int y) {
	printf("%d %d ", x, maxWd-y);
	if (x > 490 && x < 510 && maxWd-y > 490 && maxWd - y<510) {
		glColor3f(0.0, 0.0, 1.0);
		r = 0.0;
		g = 0.0;
		b = 1.0;
	}

	if (x > 490 && x < 510 && maxWd - y > 520 && maxWd - y<540) {
		glColor3f(0.0, 1.0, 0.0);
		r = 0.0;
		g = 1.0;
		b = 0.0;
	}

	if (x > 490 && x < 510 && maxWd - y > 550 && maxWd - y<570) {
		glColor3f(1.0, 0.0, 0.0);
		r = 1.0;
		g = 0.0;
		b = 0.0;
	}
}

void myMouseFunc(int x, int y)
{
	
		drawPoint(x, y);
		Point p(x,maxWd-y);
		//("x and y %d %d after saving in points %d %d",x,y, p.getX(),p.getY());
		vertexList.push_back(p);
}
void print() {
	int x1, y1, x2, y2;
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	for (unsigned i = 0; i < vertexList.size(); i++)
	{
		if (i == 0) {
			x1 = vertexList.at(i).x;
			y1 = vertexList.at(i).y;
		}
		if (i >= 2) {
			x1 = x2;
			y1 = y2;
			x2 = vertexList.at(i).x;
			y2 = vertexList.at(i).y;
			glVertex2i(x1, y1);
			glVertex2i(x2,y2);
			storeEdgeInTable(x1, y1, x2, y2);
		}
		else {
			x2 = vertexList.at(i).x;
			y2 = vertexList.at(i).y;
			glVertex2i(x1, y1);
			glVertex2i(x2, y2);
			storeEdgeInTable(x1, y1, x2, y2);
		}
	}
	glEnd();
	glFlush();
}
void MyKeyboardFunc(unsigned char Key, int x, int y)
{
	if ( Key == char(13)) {
		
		print();
		ScanlineFill();
		vertexList.clear();
		initEdgeTable();
	}
}


void main(int argc, char** argv)
{
	stream = fopen("PolyDino.txt", "r");
	if (stream == NULL)
	{
		printf("Could not open file");
		return;
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(maxHt, maxWd);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Scanline filled dinosaur");
	myInit();
	glutKeyboardFunc(MyKeyboardFunc);
	glutMouseFunc(MouseFunc);
	glutMotionFunc(myMouseFunc);
	glutDisplayFunc(display);
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);

	glutMainLoop();
	fclose(stream);
}