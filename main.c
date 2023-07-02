#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
	int id;
	float nota1;
	float nota2;
	float nota3;
} Nota;

typedef struct {
	int semana;
	int hi; // hora inicio
	int mi; // minuto inicio
	int hf; // hora fim
	int mf; // minuto fim
} Horario;

typedef struct Node {
	void *data;
	struct Node *next;
} Node;

typedef struct {
	int id;
	char *nome;
	Node *disciplinas;
	char turno;
} Curso;

typedef struct {
	int id;
	int carga;
	char *nome;
	Node *turmas;
} Disciplina;

typedef struct {
	int id;
	char letra;
	int professor;
	Node *alunos;
	Node *horarios;
	Node *notas;
} Turma;

typedef struct {
	int id;
	char *nome;
	char *cpf;
	char *telefone;
	char *email;
	int ing_ano;
	int ing_semestre;
} Aluno;

typedef struct {
	int id;
	char *nome;
	char *cpf;
	char *telefone;
	char *email;
} Professor;

char *arquivos[5] = {"cursos.txt", "disciplinas.txt", "alunos.txt", "professores.txt", "turmas.txt"};
char *turnos[3] = {"Matutino", "Vespertino", "Noturno"};
char *semana[7] = {"Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado", "Domingo"};

int adicionarregistro(int);
void printfilterid(Node *, char *, int);
int filterid(Node *, char *, int);

