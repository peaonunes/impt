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

// ############################################################################################
// ##
// ## Para executar testes de indexação:
// ##	1. Vá ao main() e adicione no vetor o path dos arquivos que devem ser comprimidos.
// ##	2. Escolha a quantidade de execuções mudando a const int EXECUTIONS.
// ##   3. Complie e execute com:
// ##
// ##	g++ -O3 ../src/sarray/sarray.cpp sarrayTests.cpp -o sarrayTests && ./sarrayTests > IndexArrayReport.txt
// ##
// ##   4. Observe o relatório de saída: IndexArrayReport.txt
// ##
// ## Ps: Considere remover as linhas 60 e 61, quando um número grande de execuções, pois estas
// ## imprimem os valores de saída para cada execução, em forma de "tabela".
// ############################################################################################

const int EXECUTIONS = 1;
const int MICROSECONDS = 1000000;

void calculate_mean_time_execution(vector<float> timestamps, bool type){
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

	printf("Média do tempo de indexação: %f.\n\n", (meanTime)/EXECUTIONS);
}

void run_test(char* txt, int txtlen){
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
		build_sarray_LRlcp(txt, txtlen, &sarray, &Llcp, &Rlcp);
	    t2 = high_resolution_clock::now();

    	duration = duration_cast<microseconds>( t2 - t1 ).count();
		seconds = duration/MICROSECONDS;		
		timestamps.push_back(seconds);
	}

	calculate_mean_time_execution(timestamps,true);
}

void run_suite(vector<char*> files){
	printf("\n######################################################");
	printf("\n################# BATERIA DE TESTES ##################");
	printf("\n##################### INDICE #########################\n");
	printf("\nLegenda:\nLinha 1: ID da Execução");
	printf("\nLinha 2: Tempo de indexação (segundos)\n");

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
			printf("\n################# ARQUIVO ID: %d ######################", (i+1));
			printf("\n######################################################\n");
			printf("Inicializando os testes para o arquivo: %s.\nArquivo de tamanho: %d\n", files.at(i),size);
			
			run_test(text, size);
		} else {
			printf("Arquivo %s fornecido não foi encontrado.\n",files.at(i));
		}
	}
}

int main() {
	vector<char*> files;

	/*CanterburyCorpus
	files.push_back("../data/cantrbry/alice29.txt");
	files.push_back("../data/cantrbry/asyoulik.txt");	
	files.push_back("../data/cantrbry/fields.c");
	files.push_back("../data/cantrbry/grammar.lsp");
	files.push_back("../data/cantrbry/lcet10.txt");
	files.push_back("../data/cantrbry/plrabn12.txt");
	files.push_back("../data/cantrbry/xargs.1");*/
	
	/*
	files.push_back("../data/proteins.1MB");
	files.push_back("../data/proteins.10MB");
	files.push_back("../data/proteins.50MB");
	files.push_back("../data/proteins.100MB");
	*/

	//files.push_back("../data/meComprima_menor.txt");
	//files.push_back("../data/bible.txt");
	//files.push_back("../data/arquivo.txt");
	run_suite(files);
}