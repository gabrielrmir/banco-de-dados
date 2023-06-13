#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int input(char *buffer, size_t size) {
    int c;
    fgets(buffer, size, stdin);
    size_t len = strlen(buffer);
    if (len == 1) return 0;
    if (strcspn(buffer, "\n") < len) {
        buffer[strcspn(buffer, "\n")] = 0;
        return 1;
    } else {
        while ((c = getchar()) != '\n' && c != EOF);
        return 0;
    }
}

int toint(char *buffer, int **sucesso) {
    int n;
    char *endptr;
    n = strtol(buffer, &endptr, 10);
    **sucesso = 1;
    if (*endptr != '\0') **sucesso = 0;
    // printf("char: %c, valor: %d\n", *endptr, (int) *endptr);
    return n;
}

int menu() {
    int opcao, n, *sucesso;
    char buffer[3];

    printf("1. Imprimir arquivo\n");
    printf("2. Adicionar registro\n");
    printf("3. Alterar registro\n");
    printf("4. Excluir registro\n");
    printf("5. Sair\n");
    printf("Escolha uma operacao: ");

    do {
        n = input(buffer, 3);
        opcao = toint(buffer, &sucesso);
        // printf("numero: %d, sucesso: %d\n", opcao, *sucesso);
    } while(n == 0 || *sucesso == 0 || (opcao < 1 || opcao > 5));

    return opcao;
}

void adicionarregistro() {
    char buffer[100];
    int n, valor, *sucesso;
    FILE *ptr;

    printf("1. Cursos\n");
    printf("2. Disciplinas\n");
    printf("3. Turmas\n");
    printf("4. Alunos\n");
    printf("5. Professores\n");
    printf("Escolha um arquivo: ");

    do {
        n = input(buffer, 100);
        valor = toint(buffer, &sucesso);
    } while (n == 0 || *sucesso == 0 || (valor < 1 || valor > 5));

    if (valor == 1) {
        ptr = fopen("cursos.txt", "a");

        int id;
        do {
            n = input(buffer, 100);
            valor = toint(buffer, &sucesso);
        } while (n == 0 || *sucesso == 0 || valor < 0);

        printf("%d\n", id);


        fclose(ptr);
    }


}

int main() {
    int opcao, i;
    FILE *ptr;
    char *arquivos[5] = {"cursos.txt", "disciplinas.txt", "turmas.txt", "alunos.txt", "professores.txt"};
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