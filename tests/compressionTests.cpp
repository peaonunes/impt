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
#include "../src/compression/lz78.h"

using namespace std;
using namespace std::chrono;

// ############################################################################################
// ##
// ## Para executar testes de compressão e descompressão:
// ##	1. Vá ao main() e adicione no vetor o path dos arquivos que devem ser comprimidos.
// ##	2. Escolha a quantidade de execuções mudando a const int EXECUTIONS.
// ##   3. Complie e execute com:
// ##
// ##	g++ -O3 ../src/compression/lz77.cpp ../src/compression/lz78.cpp compressionTests.cpp -o compressionTests && ./compressionTests > CompressionTestReport.txt
// ##
// ##   4. Observe o relatório de saída: CompressionTestReport.txt
// ##
// ## Ps: Considere remover as linhas 83 a 87, quando um número grande de execuções, pois estas
// ## imprimem os valores de saída para cada execução, em forma de "tabela".
// ############################################################################################

const int EXECUTIONS = 1;
const int MICROSECONDS = 1000000;

void calculate_mean_time_and_compression(vector<float> timestamps,vector<float> taxCompressions,vector<float> decompressionTime,vector<int> decompressionError,int type){
	float meanTime = 0;
	float meanTaxCompression = 0;
	float meanDecompressionTime = 0;
	float meanError = 0;
	char executions[150];strcpy (executions, "");
	char timeValues[150];strcpy (timeValues, "");
	char taxValues[150];strcpy (taxValues, "");
	char decompressValue[150];strcpy (decompressValue, "");
	char error[150];strcpy (error, "");

	if (type == 0)
		printf("\n######### IPMT lz77 #########\n");
	else if (type == 1)
		printf("\n######### IPMT lz78 #########\n");	
	else
		printf("\n############ GZIP ###########\n");

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
	printf("Tamanho do arquivo após compressão: %f%c.\n", (meanTaxCompression)/EXECUTIONS, '%');
	printf("Média do tempo de descompressão: %f.\n", (meanDecompressionTime)/EXECUTIONS);
	printf("Média de erro: %f.\n\n", (meanError)/EXECUTIONS);
}

int get_file_size(char* filename)
{
    FILE *p_file = NULL;
    p_file = fopen(filename,"rb");
    fseek(p_file,0,SEEK_END);
    int size = (int)ftell(p_file);
    fclose(p_file);
    return size;
}

char* build_decompress_cmd(char gzipFileName[], char fileName[], int i){
	char cmd[150];strcpy(cmd,"gzip -cd ");
	strcat(cmd,gzipFileName);
	strcat(cmd," > ");
	strcat(cmd,fileName);
	strcat(cmd,".ok");
	return cmd;
}

char* build_compress_cmd(char* filename, char gzipFileName[]){
	char cmd[150];strcpy(cmd,"gzip -c ");
	strcat(cmd,filename);
	strcat(cmd," > ");
	strcat(cmd,gzipFileName);
	return cmd;
}

