//Alunos:
//Diego Queiroz
//Emerson Veiga
//Keny Lucas Souza
//Mario Haddad

#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

char PATH[200] = "";
int waiting = 0;

//função para ler comando do terminal independete dos espaços
void lerCommando (char string[]){
	scanf("%[^\n]%*c", string);
}

//função para saber o tamanho de um vetor
int tamanho(char *v[]){
	int i=0;
	while(v[i]!=0){
		i++;
	}
	return i;
}

//função para imprimir um vetor de strings
void imprimirVetor(char *vet[]){
	int i = 0;
	while (vet[i] != NULL){
		printf("vet[%d] = %s\n",i,vet[i]);
		i++;
	}
}

//splita um caminho
void splitPathing(char string[], char *vet[]){
	int i = 0;
	char *token;
	token = strtok(string, "/");
	while (token != NULL){
		vet[i] = token;
		i++;
		token = strtok(NULL, "/");
	}
	if(i<100){
		vet[i]=NULL;
	}
}

//função que atualiza o PATH global de onde estamos
void updatePath(char string[]){
	int lastIndex = 0;
	char *vetPath[100];
	splitPathing(PATH, vetPath);
	for(; lastIndex<100; lastIndex++){
		if(vetPath[lastIndex]==NULL){
			break;
		}
	}
	lastIndex--;
	char *vetString[100];
	splitPathing(string, vetString);
	
	for(int i=0; i<100; i++){
		if(vetString[i]==NULL){
			break;
		}
		if(strcmp(vetString[i],"..")==0){
			if(lastIndex>=0){
				vetPath[lastIndex]=NULL;
				lastIndex--;
			}
		}
		else if(strcmp(vetString[i],".")==0){

		}else{
			if(lastIndex<99){
				lastIndex++;
				vetPath[lastIndex] = (char*) malloc(sizeof(char)*100);
				strcat(vetPath[lastIndex], vetString[i]);
			}
		}
	}

	char PATH_buffer[100] = "";
	for(int i = 0; i<=lastIndex; i++){
		strcat(PATH_buffer, "/");
		strcat(PATH_buffer, vetPath[i]);
	}
	strcpy(PATH, PATH_buffer);
}

//função que splita uma string por espaços
void split(char *string, char *vet[]){
	int i = 0;
	char *token;
	token = strtok(string, " ");
	while (token != NULL){
		vet[i] = token;
		i++;
		token = strtok(NULL, " ");
	}
}

//função que executa comando do sistema
int executarComando(char string[]){
	int valor = system(string);
	return valor;
}

//função que chama o comando ls
void ls(){
	char buffer[100];
	strcpy(buffer,"ls ");
	strcat(buffer, PATH);
	strcat(buffer, "/");
	executarComando(buffer);
}

//função que chama o comando cd mais string
void cd(char string[]){
	char buffer[100];
	char bufferAux[100]; 
	strcpy(buffer,"cd ");
	strcat(buffer, PATH);
	strcat(buffer, "/");
	strcpy(bufferAux,buffer);
	strcat(bufferAux,string);
	if (executarComando(bufferAux) == 0){
		updatePath(string);
	}
}

//função para concatenar strings 
char *concat(char *s1, char *s2){
    char *s = malloc(strlen(s1)+strlen(s2)+1);
    strcpy(s,s1);
    strcat(s,s2);
    return s;
}

//função que executa um programa com argumentos
void executaPrograma(char *vet[]){
	int i;
    int tam = tamanho(vet);
	int out;
	int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork falhou\n");
        exit(1);
    } else if (rc == 0) { 
		if(execvp(vet[0],vet) == -1){
			char *s = concat("./",vet[0]);
			vet[0] = s;
		}
        // printf("zsh: command not found. ERROR: %d\n",execvp(vet[0],vet));
		printf("zsh: command not found. ERROR: %d\n",execvp(vet[0],vet));
    } else {
		int wc = wait(NULL);
    }
	if (strcmp(vet[0],"cat") == 0 ){
		printf("\n");
	}
}

