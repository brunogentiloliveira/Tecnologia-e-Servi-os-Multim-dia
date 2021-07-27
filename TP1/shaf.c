/**************************************
###     Author: Bruno Oliveira      ###
###     Number: 81570               ###
###     Created: 10/06/2021         ###
###     Modified: 02/07/2021        ###
###     ----------------------      ###
###     TSM - TP1                   ###
###     Universidade do Minho       ###
**************************************/


#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define BLOCKS 64000
#define FILE 216000

struct frequencia
{
    char character;
    int repeticoes;
    char binario[8];
};

struct blocos 
{
    char *pfile;
    char *efile;
    struct frequencia frequencia[255];
};

int b_size=0, contador = 0;
char *befile;
struct blocos Blocos[10];

/*###Módulo A###*/
float funcRLE(char *bfile)
{
    int tamanho, frequencia, tamanho_compressao=0, original, j=0,aux=0;
    float total;
    char count[5];

    tamanho = strlen(bfile);
    befile=(char*)malloc(sizeof(char)*BLOCKS);
    b_size++;
    printf("Tamanho do Bloco %d: %d\n", b_size, tamanho);

        for(int i=0;i<tamanho;i++)
        {
            frequencia=1;

            while(i < tamanho-1 && bfile[i]==bfile[i+1])
            {
                frequencia++;
                i++;
            }

            if(frequencia > 3)
            {
                sprintf(count,"%d%c%d",aux, bfile[i], frequencia);
                for (int k = 0; *(count + k); k++, j++) 
                {
                    befile[j] = count[k];
                    tamanho_compressao++;
                }
            }else{
                if(bfile[i]=='0'){
                    sprintf(count,"%d%c%d",aux, bfile[i], frequencia);
                    for (int k = 0; *(count + k); k++, j++) 
                    {
                        befile[j] = count[k];
                        tamanho_compressao++;
                    }
                }else{
                    sprintf(count, "%c", bfile[i]);
                    for (int k = 0; k<frequencia;k++, j++) 
                    {
                        befile[j] = count[0];
                        tamanho_compressao++;
                    }
                }
        }

        befile[j]='\0';
        }

    total = (tamanho-tamanho_compressao)*100/tamanho;

    printf("Compressão RLE: %.2f %%\n", total);

    if(b_size == 1)
    {
        printf("Primeiros 160 bytes da codificacao: ");
        for(int i=0;i<160;i++)
        {
            printf("%c", befile[i]);
        }

        printf("\nUltimos 80 bytes da codificacao: ");
        for(int i=strlen(befile)-80;i<strlen(befile);i++)
        {
            printf("%c", befile[i]);
        }
        printf("\n");
    }

    return total;   
}

int getFrequencia(char *cfile, int n)
{
    char charecter[255], *charec=(char*)malloc(sizeof(char)*strlen(cfile)), aux;
    int repeticoes, j, k=0, i, min, swap;

    for(i=0;i<strlen(cfile);i++)
    {
        int aux=-1;

            for(j=0;j<i;j++)
            {
                if(cfile[j]==cfile[i])
                {
                    aux = 0;
                    break;
                }
            }
        
        if(aux == -1)
        {
            charec[k] = cfile[i];
            k++;
        }
    }

    for(int i=0;i<k-1;i++)
    {
        Blocos[n].frequencia[i].character = charec[i];
        Blocos[n].frequencia[i].repeticoes = 0;
        for(j=0;j<strlen(cfile);j++)
        {
            if(Blocos[n].frequencia[i].character == cfile[j])
            {
                Blocos[n].frequencia[i].repeticoes++;
            }
        }
    }

    for (int i = 0; i < k - 1; i++)
    {
        for (int j = i; j < k - 1; j++)
        {
            if (Blocos[n].frequencia[i].repeticoes < Blocos[n].frequencia[j].repeticoes)
            {
                swap = Blocos[n].frequencia[i].repeticoes;
                aux = Blocos[n].frequencia[i].character;
                Blocos[n].frequencia[i].repeticoes = Blocos[n].frequencia[j].repeticoes;
                Blocos[n].frequencia[i].character = Blocos[n].frequencia[j].character;
                Blocos[n].frequencia[j].repeticoes = swap;
                Blocos[n].frequencia[j].character = aux;
            }
        }
    }

    return k;   
}

/* #####Módulo B###### */
void add_bit_code(char bit, int start, int end, int n)
{
    int i=start;
    while (i<=end)
    {
        strncat(Blocos[n].frequencia[i].binario, &bit, 1);
        i = i+1;
    }
}

int best_split(int start, int end, int n)
{
    int total = 0, mindif=0, dif=0, g = 0, divison = start, best=0;

    for(int a = start; a < end; a++)
    {
        total = Blocos[n].frequencia[a].repeticoes + total;
        mindif = total;
        dif = total;
    }

    while (dif==mindif)
    {
        g = g + Blocos[n].frequencia[divison].repeticoes;
        dif = abs(2 * g - total);
        if (dif <= mindif)
        {
            divison = divison + 1;
            mindif = dif;
        }else dif = mindif + 1;
    }
    best = divison - 1;

    return best;
}

void SFalgorithm(int start, int end, int n)
{
    while (Blocos[n].frequencia[end].repeticoes==0) end=end-1;
    if(start != end)
    {
        int div = best_split(start, end, n);
        add_bit_code('0', start, div, n);
        add_bit_code('1', div+1, end, n);
        SFalgorithm(start, div, n);
        SFalgorithm(div+1, end, n);
    }
}

