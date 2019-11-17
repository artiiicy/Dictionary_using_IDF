#ifndef DICTIONARY_H
#define DICTIONARY_H
#include <iostream>
#include <fstream>
#include <cstring>
#define MAXLEN 256
using namespace std;

template <class T>
void ChangeSize1D(T *&a, const int oldSize, const int newSize) {
	//배열크기 변경 프로그램: DS교재참조
	if (newSize < 0) throw "뭐야이거!";
	T* temp = new T[newSize];
	int number = min(oldSize, newSize);
	copy(a, a+number, temp);
	delete [] a;
	a = temp;
}

struct Posting { //어느 문서에 몇차례
	Posting(int dp=-1, int fr=0) : docpos(dp), freq(fr) { }
	int docpos; // ir.docnames내의 문서위치
	int freq; // 해당언어의 문서 내 빈도
};

struct Dict_Term { //사전내의 entry
	int wordpos; // ir.words내의 위치
	int poststart; //1째 post위치 in ir.postings
	int numposts; // 관련 postings의 수
	int dict_freq;	// 전체 문서에서 몇번 나오는지 저장
	double idf; // idf of this word
};

bool IsAlpha(int c);
bool IsDigit(int c);
bool IsBlank(int c);
bool IsOtherCode(int c); // not ascii, not EOF
bool IsLetter(int c);

void SkipBlanks(istream& in);
void Capital2Lower(char *word);
void Filepos2Str(istream& in, int pos, char *str);

bool NextWord(istream& in, char *word);
bool IsStopWord(char *word);
bool NextNonStopWord(istream& in, char *word);

bool BinarySearchDictionary(ifstream& din, char * word, Dict_Term& tm);
int NumDocs();
int NumWords();
double MaxIDF(); // max IDF in the collection
#endif
