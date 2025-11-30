/*
 * PROJETO FINAL: Labirinto RPG
 * -------------------------------------------------------------------------
 * INTEGRANTES: Enzo, Diogo
 * TEMA: Estrutura de Grafo não orientado no estilo livro jogo RPG
 * ESTRUTURA: Grafo Não Orientado
 * REPRESENTAÇÃO: Matriz de Adjacência
 * PERCURSO: Profundidade
 * FUNCIONALIDADES ADICIONAIS : Verificar Conectividade, Busca de Caminho entre Dois Vértices BFS,
 *                              Menor Caminho, Persistência de Arquivos(gerador de arquivo .dot).
 * -------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h> // Necessário para acentuação (PT-BR)

// Definição de constantes
#define MAX_BUFFER 250 // tamanho máximo das strings
#define ARQUIVO_DADOS "mapa_prisao.txt"

// --- ESTRUTURAS DE DADOS ---

// Estrutura para representar um nó adjacente
typedef struct Adjacente {
    int destino;
    struct Adjacente *prox;
} Adjacente;

// Estrutura para representar um nó do grafo
typedef struct Nodo {
    int id;
    Adjacente *adjacentes;
    struct Nodo *prox;

    // Mecânicas de Jogo
    char nome[MAX_BUFFER];
    char descricao[MAX_BUFFER];
    bool temItem;          // 0 = Não, 1 = Sim (ex: Chave)
    bool precisaItem;      // 0 = Não, 1 = Sim (Porta Trancada)
} Nodo;

// Estrutura do grafo
typedef struct {
    Nodo *inicio;
    int numNodos;
} Grafo;

// --- IMPLEMENTAÇÃO ---
// Inicializa o grafo
void inicializarGrafo(Grafo *g) {
    g->inicio = NULL;
    g->numNodos = 0;
}

// Busca no grafo o endereço de um nodo especifico
Nodo* buscarNodo(Grafo *g, int id) {
    Nodo *atual = g->inicio;
    while (atual != NULL) {
        if (atual->id == id)
            return atual;
        atual = atual->prox;
    }
    return NULL;
}

// Adiciona um nodo ao grafo
void adicionarNodo(Grafo *g, int id) {
    char nomeSala[MAX_BUFFER], descriçãoSala[MAX_BUFFER];
    bool editando = 1, chave = 0, tranca = 0;
    // verificar se já existe o nodo
    if (buscarNodo(g, id) != NULL) {
        printf("Nodo %d ja existe!\n", id);
        return;
    }
    while (editando == 1) {
        printf("Insira o nome da Sala:");scanf("%s", nomeSala);
        printf("Insira a descrição da Sala:");scanf("%s", descriçãoSala);
        printf("Sala possui chave?\n(1 - Sim; 2 - Não)\n");scanf("%s", chave);
        printf("Sala trancada?\n(1 - Sim; 2 - Não)\n");scanf("%s", tranca);
        editando = 0;
    }

    //alocar todos os campos para o novo
    Nodo *novo = (Nodo*)malloc(sizeof(Nodo));
    novo->id = id;
    strcpy(novo->nome, nomeSala);
    strcpy(novo->descricao, descriçãoSala);
    novo->temItem = chave;
    novo->precisaItem = tranca;
    novo->adjacentes = NULL;
    novo->prox = g->inicio;
    g->inicio = novo;
    g->numNodos++;

    printf("Nodo %d adicionado com sucesso!\n", id);
}

// Remove uma aresta da lista de adjacências
void removerDaListaAdj(Nodo *nodo, int destino) {
    Adjacente *atual = nodo->adjacentes;
    Adjacente *anterior = NULL;

    while (atual != NULL) {
        if (atual->destino == destino) {
            if (anterior == NULL) {
                nodo->adjacentes = atual->prox;
            } else {
                anterior->prox = atual->prox;
            }
            free(atual);
            return;
        }
        anterior = atual;
        atual = atual->prox;
    }
}

// Remove um nodo do grafo
void removerNodo(Grafo *g, int id) {
    Nodo *nodo = buscarNodo(g, id);
    if (nodo == NULL) {
        printf("Nodo %d nao existe!\n", id);
        return;
    }

    // Remove todas as arestas que apontam para este nodo
    Nodo *atual = g->inicio;
    while (atual != NULL) {
        if (atual->id != id) {
            removerDaListaAdj(atual, id);
        }
        atual = atual->prox;
    }

    // Libera a lista de adjacentes do nodo
    Adjacente *adj = nodo->adjacentes;
    while (adj != NULL) {
        Adjacente *temp = adj;
        adj = adj->prox;
        free(temp);
    }

    // Remove o nodo da lista
    Nodo *ant = NULL;
    atual = g->inicio;
    while (atual != NULL) {
        if (atual->id == id) {
            if (ant == NULL) {
                g->inicio = atual->prox;
            } else {
                ant->prox = atual->prox;
            }
            free(atual);
            g->numNodos--;
            printf("Nodo %d removido com sucesso!\n", id);
            return;
        }
        ant = atual;
        atual = atual->prox;
    }
}

// Adiciona uma aresta entre dois nodos
void adicionarAresta(Grafo *g, int origem, int destino) {
    Nodo *nodoOrigem = buscarNodo(g, origem);
    if (nodoOrigem == NULL ) {
        printf("Nodo de origem não existe!\n");
        return;
    }
    Nodo *nodoDestino = buscarNodo(g, destino);
    if (nodoDestino == NULL) {
        printf("Nodo de destino não existe!\n");
        return;
    }

    // Verifica se a aresta já existe percorrendo a lista encadeada
    Adjacente *adj = nodoOrigem->adjacentes;
    while (adj != NULL) {
        if (adj->destino == destino) {
            printf("Aresta ja existe!\n");
            return;
        }
        adj = adj->prox;
    }

    // Adiciona destino à lista de adjacentes da origem
    Adjacente *novoAdj1 = (Adjacente*)malloc(sizeof(Adjacente));
    novoAdj1->destino = destino;
    novoAdj1->prox = nodoOrigem->adjacentes;
    nodoOrigem->adjacentes = novoAdj1;

    // Como é não orientado, adiciona origem à lista de adjacentes do destino
    Adjacente *novoAdj2 = (Adjacente*)malloc(sizeof(Adjacente));
    novoAdj2->destino = origem;
    novoAdj2->prox = nodoDestino->adjacentes;
    nodoDestino->adjacentes = novoAdj2;

    printf("Aresta entre %d e %d adicionada com sucesso!\n", origem, destino);
}

// Remove uma aresta entre dois nodos
void removerAresta(Grafo *g, int origem, int destino) {
    Nodo *nodoOrigem = buscarNodo(g, origem);
    if (nodoOrigem == NULL) {
        printf("Nodo de Origem não existe!\n");
        return;
    }
    Nodo *nodoDestino = buscarNodo(g, destino);
    if ( nodoDestino == NULL) {
        printf("Nodo de destino não existe!\n");
        return;
    }

    removerDaListaAdj(nodoOrigem, destino);
    removerDaListaAdj(nodoDestino, origem);

    printf("Passagem entre %d e %d removida com sucesso!\n", origem, destino);
}

// Imprime o grafo
void imprimirGrafo(Grafo *g) {
    if (g->inicio == NULL) {
        printf("Grafo vazio!\n");
        return;
    }

    printf("\n=== GRAFO ===\n");
    Nodo *nodo = g->inicio;
    while (nodo != NULL) {
        printf("Nodo %d: ", nodo->id);
        Adjacente *adj = nodo->adjacentes;
        if (adj == NULL) {
            printf("sem conexoes");
        } else {
            while (adj != NULL) {
                printf("%d", adj->destino);
                if (adj->prox != NULL)
                    printf(" -> ");
                adj = adj->prox;
            }
        }
        printf("\n");
        nodo = nodo->prox;
    }
    printf("=============\n\n");
}

// Libera toda a memória do grafo
void liberarGrafo(Grafo *g) {
    Nodo *nodo = g->inicio;
    while (nodo != NULL) {
        Adjacente *adj = nodo->adjacentes;
        while (adj != NULL) {
            Adjacente *temp = adj;
            adj = adj->prox;
            free(temp);
        }
        Nodo *temp = nodo;
        nodo = nodo->prox;
        free(temp);
    }
    g->inicio = NULL;
    g->numNodos = 0;
}
 void percorrerGrafo(Grafo *g, int ini) {
    int existe =  0 , quantidade = 0;
    if (g->inicio == NULL) {
        printf("Grafo vazio!\n");
        return;
    }
    Nodo *nodo = g->inicio;
    while (nodo->prox != NULL) {
            if (nodo->id == ini) {
                existe = 1;
                quantidade++;
            }
            else if ((nodo->prox == NULL) && (existe == 0)){
                printf("Nodo não existe para iniciar o percurso!\n");
                return;
            }else {
                quantidade++;
            }
            nodo = nodo->prox;
        }
    printf("\n=== DFS ===\n");
    printf("%d", quantidade);



    printf("=============\n\n");
}
// --- MENU PRINCIPAL ---
int main() {
    setlocale(LC_ALL, "Portuguese"); // Ativa acentos no console
    Grafo mapa;
    inicializarGrafo(&mapa);
    int op2, op1, u, v, op0, ID;

    do {
        printf("\n=== LABIRINTO RPG: FUGA DA PRISÃO ===\n");
        printf("1. Jogar\n");
        printf("2. Desenvolver\n");
        printf("0. Sair\n");
        printf("->");
        scanf("%d", &op0);
        if (op0 == 1) {
            do {
                printf("1. Fuga da Prisão\n");
                printf("2. Carregar mapa externo\n");
                printf("0. Voltar\n");
                scanf("%d", &op2);
                switch (op2) {
                    case 1:
                        break;
                    case 2:
                        break;
                    case 0:
                        op0 = 3;
                        break;
                    default:
                        printf("Digite uma opção valida");
                }
            }while (op0 == 1);
        }
        if (op0 == 2) {
            do {
                printf("1. Adicionar Sala (Cria Nodo)\n");
                printf("2. Criar Passagem (Cria Aresta)\n");
                printf("3. Remover Sala (Remove Nodo)\n");
                printf("4. Remover Passagem(Remover Aresta)\n");
                printf("5. Visualizar Mapa (Matriz)\n");

                printf("6. Explorar (BFS)\n");
                printf("7. Testar Mapa Padrão (BFS)\n");
                printf("--- FERRAMENTAS AVANÇADAS ---\n");
                printf("8. Analisar Segurança (Graus)\n");
                printf("9. Verificar Conectividade\n");
                printf("10. Rota de Fuga (Menor Caminho)\n");
                printf("11. Salvar Mapa em Arquivo\n");
                printf("12. Carregar Mapa de Arquivo\n");
                printf("13. Executar Testes Automatizados\n");
                printf("0. Voltar\n");
                printf("Opção: ");
                scanf("%d", &op1);

                switch(op1) {
                    case 1:
                        printf("Digite o ID da sala: ");
                        scanf("%d", &ID);
                        adicionarNodo(&mapa, ID); break;
                    case 2:
                        printf("ID de origem: ");scanf("%d", &u);
                        printf("ID de destino: ");scanf("%d", &v);
                        adicionarAresta(&mapa, u, v);break;
                    case 3:
                        printf("ID da Sala: ");scanf("%d", &u);
                        removerNodo(&mapa, u);break;
                    case 4:
                        printf("ID de origem: ");scanf("%d", &u);
                        printf("ID de destino: ");scanf("%d", &v);
                        removerAresta(&mapa, u, v);break;
                    case 5:
                        imprimirGrafo(&mapa);break;
                    case 6:
                        printf("Começar de ID: ");scanf("%d", &u);
                        percorrerGrafo(&mapa, u);break;
                    /*case 7:
                        carregarMapaPadrao(meuGrafo);break;
                    case 8:
                        verificarGrau(meuGrafo);break;
                    case 9:
                        verificarConectividade(meuGrafo); break;
                    case 10:
                        printf("Início: "); scanf("%d", &u);
                        printf("Fim: "); scanf("%d", &v);
                        buscarMenorCaminho(meuGrafo, u, v); break;
                    case 11:
                        salvarGrafoArquivo(meuGrafo); break;
                    case 12:
                        carregarGrafoArquivo(&meuGrafo); break;
                    case 13:
                        testesAutomatizados(); break;*/
                    case 0:
                        op0 = 3; break;
                    default:
                        printf("Opção inválida.\n");
                }

            } while (op0 == 2);

        }
    } while (op1 != 0);
    printf("Saindo");
    liberarGrafo(&mapa);
    return 0;
}