/* #####Módulo C###### */
void BlockCodification(int bloco, int simbolos)
{
    /*char *file=(char*)malloc(sizeof(char)*strlen(Blocos[bloco].efile));*/
    befile=(char*)malloc(sizeof(char)*(simbolos*10));
    char bit = '.';
    int i,j,n, fs;

    for(i=0; i<strlen(Blocos[bloco].efile); i++)
    {
        for(j=0; j<simbolos;j++)
        {
            n=i+1;
            if(Blocos[bloco].frequencia[j].character == Blocos[bloco].efile[i])
            {
                strcat(befile, Blocos[bloco].frequencia[j].binario);
                strncat(befile, &bit, 1);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int fe, fs, size, i=0, aux, block_size=0, option=0, compressed=-1, saida;
    float compressao, total, total2;
    clock_t start, end;


    start=clock();

    printf("Author: Bruno Oliveira\n Number: 81570\n Created: 10/06/2021\n");

    for(aux=0;aux<argc;aux++)
    {
        if(argv[aux]=="-S")
        {
            sscanf(argv[aux+1], "%d", &block_size);
        }

        if(argv[aux]=="-A")
        {
            printf("Apenas será realizado o módulo da Fase A\n");
            option = 1;
        }

        if(argv[aux]=="-B")
        {
            printf("Apenas será realizado o módulo da Fase A e da Fase B\n");
            option = 2;
        }
    }

    if(block_size > 64000)
    {
        for(i=0;i<10;i++)
        {
            Blocos[i].pfile = (char*)malloc(sizeof(char)*block_size);
            Blocos[i].efile = (char*)malloc(sizeof(char)*block_size);
        }
        i=0;
    }else{
        for(i=0;i<10;i++)
        {
            Blocos[i].pfile = (char*)malloc(sizeof(char)*BLOCKS);
            Blocos[i].efile = (char*)malloc(sizeof(char)*BLOCKS);
        }
        i=0;
        block_size = BLOCKS;
    }
    
    fe=open(argv[1], O_RDONLY, 0666);
    printf("Ficheiro de entrada: %s\n", argv[1]);

    if(fe < 0)
    {
        perror("Erro ao abrir o ficheiro");
        return 0;
    }

    size = lseek(fe, 0, SEEK_END);
    lseek(fe, 0, SEEK_SET);

    printf("Tamanho do Ficheiro Original: %d\n", size);
    printf("block size %d\n", block_size);

    fs = open("codificacao.rle", O_CREAT | O_RDWR | O_TRUNC, 0666);

    while (read(fe, Blocos[i].pfile, block_size) != 0)
    {
        compressao = funcRLE(Blocos[i].pfile);
        if(i==0)
        {
            if(compressao < 5)
            {
                compressed = 0;
                break;
            }else{
                compressed = 1;
                strcat(Blocos[i].efile, befile);
                write(fs, Blocos[i].efile, strlen(befile));
                i++;
            }
        }else{
            compressed = 1;
            strcat(Blocos[i].efile, befile);
            write(fs, Blocos[i].efile, strlen(befile));
            i++;
        } 
    }

    saida = lseek(fs, 0, SEEK_END);
    lseek(fe, 0, SEEK_SET);

    close(fe); close(fs);

    if(compressed==1)
    {
        int counter[5], j=0, aux2=0, totals = 0;
        printf("Numero de blocos: %d\n", i);

        for(j=0;j<i;j++)
        {
            counter[j] = getFrequencia(Blocos[j].efile, j);
            totals = counter[j] + totals;
        }

        printf("\tSimbolo \tFrequencia\n");
        /*for(int aux=0;aux<i;aux++)
        {
            for(j=0;j<counter[aux];j++)
            {
                printf("\t%c  \t\t%d\n", Blocos[aux].frequencia[j].character, Blocos[aux].frequencia[j].repeticoes);
            }
        }*/
        for(j=0;j<counter[0];j++)
            {
                printf("\t%c  \t\t%d\n", Blocos[0].frequencia[j].character, Blocos[0].frequencia[j].repeticoes);
            }
        
        end = clock();
        total = (double)(end-start)/CLOCKS_PER_SEC;

        printf("Foi utilizada compressao RLE\n");

        printf("Nome do ficheiro de saida: codificacao.rle\n");
        printf("Tamanho ficheiro de saida: %d\n", saida);
        printf("Tempo de Execução da Fase A: %f segundos\n", total);

        start = clock();
        for(int aux=0;aux<i;aux++)
        {
            SFalgorithm(0, counter[aux], aux);
        }
        
        printf("\tSimbolo \tFrequencia \tShannon-Fano\n");
        /*for(int aux=0;aux<i;aux++)
        {
            for(int j=0; j<counter[aux]-1; j++)
            {
                printf("\t%c  \t\t%d \t\t%s\n", Blocos[aux].frequencia[j].character, Blocos[aux].frequencia[j].repeticoes, Blocos[aux].frequencia[j].binario);
            }
        }*/
        for(int j=0; j<counter[0]; j++)
            {
                printf("\t%c  \t\t%d \t\t%s\n", Blocos[0].frequencia[j].character, Blocos[0].frequencia[j].repeticoes, Blocos[0].frequencia[j].binario);
            }

        end = clock();
        total2 = (double)(end-start)/CLOCKS_PER_SEC;

        printf("Tempo de Execução da Fase B: %f segundos\n", total2);

        start = clock();

        for(j=0;j<i;j++)
        {
            BlockCodification(j, counter[j]);
        }
    }
      
    return 0;
}

