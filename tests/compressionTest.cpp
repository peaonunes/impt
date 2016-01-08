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

#include "../src/compression/lz77.h"

using namespace std;
using namespace std::chrono;


// SET HERE DE NUMBER OF EXECUTIONS FOR EACH FILE
const int EXECUTIONS = 10;
const int MICROSECONDS = 1000000;

void calculateMeanTimeAndCompression(vector<float> timestamps,vector<float> taxCompressions,vector<float> decompressionTime,vector<int> decompressionError,bool type){
	float meanTime = 0;
	float meanTaxCompression = 0;
	float meanDecompressionTime = 0;
	float meanError = 0;
	char executions[100];strcpy (executions, "");
	char timeValues[100];strcpy (timeValues, "");
	char taxValues[100];strcpy (taxValues, "");
	char decompressValue[100];strcpy (decompressValue, "");
	char error[100];strcpy (error, "");

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

		string s3 = to_string(taxCompressions.at(i));
		strcat (taxValues,s3.c_str());
		strcat (taxValues,"|");
		meanTaxCompression+=taxCompressions.at(i);

		string s4 = to_string(decompressionTime.at(i));
		strcat (decompressValue,s4.c_str());
		strcat (decompressValue,"|");
		meanDecompressionTime+=decompressionTime.at(i);

		string s5 = to_string(decompressionError.at(i));
		strcat (error,s5.c_str());
		strcat (error,"       |");
		meanError+=decompressionError.at(i);
		
	}

	printf("%s\n", executions);
	printf("%s\n", timeValues);
	printf("%s\n", taxValues);
	printf("%s\n", decompressValue);
	printf("%s\n\n", error);

	printf("Média do tempo de compressão: %f.\n", (meanTime)/EXECUTIONS);
	printf("Média da taxa de compressão: %f.\n", (meanTaxCompression)/EXECUTIONS);
	printf("Média do tempo de descompressão: %f.\n", (meanDecompressionTime)/EXECUTIONS);
	printf("Média de erro: %f.\n\n", (meanError)/EXECUTIONS);
}

int getFileSize(char* filename)
{
    FILE *p_file = NULL;
    p_file = fopen(filename,"rb");
    fseek(p_file,0,SEEK_END);
    int size = (int)ftell(p_file);
    fclose(p_file);
    return size;
}

char* buildDeCompressCmd(char gzipFileName[], char fileName[], int i){
	char cmd[100];strcpy(cmd,"gzip -cd ");
	strcat(cmd,gzipFileName);
	strcat(cmd," > ");
	strcat(cmd,fileName);
	strcat(cmd,".ok");
	return cmd;
}

char* buildCompressCmd(char* filename, char gzipFileName[]){
	char cmd[50];strcpy(cmd,"gzip -c ");
	strcat(cmd,filename);
	strcat(cmd," > ");
	strcat(cmd,gzipFileName);
	return cmd;
}

