#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;

class color {
public:
	int c[3];
};

class nucleus {
public:
	string name;
	int z;
	int n;
	string color;
};

//int main(int argc, char *argv[]) {
int main() {
	map<string,color> colors;
	vector<nucleus> nuclei;
	vector<int> magic;
	map<int,string> elements;
	map<int,pair<int,int>> zLimits;
	map<int,pair<int,int>> nLimits;
	int maxZ=0;
	int maxN=0;
	const int scale = 10;

	/*
	 * Load in the colors
	 * The file is formatted as:
	 * name	r	g	b
	 */
	ifstream colfile("data/colors");
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
	ifstream nucfile("data/nuclei");
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
	ifstream elemfile("data/elements");
	while(getline(elemfile, str), elemfile.good()) {
		stringstream ss(str);
		 int z;
		string n;

		ss >> z >> n;

		elements.insert(pair<int,string>(z,n));
	}
	elemfile.close();

	/*
	 * Load in the magic numbers
	 * The file is formatted as:
	 * n
	 */
	ifstream magfile("data/magic");
	while(getline(magfile, str), magfile.good()) {
		stringstream ss(str);
		int n;

		ss >> n;

		magic.push_back(n);
	}
	magfile.close();

	/*
	 * Determine the limits of the chart
	 */
	for (vector<nucleus>::iterator it=nuclei.begin(); it!=nuclei.end(); ++it) {
		if (it->n > maxN) {
			maxN=it->n;
		}
		if (it->z > maxZ) {
			maxZ=it->z;
		}
	}

	/*
	 * Determine the lowest and highest Z for each N
	 * and lowest and highest N for each Z
	 */
	for (vector<nucleus>::iterator it=nuclei.begin(); it!=nuclei.end(); ++it) {
		if (zLimits.count(it->z)==0) {
			zLimits.insert(pair<int,pair<int,int>>(it->z,pair<int,int>(it->n,it->n)));
		} else if (it->n < zLimits[it->z].first) {
			zLimits[it->z].first = it->n;
		} else if (it->n > zLimits[it->z].second) {
			zLimits[it->z].second = it->n;
		}
		if (nLimits.count(it->n)==0) {
			nLimits.insert(pair<int,pair<int,int>>(it->n,pair<int,int>(it->z,it->z)));
		} else if (it->z < nLimits[it->n].first) {
			nLimits[it->n].first = it->z;
		} else if (it->z > nLimits[it->n].second) {
			nLimits[it->n].second = it->z;
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
		// Only include element symbol if one of its isotopes is included
		if (zLimits.count(it->first)) {
			int x=zLimits[it->first].first;
			if (zLimits.count((it->first)+1) && zLimits[(it->first)+1].first < x) {
				x=zLimits[(it->first)+1].first;
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
	}

	// Magic Number Outlines
	for (vector<int>::iterator it=magic.begin(); it!=magic.end(); ++it) {
		// Only include magic number outline if one of those isotones is include
		if (nLimits.count(*it)) {
			svgfile << "<rect"
			        << " x=\"" << *it << "\""
			        << " y=\"" << maxZ-nLimits[*it].second << "\""
			        << " width=\"1\""
			        << " height=\"" << nLimits[*it].second-nLimits[*it].first+1 << "\""
			        << " style=\""
			        << "fill:none;"
			        << "stroke:black;"
			        << "stroke-width:.25"
			        << "\"/>" << endl;
		}
		// Only include magic number outline if one of those isotopes is include
		if (zLimits.count(*it)) {
			svgfile << "<rect"
			        << " x=\"" << zLimits[*it].first << "\""
			        << " y=\"" << maxZ-*it << "\""
			        << " width=\"" << zLimits[*it].second-zLimits[*it].first+1 << "\""
			        << " height=\"1\""
			        << " style=\""
			        << "fill:none;"
			        << "stroke:black;"
			        << "stroke-width:.25"
			        << "\"/>" << endl;
		}
	}

	svgfile << "</g>" << endl;
	svgfile << "</svg>" << endl;
	svgfile.close();
}
