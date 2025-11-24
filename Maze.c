//
// Created by dio on 11/12/25.
//
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

void iniciaGrafo();

int main() {
    int opt, init = 1;
    while (init) {
        printf("Inicio do projeto\n");
        printf("Opções:\n1. Iniciar Grafo\n2. Grafos Disponiveis\n3. Carregar Grafo de Arquivo\n0. Sair");
        scanf("%d",&opt);
        if ((opt < 0)||(opt >3)) {
            printf("Insira uma opção válida");
        }
        switch (opt) {
            case 0:
                init = 0;
                break;
            case 1:
                iniciaGrafo();
                break;
            case 2:
                break;

        }
        return 0;
    }

}
// Função ver Grafos(historias) Disponiveis

//Função que inicia Grafo
void iniciaGrafo() {
    printf("Iniciar Grafo");
}
//Função Adiciona Nodo ao grafo
void adicionaNodo() {
    printf("adiciona grafo");
}
//Função Remove Nodo
void removeNodo() {
    printf("remove nodo");
}
//Verifica Conectividade
void conectividade() {
    printf("verifica conectividade");
}
//Buscar CAminho entre dois vertices
void caminho() {
    printf("Iniciar Grafo");
}
//Verificar Grau do vertice
void verificaGrau() {
    printf("verifica grau");
}
//Salvar e Carregar arquivo do grafo
void carregar() {
    printf("carrega Grafo");
}
void salvar() {
    printf("Salva Grafo");
}