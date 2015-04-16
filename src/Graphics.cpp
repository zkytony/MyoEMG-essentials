//#include "Graphics.h"
#include "matplotpp.h"

extern double y[100];

void Plot::DISPLAY()
{
	// Use Examples ======================
	//debug1();
	//test();
	//creating_plot();
	//multiple_plots();
	//style_n_color();
	//multiple_axes();
	//Surface_n_Contours();
	//line_n_surface_3d();
	//create_slider();
	//animation();
	//advanced();
	
	// Create your own Plot =====================
	dvec lim(2);
	vector<double> x(100),y0(100), y1(100);
	for(int i=0;i<100;++i)
	{	x[i]=0.1*i;
		y0[i]=1+sin(x[i]);
		y1[i]=1+cos(x[i]);
	}

	// To create a layer 
	layer("My Plot",1);
	int hy = subplot(2,1,1);
	title("Plot");
	plot(x, y0);
	gca(hy);
	axis(0.0, 12.0, 0.0, 0.0); // range only on x
	text(2.0, 0.0, "Vertical text", true, "r");

	hy = subplot(2,1,2);
	
	plot(x, y0, 2, "g");
	gca(hy);
	axis(0.0, 12.0, 0.0, 0.0); // range only on x
	text(2.0, 0.0, "Vertical text", true, "r");
	//set(hy, "YScale", "log");

	// plot from remote data ==============
	/*dvec x;
	
	int n = sizeof y/ sizeof y[0];
	dvec y_vec(y, y + n);
	x=linspace(-2,2,n);
	int hh = layer("custom");
	set("YAxisLocation", "right");
	set("TickColor","r");
	ylabel("y label");
	set("YScale","log");
	plot(x,y_vec); set("o");*/
	
	text(0, 0.5, "text",true);
}