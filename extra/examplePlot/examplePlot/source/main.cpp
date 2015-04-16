#include "matplotpp.h"
#include <conio.h>
#include <windows.h>

#define _USEGRAPHICS true

const int n = 100;
double y[n];

int main(int argc,char* argv[])
{
	bool graphics_exit = FALSE;
	int i;
	double t=0;
	
	

	printf("\nMain:>\t Starting graphics");
	if(_USEGRAPHICS) 
		Graphics_init(argc, argv, 20, 10, 1200, 800, &graphics_exit, "MatPlotPP Examples"); //Set up graphics
	
		
	t=0;
	printf("\nMain:>\t Enterning user functions");
	while(!graphics_exit && !_kbhit() ) // you are not using graphics
	{ 
		// Do something
		for(i=0;i<n;++i)
			y[i]=sin((double)i/5 + t);
		
		t = (t==10)? 0 : t+.005;
		Sleep(1);
	}
	printf("\nMain:>\t User functions exited");
	
	if(_USEGRAPHICS) 
		Graphics_Close();

	//Sleep(5000);

	printf("\nMain:>\t Press enter to exit");
	//getchar();
	return 0;
}