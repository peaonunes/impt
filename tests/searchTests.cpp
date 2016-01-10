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
// ##	g++ -O3 ../src/sarray/sarray.cpp searchTests.cpp -o searchTests && ./searchTests > SearchTestReport.txt
// ##
// ##   4. Observe o relatório de saída: IndexArrayReport.txt
// ##
// ## Ps: Considere remover as linhas 60 e 61, quando um número grande de execuções, pois estas
// ## imprimem os valores de saída para cada execução, em forma de "tabela".
// ############################################################################################

const int EXECUTIONS = 1;
const int MICROSECONDS = 1000000;

char* build_search_cmd(char* filename, char* pattern){
	//grep -o ba teste.txt | wc -l
	char cmd[150];strcpy(cmd,"grep -o -q ");
	strcat(cmd,pattern);
	strcat(cmd," ");
	strcat(cmd,filename);
	strcat(cmd," | wc -l");
	return cmd;
}

void calculate_pattern_mean_time(vector<float> timestamps,char* pattern, bool type){
	float meanTime = 0;
	char executions[100];strcpy (executions, "");
	char timeValues[100];strcpy (timeValues, "");

	if (type)
		printf("\n######### IPMT #########\n");
	else
		printf("\n######### GREP #########\n");

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

	printf("Média do tempo de busca para %s: %f.\n\n", pattern, (meanTime)/EXECUTIONS);
}

void run_test(char* text, int size, char* fileName, vector<char*> patterns){

	int* Llcp; int* Rlcp;
	int* sarray;
	build_sarray_LRlcp(text, size, &sarray, &Llcp, &Rlcp);

	high_resolution_clock::time_point t1;
	high_resolution_clock::time_point t2;
	float duration;
	float seconds;
	int matches_start;
 	int matches_end;
 	char* cmd;
 	int patlen = 0;

	for (int j = 0; j < patterns.size(); j++){

		cmd = build_search_cmd(fileName, patterns.at(j));
		patlen = strlen(patterns.at(j));

		vector<float> timestamps;
		vector<float> timestampsGREP;

		duration = 0;
		seconds = 0;

		matches_start = 0;
		matches_end = 0;
		
		high_resolution_clock::time_point t1;
		high_resolution_clock::time_point t2;
		
		for (int i = 0; i < EXECUTIONS; i++){
			// ################################## CALL IMPT ##########################################
			t1 = high_resolution_clock::now();
			find_occurrences(&matches_start,&matches_end,text,size,patterns.at(j),patlen,sarray, Llcp, Rlcp);
			t2 = high_resolution_clock::now();

	    	duration = duration_cast<microseconds>( t2 - t1 ).count();
			seconds = duration/MICROSECONDS;		
			timestamps.push_back(seconds);

			// ################################## CALL GREP ##########################################
			t1 = high_resolution_clock::now();
			system (cmd);
			t2 = high_resolution_clock::now();

			duration = duration_cast<microseconds>( t2 - t1 ).count();
			seconds = duration/MICROSECONDS;
			timestampsGREP.push_back(seconds);
		}

		calculate_pattern_mean_time(timestamps,patterns.at(j),true);
		calculate_pattern_mean_time(timestampsGREP,patterns.at(j),false);
	}

}

void run_suite(vector<char*> files, vector<char*> patterns){

	printf("\n######################################################");
	printf("\n################# BATERIA DE TESTES ##################");
	printf("\n###################### BUSCA #########################\n");
	printf("\nLegenda:\nLinha 1: ID da Execução");
	printf("\nLinha 2: Tempo de busca (segundos)\n");

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
			
			run_test(text, size, files.at(i), patterns);
		} else {
			printf("Arquivo %s fornecido não foi encontrado.\n",files.at(i));
		}
	}

}

vector<char*> build_patterns(){
	vector<char*> patterns;
	patterns.push_back("Q");
	patterns.push_back("KS");
	patterns.push_back("QPA");
	patterns.push_back("ANCC");
	patterns.push_back("REVSD");
	patterns.push_back("SLEVQQ");
	return patterns;
}

int main(){
	vector<char*> files;
	files.push_back("../data/proteins.1MB");
	vector<char*> patterns = build_patterns();
	run_suite(files,patterns);
}
