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
	Node *notas;
} Disciplina;

typedef struct {
	int id;
	int professor;
	char turno;
	Node *alunos;
	Node *horarios;
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
		disciplina->notas = NULL;
		while (fgets(buffer, 256, ptr)) { // notas
			if (buffer[0] == ';') break;
			Nota *nota = (Nota *)malloc(sizeof(Nota));
			sscanf("%d%f%f%f", &(nota->id), &(nota->nota1), &(nota->nota2), &(nota->nota3));
			addnode(&(disciplina->notas), nota);
		}
		reg = disciplina;
	} else if (tipo == 2) { // turmas.txt
		Turma *turma = (Turma *)malloc(sizeof(Turma));
		turma->id = id; // id
		while (fgets(buffer, 256, ptr)) { // alunos
			if (buffer[0] == ';') break;
			int *alunoid = (int *)malloc(sizeof(int));
			*alunoid = atoi(buffer);
			addnode(&(turma->alunos), alunoid);
		}
		input(&(turma->professor), 'd', ptr); // professor
		while (fgets(buffer, 256, ptr)) { // horários
			if (buffer[0] == ';') break;
			Horario *h = (Horario *)malloc(sizeof(Horario));
			sscanf(ptr, "%d%d%d%d%d", &(h->semana), &(h->hi), &(h->mi), &(h->hf), &(h->mf));
			addnode(&(turma->horarios), h);
		}
		reg = turma;
	} else if (tipo == 3) { // alunos.txt
		Aluno *aluno = (Aluno *)malloc(sizeof(Aluno));
		aluno->id = id; // id
		input(&(aluno->nome), 's', ptr); // nome
		input(&(aluno->cpf), 'd', ptr); // cpf
		input(&(aluno->telefone), 'd', ptr); // telefone
		input(&(aluno->email), 's', ptr); // email
		sscanf(ptr, '%d%d', &(aluno->ing_ano), &(aluno->ing_semestre)); // ano, semestre
		reg = aluno;
	} else if (tipo == 4) { // professores.txt
		Professor *professor = (Professor *)malloc(sizeof(Professor));
		professor->id = id; // id
		input(&(professor->nome), 's', ptr); // nome
		input(&(professor->cpf), 'd', ptr); // cpf
		input(&(professor->telefone), 'd', ptr); // telefone
		input(&(professor->email), 's', ptr); // email
		reg = professor;
	}
	fclose(ptr);
	return reg;
}

void printarquivo(char *arquivo) {
	FILE *ptr = fopen(arquivo, "r");
	char buffer[256];
	printf("|-id--|-nome----\n");
	while (fgets(buffer, 256, ptr)) {
		if (strchr(buffer, '\n')) buffer[strcspn(buffer, "\n")] = 0;
		if (buffer[0] == '.') {
			printf("| %-4s| ", &buffer[1]);
		} else if (strcmp(buffer, "#nome") == 0) {
			fgets(buffer, 256, ptr);
			printf("%s", buffer);
		}
	}
	fclose(ptr);
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
			fprintf(ptr, "%d\n", node->data);
			node = node->next;
		}
		fprintf(ptr, ";\n#turno\n");
		fprintf(ptr, "%c\n;\n", curso->turno);
		fclose(ptr);
	}
}

