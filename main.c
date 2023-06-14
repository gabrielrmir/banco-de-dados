#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Nota {
	int id;
	float nota1;
	float nota2;
	float nota3;
	struct Nota *next;
} Nota;

typedef struct Node {
	int id;
	struct Node *next;
} Node;

typedef struct {
	int id;
	char *nome;
	Node *disciplinas;
} Curso;

typedef struct {
	int id;
	int carga;
	char *nome;
	Node *turmas;
	Nota *notas;
} Disciplina;

typedef struct {
	int id;
	int professor;
	char turno;
	Node *alunos;
	// horario
} Turma;

typedef struct {
	int id;
	char *nome;
	char *email;
	int cpf;
	int telefone;
	int ing_semestre;
	int ing_ano;
} Aluno;

typedef struct {
	int id;
	int cpf;
	int telefone;
	char *nome;
	char *email;
} Professor;

void input(void *target, char t) {
	int c, i;
	size_t l, size = 256;
	char buffer[256];

	while (1) {
		fgets(buffer, size, stdin);

		l = strlen(buffer);
		if (l == 1) continue;

		if (strchr(buffer, '\n') != NULL) {
			buffer[strcspn(buffer, "\n")] = 0;
			if (t == 'c') {
				if (l != 2) continue;
				memcpy(target, &buffer[0], sizeof(char));
			} else if (t == 'f') {
				char *endptr;
				float f = strtof(buffer, &endptr);
				if (*endptr != '\0' || f < 0) continue;
				memcpy(target, &f, sizeof(float));
			} else if (t == 'd') {
				char *endptr;
				int n = strtol(buffer, &endptr, 10);
				if (*endptr != '\0' || n < 0) continue;
				memcpy(target, &n, sizeof(int));
			} else if (t == 's') {
				buffer[strcspn(buffer, "\n")] = 0;
				char **str = (char **) target;
				*str = (char *) malloc(strlen(buffer)+1);
				strcpy(*str, buffer);
			}
			break;
		} else {
			while ((c = getchar()) != '\n' && c != EOF);
			continue;
		}
	}
}

// void inputstr(char **str) {
// 	size_t len;
// 	int c;
// 	char buffer[256];
// 	while (1) {
// 		fgets(buffer, 256, stdin);
// 		len = strlen(buffer);
// 		if (len == 1) continue;
// 		if (strchr(buffer, '\n') != NULL) {
// 			buffer[strcspn(buffer, "\n")] = 0;
// 			*str = (char *) malloc(len+2);
// 			strcpy(*str, buffer);
// 			break;
// 		} else {
// 			while ((c = getchar()) != '\n' && c != EOF);
// 			continue;
// 		}
// 	}
// }

// Ex: .000, .001, .002, ..., .999
// Retorna a primeira linha a ser deletada
// Caso a linha nao foi encontrada, retorna 0
int deletarid(int id, char *arquivo) {
	int id_atual = -1, sucesso = 0, linha = 0, i = 1;
	char buffer[256];
	FILE *ptr1 = fopen(arquivo, "r");
	FILE *ptr2 = fopen("temp.txt", "w");
	while (fgets(buffer, 256, ptr1)) {
		if (buffer[0] == '.') {
			id_atual = atoi(&buffer[1]);
		}
		if (id_atual != id) {
			fputs(buffer, ptr2);
		} else if (!sucesso) {
			sucesso = 1;
			linha = i;
		}
		i++;
	}
	fclose(ptr1);
	fclose(ptr2);

	if (!sucesso) printf("Erro: Id nao encontrado!\n");
	return linha;
}

void modreg(void *data, int tipo) { // alterar/adicionar registro a um arquivo
	if (tipo == 0) { // cursos.txt

	}
}

int menu() {
	int opcao;

	printf("1. Imprimir arquivo\n");
	printf("2. Adicionar registro\n");
	printf("3. Alterar registro\n");
	printf("4. Excluir registro\n");
	printf("5. Sair\n");
	printf("Escolha uma operacao: ");

	do {
		input(&opcao, 'd');
	} while(opcao < 1 || opcao > 5);

	return opcao;
}

void adicionarregistro() {
	int opcao;

	printf("1. Cursos\n");
	printf("2. Disciplinas\n");
	printf("3. Turmas\n");
	printf("4. Alunos\n");
	printf("5. Professores\n");
	printf("6. Voltar\n");
	printf("Escolha um arquivo: ");

	do {
		input(&opcao, 'd');
	} while (opcao < 1 || opcao > 6);
	if (opcao == 6) return;

	if (opcao == 1) {
		Curso curso;
		printf("Id: ");
		do { 
			input(&(curso.id), 'd');
		} while (curso.id < 0 || curso.id > 999);
		printf("Nome do curso: ");
		input(&(curso.nome), 's');
	} else if (opcao == 2) {
		Disciplina disciplina;
		printf("Id: ");
		do {
			input(&(disciplina.id), 'd');
		} while (disciplina.id < 0 || disciplina.id > 999);
		printf("Nome da disciplina: ");
		input(&(disciplina.nome), 's');
		printf("Carga horaria: ");
		do {
			input(&(disciplina.carga), 'd');
		} while (disciplina.carga <= 0);

		printf("Id: %d\n", disciplina.id);
		printf("Nome: %s\n", disciplina.nome);
		printf("Carga: %d\n", disciplina.carga);
	}
}

char *arquivos[5] = {"cursos.txt", "disciplinas.txt", "turmas.txt", "alunos.txt", "professores.txt"};
int main() {
	// teste
	int id, l;
	printf("(teste) deletar id\nescolha um id que esteja presente em cursos.txt\nresultado direcionado para o arquivo temp.txt\nid: ");
	input(&id, 'd');
	l = deletarid(id, arquivos[0]);
	printf("linha deletada: %d\n", l);
	return 0;
	//fim do teste
	
	int opcao, i;
	FILE *ptr;
	for (i = 0; i < 5; i++) {
		ptr = fopen(arquivos[i], "a");
		fclose(ptr);
	}

	while (1) {
		opcao = menu();
		
		if (opcao == 1) {
		} else if (opcao == 2) {
			adicionarregistro();
		} else if (opcao == 5) break;
	}

	return 0;
}