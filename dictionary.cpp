// B511026 김민준

#include "dictionary.h"

bool IsAlpha(int c) { return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z'; }

bool IsDigit(int c) { return c >= '0' && c <= '9'; }

bool IsBlank(int c) { return c == ' ' || c == '\t' || c == '\n'; }

bool IsOtherCode(int c) { return c < -1 || c > 127; }	// ascii나 eof아님 (한글, 한문 등)

bool IsLetter(int c) { return IsAlpha(c) || IsOtherCode(c); }

void SkipBlanks(istream& in)
{
	char c;
	while ((c=in.peek()) != EOF && IsBlank(c))
		in.get();
}

void Capital2Lower(char * word)
{
	// word내의 대문자를 모두 소문자로 바꿈
	for(int i = 0; i < MAXLEN + 1 ; i++)
	{
		if(word[i] >= 'A' && word[i] <= 'Z')
		{
			word[i] = word[i] - ('A' - 'a');
		}
	}
}

void Filepos2Str(istream& in, int pos, char* str)
{
	// pos위치에서 문자열 읽음
	if(!(in.seekg(pos, ios::beg)))
		throw "Seekg Failed";
	in >> str;
}

bool NextWord(istream& in, char* word)
{
	// [a-zA-Z]+ 를 찾아 word에 저장
	char c;
	while (!IsLetter(c = in.peek()))
	{
		if (c==EOF) return false;
		in.get();
	}

	int wordlen = 0;
	while(IsLetter(in.peek()))
	{
		word[wordlen++] = in.get();
		if (wordlen == MAXLEN) break;
	}
	word[wordlen] = '\0';

	return true;
}

bool IsStopWord(char *word)
{
	// {is, the, very} = stopword
	Capital2Lower(word);

	int wordlen = 0;
	int i = 0;

	while(word[i] != '\0')
	{
		wordlen++;
		i++;
	}

	if(wordlen == 1)
		return true;

	if(!strcmp(word, "it") || !strcmp(word, "is") || !strcmp(word, "the") || !strcmp(word, "very"))
		return true;

	if(!strcmp(word, "so") || !strcmp(word, "ve") || !strcmp(word, "to") || !strcmp(word, "an") || !strcmp(word, "am") || !strcmp(word, "as") || !strcmp(word, "at")
		|| !strcmp(word, "ir") || !strcmp(word, "in") || !strcmp(word, "on") || !strcmp(word, "or") || !strcmp(word, "of") || !strcmp(word, "re") || !strcmp(word, "is")
		|| !strcmp(word, "it") || !strcmp(word, "ll") || !strcmp(word, "for") || !strcmp(word, "not") || !strcmp(word, "don") || !strcmp(word, "you") || !strcmp(word, "my") || !strcmp(word, "was"))
		return true;
}

bool NextNonStopWord(istream& in, char *word)
{
	while(1)
	{
		if(!NextWord(in, word)) return false;
		if(!IsStopWord(word)) return true;
	}
}

// consult에서 필요한 함수들
// "ir.info"에 저장된 3값 중 1개 반환 함수들
int NumDocs()
{
	ifstream iin("ir.info");
	int numdocs, numwords;
	float maxidf;

	if(!(iin >> numdocs >> numwords >> maxidf))
		throw "ir.info does not exist";
	iin.close();
	return numdocs;
}

int NumWords()
{
	 /* NumDocs와 유사 */
	ifstream iin("ir.info");
	int numdocs, numwords;
	float maxidf;

	if(!(iin >> numdocs >> numwords >> maxidf))
		throw "ir.info does not exist";
	iin.close();
	return numwords;
}

double MaxIDF()
{
	/* NumDocs와 유사 */
	ifstream iin("ir.info");
	int numdocs, numwords;
	float maxidf;

	if(!(iin >> numdocs >> numwords >> maxidf))
		throw "ir.info does not exist";
	iin.close();
	return maxidf;
}

bool BinarySearchDictionary(ifstream& din, char* word, Dict_Term& tm)
{
	//사전인 ir.dictionary의 2진탐색
	//word에 해당되는 Dict_Term을 찾는다

	ifstream fword("ir.words");
	string found_word;
	char input_char[MAXLEN];
	int file_length;
	
	int _wordpos; // ir.words내의 위치
	int _poststart; //1째 post위치 in ir.postings
	int _numposts; // 관련 postings의 수
	int _dict_freq;	// 전체 문서에서 몇번 나오는지 저장
	double _idf; // idf of this word

	// dictionary file 크기 구하기
	din.seekg(0, ios::end);
	file_length = din.tellg();
	din.seekg(0, ios::beg);

	// binary search
	int left = 0;
	int right = file_length/sizeof(tm)-1;

	while(left <= right)
	{
		int mid = (left + right) / 2;
		mid *= sizeof(tm);

		// Dict_Term에서 wordpos 구하기
		din.seekg(mid, ios::beg);
		din.read((char *)&_wordpos, sizeof(int));
		
		// ir.words에서 단어 받아오기
		fword.seekg(_wordpos, ios::beg);
		fword >> found_word;
		
		// string to char
		strcpy(input_char, found_word.c_str());

		if(!strcmp(input_char, word))	// 찾은 경우
		{
			din.read((char *)&_poststart, sizeof(int));
			din.read((char *)&_numposts, sizeof(int));
			din.read((char *)&_dict_freq, sizeof(int));
			din.read((char *)&_idf, sizeof(double));

			tm.wordpos = _wordpos;
			tm.poststart = _poststart;
			tm.numposts = _numposts;
			tm.dict_freq = _dict_freq;
			tm.idf = _idf;

			return 1;
		}

		else if(strcmp(input_char, word) < 0)
		{
			mid /= sizeof(tm);
			left = mid + 1;
		}

		else
		{
			mid /= sizeof(tm);
			right = mid - 1;
		}
	}

	return 0;
}