int menu() {
	int opcao;

	printf("1. Imprimir arquivo\n");
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

void inputnome(const char *str, char **dest) {
	printf("%s", str);
	char *nome;
	input(&nome, 's', stdin);
	*dest = malloc(strlen(nome)+1);
	strcpy(*dest, nome);
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

int nodehasid(Node *node, int id) {
	while (node) {
		if (node->data == id) {
			return 1;
		}
		node = node->next;
	}
	return 0;
}

void addnode(Node **node, void* data) {
	while (*node) {
		node = &((*node)->next);
	}
	*node = malloc(sizeof(Node));
	(*node)->data = data;
	(*node)->next = NULL;
}

void inputnode(Node **node, char *arquivo) {
	int id;
	do {
		input(&id, 'd', stdin);
	} while (id > 999 || !hasreg(id, arquivo) || nodehasid(*node, id));
	addnode(node, id);
}

void delnode(Node **node) {
	int id;
	do {
		input(&id, 'd', stdin);
	} while (!nodehasid(*node, id));
	
	Node *prev = NULL;
	while (*node) {
		if ((*node)->data == id) {
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

void printfilterid(Node *node, char *arquivo) {
	char buffer[256];
	int id;
	FILE *ptr = fopen(arquivo, "r");
	printf("|-id--|-nome----\n");
	while (fgets(buffer, 256, ptr)) {
		if (buffer[0] != '.') continue;
		id = atoi(&buffer[1]);
		if (!nodehasid(node, id)) continue;
		while (fgets(buffer, 256, ptr)) {
			if (strcmp(buffer, "#nome\n") != 0) continue;
			fgets(buffer, 256, ptr);
			printf("| %-4d| %s", id, buffer);
			break;
		}
	}
}

void alterarregistro() {
	char buffer[256];
	void *reg;
	int arquivo = escolherarquivo(), opcao, id;
	if (arquivo == 5) return;
	
	printarquivo(arquivos[arquivo]);
	printf("Digite o registro que deseja alterar: ");
	do {
		input(&id, 'd', stdin);
	} while(!hasreg(id, arquivos[arquivo]));

	if (arquivo == 0) {
		Curso *curso = (Curso *) getreg(id, arquivo);

		printf("1. Alterar nome\n");
		printf("2. Adicionar disciplina\n");
		printf("3. Remover disciplina\n");
		printf("4. Alterar turno\n");
		printf("5. Voltar\n");
		printf("Escolha uma opcao: ");
		do {
			input(&opcao, 'd', stdin);
		} while(opcao < 1 || opcao > 5);
		if (opcao == 5) return;

		if (opcao == 1) { // nome
			inputnome("Digite o novo nome: ", &(curso->nome));
		} else if (opcao == 2) { // add disciplina
			printarquivo(arquivos[1]);
			printf("Digite o id da disciplina que deseja adicionar: ");
			inputnode(&(curso->disciplinas), arquivos[1]);
		} else if (opcao == 3) { // del disciplina
			printfilterid(curso->disciplinas, arquivos[1]);
			printf("Digite o id da disciplina que deseja remover: ");
			delnode(&(curso->disciplinas));
		} else if (opcao == 4) { // turno
			inputturno("Digite o turno: ", &(curso->turno));
		}
		reg = curso;
	}
	
	modreg(reg, arquivo);
}

void adicionarregistro(int opcao) {
	void *reg;
	int id = newid(arquivos[opcao]);
	if (opcao == 0) { // curso
		Curso curso;
		curso.id = id;
		inputnome("Nome do curso: ", &(curso.nome));
		
		// TODO: mover para uma função própria loopaddnode
		curso.disciplinas = NULL;
		printarquivo(arquivos[1]);
		printf("- Escolha as disciplinas que deseja adicionar ao curso ou uma das opcoes abaixo:\n");
		printf("- 'n' ... Criar uma nova disciplina\n");
		printf("- 'r' ... Remover disciplina do curso\n");
		printf("- 'q' ... Finalizar essa etapa\n");
		char *str;
		int discid;
		do {
			printf(": ");
			input(&str, 's', stdin);
			if (strtoid(str, &discid)) {
				if (!hasreg(discid, arquivos[1])) { free(str); continue; }
				if (nodehasid(curso.disciplinas, discid)) { free(str); continue;}
				addnode(&(curso.disciplinas), discid);
				printfilterid(curso.disciplinas, arquivos[1]);
			} else if (strlen(str) == 1) {
				if (str[0] == 'q') { free(str); break; }
				else if (str[0] == 'n') {
					adicionarregistro(1);
					printarquivo(arquivos[1]);
				} else if (str[0] == 'r') {
					printfilterid(curso.disciplinas, arquivos[1]);
					delnode(&(curso.disciplinas));
				}
			}
			free(str);
		} while (1);
		//
		
		inputturno("Turno: ", &(curso.turno));
		reg = &curso;
	} else if (opcao == 1) { // disciplina
		Disciplina disciplina;
		disciplina.id = id;
		inputnome("Nome da disciplina: ", &(disciplina.nome));
		disciplina.turmas = NULL;
		printf("Carga horaria: ");
		do { input(&(disciplina.carga), 'd', stdin);
		} while (disciplina.carga <= 0);
		disciplina.notas = NULL;
		reg = &disciplina;
	} else if (opcao == 2) { // turma
		Turma turma;
		turma.id = id;
		turma.alunos = NULL;
		printarquivo(arquivos[4]);
		printf("Professor: ");
		do { input(&(turma.professor), 'd', stdin);
		} while (!hasreg(turma.professor, arquivos[4]));
		// TODO: adicionar horario da turma
		reg = &turma;
	} else if (opcao == 3) { // aluno
		Aluno aluno;
		aluno.id = id;
		// TODO: implementar adicionar aluno
		reg = &aluno;
	} else if (opcao == 4) { // professor
		Professor professor;
		professor.id = id;
		// TODO: implementar adicionar professor
		reg = &professor;
	}
	// TODO: implementar modreg/getreg para todos os arquivos
	modreg(reg, opcao);
}

int main() {
	int opcao, i;
	FILE *ptr;
	for (i = 0; i < 5; i++) {
		ptr = fopen(arquivos[i], "a");
		fclose(ptr);
	}

	srand(time(NULL));
	rand();

	while (1) {
		opcao = menu();
		
		if (opcao == 1) {
			int arquivo = escolherarquivo();
			printarquivo(arquivos[arquivo]);
		} else if (opcao == 2) {
			int arquivo = escolherarquivo(), id;
			printarquivo(arquivos[arquivo]);
			printf("Escolha um registro para ler mais detalhes: ");
			do { input(&id, 'd', stdin);
			} while (!hasreg(id, arquivos[arquivo]));
			// TODO: implementar printregistro
			printf("funcao ainda nao implementada\n");
		} else if (opcao == 3) {
			int arquivo = escolherarquivo();
			if (arquivo == 5) continue;
			adicionarregistro(arquivo);
		} else if (opcao == 4) {
			alterarregistro();
		} else if (opcao == 5) {
			int arquivo = escolherarquivo(), id;
			if (arquivo == 5) continue;
			printarquivo(arquivos[arquivo]);
			while (1) {
				printf("Digite o id: ");
				input(&id, 'd', stdin);
				if (hasreg(id, arquivos[arquivo])) break;
				else printf("Erro: Registro nao encontrado!\n");
			}
			deletarid(id, arquivos[arquivo]);
		} else if (opcao == 6) break;
	}

	return 0;
}