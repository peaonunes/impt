#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string> 
#include <vector>
#include <iostream>
#include <chrono>
#include <iostream>
#include <cmath>
#include <typeinfo>
#include <stdio.h>
#include <string.h>

#include "../src/sarray/sarray.h"

using namespace std;
using namespace std::chrono;

// SET HERE DE NUMBER OF EXECUTIONS FOR EACH FILE
const int EXECUTIONS = 10;
const int MICROSECONDS = 1000000;

void calculateMeanTimeAndCompression(vector<float> timestamps, bool type){
	float meanTime = 0;
	char executions[100];strcpy (executions, "");
	char timeValues[100];strcpy (timeValues, "");

	if (type)
		printf("\n######### IPMT #########\n");
	else
		printf("\n######### GZIP #########\n");

	for (int i = 0 ; i < EXECUTIONS; i++){

		string s1 = to_string(i);
		strcat (executions,s1.c_str());
		strcat (executions,"       |");

		string s2 = to_string(timestamps.at(i));
		strcat (timeValues,s2.c_str());
		strcat (timeValues,"|");
		meanTime+=timestamps.at(i);

	}

	printf("%s\n", executions);
	printf("%s\n", timeValues);

	printf("Média do tempo de compressão: %f.\n\n", (meanTime)/EXECUTIONS);
}

void runTest(char* txt, int txtlen){
	int* sarray;
	int* Llcp;
	int* Rlcp;

	vector<float> timestamps;

	high_resolution_clock::time_point t1;
	high_resolution_clock::time_point t2;
	float duration = 0;
	float seconds = 0;

	for (int i = 0; i < EXECUTIONS; i++){

		t1 = high_resolution_clock::now();
		//build_sarray_LRlcp(txt, txtlen, &sarray, &Llcp, &Rlcp);
		
		//void build_sarray_LRlcp(char* text, int text_length, int** sarray, int** Llcp, int** Rlcp);
		//build_sarray_LRlcp(text, size, &sarray, &Llcp, &Rlcp);
	    
	    t2 = high_resolution_clock::now();

    	duration = duration_cast<microseconds>( t2 - t1 ).count();
		seconds = duration/MICROSECONDS;		
		timestamps.push_back(seconds);
	}

	calculateMeanTimeAndCompression(timestamps,true);
}

void runSuite(vector<char*> files){
	printf("\n######################################################");
	printf("\n################# BATERIA DE TESTES ##################");
	printf("\n##################### INDICE #########################\n");
	printf("\nLegenda:\nLinha 1: ID da Execução");
	printf("\nLinha 2: Tempo de Compressão (segundos)\n");

	int size;
	char *text;
	for (int i = 0; i < files.size(); i++){

		FILE *fp = fopen(files.at(i), "r");

		if(fp){

			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			text = (char*)malloc((size + 1)*sizeof(char));
			fread(text, sizeof(char), size, fp);
			text[size] = 0;
			fclose(fp);

			printf("\n######################################################");
			printf("\n################ ARQUIVO ID: %d ######################", (i+1));
			printf("\n######################################################\n");
			printf("Inicializando os testes para o arquivo: %s.\nArquivo de tamanho: %d\n", files.at(i),size);
			
			runTest(text, size);
		} else {
			printf("Arquivo %s fornecido não foi encontrado.\n",files.at(i));
		}
	}
}

int main() {
	// RUN WITH
	// clear && g++ -O3 ../src/sarray/sarray.cpp sarrayTests.cpp -o sarrayTests && ./sarrayTests > INDEX-REPORT.TXT
	//SET HERE THE FILES YOU WANT TO COMPRESS
	vector<char*> files;
	files.push_back("../data/english.1MB");
	//files.push_back("../data/english.2MB");
	//files.push_back("../data/english.5MB");
	//files.push_back("../data/english.10MB");
	runSuite(files);
}