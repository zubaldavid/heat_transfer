
#include <iostream>
#include <iomanip>
#include <array>
#include <algorithm>
using namespace std;

const double THRESHOLD = 0.0001; //Stop when the changes are less than .01% of the total array heat
const int MAX_ITERATIONS = 300; //Stop if we iterate more than this many times over the arrays
int SIZE_X,SIZE_Y; //Dimensions of the array

//Maps 2D coordinates to a 1D index space
//Does circular wrapping - going off the edge one way wraps around to the other side
int index(int i, int j) {
	while (i < 0) i+=SIZE_X;
	while (j < 0) j+=SIZE_Y;
	if (i >= SIZE_X) i %= SIZE_X;
	if (j >= SIZE_Y) j %= SIZE_Y;
	return (i*SIZE_Y+j);
}

void clear_array(double *sheet,double value) {  //passed in pointer which clears the array  
	for (int i = 0; i < SIZE_X; i++)
		for (int j = 0; j < SIZE_Y; j++) 
			sheet[index(i,j)] = value;
}

void print_array(double *sheet, bool clear=true, int precision = 2, int width=8) { //Keeps the data in a set precision 
	if (clear) system("clear");
	cout << setprecision(precision);
	cout << fixed;
	for (int i = 0; i < SIZE_X; i++) {
		for (int j = 0; j < SIZE_Y; j++)
			cout << setw(width) << sheet[index(i,j)];
		cout << endl;
	}
}

void die() {
	cout << "Invalid input.\n";
	exit(EXIT_FAILURE);
}


void wait_ticks(int ticks) {
	clock_t start = clock();
	while ((clock() - start) < ticks)
		; //Do nothing
}


int main() {
	cout << "Please type in the size of the sheet of metal (x y): ";
	cin >> SIZE_X;
	if (!cin || SIZE_X < 1) die();
	cin >> SIZE_Y; 
	if (!cin || SIZE_Y < 1) die();

	//Allocate two buffers to hold our temperatures
	double *sheet = new double[SIZE_X*SIZE_Y]; //2D array held as 1D array
	double *sheet2 = new double[SIZE_X*SIZE_Y];

	cout << "Please type in the starting temperature for the sheet: ";
	double temp;
	cin >> temp;
	if (!cin) die();
	clear_array(sheet,temp); //Initialize the whole sheet to this number
	print_array(sheet);

	cout << "Please type in the coordinate and a temperature to inject into the sheet (x y temp): ";
	int x,y;
	cin >> x;
	if (!cin || x < 0 || x >= SIZE_X) die();
	cin >> y; 
	if (!cin || y < 0 || y >= SIZE_Y) die();
	cin >> temp;
	if (!cin) die();
	sheet[index(x,y)] = temp;
	print_array(sheet);

	cout << "Please type in the thermal resistance of the sheet from 0 to 1 (0 = no resistance, 1 = perfect resistance): ";
	double R_factor;
	cin >> R_factor;
	if (!cin || R_factor < 0 || R_factor > 1) die();

	cout << "Ready to run. Type anything to begin execution.\n";
	string trash;
	cin >> trash;

	//Main loop
	int iterations = 0; //Safety measure against infinite loops
	while (true) {
		//Clear the screen 
		system("clear");
		print_array(sheet);
		cout << "Frame: " << iterations << endl;
		//Animate by clearing and redrawing the screen every 0.2 seconds
		wait_ticks(CLOCKS_PER_SEC * 0.2);

		//First, run the stencil and save the results into sheet2
		for (int i = 0; i < SIZE_X; i++) {
			for (int j = 0; j < SIZE_Y; j++) {
				//Right now it just copies the values over, but you should set each point in
				// sheet2 to be the average of the points neighboring it from sheet. 
				// Remember to include thermal resistance values once you get it working
				// By default, set it to the average of its four neighbors. 
				//sheet2[index(i,j)] = (sheet[index(i+1,j)] + sheet[index(i-1,j)] + sheet[index(j+1,i)] + sheet[index(j-1,i)])/4; //Finds the average of sheet2 using surrouniing temps
				double a = (sheet[index(i+1,j)] + sheet[index(i-1,j)] + sheet[index(i,j+1)] + sheet[index(i,j-1)])/4; //Finds the average of sheet2 using surrouniing temps
				double b = sheet[index(i,j)];
				sheet2[index(i,j)] = a + ( b - a )*R_factor; //lerping 
			}
		}

		//You: Second, test for convergence
		//If there's no major changes in the array, break out of the infinite loop
		//Basically, you'll be taking the difference of the old array and the new array
		// as the sum of the absolute values of the differences of every point, then
		// dividing by the size of the array. This is called the "delta", and is a double.
		// If it ever drops below THRESHOLD (a constant defined up top), end the simulation
		double delta = 0;
		double area = SIZE_X*SIZE_Y; 
		for (int i = 0; i < SIZE_X; i++ ){ 
			for (int j = 0; j < SIZE_Y; j++ ){
				delta += fabs(sheet2[index(i,j)] - sheet[index(i,j)]);
			}
		}
		if((delta/area)<THRESHOLD){
			cout <<"Delta:" << delta << endl;
			break;
		}
		//Third test for infinite looping as a safety measure
		if (++iterations > MAX_ITERATIONS) {
			cout << "Run failed. Array did not converge.\n";
			exit(EXIT_FAILURE);
		}

		//Fourth, copy sheet2 back to sheet
		for (int i = 0; i < SIZE_X; i++) {
			for (int j = 0; j < SIZE_Y; j++) {
				sheet[index(i,j)] = sheet2[index(i,j)];
			}
		}

	}
	cout << "Simulation Finished.\n";
	}
