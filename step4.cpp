#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>
#include "dictionary.h"
using namespace std;

bool GetDocnamenDocpos(ifstream& fin, char *docname, int& docpos) {
	SkipBlanks(fin);
	if(fin.peek() == EOF) return false;
	docpos = fin.tellg();
	if(!(fin >> docname)) throw "말도안돼";
	return true;
}

int main()
{
	int count_word = 0;
	int numwords = 0;
	int numdocs = 0;
	int word_position = 0;
	double max_idf = 0;
	Dict_Term tm;
	Posting pst;
	string input;

	// 파일 입출력
	ofstream wout("ir.words");
	ofstream iout("ir.info");
	ofstream dout("ir.dictionary", ios::out | ios::binary);
	ofstream pout("ir.postings", ios::out | ios::binary);

	// 전체 문서 수 구하기
	ifstream fin("ir.docnames");
	//ifstream fin("ir.mydocnames");
	char docname[MAXLEN+1];	//문서명
	int docpos;

	while (GetDocnamenDocpos(fin, docname, docpos))
	{
		ifstream in(docname);
		if(!in)
			continue;
		numdocs++;
		in.close();
	}

	while(cin >> input)
	{
		int totalfreq;

		tm.wordpos = wout.tellp();
		
		// ir.words 파일에 단어 쓰기
		wout << input;
		wout << " ";

		numwords++;
		count_word++;
		if(count_word == 5)
		{
			wout << "\n";
			count_word = 0;
		}

		cin >> tm.numposts >> tm.dict_freq;

		// idf 계산 및 최대 idf 저장
		tm.idf = log((double)numdocs / tm.numposts)/log((double)2);
		max_idf = max(tm.idf, max_idf);

		// Term의 첫 문서위치 저장
		tm.poststart = pout.tellp() / sizeof(Posting);

		// Term의 (문서위치, 문서 내 빈도) 저장
		for(int i = 0; i < tm.numposts; i++)
		{
			cin >> pst.docpos >> pst.freq;
			pout.write(reinterpret_cast <const char *> (&pst), sizeof(Posting));
		}
		dout.write(reinterpret_cast<const char *> (&tm), sizeof(Dict_Term));
	}

	// ir.info 파일에 정보 저장
	iout << numdocs << " " << numwords << " " <<  max_idf;

	iout.close();
	wout.close();
	dout.close();
	pout.close();
}
