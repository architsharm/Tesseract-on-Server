// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <stdlib.h>
#include <cv.hpp>
#include <highgui.hpp>
#include <fstream>
#include<conio.h>
#include<string>
#include <vector>
using namespace cv;
using namespace std;
using std::vector;
vector < vector<int> > s_box1;
void power(vector <string> words, vector <vector <int>> w_box, vector <vector <int>> s_box) {
	int i, j, k;
	k = 0;
	for (i = 0; i < words.size(); i++) {
	
		string temp = words[i];
		int y1 = w_box[i][1]; int y2 = w_box[i][3];
		float m1, m2, m3;
		m1 = (y1 + y2) / 2;
		m2 = y1 + ((float)5 * (y2 - y1))/6;
		
		m3 = y1 + ((float)1 * (y2 - y1))/3;
		
		for (j = 0; j < temp.size(); j++) {
			//cout << j + k <<'\n';
			float m4 = (float)(s_box[j+k][1] + s_box[j+k][3]) / 2;
			
			if (m4 > m2) { s_box[j+k].push_back(1); } //subscript
			if (m4 < m3) { s_box[j+k].push_back(2); } // superscript
			else { s_box[j+k].push_back(0); }
		}
		
		k = k + temp.size();
		
	}

	s_box1 = s_box;
}

int main() {
	std::ofstream ofs;
	
	string image_path;
	cin >> image_path;

	PIX *image = pixRead(image_path.c_str());
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
	//api->Init("", "eng35+eng");
	api->Init("", "math+eng5");
	api->SetImage(image);
	api->Recognize(0);
	tesseract::ResultIterator* ri = api->GetIterator();
	tesseract::ResultIterator* ri1 = api->GetIterator();
	tesseract::PageIteratorLevel level1 = tesseract::RIL_SYMBOL;
	tesseract::PageIteratorLevel level = tesseract::RIL_WORD;

	int k;


	//Characters recognition (Word(each block) + Symbols(individual letters))

	ofs.open("word.txt", std::ofstream::out | std::ofstream::trunc);
	if (ri!=0){
		do {
			const char* word = ri->GetUTF8Text(level);
			float conf = ri->Confidence(level);
			int x1, y1, x2, y2;
			ri->BoundingBox(level, &x1, &y1, &x2, &y2);
			ofs << word <<" "<<x1<< " "<<y1<<" "<<x2<<" "<<y2 << endl;
			delete[] word;
		} while (ri->Next(level));
	}
	ofs.close();
	ofs.open("symbol.txt", std::ofstream::out | std::ofstream::trunc);
	if (ri1 != 0) {
		do {
			const char* word = ri1->GetUTF8Text(level1);
			float conf = ri1->Confidence(level1);
			int x1, y1, x2, y2;
			ri1->BoundingBox(level1, &x1, &y1, &x2, &y2);
			ofs << word <<" "<< x1 << " " << y1 << " " << x2 << " " << y2 << endl;
			delete[] word;
		} while (ri1->Next(level1));
	}
	ofs.close();


	// Reading generated recognized characters file
	ifstream ifile;
	ifile.open("word.txt");
	int i = 0;
	
	vector <string> words;
	string word;
	vector < vector<int> > w_box;
	vector <int> v2;
	int num,j;
	do
	{
		ifile >> word;
		if (ifile.eof()) break;
		words.push_back(word);
		for (j = 0; j < 4; j++) {
			ifile >> num;
			v2.push_back(num);
		}
		w_box.push_back(v2);
		v2.clear();
	} while (!ifile.eof());
	
	ifile.close();
	
	
	vector <string> symbols;
	vector < vector<int> > s_box;
	
	ifile.open("symbol.txt");
	while (!ifile.eof())
	{
		ifile >> word;
		if (ifile.eof()) break;
		symbols.push_back(word);
		for (j = 0; j < 4; j++) {
			ifile >> num;
			v2.push_back(num);
		}
		for (k = 0; k < word.size(); k++) {
		s_box.push_back(v2);
	}
		v2.clear();
		
	}
	
	ifile.close();
	//cout << "234\n";
	power(words,w_box,s_box);
	
	
	//output to file 
	k = 0;
	ofs.open("output.txt", std::ofstream::out | std::ofstream::trunc);
	for (int i = 0; i < words.size(); i++)
	{
		
		
		for (int j = 0; j < words[i].size(); j++)
		{
			
			if ((k+j)!= 0) {

				if ((s_box1[k + j][4] != s_box1[k + j - 1][4]) && s_box1[k+j][4]==0) { ofs << " "; }

				if (s_box1[k+j][4] == 2 && s_box1[k+j-1][4] != 2) {
					ofs << "^" << words[i][j];
				}
				else if (s_box1[k+j][4] == 1 && s_box1[k+j-1][4] != 1) {
				
					ofs << "{" << words[i][j] <<"}";
				}
				else {
				
					ofs << words[i][j];
				
				}
			
			}
			else {
				if (s_box1[k+j][4] == 2 ) {
					ofs << "^" << words[i][j];
				}
				else if (s_box1[k+j][4] == 1) { ofs << "{" << words[i][j] <<"}"; }
				else {ofs << words[i][j]; }
			
			}
		}
		
		k = k + words[i].size();
		ofs << "\n";
	}
	ofs.close();


	return 0;
}