void run_test(char* txt, int txtlen, char cmd[], char fileName[], char gzipFileName[]){
	// SET LS & LL
	int search_window_length =  (1 << 12) - 1;
	int lookahead_length = (1 << 4) - 1;
	
	uint32_t codelen = 0;
	uint32_t codelen2 = 0;

	vector<float> timestamps;
	vector<float> taxCompressions;
	vector<float> decompressionTime;
	vector<int> decompressionError;

	vector<float> timestamps78;
	vector<float> taxCompressions78;
	vector<float> decompressionTime78;
	vector<int> decompressionError78;

	vector<float> timestampsGZIP;
	vector<float> taxCompressionsGZIP;
	vector<float> decompressionTimeGZIP;
	vector<int> decompressionErrorGZIP;

	high_resolution_clock::time_point t1;
	high_resolution_clock::time_point t2;

	uint8_t* encoded_text;
	uint8_t* encoded_text78;
	char* decoded_text = (char*)malloc((txtlen+1)*sizeof(char));
	//char* decoded_text = (char*)malloc(txtlen+1);
	//char* decoded_text78 = (char*)malloc((txtlen+1)*sizeof(char));

	float duration = 0;
	float seconds = 0;
	float c = 0;
	
	for (int i = 0; i < EXECUTIONS; i++){
		// ################################## COMPRESSION ########################################
		// ################################## CALL lz77 ##########################################
		t1 = high_resolution_clock::now();
		encoded_text = lz77_encode(txt, txtlen, search_window_length, lookahead_length, &codelen);	
	    t2 = high_resolution_clock::now();

    	duration = duration_cast<microseconds>( t2 - t1 ).count();
		seconds = duration/MICROSECONDS;
		c = (float)codelen/(float)txtlen;
		
		timestamps.push_back(seconds);
		taxCompressions.push_back(c);

		//################################## CALL lz78 ##########################################
		/*t1 = high_resolution_clock::now();
		encoded_text78 = lz78_encode (txt, txtlen, &codelen2);
	    t2 = high_resolution_clock::now();

    	duration = duration_cast<microseconds>( t2 - t1 ).count();
		seconds = duration/MICROSECONDS;
		c = (float)codelen2/(float)txtlen;
		
		timestamps78.push_back(seconds);
		taxCompressions78.push_back(c);*/

		// ################################## CALL GZIP ##########################################
		t1 = high_resolution_clock::now();
		system (cmd);
		t2 = high_resolution_clock::now();

		duration = duration_cast<microseconds>( t2 - t1 ).count();
		seconds = duration/MICROSECONDS;
		c = (float)get_file_size(gzipFileName)/(float)txtlen;
		
		timestampsGZIP.push_back(seconds);
		taxCompressionsGZIP.push_back(c);

		//################################## DECOMPRESSION ########################################
		//################################## CALL lz77 ###########################################
		t1 = high_resolution_clock::now();
		lz77_decode(encoded_text, codelen, search_window_length, lookahead_length, decoded_text);
	    t2 = high_resolution_clock::now();

	    duration = duration_cast<microseconds>( t2 - t1 ).count();
		seconds = duration/MICROSECONDS;

		decompressionTime.push_back(seconds);
	    decompressionError.push_back(strcmp(txt,decoded_text));

	    //################################## CALL lz78 ##########################################
	    
	    /*t1 = high_resolution_clock::now();
		decoded_text78 = lz78_decode (encoded_text78, codelen2, txtlen);	
	    t2 = high_resolution_clock::now();

	    duration = duration_cast<microseconds>( t2 - t1 ).count();
		seconds = duration/MICROSECONDS;

		decompressionTime78.push_back(seconds);
	    decompressionError78.push_back(strcmp(txt,decoded_text78));*/
		
	    // ################################## CALL GZIP ##########################################
	    char* cmd2 = build_decompress_cmd(gzipFileName,fileName,i);
	    t1 = high_resolution_clock::now();
		system (cmd2);
		t2 = high_resolution_clock::now();

		duration = duration_cast<microseconds>( t2 - t1 ).count();
		seconds = duration/MICROSECONDS;
		decompressionTimeGZIP.push_back(seconds);
		decompressionErrorGZIP.push_back(0);
	}

	calculate_mean_time_and_compression(timestamps,taxCompressions,decompressionTime,decompressionError,0);
	//calculate_mean_time_and_compression(timestamps78,taxCompressions78,decompressionTime78,decompressionError78,1);
	calculate_mean_time_and_compression(timestampsGZIP,taxCompressionsGZIP,decompressionTimeGZIP,decompressionErrorGZIP,2);

	free(decoded_text);
	//free(decoded_text78);
}

void run_suite(vector<char*> files){

	printf("\n######################################################");
	printf("\n################# BATERIA DE TESTES ##################");
	printf("\n############# COMPRESSAO E DESCOMPRESSAO #############\n");
	printf("\nLegenda:\nLinha 1: ID da Execução");
	printf("\nLinha 2: Tempo de Compressão (segundos)");
	printf("\nLinha 3: Tamanho relativo ao tamanho original do arquivo original");
	printf("\nLinha 4: Tempo de Descompressão");
	printf("\nLinha 5: Erro de Descompressão\n\n");

	for (int i = 0; i < files.size(); i++){

		FILE *fp = fopen(files.at(i), "r");
	
		fseek(fp, 0, SEEK_END);
		int txtlen = (int)ftell(fp);

		fseek(fp, 0, SEEK_SET);
		char* txt;// = (char*)malloc(txtlen+1);

		fread(txt, 1, txtlen, fp);
		txt[txtlen] = 0;

		printf("\n######################################################");
		printf("\n################## ARQUIVO ID: %d #####################", (i+1));
		printf("\n######################################################\n");
		printf("Inicializando os testes para o arquivo: %s.\nArquivo de tamanho: %d\n", files.at(i),txtlen);
		
		char gzipFileName[150];strcpy(gzipFileName,files.at(i));
		strcat(gzipFileName,".gz");

		char* cmd = build_compress_cmd(files.at(i),gzipFileName);
		
		//run_test(txt, txtlen, cmd, files.at(i), gzipFileName);
		free(txt);
	}
}

int main() {
	vector<char*> files;
	files.push_back("../data/english.1MB");
	//files.push_back("../data/english.2MB");
	//files.push_back("../data/english.5MB");
	//files.push_back("../data/english.10MB");
	//files.push_back("../data/english.25MB");
	//files.push_back("../data/english.50MB");
	//files.push_back("../data/english.100MB");
	//files.push_back("../data/english.200MB");
	//files.push_back("../data/english.500MB");
	//files.push_back("../data/english.1024MB");
	run_suite(files);
}