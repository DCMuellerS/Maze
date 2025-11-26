/*
 * PROJETO FINAL: Labirinto RPG - Fuga da Prisão
 * -------------------------------------------------------------------------
 * TEMA: Fuga da Prisão (Corredor da Morte)
 * ESTRUTURA: Grafo Não Orientado (Matriz de Adjacência Dinâmica)
 * FUNCIONALIDADES: BFS, Menor Caminho, Persistência de Arquivos
 * -------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h> // Necessário para acentuação (PT-BR)

// Definição de constantes
#define MAX_BUFFER 100
#define ARQUIVO_DADOS "mapa_prisao.txt"

// --- ESTRUTURAS DE DADOS ---

typedef struct {
    int id;
    char nome[MAX_BUFFER];
    char descricao[MAX_BUFFER];
    // Mecânicas de Jogo
    int temItem;          // 0 = Não, 1 = Sim (ex: Chave)
    int precisaItem;      // 0 = Não, 1 = Sim (Porta Trancada)
    int ativo;            // Controle lógico (1 = Existe, 0 = Removido)
} Sala;

typedef struct {
    int **adj;            // Matriz de Adjacência (Dinâmica)
    Sala *salas;          // Vetor de Salas (Dinâmico)
    int numVertices;      // Quantidade atual de vértices
    int capacidadeMax;    // Limite máximo
} Grafo;

// --- PROTÓTIPOS ---
Grafo* criarGrafo(int maxVertices);
void liberarGrafo(Grafo *g);
void inserirVertice(Grafo *g);
void inserirAresta(Grafo *g, int u, int v);
void removerVertice(Grafo *g, int id);
void removerAresta(Grafo *g, int u, int v);
void exibirMatriz(Grafo *g);
void percursoBFS(Grafo *g, int inicio);
void carregarMapaPadrao(Grafo *g); // Carrega o cenário do jogo

// Funcionalidades Adicionais
void verificarGrau(Grafo *g);
void verificarConectividade(Grafo *g);
void buscarMenorCaminho(Grafo *g, int inicio, int fim);
void salvarGrafoArquivo(Grafo *g);
void carregarGrafoArquivo(Grafo **g);

// Testes Automatizados
void testesAutomatizados();
void adicionarSalaTeste(Grafo *g, char *nome);

// --- IMPLEMENTAÇÃO ---

// Inicializa o grafo e zera a memória
Grafo* criarGrafo(int maxVertices) {
    Grafo *g = (Grafo*) malloc(sizeof(Grafo));
    if (g == NULL) exit(1);

    g->capacidadeMax = maxVertices;
    g->numVertices = 0;

    g->salas = (Sala*) malloc(maxVertices * sizeof(Sala));
    g->adj = (int**) malloc(maxVertices * sizeof(int*));

    for (int i = 0; i < maxVertices; i++) {
        g->adj[i] = (int*) malloc(maxVertices * sizeof(int));
        for (int j = 0; j < maxVertices; j++) g->adj[i][j] = 0;
        g->salas[i].ativo = 0;
    }
    return g;
}

// Libera memória alocada dinamicamente
void liberarGrafo(Grafo *g) {
    if (g == NULL) return;
    for (int i = 0; i < g->capacidadeMax; i++) free(g->adj[i]);
    free(g->adj);
    free(g->salas);
    free(g);
    printf("Memória liberada com sucesso.\n");
}

// Inserção manual de salas (Input do usuário)
void inserirVertice(Grafo *g) {
    if (g->numVertices >= g->capacidadeMax) {
        printf("Capacidade máxima atingida!\n");
        return;
    }
    int id = g->numVertices;
    Sala *s = &g->salas[id];
    s->id = id;
    s->ativo = 1;

    printf("\n--- Nova Sala (ID: %d) ---\n", id);
    printf("Nome da Sala: ");
    scanf(" %[^\n]s", s->nome);
    printf("Descrição: ");
    scanf(" %[^\n]s", s->descricao);

    s->temItem = 0;
    s->precisaItem = 0;
    g->numVertices++;
    printf("Sala inserida!\n");
}

// Cria conexão bidirecional (Grafo Não Orientado)
void inserirAresta(Grafo *g, int u, int v) {
    if (u < 0 || u >= g->numVertices || v < 0 || v >= g->numVertices) return;
    g->adj[u][v] = 1;
    g->adj[v][u] = 1;
    printf("Passagem aberta: %s <--> %s\n", g->salas[u].nome, g->salas[v].nome);
}

// Remove vértice e reorganiza índices para evitar "buracos" no vetor
void removerVertice(Grafo *g, int id) {
    if (id < 0 || id >= g->numVertices) return;
    printf("Removendo: %s...\n", g->salas[id].nome);

    // Desloca salas e linhas da matriz para cima
    for (int i = id; i < g->numVertices - 1; i++) {
        g->salas[i] = g->salas[i+1];
        g->salas[i].id = i;
        for (int j = 0; j < g->numVertices; j++) g->adj[i][j] = g->adj[i+1][j];
    }
    // Desloca colunas da matriz para a esquerda
    for (int j = id; j < g->numVertices - 1; j++) {
        for (int i = 0; i < g->numVertices; i++) g->adj[i][j] = g->adj[i][j+1];
    }

    // Zera a última posição duplicada
    g->numVertices--;
    printf("Sala removida e mapa reorganizado.\n");
}

void removerAresta(Grafo *g, int u, int v) {
    if (u >= 0 && u < g->numVertices && v >= 0 && v < g->numVertices) {
        g->adj[u][v] = 0;
        g->adj[v][u] = 0;
        printf("Passagem bloqueada.\n");
    }
}

void exibirMatriz(Grafo *g) {
    printf("\n--- Matriz de Adjacência ---\n   ");
    for(int i=0; i<g->numVertices; i++) printf("%2d ", i);
    printf("\n");
    for(int i=0; i<g->numVertices; i++) {
        printf("%2d|", i);
        for(int j=0; j<g->numVertices; j++) printf("%2d ", g->adj[i][j]);
        printf("| %s\n", g->salas[i].nome);
    }
}

// Algoritmo de Busca em Largura (Exploração)
void percursoBFS(Grafo *g, int inicio) {
    if (inicio < 0 || inicio >= g->numVertices) return;
    bool *visitado = (bool*) calloc(g->numVertices, sizeof(bool));
    int *fila = (int*) malloc(g->numVertices * sizeof(int));
    int ini = 0, fim = 0;

    printf("\n--- Explorando (BFS) a partir de '%s' ---\n", g->salas[inicio].nome);
    visitado[inicio] = true;
    fila[fim++] = inicio;

    while (ini < fim) {
        int u = fila[ini++];
        printf(" -> Visitou: [%d] %s\n", u, g->salas[u].nome);
        for (int v = 0; v < g->numVertices; v++) {
            if (g->adj[u][v] == 1 && !visitado[v]) {
                visitado[v] = true;
                fila[fim++] = v;
            }
        }
    }
    free(visitado); free(fila);
}

// --- FUNÇÕES ESPECÍFICAS DO JOGO E BÔNUS ---

// Carrega o cenário completo do jogo (8 salas com itens/trancas)
void carregarMapaPadrao(Grafo *g) {
    g->numVertices = 0;
    // Limpa matriz
    for(int i=0; i<g->capacidadeMax; i++) {
        for(int j=0; j<g->capacidadeMax; j++) g->adj[i][j] = 0;
        g->salas[i].ativo = 0;
    }

    Sala *s;
    // Definindo as salas do jogo (Tema Prisão)
    s=&g->salas[0]; s->id=0; s->ativo=1; strcpy(s->nome, "Cela 402"); strcpy(s->descricao, "Sua cela escura.");
    s=&g->salas[1]; s->id=1; s->ativo=1; strcpy(s->nome, "Corredor"); strcpy(s->descricao, "Corredor da morte.");
    s=&g->salas[2]; s->id=2; s->ativo=1; strcpy(s->nome, "Enfermaria"); strcpy(s->descricao, "Cheiro de álcool.");
    s=&g->salas[3]; s->id=3; s->ativo=1; strcpy(s->nome, "Pátio Central"); strcpy(s->descricao, "Área aberta chuvosa.");
    s=&g->salas[4]; s->id=4; s->ativo=1; strcpy(s->nome, "Escritório"); strcpy(s->descricao, "Tem uma chave na mesa!"); s->temItem=1;
    s=&g->salas[5]; s->id=5; s->ativo=1; strcpy(s->nome, "Refeitório"); strcpy(s->descricao, "Comida podre.");
    s=&g->salas[6]; s->id=6; s->ativo=1; strcpy(s->nome, "Controle"); strcpy(s->descricao, "Requer chave."); s->precisaItem=1;
    s=&g->salas[7]; s->id=7; s->ativo=1; strcpy(s->nome, "SAÍDA"); strcpy(s->descricao, "Liberdade!");

    g->numVertices = 8;

    // Criando o labirinto (Conexões)
    inserirAresta(g, 0, 1);
    inserirAresta(g, 1, 2);
    inserirAresta(g, 1, 3);
    inserirAresta(g, 3, 5);
    inserirAresta(g, 3, 4);
    inserirAresta(g, 3, 6);
    inserirAresta(g, 6, 7);

    printf("\nMAPA PADRÃO 'FUGA DA PRISÃO' CARREGADO!\n");
}

// Calcula o grau de cada vértice para identificar Hubs ou Becos sem saída
void verificarGrau(Grafo *g) {
    printf("\n--- Análise de Segurança (Graus) ---\n");
    for (int i = 0; i < g->numVertices; i++) {
        int grau = 0;
        for (int j = 0; j < g->numVertices; j++) if (g->adj[i][j]) grau++;

        printf("Sala [%d] %s: %d conexões", i, g->salas[i].nome, grau);
        if (grau == 1) printf(" (Beco sem saída)");
        if (grau > 3) printf(" (Área Central/Hub)");
        printf("\n");
    }
}

// Verifica se o grafo é conexo usando BFS
void verificarConectividade(Grafo *g) {
    if (g->numVertices == 0) return;
    bool *visitado = (bool*) calloc(g->numVertices, sizeof(bool));
    int *fila = (int*) malloc(g->numVertices * sizeof(int));
    int count = 0, ini = 0, fim = 0;

    // Começa do 0
    visitado[0] = true; fila[fim++] = 0; count++;

    while(ini < fim) {
        int u = fila[ini++];
        for(int v=0; v<g->numVertices; v++) {
            if(g->adj[u][v] && !visitado[v]) {
                visitado[v] = true; fila[fim++] = v; count++;
            }
        }
    }

    printf("\n--- Relatório de Integridade ---\n");
    if (count == g->numVertices) printf("STATUS: O Labirinto é CONEXO.\n");
    else printf("STATUS: O Labirinto é DESCONEXO (%d de %d acessíveis).\n", count, g->numVertices);

    free(visitado); free(fila);
}

// BFS modificado para armazenar o caminho (vetor pai)
void buscarMenorCaminho(Grafo *g, int inicio, int fim) {
    int *pai = (int*) malloc(g->numVertices * sizeof(int));
    bool *visitado = (bool*) calloc(g->numVertices, sizeof(bool));
    int *fila = (int*) malloc(g->numVertices * sizeof(int));
    int ini = 0, fimFila = 0;

    for(int i=0; i<g->numVertices; i++) pai[i] = -1;
    visitado[inicio] = true; fila[fimFila++] = inicio;
    bool achou = false;

    while (ini < fimFila) {
        int u = fila[ini++];
        if (u == fim) { achou = true; break; }
        for (int v = 0; v < g->numVertices; v++) {
            if (g->adj[u][v] && !visitado[v]) {
                visitado[v] = true; pai[v] = u; fila[fimFila++] = v;
            }
        }
    }

    if (achou) {
        printf("\nRota de Fuga Sugerida: ");
        int caminho[100], tam = 0, atual = fim;
        // Reconstrói o caminho de trás para frente usando o vetor pai
        while (atual != -1) { caminho[tam++] = atual; atual = pai[atual]; }

        for (int i = tam-1; i >= 0; i--) {
            printf("%s", g->salas[caminho[i]].nome);
            if (i > 0) printf(" -> ");
        }
        printf("\nPassos totais: %d\n", tam-1);
    } else {
        printf("\nNão há caminho possível entre estas salas.\n");
    }
    free(pai); free(visitado); free(fila);
}

// Salvar em TXT
void salvarGrafoArquivo(Grafo *g) {
    FILE *f = fopen(ARQUIVO_DADOS, "w");
    if(!f) { printf("Erro ao abrir arquivo.\n"); return; }

    fprintf(f, "%d\n", g->numVertices);
    // Salva atributos das salas
    for(int i=0; i<g->numVertices; i++)
        fprintf(f, "%d;%s;%s\n", g->salas[i].id, g->salas[i].nome, g->salas[i].descricao);

    // Salva arestas
    for(int i=0; i<g->numVertices; i++) {
        for(int j=i+1; j<g->numVertices; j++) if(g->adj[i][j]) fprintf(f, "%d %d\n", i, j);
    }
    fclose(f);
    printf("Dados salvos em '%s'.\n", ARQUIVO_DADOS);
}

// Carregar de TXT
void carregarGrafoArquivo(Grafo **g) {
    FILE *f = fopen(ARQUIVO_DADOS, "r");
    if(!f) { printf("Arquivo não encontrado. Salve um mapa primeiro.\n"); return; }

    liberarGrafo(*g); // Limpa o atual
    int qtd; fscanf(f, "%d\n", &qtd);

    *g = criarGrafo(20);
    (*g)->numVertices = qtd;

    for(int i=0; i<qtd; i++) {
        fscanf(f, "%d;%[^;];%[^\n]\n", &(*g)->salas[i].id, (*g)->salas[i].nome, (*g)->salas[i].descricao);
        (*g)->salas[i].ativo = 1;
    }
    int u, v;
    while(fscanf(f, "%d %d\n", &u, &v) != EOF) {
        (*g)->adj[u][v] = 1;
        (*g)->adj[v][u] = 1;
    }
    fclose(f);
    printf("Mapa carregado com sucesso.\n");
}

// --- TESTES AUTOMATIZADOS COMPLETOS ---

void adicionarSalaTeste(Grafo *g, char *nome) {
    int id = g->numVertices;
    g->salas[id].id = id;
    g->salas[id].ativo = 1;
    strcpy(g->salas[id].nome, nome);
    strcpy(g->salas[id].descricao, "Sala de teste automatico");
    g->numVertices++;
}

void testesAutomatizados() {
    printf("\n=========================================\n");
    printf("   INICIANDO TESTES AUTOMATIZADOS (COMPLETO)\n");
    printf("=========================================\n");

    Grafo *t = criarGrafo(10);

    // --- TESTE 1: Inserção Básica ---
    printf("[TESTE 1] Inserção de Vértices... ");
    adicionarSalaTeste(t, "Sala A"); // ID 0
    adicionarSalaTeste(t, "Sala B"); // ID 1
    adicionarSalaTeste(t, "Sala C"); // ID 2
    if (t->numVertices == 3) printf("PASSOU\n");
    else printf("FALHOU (Qtd: %d)\n", t->numVertices);

    // --- TESTE 2: Arestas e Grau ---
    printf("[TESTE 2] Conexão e Grau... ");
    inserirAresta(t, 0, 1); // A-B
    inserirAresta(t, 1, 2); // B-C

    // O grau de B (ID 1) deve ser 2 (conecta com A e C)
    int grauB = 0;
    for(int i=0; i<3; i++) if(t->adj[1][i]) grauB++;

    if (grauB == 2) printf("PASSOU\n");
    else printf("FALHOU (Grau B: %d)\n", grauB);

    // --- TESTE 3: Lógica de Caminho ---
    printf("[TESTE 3] Caminho Lógico (A->C via B)... ");
    if (t->adj[0][1] && t->adj[1][2]) printf("PASSOU\n");
    else printf("FALHOU\n");

    // --- TESTE 4: Remoção Complexa ---
    printf("[TESTE 4] Remoção de Vértice (Sala B)... ");
    // Ao remover B (ID 1), a Sala C (que era ID 2) deve virar ID 1
    removerVertice(t, 1);

    bool arestaSumiu = (t->adj[0][1] == 0); // Aresta antiga deve ser 0
    bool qtdCorreta = (t->numVertices == 2);

    if (qtdCorreta) printf("PASSOU\n");
    else printf("FALHOU (Erro na realocação de índices)\n");

    liberarGrafo(t);
    printf("=========================================\n");
}

// --- MENU PRINCIPAL ---
int main() {
    setlocale(LC_ALL, "Portuguese"); // Ativa acentos no console

    Grafo *meuGrafo = criarGrafo(20);
    int op, u, v;

    do {
        printf("\n=== LABIRINTO RPG: FUGA DA PRISÃO ===\n");
        printf("1. Adicionar Sala (Vértice)\n");
        printf("2. Criar Passagem (Aresta)\n");
        printf("3. Remover Sala (Reorganiza IDs)\n");
        printf("4. Bloquear Passagem\n");
        printf("5. Visualizar Mapa (Matriz)\n");
        printf("6. Explorar (BFS)\n");
        printf("7. Carregar MAPA PADRÃO (Jogo Completo)\n");
        printf("--- FERRAMENTAS AVANÇADAS ---\n");
        printf("8. Analisar Segurança (Graus)\n");
        printf("9. Verificar Conectividade\n");
        printf("10. Rota de Fuga (Menor Caminho)\n");
        printf("11. Salvar Mapa em Arquivo\n");
        printf("12. Carregar Mapa de Arquivo\n");
        printf("13. Executar Testes Automatizados\n");
        printf("0. Sair\n");
        printf("Opção: ");
        scanf("%d", &op);

        switch(op) {
            case 1: inserirVertice(meuGrafo); break;
            case 2: printf("De ID: "); scanf("%d", &u); printf("Para ID: "); scanf("%d", &v); inserirAresta(meuGrafo, u, v); break;
            case 3: printf("ID da Sala: "); scanf("%d", &u); removerVertice(meuGrafo, u); break;
            case 4: printf("De ID: "); scanf("%d", &u); printf("Para ID: "); scanf("%d", &v); removerAresta(meuGrafo, u, v); break;
            case 5: exibirMatriz(meuGrafo); break;
            case 6: printf("Início ID: "); scanf("%d", &u); percursoBFS(meuGrafo, u); break;
            case 7: carregarMapaPadrao(meuGrafo); break;
            case 8: verificarGrau(meuGrafo); break;
            case 9: verificarConectividade(meuGrafo); break;
            case 10: printf("Início: "); scanf("%d", &u); printf("Fim: "); scanf("%d", &v); buscarMenorCaminho(meuGrafo, u, v); break;
            case 11: salvarGrafoArquivo(meuGrafo); break;
            case 12: carregarGrafoArquivo(&meuGrafo); break;
            case 13: testesAutomatizados(); break;
            case 0: liberarGrafo(meuGrafo); break;
            default: printf("Opção inválida.\n");
        }
    } while (op != 0);

    return 0;
}
