#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "color.h"
#include "nucleus.h"

using namespace std;

int main(int argc, char *argv[]) {
	map<string,color> colors;
	vector<nucleus> nuclei;

	/*
	 * Load in the colors
	 * The colors file is formatted as:
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
	 * The nuclei file is formatted as:
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
	 * Output the SVG
	 */
	ofstream svgfile("out.svg");
	unsigned int maxZ=0;
	unsigned int maxN=0;
	for (vector<nucleus>::iterator it=nuclei.begin(); it!=nuclei.end(); ++it) {
		if (it->n > maxN) {
			maxN=it->n;
		}
		if (it->z > maxZ) {
			maxZ=it->z;
		}
	}

	svgfile << "<?xml version='1.0'?>" << endl;
	svgfile << "<svg"
	        << " width='" << (maxN+1)*100 << "'"
	        << " height='" <<  (maxZ+1)*100 << "'"
	        << ">" << endl;

	for (vector<nucleus>::iterator it=nuclei.begin(); it!=nuclei.end(); ++it) {
		svgfile << "<rect"
		        << " x='" << it->n*100 << "'"
		        << " y='" << (maxZ - it->z)*100 << "'"
		        << " width='100' height='100'"
		        << " style='"
		        << "fill:rgb(" << colors[it->color].c[0] << "," << colors[it->color].c[1] << "," << colors[it->color].c[2] << ");"
		        << "stroke:black;"
		        << "stroke-width:10'"
		        << "/>" << endl;
	}

	svgfile << "</svg>" << endl;
	svgfile.close();
}
