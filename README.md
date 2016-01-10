# Projeto 2 - Processamento de Cadeias de Caracteres

(Este arquivo está escrito em [Markdown](https://daringfireball.net/projects/markdown/syntax#link). Para lê-lo em toda sua glória, abra o [repósitório do projeto no Github](https://github.com/peaonunes/impt) ou passe esse texto por algum pré-processador)

## Integrantes:
- Daniel de Jesus Oliveira (djo)
- Rafael Nunes Galdino da Silveira (rngs)
- Túlio Paulo Lages da Silva (tpls)

## Instruções de compilação
Para compilar o projeto, basta executar o comando `make` ou `make ipmt`, e para executá-lo, rodar o executável `ipmt` que será gerado no diretório `bin`. Para mais informações sobre o uso do programa, a opção `--help` está disponível. Além disso, é possível executar o programa através do comando `make run args="<argumentos>"`, onde *argumentos* são os mesmos argumentos da execução normal do `ipmt`, entre aspas.

## Instruções de execução 
Exemplo de uso para indexação:
ipmt index [opções] textfile 

Exemplo de uso para busca:
ipmt search [opções] patterns indexfile


Opções:

-h , --help
Mostra uma versão em inglês destas instruções

-c , --count
Faz com que o programa apenas conte as ocorrências de cada padrão, ao invés de imprimir um trecho do texto ao redor de cada uma

-p , --pattern=<pattern file>
Especifica o arquivo de onde serão lidos os padrões a serem buscados (um por linha)

-x, --compression=<algorithm>	
Opção extra que configura o tipo de compressão

Se um arquivo com padrões não for especificado, o primeiro argumento após as opções dado ao impt será interpretado como o único padrão a ser buscado. Vários arquivos de entrada podem ser especificados.


## Opções de algoritmos

Por default a exeução contará com o algoritmo de compressão Lz78, através da opção -x --compression=Lz77, você pode utilizar o Lz77.