//função que executa um programa com argumentos com &
void executaProgramaSemWaiting(char *vet[]){
	int i;
    int tam = tamanho(vet);
	int out;
	int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork falhou\n");
        exit(1);
    } else if (rc == 0) { 
        if(execvp(vet[0],vet) == -1){
			char *s = concat("./",vet[0]);
			vet[0] = s;
		}
        // printf("zsh: command not found. ERROR: %d\n",execvp(vet[0],vet));
		printf("zsh: command not found. ERROR: %d\n",execvp(vet[0],vet));
    }
	if (strcmp(vet[0],"cat") == 0 ){
		printf("\n");
	}
}

//função que executa o programa com o modificador >
void executaProgramaMaiorQue(char *vet[]){
    int i;
    int tam = tamanho(vet);
	imprimirVetor(vet);
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork falhou\n");
        exit(1);
    } else if (rc == 0) {       
		close(STDOUT_FILENO);
		int out = open(vet[tam-1], O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
		
		printf("%d\n",execvp(vet[0], vet));
	
		if(execvp(vet[0],vet) == -1){
			close(out);
			fprintf(stderr, "%s :programa inexistente no diretório atual\n",vet[0]);
			exit(EXIT_FAILURE); 
		}
    } else {
		int wc = wait(NULL);
    }
}
 
//função que executa o programa com o modificador <
void executaProgramaMenorQue(char *vet[]){
    int i;
    int tam = tamanho(vet);
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork falhou\n");
        exit(1);
    } else if (rc == 0) {
		close(0);
		int in = open(vet[tam-1], O_RDONLY);

		printf("%d\n",execvp(vet[0],vet));
		
		if(execvp(vet[0],vet) == -1){
			close(in);
			fprintf(stderr, "%s :programa inexistente no diretório atual\n",vet[0]);
			exit(EXIT_FAILURE); 
		}
    } else {
		int wc = wait(NULL);
    }
}

//função que executa o programa com o modificador |
void executaProgramaPipe(char *vet[]){
    int i;
    int tam = tamanho(vet);
	char **arrProg1 = malloc(sizeof(char*) * 100);
	char **arrProg2 = malloc(sizeof(char*) * 100);
	arrProg1[0] = vet[0]; arrProg1[1] = ">"; arrProg1[2] = "bufferTemporario";
	arrProg2[0] = vet[2]; arrProg2[1] = "<"; arrProg2[2] = "bufferTemporario";
	int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork falhou\n");
        exit(1);
    } else if (rc == 0) {
		executaProgramaMaiorQue(arrProg1);
		executaProgramaMenorQue(arrProg2);
		remove("bufferTemporario");
    } else {
		int wc = wait(NULL);
    }
}

//função que recebe uma string e verifica se a stirng é igual a ls ou cd
void parser(char *vet[]) {
	int tam = tamanho(vet);
	if (strcmp(vet[0], "ls") == 0) {
        ls();
	} else if (strcmp(vet[0], "exit") == 0) { 
		exit(1);
	} else if (strcmp(vet[0], "cd") == 0 && tamanho(vet)==2) {
        cd(vet[1]);
	} else if (tamanho(vet) > 1 && strcmp(vet[tam-2], "<") == 0) {	
   		executaProgramaMenorQue(vet);
    } else if (tamanho(vet) > 1 && strcmp(vet[tam-2], ">") == 0) {	
   		executaProgramaMaiorQue(vet);
	} else if (tamanho(vet) > 1 && strcmp(vet[tam-2], "|") == 0) {	
		executaProgramaPipe(vet);
    } else if (tamanho(vet) > 1 && strcmp(vet[tam-1], "&") == 0) {
		executaProgramaSemWaiting(vet);
		//alterar variavel global do wait
		//bloquear saida paddrão do programa
    } else {
		executaPrograma(vet);
    }
}

//função para limpar um vetor
void limparVetor(char *vet[]){	
	int i = 0;
	while (vet[i] != NULL){
		vet[i] = NULL;
		i++;
	}
}

//função que retorna o PATH atual
void pwdReturn(){
	getcwd(PATH, sizeof(PATH));
}

// função main que mantém o terminal em loop e aloca as strings, arquivos e arrays.
void main() {
	char string[100];
	char **arr = malloc(sizeof(char*) * 100);
	FILE* arq;
	arq = fopen("entrada.txt","r");
	pwdReturn();
	while (1) {
		string[0] = '\0';
		printf("\033[1;31m"); printf("$%s ",PATH); printf("\033[0m");
		lerCommando(string);
		split(string, arr);
		parser(arr);
		limparVetor(arr);
		waiting = 0;
	}
}