void input(void *target, char t, FILE *src) {
	int c;
	size_t l, size = 256;
	char buffer[256], *endptr;

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
				float f = strtof(buffer, &endptr);
				if (*endptr != '\0' || f < 0) continue;
				memcpy(target, &f, sizeof(float));
			} else if (t == 'd') {
				int n = strtol(buffer, &endptr, 10);
				if (*endptr != '\0' || n < 0) continue;
				memcpy(target, &n, sizeof(int));
			} else if (t == 's') {
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

void addnode(Node **node, void* data) {
	while (*node) {
		node = &((*node)->next);
	}
	*node = malloc(sizeof(Node));
	(*node)->data = data;
	(*node)->next = NULL;
}

void delnodeat(Node **node, int pos) {
	Node *prev = NULL;
	int i = 0;
	while (*node) {
		if (i == pos) {
			if (prev) { prev->next = (*node)->next;
			} else { *node = (*node)->next; }
			break;
		}
		prev = *node;
		node = &((*node)->next);
		i++;
	}
}

void delnode(Node **node, void *data, size_t size) {
	Node *prev = NULL;
	while (*node) {
		if (memcmp((*node)->data, data, size) == 0) {
			if (prev) {
				prev->next = (*node)->next;
			} else {
				*node = (*node)->next;
			}
			break;
		}
		prev = *node;
		node = &((*node)->next);
	}
}

int nodesize(Node *node) {
	int tam = 0;
	while (node) {
		tam++;
		node = node->next;
	}
	return tam;
}

// Ex: .000, .001, .002, ..., .999
// Retorna a primeira linha a ser deletada
// Caso a linha nao foi encontrada, retorna 0
int delreg(int id, char *arquivo) {
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
	
	return linha;
}

int hasreg(int id, char *arquivo) {
	int id_arquivo, linha = 0, i = 0;
	char buffer[256];
	FILE *ptr = fopen(arquivo, "r");
	while (fgets(buffer, 256, ptr)) {
		i++;
		if (buffer[0] != '.') continue;
		if (atoi(&buffer[1]) != id) continue;
		linha = i; break;
	}
	fclose(ptr);
	return linha;
}

// retorna a quantidade de registros em um arquivo
int regsize(char *arquivo) {
	int tam = 0;
	char buffer[256];
	FILE *ptr = fopen(arquivo, "r");
	while (fgets(buffer, 256, ptr)) {
		if (buffer[0] == '.') tam++;
	}
	fclose(ptr);
	return tam;
}

void *getreg(int id, int tipo) {
	char buffer[256];
	void *reg = NULL;
	FILE *ptr;

	if (!hasreg(id, arquivos[tipo])) return NULL;
	ptr = fopen(arquivos[tipo], "r");
	while (fgets(buffer, 256, ptr)) {
		if (buffer[0] != '.') continue;
		if(atoi(&buffer[1]) == id) break;
	}

	if (tipo == 0) { // cursos.txt
		Curso *curso = (Curso *)malloc(sizeof(Curso));
		curso->id = id; // id
		input(&(curso->nome), 's', ptr); // nome
		curso->disciplinas = NULL;
		while (fgets(buffer, 256, ptr)) { // disciplinas
			if (buffer[0] == ';') break;
			int *discid = (int *)malloc(sizeof(int));
			*discid = atoi(buffer);
			addnode(&(curso->disciplinas), discid);
		}
		input(&(curso->turno), 'c', ptr); // turno
		reg = curso;
	} else if (tipo == 1) { // disciplinas.txt
		Disciplina *disciplina = (Disciplina *)malloc(sizeof(Disciplina));
		disciplina->id = id; // id
		input(&(disciplina->nome), 's', ptr); // nome
		disciplina->turmas = NULL;
		while (fgets(buffer, 256, ptr)) { // turmas
			if (buffer[0] == ';') break;
			int *turmaid = (int *)malloc(sizeof(int));
			*turmaid = atoi(buffer);
			addnode(&(disciplina->turmas), turmaid);
		}
		input(&(disciplina->carga), 'd', ptr); // carga horária
		reg = disciplina;
	} else if (tipo == 2) { // alunos.txt
		Aluno *aluno = (Aluno *)malloc(sizeof(Aluno));
		aluno->id = id; // id
		input(&(aluno->nome), 's', ptr); // nome
		input(&(aluno->cpf), 's', ptr); // cpf
		input(&(aluno->telefone), 's', ptr); // telefone
		input(&(aluno->email), 's', ptr); // email
		fgets(buffer, 256, ptr);
		sscanf(buffer, "%d %d", &(aluno->ing_ano), &(aluno->ing_semestre)); // ano, semestre
		reg = aluno;
	} else if (tipo == 3) { // professores.txt
		Professor *professor = (Professor *)malloc(sizeof(Professor));
		professor->id = id; // id
		input(&(professor->nome), 's', ptr); // nome
		input(&(professor->cpf), 's', ptr); // cpf
		input(&(professor->telefone), 's', ptr); // telefone
		input(&(professor->email), 's', ptr); // email
		reg = professor;
	} else if (tipo == 4) { // turmas.txt
		Turma *turma = (Turma *)malloc(sizeof(Turma));
		turma->id = id; // id
		input(&(turma->letra), 'c', ptr); // letra
		input(&(turma->professor), 'd', ptr); // professor
		turma->horarios = NULL;
		while (fgets(buffer, 256, ptr)) { // horários
			if (buffer[0] == ';') break;
			Horario *h = (Horario *)malloc(sizeof(Horario));
			sscanf(buffer, "%d%d%d%d%d", &(h->semana), &(h->hi), &(h->mi), &(h->hf), &(h->mf));
			addnode(&(turma->horarios), h);
		}
		turma->notas = NULL;
		turma->alunos = NULL;
		while (fgets(buffer, 256, ptr)) { // notas. Ex: 123 10.0 5.5 8.9
			if (buffer[0] == ';') break;
			Nota *nota = (Nota *)malloc(sizeof(Nota));
			sscanf(buffer, "%d%f%f%f", &(nota->id), &(nota->nota1), &(nota->nota2), &(nota->nota3));
			addnode(&(turma->notas), nota);
			addnode(&(turma->alunos), &(nota->id));
		}
		reg = turma;
	}
	fclose(ptr);
	return reg;
}

void printarquivo(char *arquivo) {
	FILE *ptr = fopen(arquivo, "r");
	char buffer[256];
	printf("|-id--|-nome----\n");
	while (fgets(buffer, 256, ptr)) {
		if (buffer[0] == '.') {
			buffer[strcspn(buffer, "\n")] = 0;
			printf("| %-4s| ", &buffer[1]);
			fgets(buffer, 256, ptr);
			printf("%s", buffer);
		}
	}
	fclose(ptr);
}

void printregistro(int id, int arquivo) {
	void *reg = getreg(id, arquivo);
	printf("\n=-----------------------=\n");
	if (arquivo == 0) { // Curso
		Curso *curso = (Curso *) reg;
		int turno;
		switch (curso->turno) {
		case 'v': // vespertino
			turno = 1; break;
		case 'n': // noturno
			turno = 2; break;
		default: // matutino
			turno = 0; break;
		}
		printf("Id: %d\nNome: %s\nTurno: %s\nDisciplinas:\n", curso->id, curso->nome, turnos[turno]);
		printfilterid(curso->disciplinas, arquivos[1], 0);
	} else if (arquivo == 1) { // Disciplina
		Disciplina *disciplina = (Disciplina *) reg;
		printf("Id: %d\nNome: %s\nCarga: %d horas\n", disciplina->id, disciplina->nome, disciplina->carga);
		int size_turmas = filterid(disciplina->turmas, arquivos[4], 0);
		printf("Turmas: %d ", size_turmas);
		if (size_turmas <= 1) printf("turma\n"); else printf("turmas\n");
	} else if (arquivo == 2) { // Aluno
		Aluno *aluno = (Aluno *) reg;
		printf("Matricula: %d\nNome: %s\nCpf: %s\nTelefone: %s\nEmail: %s\n", 
			aluno->id, aluno->nome, aluno->cpf, aluno->telefone, aluno->email);
		printf("Ingresso: %d, %dº semestre\n", aluno->ing_ano, aluno->ing_semestre);
	} else if (arquivo == 3) { // Professor
		Professor *professor = (Professor *) reg;
		printf("Matricula: %d\nNome: %s\nCpf: %s\nTelefone: %s\nEmail: %s\n", 
			professor->id, professor->nome, professor->cpf, professor->telefone, professor->email);
	}
	printf("=-----------------------=\n");
}

// adicionar registro a um arquivo
void addreg(void *data, int tipo) {
	FILE *ptr = fopen(arquivos[tipo], "a");
	Node *node;
	if (tipo == 0) { // cursos.txt
		Curso *curso = (Curso *) data;
		fprintf(ptr, ".%d\n", curso->id); // id
		fprintf(ptr, "%s\n", curso->nome); // nome
		node = curso->disciplinas;
		while (node) { // disciplinas
			fprintf(ptr, "%d\n", *((int *)(node->data)));
			node = node->next;
		}
		fprintf(ptr, ";\n%c\n", curso->turno); // turno
	} else if (tipo == 1) { // disciplinas.txt
		Disciplina *disciplina = (Disciplina *) data;
		fprintf(ptr, ".%d\n", disciplina->id); // id
		fprintf(ptr, "%s\n", disciplina->nome); // nome
		node = disciplina->turmas;
		while (node) { // turmas
			fprintf(ptr, "%d\n", *((int *)(node->data)));
			node = node->next;
		}
		fprintf(ptr, ";\n%d\n", disciplina->carga); // carga horaria
	} else if (tipo == 2) { // alunos.txt
		Aluno *aluno = (Aluno *) data;
		fprintf(ptr, ".%d\n", aluno->id); // id
		fprintf(ptr, "%s\n", aluno->nome); // nome
		fprintf(ptr, "%s\n", aluno->cpf); // cpf
		fprintf(ptr, "%s\n", aluno->telefone); // telefone
		fprintf(ptr, "%s\n", aluno->email); // email
		fprintf(ptr, "%d %d\n", aluno->ing_ano, aluno->ing_semestre); // ingresso. (ano + semestre (1 ou 2))
	} else if (tipo == 3) { // professores.txt
		Professor *professsor = (Professor *) data;
		fprintf(ptr, ".%d\n", professsor->id); // id
		fprintf(ptr, "%s\n", professsor->nome); // nome
		fprintf(ptr, "%s\n", professsor->cpf); // cpf
		fprintf(ptr, "%s\n", professsor->telefone); // telefone
		fprintf(ptr, "%s\n", professsor->email); // email
	} else if (tipo == 4) { // turmas.txt
		Turma *turma = (Turma *) data;
		fprintf(ptr, ".%d\n", turma->id); // id
		fprintf(ptr, "%c\n", turma->letra); // letra
		fprintf(ptr, "%d\n", turma->professor); // professor
		node = turma->horarios;
		while (node) { // horarios. (dia da semana (0-6) + hora inicio + minuto inicio + hora fim + minuto fim )
			Horario *h = (Horario *)(node->data);
			fprintf(ptr, "%d %d %d %d %d\n", h->semana, h->hi, h->mi, h->hf, h->mf);
			node = node->next;
		}
		fprintf(ptr, ";\n");
		node = turma->notas;
		while (node) { // notas. Ex: 123 10.0 5.5 8.9
			Nota *nota = (Nota *)(node->data);
			fprintf(ptr, "%d %.2f %.2f %.2f\n", nota->id, nota->nota1, nota->nota2, nota->nota3);
			node = node->next;
		}
		fprintf(ptr, ";\n");
	}
	fclose(ptr);
}

int menu() {
	int opcao;

	printf("\n1. Imprimir arquivo\n");
	printf("2. Imprimir registro\n");
	printf("3. Adicionar registro\n");
	printf("4. Alterar registro\n");
	printf("5. Excluir registro\n");
	printf("6. Sair\n");
	printf("Escolha uma operacao: ");

	do {
		input(&opcao, 'd', stdin);
	} while(opcao < 1 || opcao > 6);

	return opcao;
}

int escolherarquivo() {
	int opcao;

	printf("\n1. Cursos\n");
	printf("2. Disciplinas\n");
	printf("3. Alunos\n");
	printf("4. Professores\n");
	printf("5. Voltar\n");
	printf("Escolha um arquivo: ");

	do {
		input(&opcao, 'd', stdin);
	} while (opcao < 1 || opcao > 5);
	
	return opcao-1;
}

// Converte uma string para um id valido,
// modifica o valor de destino e retorna 1.
// Caso não seja possível, mantém o valor
// de destino inalterado e retorna 0.
int strtoid(const char *str, int *dest) {
	char *endptr;
	int valor = strtol(str, &endptr, 10);
	if (*endptr) return 0;
	if (valor < 0 || valor > 999) return 0;
	*dest = valor;
	return 1;
}

int newid(char *arquivo) {
	int id;
	do {
		id = (int)(rand()*1000.0/RAND_MAX);
	} while (hasreg(id, arquivo) || id > 999 || id < 0);
	return id;
}

// int inputid() {
// 	int id;
// 	do { input(&id, 'd', stdin);
// 	} while (id > 999);
// 	return id;
// }

void inputlen(const char *str, char **dest, size_t length) {
	printf("%s", str);
	char *nome;
	while (1) {
		input(&nome, 's', stdin);
		if (strlen(nome) == length || length == 0) break;
		free(nome);
	}
	*dest = nome;
}

void inputturno(const char *str, char *dest) {
	char c;
	printf("m: Matutino\n");
	printf("v: Vespertino\n");
	printf("n: Noturno\n");
	printf("%s", str);
	do {
		input(&c, 'c', stdin);
	} while (c != 'm' && c != 'v' && c != 'n');
	*dest = c;
}

int nodehasdata(Node *node, void *data, size_t size) {
	while (node) {
		if (memcmp(node->data, data, size) == 0) {
			return 1;
		}
		node = node->next;
	}
	return 0;
}

// void inputnode(Node **node, char *arquivo) {
// 	int id;
// 	do {
// 		input(&id, 'd', stdin);
// 	} while (id > 999 || !hasreg(id, arquivo) || nodehasdata(*node, &id, sizeof(int)));
// 	addnode(node, id);
// }

int filterid(Node *node, char *arquivo, int inv) {
	char buffer[256];
	int id, count = 0;
	FILE *ptr = fopen(arquivo, "r");
	while (fgets(buffer, 256, ptr)) {
		if (buffer[0] != '.') continue;
		id = atoi(&buffer[1]);
		int hasdata = nodehasdata(node, &id, sizeof(int)) ? 1 : 0;
		if (hasdata == (inv ? 1 : 0)) continue;
		count++;
	}
	fclose(ptr);
	return count;
}

// imprime id e nome dos registros que estao contidos em node
void printfilterid(Node *node, char *arquivo, int inv) {
	char buffer[256];
	int id;
	FILE *ptr = fopen(arquivo, "r");
	printf("|-id--|-nome----\n");
	while (fgets(buffer, 256, ptr)) {
		if (buffer[0] != '.') continue;
		id = atoi(&buffer[1]);
		int hasdata = nodehasdata(node, &id, sizeof(int)) ? 1 : 0;
		if (hasdata == (inv ? 1 : 0)) continue;
		fgets(buffer, 256, ptr);
		printf("| %-4d| %s", id, buffer);
	}
	fclose(ptr);
}

void alterarregistro(int arquivo, int id) {
	char buffer[256];
	void *reg;
	int opcao;

	if (id < 0) {
		printarquivo(arquivos[arquivo]);
		printf("Digite o registro que deseja alterar: ");
		do { input(&id, 'd', stdin);
		} while(!hasreg(id, arquivos[arquivo]));
	}

	if (arquivo == 0) { // cursos.txt
		Curso *curso = (Curso *) getreg(id, arquivo);
		printf("1. Alterar nome\n");
		printf("2. Adicionar disciplina\n");
		printf("3. Remover disciplina\n");
		printf("4. Alterar turno\n");
		printf("5. Voltar\n");
		printf("Escolha uma opcao: ");
		do { input(&opcao, 'd', stdin);
		} while(opcao < 1 || opcao > 5);
		if (opcao == 5) return;

		if (opcao == 1) { inputlen("Novo nome: ", &(curso->nome), 0); // nome
		} else if (opcao == 2) { // add disciplina
			if (filterid(curso->disciplinas, arquivos[1], 1) == 0) { printf("Nao ha disciplinas disponiveis para este curso\n"); return; }
			printfilterid(curso->disciplinas, arquivos[1], 1);
			printf("Digite o id da disciplina que deseja adicionar: ");
			int *discid = (int *)malloc(sizeof(int));
			do { input(discid, 'd', stdin);
			} while (!hasreg(*discid, arquivos[1]) || nodehasdata(curso->disciplinas, discid, sizeof(int)));
			addnode(&(curso->disciplinas), discid);
		} else if (opcao == 3) { // del disciplina
			if (nodesize(curso->disciplinas) == 0) { printf("Nao ha disciplinas para remover\n"); return; }
			printfilterid(curso->disciplinas, arquivos[1], 0);
			printf("Digite o id da disciplina que deseja remover: ");
			int *discid = (int *)malloc(sizeof(int));
			do { input(discid, 'd', stdin);
			} while (!nodehasdata(curso->disciplinas, discid, sizeof(int)));
			delnode(&(curso->disciplinas), discid, sizeof(int));
			free(discid);
		} else if (opcao == 4) { inputturno("Digite o turno: ", &(curso->turno)); } // turno
		reg = curso;
	} else if (arquivo == 1) { // disciplinas.txt
		Disciplina *disciplina = (Disciplina *) getreg(id, arquivo);
		printf("1. Alterar nome\n");
		printf("2. Alterar carga horaria\n");
		printf("3. Adicionar turma\n");
		printf("4. Alterar turma\n");
		printf("5. Excluir turma\n");
		printf("6. Voltar\n");
		printf("Escolha uma opcao: ");
		do { input(&opcao, 'd', stdin);
		} while(opcao < 1 || opcao > 6);
		if (opcao == 6) return;
		if (opcao == 1) { inputlen("Novo nome: ", &(disciplina->nome), 0); // nome
		} else if (opcao == 2) {  // carga horaria
			printf("Carga horaria: ");
			do { input(&(disciplina->carga), 'd', stdin);
			} while (disciplina->carga <= 0);
		} else if (opcao == 3) { // add turma
			if (regsize(arquivos[3]) == 0) { printf("Nao e possivel adicionar uma turma a esta disciplina\nAdicione um professor antes\n"); return; }
			int *discid = (int *)malloc(sizeof(int));
			*discid = adicionarregistro(4);
			addnode(&(disciplina->turmas), discid);
		} else if (opcao == 4) { // alterar turma
			if (filterid(disciplina->turmas, arquivos[4], 0) == 0) { printf("Esta disciplina nao possui turmas\n"); return; }
			printfilterid(disciplina->turmas, arquivos[4], 0);
			int turmaid;
			printf("Id da turma: ");
			do { input(&turmaid, 'd', stdin);
			} while (!nodehasdata(disciplina->turmas, &turmaid, sizeof(int)));
			alterarregistro(4, turmaid);
		} else if (opcao == 5) { // del turma
			if (nodesize(disciplina->turmas) == 0) { printf("Esta disciplina nao possui turmas\n"); return; }
			printfilterid(disciplina->turmas, arquivos[4], 0);
			int turmaid;
			do { input(&turmaid, 'd', stdin);
			} while (!nodehasdata(disciplina->turmas, &turmaid, sizeof(int)));
			delreg(turmaid, arquivos[4]);
			delnode(&(disciplina->turmas), &turmaid, sizeof(int));
		}
		reg = disciplina;
	} else if (arquivo == 2) { // alunos.txt
		Aluno *aluno = (Aluno *) getreg(id, arquivo);
		printf("1. Alterar nome\n");
		printf("2. Alterar cpf\n");
		printf("3. Alterar telefone\n");
		printf("4. Alterar email\n");
		printf("5. Voltar\n");
		printf("Escolha uma opcao: ");
		do { input(&opcao, 'd', stdin);
		} while(opcao < 1 || opcao > 5);
		if (opcao == 5) return;
		if (opcao == 1) { inputlen("Nome: ", &(aluno->nome), 0); // nome
		} else if (opcao == 2) { inputlen("Cpf: ", &(aluno->cpf), 11); // cpf
		} else if (opcao == 3) { inputlen("Telefone: ", &(aluno->telefone), 0); // telefone
		} else if (opcao == 4) { inputlen("Email: ", &(aluno->email), 0);} // email
		reg = aluno;
	} else if (arquivo == 3) { // professores.txt
		Professor *professor = (Professor *) getreg(id, arquivo);
		printf("1. Alterar nome\n");
		printf("2. Alterar cpf\n");
		printf("3. Alterar telefone\n");
		printf("4. Alterar email\n");
		printf("5. Voltar\n");
		printf("Escolha uma opcao: ");
		do { input(&opcao, 'd', stdin);
		} while(opcao < 1 || opcao > 5);
		if (opcao == 5) return;
		if (opcao == 1) { inputlen("Nome: ", &(professor->nome), 0); // nome
		} else if (opcao == 2) { inputlen("Cpf: ", &(professor->cpf), 11); // cpf
		} else if (opcao == 3) { inputlen("Telefone: ", &(professor->telefone), 0); // telefone
		} else if (opcao == 4) { inputlen("Email: ", &(professor->email), 0);} // email
		reg = professor;
	} else if (arquivo == 4) { // turmas.txt
		Turma *turma = (Turma *) getreg(id, arquivo);
		printf("1. Alterar letra\n");
		printf("2. Alterar professor\n");
		printf("3. Adicionar aluno\n");
		printf("4. Remover aluno\n");
		printf("5. Alterar nota\n");
		printf("6. Adicionar horario\n");
		printf("7. Remover horario\n");
		printf("8. Voltar\n");
		printf("Escolha uma opcao: ");
		do { input(&opcao, 'd', stdin);
		} while(opcao < 1 || opcao > 8);
		if (opcao == 8) return;
		if (opcao == 1) { printf("Letra: "); input(&(turma->letra), 'c', stdin); // letra
		} else if (opcao == 2) { // professor
			printarquivo(arquivos[3]);
			printf("Professor: ");
			do { input(&(turma->professor), 'd', stdin);
			} while (!hasreg(turma->professor, arquivos[3]));
		} else if (opcao == 3) { // add aluno
			if (filterid(turma->alunos, arquivos[2], 1) == 0) { printf("Nao ha alunos disponiveis para esta turma\n"); return; }
			printfilterid(turma->alunos, arquivos[2], 1);
			printf("Aluno: ");
			int *alunoid = (int *)malloc(sizeof(int));
			do { input(alunoid, 'd', stdin);
			} while (nodehasdata(turma->alunos, alunoid, sizeof(int)) || !hasreg(*alunoid, arquivos[2]));
			addnode(&(turma->alunos), alunoid);
			Nota *nota = (Nota *)malloc(sizeof(Nota));
			nota->id = *alunoid;
			nota->nota1 = 0; nota->nota2 = 0; nota->nota3 = 0;
			addnode(&(turma->notas), nota);
		} else if (opcao == 4) { // del aluno
			if (nodesize(turma->alunos) == 0) { printf("Esta turma esta vazia\n"); return; }
			printfilterid(turma->alunos, arquivos[2], 0);
			int alunoid;
			do { input(&alunoid, 'd', stdin);
			} while (!nodehasdata(turma->alunos, &alunoid, sizeof(int)));
			delnode(&(turma->alunos), &alunoid, sizeof(int));
			Node *head = turma->notas;
			while (head) {
				Nota *nota = (Nota *)head->data;
				if (nota->id == alunoid) { delnode(&(turma->notas), nota, sizeof(Nota)); break; }
				head = head->next;
			}
		} else if (opcao == 5) { // alterar nota
			if (nodesize(turma->alunos) == 0) { printf("Esta turma esta vazia\n"); return; }
			printfilterid(turma->alunos, arquivos[2], 0);
			printf("Aluno: ");
			int alunoid;
			do { input(&alunoid, 'd', stdin);
			} while (!nodehasdata(turma->alunos, &alunoid, sizeof(int)));
			Nota *nota = NULL;
			Node *head = turma->notas;
			while (head) {
				Nota *temp = (Nota *)head->data;
				if (temp->id == alunoid) {nota = temp; break;};
				head = head->next;
			}
			if (nota == NULL) return;
			printf("1. Nota 1: %.2f\n", nota->nota1);
			printf("2. Nota 2: %.2f\n", nota->nota2);
			printf("3. Nota 3: %.2f\nEscolha a nota que deseja alterar: ", nota->nota3);
			int opcaonota;
			do { input(&opcaonota, 'd', stdin);
			} while (opcaonota < 1 || opcaonota > 3);
			printf("Nova nota: ");
			if (opcaonota == 1) { input(&nota->nota1, 'f', stdin);
			} else if (opcaonota == 2) { input(&nota->nota2, 'f', stdin);
			} else if (opcaonota == 3) { input(&nota->nota3, 'f', stdin); }
		} else if (opcao == 6) { // add horario
			Horario *horario = (Horario *)malloc(sizeof(Horario));
			for (int i = 0; i < 7; i++) { printf("%d. %s\n", i+1, semana[i]); }
			printf("Dia da semana: ");
			do { input(&(horario->semana), 'd', stdin);
			} while (horario->semana < 1 || horario->semana > 7);
			horario->semana--;
			printf("Inicio (Hora): "); do {input(&(horario->hi), 'd', stdin);}
			while (horario->hi < 0 || horario->hi > 24);
			printf("Inicio (Minuto): "); do {input(&(horario->mi), 'd', stdin);}
			while (horario->hi < 0 || horario->hi > 59);
			printf("Fim (Hora): "); do {input(&(horario->hf), 'd', stdin);}
			while (horario->hi < 0 || horario->hi > 24);
			printf("Fim (Minuto): "); do {input(&(horario->mf), 'd', stdin);}
			while (horario->hi < 0 || horario->hi > 59);
			addnode(&(turma->horarios), horario);
		} else if (opcao == 7) { // del horario
			int opcoes = nodesize(turma->horarios);
			if (opcoes == 0) {printf("Esta turma nao possui horarios\n"); return;}
			int i = 0;
			Node *head = turma->horarios;
			while (head) {
				Horario *h = (Horario *)head->data;
				printf("%d. (%s) %d:%d - %d:%d\n", i+1, semana[h->semana], h->hi, h->mi, h->hf, h->mf);
				head = head->next; i++;
			}
			printf("Horario: ");
			int horarioopcao;
			do { input(&horarioopcao, 'd', stdin);
			} while (horarioopcao < 1 || horarioopcao > opcoes);
			delnodeat(&(turma->horarios), horarioopcao-1);
		}
		reg = turma;
	}
	
	delreg(id, arquivos[arquivo]);
	addreg(reg, arquivo);
}

int adicionarregistro(int opcao) {
	void *reg = NULL;
	int id = newid(arquivos[opcao]);
	if (opcao == 0) { // curso
		Curso curso;
		curso.id = id;
		inputlen("Nome do curso: ", &(curso.nome), 0);
		curso.disciplinas = NULL;
		inputturno("Turno: ", &(curso.turno));
		reg = &curso;
	} else if (opcao == 1) { // disciplina
		Disciplina disciplina;
		disciplina.id = id;
		inputlen("Nome da disciplina: ", &(disciplina.nome), 0);
		disciplina.turmas = NULL;
		printf("Carga horaria: ");
		do { input(&(disciplina.carga), 'd', stdin);
		} while (disciplina.carga <= 0);
		reg = &disciplina;
	} else if (opcao == 2) { // aluno
		Aluno aluno;
		aluno.id = id;
		inputlen("Nome do aluno: ", &(aluno.nome), 0);
		inputlen("Cpf: ", &(aluno.cpf), 11);
		inputlen("Telefone: ", &(aluno.telefone), 0);
		inputlen("Email: ", &(aluno.email), 0);
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		aluno.ing_ano = tm.tm_year + 1900;
		aluno.ing_semestre = (tm.tm_mon < 6) ? 1 : 2;
		// printf("ano: %d\nsemestre: %d\n", aluno.ing_ano, aluno.ing_semestre);
		reg = &aluno;
	} else if (opcao == 3) { // professor
		Professor professor;
		professor.id = id;
		inputlen("Nome do professor: ", &(professor.nome), 0);
		inputlen("Cpf: ", &(professor.cpf), 11);
		inputlen("Telefone: ", &(professor.telefone), 0);
		inputlen("Email: ", &(professor.email), 0);
		reg = &professor;
	} else if (opcao == 4) { // turma
		Turma turma;
		turma.id = id;
		printf("Letra: ");
		input(&(turma.letra), 'c', stdin);
		turma.alunos = NULL;
		turma.horarios = NULL;
		turma.notas = NULL;
		printarquivo(arquivos[3]);
		printf("Professor: ");
		do { input(&(turma.professor), 'd', stdin);
		} while (!hasreg(turma.professor, arquivos[3]));
		reg = &turma;
	}
	addreg(reg, opcao);
	return id;
}

int main() {
	int opcao, arquivo, i;
	FILE *ptr;
	for (i = 0; i < 5; i++) {
		ptr = fopen(arquivos[i], "a");
		fclose(ptr);
	}

	srand(time(NULL));
	rand();

	while (1) {
		opcao = menu();
		if (opcao == 6) break;

		arquivo = escolherarquivo();
		if (arquivo == 4) continue; // sair
		if (opcao == 1) { // imprimir arquivo
			printarquivo(arquivos[arquivo]);
		} else if (opcao == 2) { // imprimir registro
			if (regsize(arquivos[arquivo]) == 0) {
				printf("Arquivo vazio.\n");
				continue;
			}
			int id;
			printarquivo(arquivos[arquivo]);
			printf("Escolha um registro para ler mais detalhes: ");
			do { input(&id, 'd', stdin);
			} while (!hasreg(id, arquivos[arquivo]));
			printregistro(id, arquivo);
		} else if (opcao == 3) { // adicionar registro
			adicionarregistro(arquivo);
		} else if (opcao == 4) { // alterar registro
			if (regsize(arquivos[arquivo]) == 0) {
				printf("Arquivo vazio\n");
				continue;
			}
			alterarregistro(arquivo, -1);
		} else if (opcao == 5) { // deletar registro
			if (regsize(arquivos[arquivo]) == 0) {
				printf("Arquivo vazio.\n");
				continue;
			}
			int id;
			printarquivo(arquivos[arquivo]);
			while (1) {
				printf("Digite o id: ");
				input(&id, 'd', stdin);
				if (hasreg(id, arquivos[arquivo])) break;
				else printf("Erro: Registro nao encontrado!\n");
			}
			delreg(id, arquivos[arquivo]);
		}
	}

	return 0;
}