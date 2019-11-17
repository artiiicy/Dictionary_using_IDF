// B511026 김민준

#include <iostream>
#include "dictionary.h"

using namespace std;

int main()
{
	ifstream docfile("ir.docnames");
	//ifstream docfile("ir.mydocnames");

	ifstream wordfile("ir.words");
	ifstream infofile("ir.info");
	ifstream dictfile("ir.dictionary", ios::in | ios::binary);
	ifstream postfile("ir.postings", ios::in | ios::binary);

	string _word;
	//char * _doc;
	string _doc;

	int _wordpos; // ir.words내의 위치
	int _poststart; //1째 post위치 in ir.postings
	int _numposts; // 관련 postings의 수
	int _dict_freq;	// 전체 문서에서 몇번 나오는지 저장
	double _idf; // idf of this word
	int _docpos; // ir.docnames내의 문서위치
	int _freq; // 해당언어의 문서 내 빈도
	int num_of_docs;
	int num_of_words;
	double max_idf;

	while(wordfile >> _word)	// 단어 수만큼 출력
	{
		dictfile.read((char *)&_wordpos, sizeof(int));
		dictfile.read((char *)&_poststart, sizeof(int));
		dictfile.read((char *)&_numposts, sizeof(int));
		dictfile.read((char *)&_dict_freq, sizeof(int));
		dictfile.read((char *)&_idf, sizeof(double));


		cout << _word << " appeared " << _dict_freq << " time(s) in " << _numposts << " document(s) [ idf=" << _idf << " ]" << endl;
		for(int i = 0; i < _numposts; i++)
		{
			postfile.read((char *)&_docpos, sizeof(int));
			postfile.read((char *)&_freq, sizeof(int));
			
			docfile.seekg(_docpos, ios::beg);
			docfile >> _doc;

			cout << "  " << _doc << " " << _freq << "   ";
		}
		cout << endl;
	}
	infofile >> num_of_docs;
	infofile >> num_of_words;
	infofile >> max_idf;


	cout << endl << "<Collection Summary>" << endl;
	cout << " " << "#Docs = " << num_of_docs << " #Words = " << num_of_words << " Max. IDF = " << max_idf << endl;

	docfile.close();
	wordfile.close();
	infofile.close();
	dictfile.close();
	postfile.close();
}