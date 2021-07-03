/**************************************
###     Author: Bruno Oliveira      ###
###     Number: 81570               ###
###     Created: 10/06/2021         ###
###     Modified: 02/07/2021        ###
###     ----------------------      ###
###     TSM - TP1                   ###
###     Módulo Fase A               ###
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

struct blocos 
{
    char *pfile;
    char *efile;
};

struct frequencia
{
    char character;
    int repeticoes;
};

int b_size;
char file[216000];
char *befile;

float funcRLE(char *bfile)
{
    int tamanho, frequencia, tamanho_compressao=0, original, j=0,aux=0;
    float total;
    char count[5];

    befile=(char*)malloc(sizeof(char)*BLOCKS);
    tamanho = strlen(bfile);

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

    printf("Tamanho Ficheiro Comprimido: %d\n", tamanho_compressao);

    total = (tamanho-tamanho_compressao)*100/tamanho;

    printf("Compressão RLE: %.2f %%\n", total);

    for(int i=0;i<strlen(befile);i++)
    {
        printf("%c", befile[i]);
    }

    return total;   
}

void getFrequencia(char *cfile)
{
    char charecter[255], *charec=(char*)malloc(sizeof(char)*strlen(cfile)), aux;
    int repeticoes, j, k=0, i, min, swap;
    struct frequencia frequencia[255];

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
        frequencia[i].character = charec[i];
        frequencia[i].repeticoes = 0;
        for(j=0;j<strlen(cfile);j++)
        {
            if(frequencia[i].character == cfile[j])
            {
                frequencia[i].repeticoes++;
            }
        }
    }

    for (int i = 0; i < k - 1; i++)
    {
        for (int j = i; j < k - 1; j++)
        {
            if (frequencia[i].repeticoes < frequencia[j].repeticoes)
            {
                swap = frequencia[i].repeticoes;
                aux = frequencia[i].character;
                frequencia[i].repeticoes = frequencia[j].repeticoes;
                frequencia[i].character = frequencia[j].character;
                frequencia[j].repeticoes = swap;
                frequencia[j].character = aux;
            }
        }
    }

    printf("\tSimbolo \tFrequencia\n");
    for(i=0;i<k-1;i++)
    {
        printf("\t%c  \t\t%d\n", frequencia[i].character, frequencia[i].repeticoes);
    }
    
}

int main(int argc, char *argv[])
{
    int fe, size, i=0, aux, block_size, option=0, compressao=1;
    float compressao;
    struct blocos Blocos[10];

    printf("Author: Bruno Oliveira\n Number: 81570\n Created: 10/06/2021\n");

    for(aux=0;aux<argc;aux++)
    {
        if(argv[aux]=="-S")
        {
            sscanf(argv[aux+1], "%d", &block_size);
        }

        if(argv[aux]=="-A")
        {
            printf("Apenas será realizado o módulo da Fase A");
            option = 1;
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
    }
    
    fe=open(argv[1], O_RDONLY, 0666);
    printf("Ficheiro de entrada: %s", argv[1]);

    if(fe < 0)
    {
        perror("Erro ao abrir o ficheiro");
        return 0;
    }

    size = lseek(fe, 0, SEEK_END);
    lseek(fe, 0, SEEK_SET);

    printf("Tamanho do Ficheiro Original: %d\n", size);

    while (read(fe, Blocos[i].pfile, strlen(Blocos[i].pfile)) != 0)
    {
        compressao = funcRLE(Blocos[i].pfile);
        if(i==0)
        {
            if(compressao < 5)
            {
                compressao = 0;
                break;
            }else{
                strcat(Blocos[i].efile, befile); 
                i++;
            }
        } 
    }

    if(compressao==1)
    {
        printf("Numero de blocos: %d\n", i);

        for(int j=1;j<i+1;j++)
        {
            printf("Bloco %d\n", j);
            getFrequencia(Blocos[j-1].efile);
        }
    }
    
    
    
    return 0;
}

