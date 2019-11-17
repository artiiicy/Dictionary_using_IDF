// B511026 김민준

#include <iostream>
#include <stdio.h>
#include <string>
#include <algorithm>
#include "dictionary.h"

using namespace std;

string input[100];
char input_char[MAXLEN];

struct accumulator
{
	accumulator(int doc_position = 0, double weight = 0, struct accumulator * next = NULL) : doc_position(doc_position), weight(weight), next(next) { }

	int doc_position;
	double weight;
	struct accumulator * next;
};

struct hash_table
{
	hash_table(bool check = 0, struct accumulator * accum = NULL) : check(check), first(accum) { }

	bool check;
	struct accumulator * first;
};

bool com(const accumulator& a, const accumulator& b)
{
	if(a.weight > b.weight)
		return true;

	else if(a.weight == b.weight)
	{
		string a_file_name, b_file_name;
		ifstream temp_file("ir.docnames");
		temp_file.seekg(a.doc_position, ios::beg);
		temp_file >> a_file_name;
		temp_file.seekg(b.doc_position, ios::beg);
		temp_file >> b_file_name;

		temp_file.close();

		if(a_file_name.compare(b_file_name) < 0)
			return true;
	}


	return a.weight > b.weight;
}

int main(int argc, char *argv[])	// argc : main함수에 전달 된 인자의 개수. 인자를 아무것도 주지 않고 main함수 호출 시 argc=1 (호출 이름 때문.)
									// argv : main함수로 전달 되는 데이터. 문자열의 형태를 띈다.
{
	if (argc < 2)	//인자 잘못 넣었을 경우 출력.
	{
		cout << "Usage : " << argv[0] << " [-v] query" << endl;;	// 인자 잘못 입력한 경우 예외처리
		return 0;
	}

	else if (argc == 2)	// argv[1] = query file 이름
	{
		int numwords = 0;
		int freq;	// 이진파일 posting에서의 frequency (단어의 해당 문서 내 빈도) 저장할 변수
		int num_of_docs;
		int temp_doc_pos;
		int temp_freq;
		int num_accum = 0;
		string temp;
		bool found;	// 찾았으면 1, 못찾았으면 0으로 지정해줄 변수
		struct Dict_Term found_entry;	// 단어 찾았을 때, 해당 단어의 Dict_Term Entry정보 저장할 변수

		ifstream fin(argv[1]);	// open the query file
		ifstream infofile("ir.info");
		ifstream dict("ir.dictionary", ios::in | ios::binary);
		ifstream pos("ir.postings", ios::in | ios::binary);
		ifstream docs("ir.docnames");
		//ifstream docs("ir.mydocnames");


		infofile >> num_of_docs;
		// hash table 생성
		struct hash_table ht[num_of_docs];

		// query의 모든 단어들 input[]에 저장
		while(fin >> temp)
			input[numwords++] = temp;

		// query 출력
		cout << "[Query]" << endl;
		for(int i = 0; i < numwords; i++)
			cout << input[i] << " ";
		cout << endl << "---------------------------" << endl;

		for(int i = 0; i < numwords; i++)
		{
			/*
				1. ir.dictioinary에서 Binary Search로 찾기 (Dict_Term의 wordpos를 통해 ir.words의 word와 input[i]랑 비교
				2. 1.에서의 idx로 ir.dictionary의 wordpos와 비교하여 해당 entry에 접근해서 값들 빼오기. (이 때, 이진탐색 사용)
				3. poststart로 ir.postings에 접근하여 문서 내 정보 빼오기.
			*/

			// string to char
			strcpy(input_char, input[i].c_str());
			Capital2Lower(input_char);

			if(BinarySearchDictionary(dict, input_char, found_entry))	// 항목 찾았을 경우
			{
				//cout << input_char << "(idf=" << found_entry.idf << "): " << found_entry.numposts << " Posting(s) from " << found_entry.poststart << "-th" << endl;
				found = 1;

				pos.seekg(found_entry.poststart * sizeof(Posting), ios::beg);

				for(int k=0; k<found_entry.numposts; k++)
				{
					pos.read((char *)&temp_doc_pos, sizeof(int));
					pos.read((char *)&temp_freq, sizeof(int));

					// hash table에 추가
					int hash_idx = temp_doc_pos % num_of_docs;
					if(ht[hash_idx].check == 0)	// hash table에 항목이 없으면
					{
						accumulator * new_accum = new accumulator;
						new_accum->doc_position = temp_doc_pos;
						new_accum->weight = temp_freq * found_entry.idf;

						ht[hash_idx].first = new_accum;
						ht[hash_idx].check = 1;
						num_accum++;
					}

					else	// hash table에 항목이 있으면
					{
						accumulator * searching = ht[hash_idx].first;

						while(searching != NULL)
						{
							if(searching->doc_position == temp_doc_pos)	// 동일한 항목이 존재하는 경우 weight 합산
							{
								searching->weight += temp_freq * found_entry.idf;
								break;
							}

							searching = searching->next;
						}

						if(searching == NULL)	// 동일한 항목이 없다면 맨 앞에 추가
						{
							accumulator * new_accum = new accumulator;
							new_accum->doc_position = temp_doc_pos;
							new_accum->weight = temp_freq * found_entry.idf;

							new_accum->next = ht[hash_idx].first;
							ht[hash_idx].first = new_accum;
							num_accum++;
						}
					}
				}
			}
		}
		
		// if there is no relevant Documents
		if(!found)
		{
			cout << "No Relevant Documents Found!" << endl;
			return 0;
		}

		// ranking 매겨서 저장
		accumulator *ranked_arr = new accumulator[num_accum];
		int j = 0;

		for(int i=0; i<num_of_docs; i++)
		{
			if(ht[i].check == 1)
			{
				accumulator * temp = ht[i].first;

				while(temp != NULL)
				{
					ranked_arr[j].weight = temp->weight;
					ranked_arr[j++].doc_position = temp->doc_position;

					temp = temp->next;
				}
			}
		}

		sort(ranked_arr, ranked_arr+num_accum, com);

		// ranking 순서대로 출력
		double before_weight = 0;
		for(int i=0, j=0; i<num_accum; i++)
		{
			string docname;
			char char_docname[1000];
			string input;
			docs.seekg(ranked_arr[i].doc_position, ios::beg);
			docs >> docname;

			if(before_weight != ranked_arr[i].weight)
				j++;
			before_weight = ranked_arr[i].weight;

			cout << "[" << j << "]" << docname << "         [weight : " << ranked_arr[i].weight << "]" << endl;
			
			strcpy(char_docname, docname.c_str());
			ifstream found_doc(char_docname);
			while(getline(found_doc, input))
            	cout << input << endl;
            found_doc.close();
		}

		fin.close();
		infofile.close();
		dict.close();
		pos.close();
	}

	else	// 말 많은 출력, argv[2] = query file 이름
	{
		if (strcmp(argv[1], "-v"))	// 인자 잘못 입력한 경우 예외처리
		{
		    cout << "Usage : " << argv[0] << " [-v] query" << endl;
			return 0;
		}

		int numwords = 0;
		int freq;	// 이진파일 posting에서의 frequency (단어의 해당 문서 내 빈도) 저장할 변수
		int num_of_docs;
		int temp_doc_pos;
		int temp_freq;
		int num_accum = 0;
		string temp;
		bool found;	// 찾았으면 1, 못찾았으면 0으로 지정해줄 변수
		struct Dict_Term found_entry;	// 단어 찾았을 때, 해당 단어의 Dict_Term Entry정보 저장할 변수

		ifstream fin(argv[2]);	// open the query file
		ifstream infofile("ir.info");
		ifstream dict("ir.dictionary", ios::in | ios::binary);
		ifstream pos("ir.postings", ios::in | ios::binary);
		ifstream docs("ir.docnames");
		//ifstream docs("ir.mydocnames");

		infofile >> num_of_docs;
		// hash table 생성
		struct hash_table ht[num_of_docs];

		// query의 모든 단어들 input[]에 저장
		while(fin >> temp)
			input[numwords++] = temp;

		// query 출력
		cout << "[Query]" << endl;
		for(int i = 0; i < numwords; i++)
			cout << input[i] << " ";
		cout << endl << "---------------------------" << endl;

		for(int i = 0; i < numwords; i++)
		{
			/*
				1. ir.dictioinary에서 Binary Search로 찾기 (Dict_Term의 wordpos를 통해 ir.words의 word와 input[i]랑 비교
				2. 1.에서의 idx로 ir.dictionary의 wordpos와 비교하여 해당 entry에 접근해서 값들 빼오기. (이 때, 이진탐색 사용)
				3. poststart로 ir.postings에 접근하여 문서 내 정보 빼오기.
			*/

			// string to char
			strcpy(input_char, input[i].c_str());
			Capital2Lower(input_char);

			if(BinarySearchDictionary(dict, input_char, found_entry))	// 항목 찾았을 경우
			{
				cout << input_char << "(idf=" << found_entry.idf << "): " << found_entry.numposts << " Posting(s) from " << found_entry.poststart << "-th" << endl;
				found = 1;

				pos.seekg(found_entry.poststart * sizeof(Posting), ios::beg);

				for(int k=0; k<found_entry.numposts; k++)
				{
					pos.read((char *)&temp_doc_pos, sizeof(int));
					pos.read((char *)&temp_freq, sizeof(int));

					// hash table에 추가
					int hash_idx = temp_doc_pos % num_of_docs;
					if(ht[hash_idx].check == 0)	// hash table에 항목이 없으면
					{
						accumulator * new_accum = new accumulator;
						new_accum->doc_position = temp_doc_pos;
						new_accum->weight = temp_freq * found_entry.idf;

						ht[hash_idx].first = new_accum;
						ht[hash_idx].check = 1;
						num_accum++;
					}

					else	// hash table에 항목이 있으면
					{
						accumulator * searching = ht[hash_idx].first;

						while(searching != NULL)
						{
							if(searching->doc_position == temp_doc_pos)	// 동일한 항목이 존재하는 경우 weight 합산
							{
								searching->weight += temp_freq * found_entry.idf;
								break;
							}

							searching = searching->next;
						}

						if(searching == NULL)	// 동일한 항목이 없다면 맨 앞에 추가
						{
							accumulator * new_accum = new accumulator;
							new_accum->doc_position = temp_doc_pos;
							new_accum->weight = temp_freq * found_entry.idf;

							new_accum->next = ht[hash_idx].first;
							ht[hash_idx].first = new_accum;
							num_accum++;
						}
					}
				}
			}
		}

		// if there is no relevant Documents
		if(!found)
		{
			cout << "No Relevant Documents Found!" << endl;
			return 0;
		}

		// ranking 매겨서 저장
		accumulator *ranked_arr = new accumulator[num_accum];
		int j = 0;

		for(int i=0; i<num_of_docs; i++)
		{
			if(ht[i].check == 1)
			{
				accumulator * temp = ht[i].first;

				while(temp != NULL)
				{
					ranked_arr[j].weight = temp->weight;
					ranked_arr[j++].doc_position = temp->doc_position;

					temp = temp->next;
				}
			}
		}

		sort(ranked_arr, ranked_arr+num_accum, com);

		// ranking 순서대로 출력
		double before_weight = 0;
		for(int i=0, j=0; i<num_accum; i++)
		{
			string docname;
			char char_docname[1000];
			string input;
			docs.seekg(ranked_arr[i].doc_position, ios::beg);
			docs >> docname;

			if(before_weight != ranked_arr[i].weight)
				j++;
			before_weight = ranked_arr[i].weight;

			cout << "[" << j << "]" << docname << "         [weight : " << ranked_arr[i].weight << "]" << endl;
			
			strcpy(char_docname, docname.c_str());
			ifstream found_doc(char_docname);
			while(getline(found_doc, input))
            	cout << input << endl;
            found_doc.close();
		}

		fin.close();
		infofile.close();
		dict.close();
		pos.close();
	}
}