void runTest(char* txt, int txtlen, char cmd[], char fileName[], char gzipFileName[]){
	// SET LS & LL
	int search_window_length =  (1 << 12) - 1;
	int lookahead_length = (1 << 4) - 1;
	
	uint32_t codelen = 0;

	vector<float> timestamps;
	vector<float> taxCompressions;
	vector<float> decompressionTime;
	vector<int> decompressionError;

	vector<float> timestampsGZIP;
	vector<float> taxCompressionsGZIP;
	vector<float> decompressionTimeGZIP;
	vector<int> decompressionErrorGZIP;

	high_resolution_clock::time_point t1;
	high_resolution_clock::time_point t2;

	uint8_t* encoded_text;
	char* decoded_text = (char*)malloc((txtlen+1)*sizeof(char));

	float duration = 0;
	float seconds = 0;
	float c = 0;
	
	for (int i = 0; i < EXECUTIONS; i++){
		// ################################## COMPRESSION ########################################
		// ################################## CALL IPMT ##########################################
		t1 = high_resolution_clock::now();
		encoded_text = lz77_encode(txt, txtlen, search_window_length, lookahead_length, &codelen);	
	    t2 = high_resolution_clock::now();

    	duration = duration_cast<microseconds>( t2 - t1 ).count();
		seconds = duration/MICROSECONDS;
		c = (float)codelen/(float)txtlen;
		
		timestamps.push_back(seconds);
		taxCompressions.push_back(c);
		
		// ################################## CALL GZIP ##########################################
		t1 = high_resolution_clock::now();
		system (cmd);
		t2 = high_resolution_clock::now();

		duration = duration_cast<microseconds>( t2 - t1 ).count();
		seconds = duration/MICROSECONDS;
		c = (float)getFileSize(gzipFileName)/(float)txtlen;
		
		timestampsGZIP.push_back(seconds);
		taxCompressionsGZIP.push_back(c);

		//################################## DECOMPRESSION ##########################################
		//################################## CALL IPMT ##########################################
		t1 = high_resolution_clock::now();
		lz77_decode(encoded_text, codelen, search_window_length, lookahead_length, decoded_text);
	    t2 = high_resolution_clock::now();

	    duration = duration_cast<microseconds>( t2 - t1 ).count();
		seconds = duration/MICROSECONDS;

		decompressionTime.push_back(seconds);
	    decompressionError.push_back(strcmp(txt,decoded_text));

	    // ################################## CALL GZIP ##########################################
	    char* cmd2 = buildDeCompressCmd(gzipFileName,fileName,i);
	    t1 = high_resolution_clock::now();
		system (cmd2);
		t2 = high_resolution_clock::now();

		duration = duration_cast<microseconds>( t2 - t1 ).count();
		seconds = duration/MICROSECONDS;
		decompressionTimeGZIP.push_back(seconds);
		decompressionErrorGZIP.push_back(0);
	}

	calculateMeanTimeAndCompression(timestamps,taxCompressions,decompressionTime,decompressionError,true);
	calculateMeanTimeAndCompression(timestampsGZIP,taxCompressionsGZIP,decompressionTimeGZIP,decompressionErrorGZIP,false);
}

void runSuite(vector<char*> files){

	printf("\n######################################################");
	printf("\n################# BATERIA DE TESTES ##################");
	printf("\n############# COMPRESSAO E DESCOMPRESSAO #############\n");
	printf("\nLegenda:\nLinha 1: ID da Execução");
	printf("\nLinha 2: Tempo de Compressão (segundos)");
	printf("\nLinha 3: Taxa de Compressão (tamanho final com relação ao original)");
	printf("\nLinha 4: Tempo de Descompressão");
	printf("\nLinha 5: Erro de Descompressão\n\n");

	for (int i = 0; i < files.size(); i++){

		FILE *fp = fopen(files.at(i), "r");
	
		fseek(fp, 0, SEEK_END);
		int txtlen = (int)ftell(fp);

		fseek(fp, 0, SEEK_SET);
		char* txt = (char*)malloc(txtlen+1);

		fread(txt, 1, txtlen, fp);
		txt[txtlen] = 0;

		printf("\n######################################################");
		printf("\n################ ARQUIVO ID: %d ######################", (i+1));
		printf("\n######################################################\n");
		printf("Inicializando os testes para o arquivo: %s.\nArquivo de tamanho: %d\n", files.at(i),txtlen);
		
		char gzipFileName[100];strcpy(gzipFileName,files.at(i));
		strcat(gzipFileName,".gz");

		char* cmd = buildCompressCmd(files.at(i),gzipFileName);
		
		runTest(txt, txtlen, cmd, files.at(i), gzipFileName);
	}

}

int main() {
	// RUN WITH
	// clear && g++ -O3 ../src/compression/lz77.cpp compressionTest.cpp -o compressionTests && time ./compressionTests > REPORT200.TXT
	//SET HERE THE FILES YOU WANT TO COMPRESS
	vector<char*> files;
	//files.push_back("../data/english.1MB");
	//files.push_back("../data/english.2MB");
	//files.push_back("../data/english.5MB");
	//files.push_back("../data/english.10MB");
	//files.push_back("../data/english.25MB");
	//files.push_back("../data/english.50MB");
	//files.push_back("../data/english.100MB");
	files.push_back("../data/english.200MB");


	runSuite(files);
}