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

char *arquivos[5] = {"cursos.txt", "disciplinas.txt", "turmas.txt", "alunos.txt", "professores.txt"};

void input(void *target, char t, FILE *src) {
	int c, i;
	size_t l, size = 256;
	char buffer[256];

	while (1) {
		fgets(buffer, size, src);

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
				// buffer[strcspn(buffer, "\n")] = 0;
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

	remove(arquivo);
	rename("temp.txt", arquivo);

	// if (!sucesso) printf("Erro: Id nao encontrado!\n");
	return linha;
}

int hasreg(int id, char *arquivo) {
	int id_arquivo, linha = 0, i = 1;
	char buffer[256];
	FILE *ptr = fopen(arquivo, "r");
	while (fgets(buffer, 256, ptr)) {
		if (buffer[0] == '.') {
			id_arquivo = atoi(&buffer[1]);
			if (id == id_arquivo) {
				linha = i;
				break;
			}
		}
		i++;
	}
	fclose(ptr);

	return linha;
}

void *getreg(int id, int tipo) {
	FILE *ptr;
	void *reg;
	char buffer[256];

	if (hasreg(id, arquivos[tipo])) {
		ptr = fopen(arquivos[tipo], "r");
		if (tipo == 0) { // cursos.txt
			Curso curso;
			curso.id = id;
			while (fgets(buffer, 256, ptr)) {
				if (buffer[0] != '.') continue;
				if(atoi(&buffer[1]) == curso.id) break;
			}
			while (fgets(buffer, 256, ptr)) {
				if (buffer[0] == ';') break;
				if (buffer[0] == '#') {
					buffer[strcspn(buffer, "\n")] = 0;
					printf("%s\n", &buffer[1]);
					if (strcmp(&buffer[1], "nome") == 0) {
						// input(&buffer, 's', ptr);
						fgets(buffer, 256, ptr);
						printf("%s\n", buffer);
					}
				}
			}
			reg = &curso;
		}
		fclose(ptr);

		return reg;
	} else {
		return NULL;
	}
}

// alterar/adicionar registro a um arquivo
void modreg(void *data, int tipo) { 
	FILE *ptr;
	if (tipo == 0) { // cursos.txt
		Curso *curso = (Curso *) data;
		deletarid(curso->id, arquivos[tipo]);
		ptr = fopen(arquivos[tipo], "a");
		fprintf(ptr, ".%d\n", curso->id);
		fprintf(ptr, "#nome\n%s\n", curso->nome);
		fprintf(ptr, "#disciplinas\n");
		Node *node = curso->disciplinas;
		while (node != NULL) {
			fprintf(ptr, "%d\n", node->id);
			node = node->next;
		}
		fprintf(ptr, ";\n");
		fclose(ptr);
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
		input(&opcao, 'd', stdin);
	} while(opcao < 1 || opcao > 5);

	return opcao;
}

int escolherarquivo() {
	int opcao;

	printf("1. Cursos\n");
	printf("2. Disciplinas\n");
	printf("3. Turmas\n");
	printf("4. Alunos\n");
	printf("5. Professores\n");
	printf("6. Voltar\n");
	printf("Escolha um arquivo: ");

	do {
		input(&opcao, 'd', stdin);
	} while (opcao < 1 || opcao > 6);
	
	return opcao;
}

void adicionarregistro() {
	int opcao = escolherarquivo();
	if (opcao == 6) return;

	if (opcao == 1) {
		Curso curso;
		printf("Id: ");
		do { 
			input(&(curso.id), 'd', stdin);
			if (hasreg(curso.id, arquivos[opcao-1])) {
				printf("Erro: id ja existe!\n");
				curso.id = -1;
				continue;
			}
		} while (curso.id < 0 || curso.id > 999);
		printf("Nome do curso: ");
		input(&(curso.nome), 's', stdin);
		curso.disciplinas = NULL;

		modreg(&curso, opcao-1);

	} else if (opcao == 2) {
		Disciplina disciplina;
		printf("Id: ");
		do {
			input(&(disciplina.id), 'd', stdin);
		} while (disciplina.id < 0 || disciplina.id > 999);
		printf("Nome da disciplina: ");
		input(&(disciplina.nome), 's', stdin);
		printf("Carga horaria: ");
		do {
			input(&(disciplina.carga), 'd', stdin);
		} while (disciplina.carga <= 0);
	}
}

// char *arquivos[5] = {"cursos.txt", "disciplinas.txt", "turmas.txt", "alunos.txt", "professores.txt"};
int main() {
	getreg(777, 0);
	return 0;

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
		} else if (opcao == 4) {
			int id, arquivo;
			arquivo = escolherarquivo();
			if (arquivo == 6) continue;
			printf("Digite o id: ");
			while (1) {
				input(&id, 'd', stdin);
				if (hasreg(id, arquivos[arquivo-1])) {
					break;
				} else {
					printf("Erro: Registro nao encontrado!\n");
				}
			}

			deletarid(id, arquivos[arquivo-1]);
		} else if (opcao == 5) break;
	}

	return 0;
}