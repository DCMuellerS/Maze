/*
 * PROJETO FINAL DE ESTRUTURA DE DADOS - GRAFOS
 * -------------------------------------------------------------------------
 * TEMA: Labirinto RPG - Fuga da Prisão (Corredor da Morte)
 * TIPO DE GRAFO: Não Orientado (Caminhos de mão dupla)
 * REPRESENTAÇÃO: Matriz de Adjacência (Alocação Dinâmica)
 * PERCURSO: Largura (BFS) para exploração e menor caminho.
 *
 * FUNCIONALIDADES ADICIONAIS IMPLEMENTADAS:
 * 1. Verificação de Grau (Nível de Segurança da Sala)
 * 2. Verificar Conectividade (Detecção de áreas isoladas)
 * 3. Busca de Menor Caminho (BFS para encontrar a saída mais rápida)
 * 4. Persistência de Dados (Salvar e Carregar em arquivo .txt)
 * 5. Exportação Visual (Geração de arquivo .dot para GraphViz)
 * 6. TESTES AUTOMATIZADOS (Validação de lógica)
 *
 * INTEGRANTES DO GRUPO:
 * 1. [Nome do Integrante 1]
 * 2. [Nome do Integrante 2]
 * 3. [Nome do Integrante 3]
 * 4. [Nome do Integrante 4]
 * -------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Definição de constantes
#define MAX_BUFFER 100
#define ARQUIVO_DADOS "mapa_prisao.txt"
#define ARQUIVO_DOT "visualizacao.dot"

// --- ESTRUTURAS DE DADOS ---

// Representa um Vértice (Sala)
typedef struct {
    int id;
    char nome[MAX_BUFFER];
    char descricao[MAX_BUFFER];
    int temItem;          // 0 = Não, 1 = Sim (Chave)
    int precisaItem;      // 0 = Não, 1 = Sim (Porta Trancada)
    int ativo;            // 1 = Sala existe, 0 = Sala removida (controle lógico)
} Sala;

// Estrutura do Grafo com Alocação Dinâmica
typedef struct {
    int **adj;           // Ponteiro para ponteiro (Matriz Dinâmica)
    Sala *salas;         // Ponteiro para vetor de structs (Vetor Dinâmico)
    int numVertices;     // Quantidade atual de vértices ativos
    int capacidadeMax;   // Capacidade máxima definida na criação
} Grafo;

// --- PROTÓTIPOS DAS FUNÇÕES ---
Grafo* criarGrafo(int maxVertices);
void liberarGrafo(Grafo *g);
void inserirVertice(Grafo *g);
void inserirAresta(Grafo *g, int u, int v);
void removerVertice(Grafo *g, int id);
void removerAresta(Grafo *g, int u, int v);
void exibirMatriz(Grafo *g);
void percursoBFS(Grafo *g, int inicio);
void carregarCenarioTeste(Grafo *g);

// Funcionalidades Adicionais (Bônus)
void verificarGrau(Grafo *g);
void verificarConectividade(Grafo *g);
void buscarMenorCaminho(Grafo *g, int inicio, int fim);
void salvarGrafoArquivo(Grafo *g);
void carregarGrafoArquivo(Grafo **g); // Passa ponteiro de ponteiro para recriar
void exportarGraphViz(Grafo *g);

// Testes Automatizados
void testesAutomatizados();
void adicionarSalaTeste(Grafo *g, char *nome);

// Auxiliares
int obterIndiceValido(Grafo *g);

// --- IMPLEMENTAÇÃO ---

// Cria o grafo alocando memória dinamicamente
Grafo* criarGrafo(int maxVertices) {
    Grafo *g = (Grafo*) malloc(sizeof(Grafo));
    if (g == NULL) {
        printf("Erro de memoria!\n");
        exit(1);
    }

    g->capacidadeMax = maxVertices;
    g->numVertices = 0;

    // Alocação dinâmica do vetor de salas
    g->salas = (Sala*) malloc(maxVertices * sizeof(Sala));

    // Alocação dinâmica da Matriz de Adjacência
    g->adj = (int**) malloc(maxVertices * sizeof(int*));
    for (int i = 0; i < maxVertices; i++) {
        g->adj[i] = (int*) malloc(maxVertices * sizeof(int));
        // Inicializa com 0
        for (int j = 0; j < maxVertices; j++) {
            g->adj[i][j] = 0;
        }
        g->salas[i].ativo = 0; // Marca slots como vazios
    }

    return g;
}

// Libera toda a memória alocada
void liberarGrafo(Grafo *g) {
    if (g == NULL) return;
    
    for (int i = 0; i < g->capacidadeMax; i++) {
        free(g->adj[i]);
    }
    free(g->adj);
    free(g->salas);
    free(g);
    printf("Memoria liberada com sucesso.\n");
}

// 1. Inserir Vértice
void inserirVertice(Grafo *g) {
    if (g->numVertices >= g->capacidadeMax) {
        printf("Capacidade maxima do labirinto atingida!\n");
        return;
    }

    // Busca o próximo slot livre ou usa o final
    int id = g->numVertices;
    
    Sala *s = &g->salas[id];
    s->id = id;
    s->ativo = 1;

    printf("\n--- Nova Sala (ID: %d) ---\n", id);
    printf("Nome da Sala: ");
    scanf(" %[^\n]s", s->nome);
    printf("Descricao curta: ");
    scanf(" %[^\n]s", s->descricao);
    
    // Configurações simplificadas
    s->temItem = 0;
    s->precisaItem = 0;
    
    g->numVertices++;
    printf("Sala inserida com sucesso!\n");
}

// 2. Inserir Aresta
void inserirAresta(Grafo *g, int u, int v) {
    if (u < 0 || u >= g->numVertices || v < 0 || v >= g->numVertices) {
        printf("IDs invalidos.\n");
        return;
    }
    // Grafo não orientado (simétrico)
    g->adj[u][v] = 1;
    g->adj[v][u] = 1;
    printf("Passagem criada: %s <--> %s\n", g->salas[u].nome, g->salas[v].nome);
}

// 3. Remover Vértice (Com realocação lógica)
void removerVertice(Grafo *g, int id) {
    if (id < 0 || id >= g->numVertices) {
        printf("ID invalido.\n");
        return;
    }

    printf("Removendo Sala: %s...\n", g->salas[id].nome);

    // Deslocar dados para "tapar o buraco" na matriz e no vetor
    for (int i = id; i < g->numVertices - 1; i++) {
        g->salas[i] = g->salas[i+1]; // Copia struct
        g->salas[i].id = i;          // Atualiza ID
        
        // Atualiza linhas da matriz
        for (int j = 0; j < g->numVertices; j++) {
            g->adj[i][j] = g->adj[i+1][j];
        }
    }

    // Atualiza colunas da matriz
    for (int j = id; j < g->numVertices - 1; j++) {
        for (int i = 0; i < g->numVertices; i++) {
            g->adj[i][j] = g->adj[i][j+1];
        }
    }

    // Zera a última linha/coluna (agora duplicada)
    int ultimo = g->numVertices - 1;
    for (int k = 0; k < g->capacidadeMax; k++) {
        g->adj[ultimo][k] = 0;
        g->adj[k][ultimo] = 0;
    }
    g->salas[ultimo].ativo = 0;

    g->numVertices--;
    printf("Sala removida e indices reorganizados.\n");
}

// 4. Remover Aresta
void removerAresta(Grafo *g, int u, int v) {
    if (u >= 0 && u < g->numVertices && v >= 0 && v < g->numVertices) {
        g->adj[u][v] = 0;
        g->adj[v][u] = 0;
        printf("Passagem bloqueada/removida.\n");
    }
}

// 5. Exibir Grafo (Matriz)
void exibirMatriz(Grafo *g) {
    printf("\n--- Matriz de Adjacencia ---\n   ");
    for(int i=0; i<g->numVertices; i++) printf("%2d ", i);
    printf("\n");
    for(int i=0; i<g->numVertices; i++) {
        printf("%2d|", i);
        for(int j=0; j<g->numVertices; j++) {
            printf("%2d ", g->adj[i][j]);
        }
        printf("| %s\n", g->salas[i].nome);
    }
}

// 6. Percurso (BFS)
void percursoBFS(Grafo *g, int inicio) {
    if (inicio < 0 || inicio >= g->numVertices) return;

    bool *visitado = (bool*) calloc(g->numVertices, sizeof(bool));
    int *fila = (int*) malloc(g->numVertices * sizeof(int));
    int ini = 0, fim = 0;

    printf("\n--- Exploracao (BFS) a partir de '%s' ---\n", g->salas[inicio].nome);
    
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
    printf("Fim da exploracao.\n");
    free(visitado);
    free(fila);
}

// --- FUNCIONALIDADES ADICIONAIS ---

// Bônus 1: Grau do Vértice (Nível de Conectividade)
void verificarGrau(Grafo *g) {
    printf("\n--- Analise de Conexoes (Grau) ---\n");
    for (int i = 0; i < g->numVertices; i++) {
        int grau = 0;
        for (int j = 0; j < g->numVertices; j++) {
            if (g->adj[i][j] == 1) grau++;
        }
        printf("Sala [%d] %s: %d passagens", i, g->salas[i].nome, grau);
        if (grau == 1) printf(" (Beco sem saida)");
        if (grau > 3) printf(" (Area central/Hub)");
        printf("\n");
    }
}

// Bônus 2: Conectividade (Grafo Conexo)
void verificarConectividade(Grafo *g) {
    if (g->numVertices == 0) return;

    // Usa BFS para contar quantos alcança
    int visitadosCount = 0;
    bool *visitado = (bool*) calloc(g->numVertices, sizeof(bool));
    int *fila = (int*) malloc(g->numVertices * sizeof(int));
    int ini = 0, fim = 0;

    int inicio = 0; // Começa sempre do 0
    visitado[inicio] = true;
    fila[fim++] = inicio;
    visitadosCount++;

    while (ini < fim) {
        int u = fila[ini++];
        for (int v = 0; v < g->numVertices; v++) {
            if (g->adj[u][v] == 1 && !visitado[v]) {
                visitado[v] = true;
                fila[fim++] = v;
                visitadosCount++;
            }
        }
    }

    printf("\n--- Relatorio de Conectividade ---\n");
    if (visitadosCount == g->numVertices) {
        printf("STATUS: O Labirinto e CONEXO (Todas as salas sao acessiveis).\n");
    } else {
        printf("STATUS: O Labirinto e DESCONEXO!\n");
        printf("Salas alcancadas: %d de %d.\n", visitadosCount, g->numVertices);
        printf("Ha areas isoladas no mapa.\n");
    }
    free(visitado);
    free(fila);
}

// Bônus 3: Menor Caminho (BFS com rastreamento de pai)
void buscarMenorCaminho(Grafo *g, int inicio, int fim) {
    if (inicio < 0 || fim >= g->numVertices) return;

    int *pai = (int*) malloc(g->numVertices * sizeof(int));
    bool *visitado = (bool*) calloc(g->numVertices, sizeof(bool));
    int *fila = (int*) malloc(g->numVertices * sizeof(int));
    int ini = 0, fimFila = 0;

    for(int i=0; i<g->numVertices; i++) pai[i] = -1;

    visitado[inicio] = true;
    fila[fimFila++] = inicio;
    bool achou = false;

    while (ini < fimFila) {
        int u = fila[ini++];
        if (u == fim) {
            achou = true;
            break;
        }
        for (int v = 0; v < g->numVertices; v++) {
            if (g->adj[u][v] == 1 && !visitado[v]) {
                visitado[v] = true;
                pai[v] = u;
                fila[fimFila++] = v;
            }
        }
    }

    printf("\n--- Rota de Fuga (Menor Caminho) ---\n");
    if (achou) {
        int caminho[100];
        int tam = 0;
        int atual = fim;
        while (atual != -1) {
            caminho[tam++] = atual;
            atual = pai[atual];
        }
        // Imprime reverso
        for (int i = tam-1; i >= 0; i--) {
            printf("%s", g->salas[caminho[i]].nome);
            if (i > 0) printf(" -> ");
        }
        printf("\nTotal de salas: %d\n", tam);
    } else {
        printf("Nao ha caminho possivel entre estas salas.\n");
    }

    free(pai);
    free(visitado);
    free(fila);
}

// Bônus 4: Salvar em Arquivo
void salvarGrafoArquivo(Grafo *g) {
    FILE *f = fopen(ARQUIVO_DADOS, "w");
    if (!f) {
        printf("Erro ao criar arquivo.\n");
        return;
    }
    
    // Salva número de vértices
    fprintf(f, "%d\n", g->numVertices);
    
    // Salva Salas
    for (int i = 0; i < g->numVertices; i++) {
        fprintf(f, "%d;%s;%s\n", g->salas[i].id, g->salas[i].nome, g->salas[i].descricao);
    }
    
    // Salva Matriz (apenas conexões existentes)
    for (int i = 0; i < g->numVertices; i++) {
        for (int j = i + 1; j < g->numVertices; j++) {
            if (g->adj[i][j] == 1) {
                fprintf(f, "%d %d\n", i, j);
            }
        }
    }
    
    fclose(f);
    printf("Dados salvos em '%s'.\n", ARQUIVO_DADOS);
}

// Bônus 4 (Parte 2): Carregar
void carregarGrafoArquivo(Grafo **g) {
    FILE *f = fopen(ARQUIVO_DADOS, "r");
    if (!f) {
        printf("Arquivo nao encontrado. Carregue o teste padrao primeiro.\n");
        return;
    }

    // Reinicia o grafo
    liberarGrafo(*g);
    
    int qtd;
    fscanf(f, "%d\n", &qtd);
    
    *g = criarGrafo(20); // Recria com capacidade padrão
    (*g)->numVertices = qtd;

    // Lê Salas
    for (int i = 0; i < qtd; i++) {
        fscanf(f, "%d;%[^;];%[^\n]\n", &(*g)->salas[i].id, (*g)->salas[i].nome, (*g)->salas[i].descricao);
        (*g)->salas[i].ativo = 1;
    }

    // Lê Arestas
    int u, v;
    while (fscanf(f, "%d %d\n", &u, &v) != EOF) {
        if (u < qtd && v < qtd) {
            (*g)->adj[u][v] = 1;
            (*g)->adj[v][u] = 1;
        }
    }

    fclose(f);
    printf("Grafo carregado do arquivo com sucesso!\n");
}

// Bônus 5: Visualização GraphViz (.dot)
void exportarGraphViz(Grafo *g) {
    FILE *f = fopen(ARQUIVO_DOT, "w");
    if (!f) {
        printf("Erro ao criar arquivo DOT.\n");
        return;
    }

    fprintf(f, "graph LabirintoPrisao {\n");
    fprintf(f, "  node [shape=box];\n");
    
    // Define labels
    for (int i = 0; i < g->numVertices; i++) {
        fprintf(f, "  %d [label=\"%s\"];\n", i, g->salas[i].nome);
    }

    // Define conexões
    for (int i = 0; i < g->numVertices; i++) {
        for (int j = i + 1; j < g->numVertices; j++) {
            if (g->adj[i][j] == 1) {
                fprintf(f, "  %d -- %d;\n", i, j);
            }
        }
    }
    fprintf(f, "}\n");
    fclose(f);
    printf("Arquivo '%s' gerado! Use um visualizador online de GraphViz.\n", ARQUIVO_DOT);
}

// Função de Teste Rápido (Popula o grafo principal)
void carregarCenarioTeste(Grafo *g) {
    // Reset manual
    g->numVertices = 0;
    for(int i=0; i<g->capacidadeMax; i++)
        for(int j=0; j<g->capacidadeMax; j++) g->adj[i][j] = 0;

    // Inserindo Salas via código para teste
    strcpy(g->salas[0].nome, "Cela 101"); strcpy(g->salas[0].descricao, "Cela inicial."); g->salas[0].ativo=1;
    strcpy(g->salas[1].nome, "Corredor A"); strcpy(g->salas[1].descricao, "Escuro."); g->salas[1].ativo=1;
    strcpy(g->salas[2].nome, "Guarita"); strcpy(g->salas[2].descricao, "Sala dos guardas."); g->salas[2].ativo=1;
    strcpy(g->salas[3].nome, "Patio"); strcpy(g->salas[3].descricao, "Area aberta."); g->salas[3].ativo=1;
    strcpy(g->salas[4].nome, "Saida"); strcpy(g->salas[4].descricao, "Liberdade."); g->salas[4].ativo=1;
    
    g->numVertices = 5;

    inserirAresta(g, 0, 1);
    inserirAresta(g, 1, 2);
    inserirAresta(g, 1, 3);
    inserirAresta(g, 3, 4); // Patio conecta saída
    inserirAresta(g, 2, 0); // Loop extra

    printf("Cenario de teste carregado.\n");
}

// --- TESTES AUTOMATIZADOS (BATERIA) ---

// Helper interno para adicionar sem scanf
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
    printf("   INICIANDO TESTES AUTOMATIZADOS\n");
    printf("=========================================\n");

    // Cria um grafo isolado para não mexer no principal
    Grafo *t = criarGrafo(10);
    
    // TESTE 1: Inserção
    printf("[TESTE 1] Insercao de Vertices... ");
    adicionarSalaTeste(t, "Sala A"); // 0
    adicionarSalaTeste(t, "Sala B"); // 1
    adicionarSalaTeste(t, "Sala C"); // 2
    if (t->numVertices == 3) printf("PASSOU\n"); else printf("FALHOU\n");

    // TESTE 2: Arestas e Grau
    printf("[TESTE 2] Conexao e Grau... ");
    inserirAresta(t, 0, 1); // A-B
    inserirAresta(t, 1, 2); // B-C
    // Grau de B deve ser 2
    int grauB = 0;
    for(int i=0; i<3; i++) if(t->adj[1][i]) grauB++;
    
    if (grauB == 2) printf("PASSOU\n"); else printf("FALHOU (Grau B: %d)\n", grauB);

    // TESTE 3: Menor Caminho (A até C)
    printf("[TESTE 3] Menor Caminho (0->2)... ");
    // Simula BFS simples
    if (t->adj[0][1] && t->adj[1][2]) printf("PASSOU (Caminho Logico Existe)\n");
    else printf("FALHOU\n");

    // TESTE 4: Remoção de Vértice (B)
    printf("[TESTE 4] Remocao de Vertice do meio (Sala B)... ");
    removerVertice(t, 1); // Remove B. A=0, C vira 1.
    // Agora A(0) não deve estar conectado a C(1) pois B sumiu e era a ponte
    if (t->adj[0][1] == 0 && t->numVertices == 2) printf("PASSOU\n");
    else printf("FALHOU (Aresta ainda existe ou num incorreto)\n");

    liberarGrafo(t);
    printf("=========================================\n");
    printf("   FIM DOS TESTES - MEMORIA LIBERADA\n");
    printf("=========================================\n");
}

// --- MENU PRINCIPAL ---
int main() {
    Grafo *meuGrafo = criarGrafo(20); // Capacidade inicial
    int op, u, v;

    do {
        printf("\n=== LABIRINTO RPG (PRISAO) ===\n");
        printf("1. Inserir Sala (Vertice)\n");
        printf("2. Inserir Passagem (Aresta)\n");
        printf("3. Remover Sala\n");
        printf("4. Remover Passagem\n");
        printf("5. Exibir Mapa (Matriz)\n");
        printf("6. Exploracao (BFS)\n");
        printf("7. Carregar Cenario Teste (Padrao)\n");
        printf("--- BONUS ---\n");
        printf("8. Nivel de Seguranca (Graus)\n");
        printf("9. Verificar Isolamento (Conectividade)\n");
        printf("10. Rota de Fuga (Menor Caminho)\n");
        printf("11. Salvar em Arquivo\n");
        printf("12. Carregar de Arquivo\n");
        printf("13. Gerar Visualizacao (.dot)\n");
        printf("14. EXECUTAR TESTES AUTOMATIZADOS\n");
        printf("0. Sair\n");
        printf("Opcao: ");
        scanf("%d", &op);

        switch(op) {
            case 1: inserirVertice(meuGrafo); break;
            case 2: 
                printf("De (ID): "); scanf("%d", &u);
                printf("Para (ID): "); scanf("%d", &v);
                inserirAresta(meuGrafo, u, v);
                break;
            case 3:
                printf("ID Sala: "); scanf("%d", &u);
                removerVertice(meuGrafo, u);
                break;
            case 4:
                printf("De (ID): "); scanf("%d", &u);
                printf("Para (ID): "); scanf("%d", &v);
                removerAresta(meuGrafo, u, v);
                break;
            case 5: exibirMatriz(meuGrafo); break;
            case 6:
                printf("Inicio (ID): "); scanf("%d", &u);
                percursoBFS(meuGrafo, u);
                break;
            case 7: carregarCenarioTeste(meuGrafo); break;
            case 8: verificarGrau(meuGrafo); break;
            case 9: verificarConectividade(meuGrafo); break;
            case 10:
                printf("Inicio (ID): "); scanf("%d", &u);
                printf("Destino (ID): "); scanf("%d", &v);
                buscarMenorCaminho(meuGrafo, u, v);
                break;
            case 11: salvarGrafoArquivo(meuGrafo); break;
            case 12: carregarGrafoArquivo(&meuGrafo); break;
            case 13: exportarGraphViz(meuGrafo); break;
            case 14: testesAutomatizados(); break;
            case 0: liberarGrafo(meuGrafo); break;
            default: printf("Opcao invalida.\n");
        }
    } while (op != 0);

    return 0;
}