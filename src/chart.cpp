#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "color.h"
#include "nucleus.h"

using namespace std;

//int main(int argc, char *argv[]) {
int main() {
	map<string,color> colors;
	vector<nucleus> nuclei;
	map<int,string> elements;
	map<int,int> lightest;
	int maxZ=0;
	int maxN=0;
	const int scale = 10;

	/*
	 * Load in the colors
	 * The file is formatted as:
	 * name	r	g	b
	 */
	ifstream colfile("colors");
	string str;
	while(getline(colfile, str), colfile.good()) {
		stringstream ss(str);
		string n;
		color c;

		ss >> n >> c.c[0] >> c.c[1] >> c.c[2];

		colors.insert(pair<string,color>(n,c));
	}
	colfile.close();

	/*
	 * Load in the nuclei
	 * The file is formatted as:
	 * name	z	n	color
	 *
	 * The color is associated with a color name from the color file
	 */
	ifstream nucfile("nuclei");
	while(getline(nucfile, str), nucfile.good()) {
		stringstream ss(str);
		nucleus n;

		ss >> n.name >> n.z >> n.n >> n.color;

		nuclei.push_back(n);
	}
	nucfile.close();

	/*
	 * Load in the element names
	 * The file is formatted as:
	 * z	name
	 */
	ifstream elemfile("elements");
	while(getline(elemfile, str), elemfile.good()) {
		stringstream ss(str);
		 int z;
		string n;

		ss >> z >> n;

		elements.insert(pair< int,string>(z,n));
	}
	elemfile.close();

	// Determine the limits of the chart
	for (vector<nucleus>::iterator it=nuclei.begin(); it!=nuclei.end(); ++it) {
		if (it->n > maxN) {
			maxN=it->n;
		}
		if (it->z > maxZ) {
			maxZ=it->z;
		}
	}

	// Determine the lightest isotope of each element
	for (vector<nucleus>::iterator it=nuclei.begin(); it!=nuclei.end(); ++it) {
		if (lightest.count(it->z)==0) {
			lightest.insert(pair<int,int>(it->z,it->n));
		} else if (it->n < lightest[it->z]) {
			lightest.insert(pair<int,int>(it->z,it->n));
		}
	}

	/*
	 * Output the SVG
	 */
	ofstream svgfile("out.svg");
	// Header
	svgfile << "<svg xmlns=\"http://www.w3.org/2000/svg\""
	        << " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
	        << " width=\"" << (maxN+4)*scale << "\""
	        << " height=\"" << (maxZ+3)*scale << "\""
	        << ">" << endl;

	// Create Transform Group
	svgfile << "<g transform=\""
	        << "scale(" << scale << ")"
	        << " translate(2,1)"
	        << "\">" << endl;

	// Nuclide Boxes
	for (vector<nucleus>::iterator it=nuclei.begin(); it!=nuclei.end(); ++it) {
		svgfile << "<rect"
		        << " x=\"" << it->n << "\""
		        << " y=\"" << maxZ-(it->z) << "\""
		        << " width=\"1\""
		        << " height=\"1\""
		        << " style=\""
		        << "fill:rgb(" << colors[it->color].c[0] << ","
		                       << colors[it->color].c[1] << ","
		                       << colors[it->color].c[2] << ");"
		        << "stroke:black;"
		        << "stroke-width:.1"
		        << "\"/>" << endl;
	}

	// Element Symbols
	for (map<int,string>::iterator it=elements.begin(); it!=elements.end(); ++it) {
		// Determine x position
		int x=lightest[it->first];
		if (lightest.count((it->first)+1) && lightest[(it->first)+1] < x) {
			x=lightest[(it->first)+1];
		}
		svgfile << "<text"
		        << " x=\"" << x-.25 << "\""
		        << " y=\"" << maxZ-(it->first)+.75 << "\""
		        << " text-anchor=\"end\""
		        << " font-size=\".9\""
		        << ">"
		        << it->second
		        << "</text>" << endl;
	}

	svgfile << "</g>" << endl;
	svgfile << "</svg>" << endl;
	svgfile.close();
}
