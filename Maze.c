/*
 * PROJETO FINAL: Labirinto RPG - Fuga da Prisão (Versão Matriz)
 * -------------------------------------------------------------------------
 * TEMA: Fuga da Prisão (Corredor da Morte)
 * ESTRUTURA: Grafo Não Orientado (Matriz de Adjacência Dinâmica)
 * FUNCIONALIDADES: BFS, Menor Caminho, Persistência, Armadilhas e Pistas.
 * -------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>
#include <ctype.h>

// Definição de constantes
#define MAX_BUFFER 200
#define ARQUIVO_DADOS "mapa_prisao_matrix.txt"

// --- ESTRUTURAS DE DADOS ---

typedef struct {
    int id;
    char nome[MAX_BUFFER];
    char descricao[MAX_BUFFER];

    // Mecânicas de Jogo
    int temItem;          // 0 = Não, 1 = Sim (Chave)
    int precisaItem;      // 0 = Não, 1 = Sim (Porta Trancada)
    int ehSaida;          // 1 = Fim de Jogo
    int ativo;            // 1 = Existe, 0 = Removido

    // Armadilhas & Pistas
    int temArmadilha;
    char desafioArmadilha[MAX_BUFFER];
    char solucaoArmadilha[MAX_BUFFER];
    int armadilhaResolvida;
    int temPista;
    char textoPista[MAX_BUFFER];
} Sala;

typedef struct {
    int **adj;            // Matriz de Adjacência
    Sala *salas;          // Vetor de Salas
    int numVertices;      // Quantidade atual
    int capacidadeMax;    // Limite máximo
} Grafo;

// --- PROTÓTIPOS ---
void limparBuffer();
void pressionarEnter();
int lerNumero();
void normalizarTexto(char *str);

Grafo* criarGrafo(int maxVertices);
void liberarGrafo(Grafo *g);
void inserirVerticeManual(Grafo *g);
void inserirAresta(Grafo *g, int u, int v);
void removerVertice(Grafo *g, int id);
void removerAresta(Grafo *g, int u, int v);
void exibirMatriz(Grafo *g);

// Algoritmos
void percursoBFS(Grafo *g, int inicio);
void verificarGrau(Grafo *g);
void verificarConectividade(Grafo *g);
void buscarMenorCaminho(Grafo *g, int inicio, int fim);

// Arquivos e Jogo
void salvarGrafoArquivo(Grafo *g);
void carregarGrafoArquivo(Grafo **g);
void carregarCenarioSurvival(Grafo *g);
void jogarSurvival(Grafo *g);

// --- IMPLEMENTAÇÃO AUXILIAR ---

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pressionarEnter() {
    printf("\n[Pressione ENTER para continuar...]");
    int c = getchar();
    if (c != '\n') limparBuffer();
}

int lerNumero() {
    int valor;
    if (scanf("%d", &valor) == 1) {
        limparBuffer();
        return valor;
    } else {
        limparBuffer();
        return -1;
    }
}

void normalizarTexto(char *str) {
    for(int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper((unsigned char)str[i]);
        unsigned char c = (unsigned char)str[i];
        if (c >= 192 && c <= 197) str[i] = 'A';
        else if (c >= 200 && c <= 203) str[i] = 'E';
        else if (c >= 204 && c <= 207) str[i] = 'I';
        else if (c >= 210 && c <= 214) str[i] = 'O';
        else if (c >= 217 && c <= 220) str[i] = 'U';
        else if (c == 199) str[i] = 'C';
    }
}

// --- IMPLEMENTAÇÃO DO GRAFO ---

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

        // Inicialização limpa
        g->salas[i].id = i;
        g->salas[i].ativo = 0;
        strcpy(g->salas[i].nome, "");
        strcpy(g->salas[i].descricao, "");
        g->salas[i].temItem = 0;
        g->salas[i].precisaItem = 0;
        g->salas[i].ehSaida = 0;
        g->salas[i].temArmadilha = 0;
        g->salas[i].armadilhaResolvida = 0;
        g->salas[i].temPista = 0;
        strcpy(g->salas[i].desafioArmadilha, "");
        strcpy(g->salas[i].solucaoArmadilha, "");
        strcpy(g->salas[i].textoPista, "");
    }
    return g;
}

void liberarGrafo(Grafo *g) {
    if (g == NULL) return;
    for (int i = 0; i < g->capacidadeMax; i++) free(g->adj[i]);
    free(g->adj);
    free(g->salas);
    free(g);
}

void inserirVerticeManual(Grafo *g) {
    if (g->numVertices >= g->capacidadeMax) {
        printf("Capacidade máxima atingida!\n");
        return;
    }
    int id = g->numVertices;
    Sala *s = &g->salas[id];
    s->id = id;
    s->ativo = 1;

    printf("\n--- Nova Sala (ID: %d) ---\n", id);
    printf("Nome: "); scanf(" %[^\n]s", s->nome);
    printf("Descrição: "); scanf(" %[^\n]s", s->descricao);
    limparBuffer();

    printf("Tem Item Chave? (1-Sim/0-Não): ");
    int item = lerNumero(); s->temItem = (item == 1);

    printf("Porta Trancada? (1-Sim/0-Não): ");
    int tranca = lerNumero(); s->precisaItem = (tranca == 1);

    printf("É Saída? (1-Sim/0-Não): ");
    int saida = lerNumero(); s->ehSaida = (saida == 1);

    // Inicializa opcionais zerados
    s->temArmadilha = 0; s->temPista = 0;

    g->numVertices++;
    printf("Sala inserida com sucesso.\n");
}

void inserirAresta(Grafo *g, int u, int v) {
    if (u < 0 || u >= g->numVertices || v < 0 || v >= g->numVertices) return;
    g->adj[u][v] = 1;
    g->adj[v][u] = 1; // Não orientado
    printf("Passagem criada: %s <--> %s\n", g->salas[u].nome, g->salas[v].nome);
}

void removerVertice(Grafo *g, int id) {
    if (id < 0 || id >= g->numVertices) return;
    printf("Removendo: %s...\n", g->salas[id].nome);

    // Desloca para preencher buraco
    for (int i = id; i < g->numVertices - 1; i++) {
        g->salas[i] = g->salas[i+1];
        g->salas[i].id = i; // Atualiza ID
        for (int j = 0; j < g->numVertices; j++) g->adj[i][j] = g->adj[i+1][j];
    }
    for (int j = id; j < g->numVertices - 1; j++) {
        for (int i = 0; i < g->numVertices; i++) g->adj[i][j] = g->adj[i][j+1];
    }

    g->numVertices--;
    printf("Sala removida. IDs reorganizados.\n");
}

void removerAresta(Grafo *g, int u, int v) {
    if (u >= 0 && u < g->numVertices && v >= 0 && v < g->numVertices) {
        g->adj[u][v] = 0;
        g->adj[v][u] = 0;
        printf("Passagem removida.\n");
    }
}

void exibirMatriz(Grafo *g) {
    if(g->numVertices == 0) { printf("Mapa vazio.\n"); return; }
    printf("\n   ");
    for(int i=0; i<g->numVertices; i++) printf("%2d ", i);
    printf("\n");
    for(int i=0; i<g->numVertices; i++) {
        printf("%2d|", i);
        for(int j=0; j<g->numVertices; j++) printf("%2d ", g->adj[i][j]);
        printf("| %s", g->salas[i].nome);
        if(g->salas[i].ehSaida) printf(" [SAÍDA]");
        printf("\n");
    }
}

// --- ALGORITMOS ---

void percursoBFS(Grafo *g, int inicio) {
    if (inicio < 0 || inicio >= g->numVertices) return;
    bool *visitado = (bool*) calloc(g->numVertices, sizeof(bool));
    int *fila = (int*) malloc(g->numVertices * sizeof(int));
    int ini = 0, fim = 0;

    printf("\n--- BFS (Início: %s) ---\n", g->salas[inicio].nome);
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

void verificarGrau(Grafo *g) {
    printf("\n--- Análise de Salas (Grau) ---\n");
    for (int i = 0; i < g->numVertices; i++) {
        int grau = 0;
        for (int j = 0; j < g->numVertices; j++) if (g->adj[i][j]) grau++;
        printf("[%d] %-15s: %d conexões\n", i, g->salas[i].nome, grau);
    }
}

void verificarConectividade(Grafo *g) {
    if (g->numVertices == 0) return;
    bool *visitado = (bool*) calloc(g->numVertices, sizeof(bool));
    int *fila = (int*) malloc(g->numVertices * sizeof(int));
    int count = 0, ini = 0, fim = 0;

    visitado[0] = true; fila[fim++] = 0; count++;
    while(ini < fim) {
        int u = fila[ini++];
        for(int v=0; v<g->numVertices; v++) {
            if(g->adj[u][v] && !visitado[v]) {
                visitado[v] = true; fila[fim++] = v; count++;
            }
        }
    }
    printf("\nSalas acessíveis: %d de %d\n", count, g->numVertices);
    if (count == g->numVertices) printf("STATUS: Mapa Conexo.\n");
    else printf("STATUS: Mapa Desconexo.\n");
    free(visitado); free(fila);
}

void buscarMenorCaminho(Grafo *g, int inicio, int fim) {
    if (inicio < 0 || inicio >= g->numVertices || fim < 0 || fim >= g->numVertices) return;

    int *pai = (int*) malloc(g->numVertices * sizeof(int));
    bool *visitado = (bool*) calloc(g->numVertices, sizeof(bool));
    int *fila = (int*) malloc(g->numVertices * sizeof(int));
    int ini = 0, fimFila = 0;
    bool achou = false;

    for(int i=0; i<g->numVertices; i++) pai[i] = -1;
    visitado[inicio] = true; fila[fimFila++] = inicio;

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
        printf("\nRota de Fuga: ");
        int caminho[100], tam = 0, atual = fim;
        while (atual != -1) { caminho[tam++] = atual; atual = pai[atual]; }
        for (int i = tam-1; i >= 0; i--) {
            printf("%s", g->salas[caminho[i]].nome);
            if (i > 0) printf(" -> ");
        }
        printf("\nPassos: %d\n", tam-1);
    } else {
        printf("\nSem caminho possível.\n");
    }
    free(pai); free(visitado); free(fila);
}

// --- ARQUIVOS ---

void salvarGrafoArquivo(Grafo *g) {
    FILE *f = fopen(ARQUIVO_DADOS, "w");
    if(!f) { printf("Erro ao criar arquivo.\n"); return; }

    fprintf(f, "%d\n", g->numVertices);
    // Salva atributos expandidos
    for(int i=0; i<g->numVertices; i++) {
        Sala *s = &g->salas[i];
        fprintf(f, "%d;%s;%s;%d;%d;%d;%d;%s;%s;%d;%d;%s\n",
            s->id, s->nome, s->descricao, s->temItem, s->precisaItem, s->ehSaida,
            s->temArmadilha, s->desafioArmadilha, s->solucaoArmadilha, s->armadilhaResolvida,
            s->temPista, s->textoPista);
    }
    // Salva arestas
    for(int i=0; i<g->numVertices; i++) {
        for(int j=i+1; j<g->numVertices; j++) if(g->adj[i][j]) fprintf(f, "%d %d\n", i, j);
    }
    fclose(f);
    printf("Progresso salvo em '%s'.\n", ARQUIVO_DADOS);
}

void carregarGrafoArquivo(Grafo **g) {
    FILE *f = fopen(ARQUIVO_DADOS, "r");
    if(!f) { printf("Nenhum save encontrado.\n"); return; }

    liberarGrafo(*g);
    int qtd;
    if (fscanf(f, "%d\n", &qtd) != 1) { fclose(f); return; }

    *g = criarGrafo(20);
    (*g)->numVertices = qtd;

    for(int i=0; i<qtd; i++) {
        Sala *s = &(*g)->salas[i];
        fscanf(f, "%d;%[^;];%[^;];%d;%d;%d;%d;%[^;];%[^;];%d;%d;%[^\n]\n",
            &s->id, s->nome, s->descricao, &s->temItem, &s->precisaItem, &s->ehSaida,
            &s->temArmadilha, s->desafioArmadilha, s->solucaoArmadilha, &s->armadilhaResolvida,
            &s->temPista, s->textoPista);
        s->ativo = 1;
    }
    int u, v;
    while(fscanf(f, "%d %d\n", &u, &v) != EOF) {
        if (u < (*g)->numVertices && v < (*g)->numVertices) {
            (*g)->adj[u][v] = 1; (*g)->adj[v][u] = 1;
        }
    }
    fclose(f);
    printf("Jogo carregado com sucesso.\n");
}

// --- CONFIGURAÇÃO DO MAPA (SURVIVAL) ---

void configurarSala(Grafo *g, int id, char *nome, char *desc, int item, int tranca, int saida) {
    Sala *s = &g->salas[id];
    s->id = id; s->ativo = 1;
    strcpy(s->nome, nome); strcpy(s->descricao, desc);
    s->temItem = item; s->precisaItem = tranca; s->ehSaida = saida;
    // Reseta opcionais
    s->temArmadilha = 0; s->temPista = 0; s->armadilhaResolvida = 0;
}

void setArmadilha(Grafo *g, int id, char *pergunta, char *resp) {
    g->salas[id].temArmadilha = 1;
    strcpy(g->salas[id].desafioArmadilha, pergunta);
    strcpy(g->salas[id].solucaoArmadilha, resp);
    normalizarTexto(g->salas[id].solucaoArmadilha);
}

void setPista(Grafo *g, int id, char *texto) {
    g->salas[id].temPista = 1;
    strcpy(g->salas[id].textoPista, texto);
}

void carregarCenarioSurvival(Grafo *g) {
    // Reseta grafo mantendo a matriz alocada
    g->numVertices = 8;
    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++) g->adj[i][j] = 0;
    }

    // Configuração das 8 Salas
    configurarSala(g, 0, "Cela Inicial", "Onde tudo começa.", 0, 0, 0);

    configurarSala(g, 1, "Corredor Escuro", "Silêncio absoluto.", 0, 0, 0);
    setPista(g, 1, "PISTA: O que pulsa no peito e foge quando o corpo esfria? O contrário da podridão deste lugar.");

    configurarSala(g, 2, "Sala de Máquinas", "Fios expostos bloqueiam o caminho.", 0, 0, 0);
    setArmadilha(g, 2, "No Nordeste sou regra, em SP sou exceção. Tenho três dígitos e causo explosão. Quem sou?", "220");

    configurarSala(g, 3, "Almoxarifado", "Prateleiras empoeiradas.", 1, 0, 0); // CHAVE

    configurarSala(g, 4, "Esgoto", "Cheiro podre. Painel requer senha.", 0, 0, 0);
    setArmadilha(g, 4, "Digite a Senha:", "VIDA");

    configurarSala(g, 5, "Sala Controle", "Visão das câmeras.", 0, 0, 0);

    configurarSala(g, 6, "Portão Principal", "Porta blindada.", 0, 1, 0); // TRANCADO

    configurarSala(g, 7, "LIBERDADE", "O mundo exterior!", 0, 0, 1); // SAÍDA

    // Conexões
    inserirAresta(g, 0, 1);
    inserirAresta(g, 1, 2);
    inserirAresta(g, 2, 3); // Chave
    inserirAresta(g, 2, 4); // Caminho
    inserirAresta(g, 4, 5);
    inserirAresta(g, 5, 6);
    inserirAresta(g, 6, 7);

    printf("\n>>> CENÁRIO 'SURVIVAL' CARREGADO! <<<\n");
}

// --- ENGINE DO JOGO ---

void jogarSurvival(Grafo *g) {
    if (g->numVertices == 0) { printf("Carregue o cenário primeiro (Opção 10)!\n"); return; }

    int pos = 0;
    int temChave = 0;
    char resposta[MAX_BUFFER];

    printf("\n=== INÍCIO DO JOGO ===\n");

    while (1) {
        Sala *atual = &g->salas[pos];
        printf("\n---------------------------------\n");
        printf("LOCAL: [%d] %s\n", pos, atual->nome);
        printf("DESC: %s\n", atual->descricao);

        // 1. Armadilha
        if (atual->temArmadilha && !atual->armadilhaResolvida) {
            printf("\n[PERIGO] %s\n", atual->desafioArmadilha);
            printf("Resposta: ");
            scanf(" %[^\n]s", resposta); limparBuffer();
            normalizarTexto(resposta);

            if (strcmp(resposta, atual->solucaoArmadilha) == 0) {
                printf(">>> CORRETO! Caminho liberado.\n");
                atual->armadilhaResolvida = true;
            } else {
                printf(">>> ERRADO! Você morreu.\n");
                printf("\n*** GAME OVER ***\n");
                pressionarEnter();
                return;
            }
        }

        // 2. Pista
        if (atual->temPista) printf("[PISTA] Você lê: \"%s\"\n", atual->textoPista);

        // 3. Item
        if (atual->temItem) {
            printf("[ITEM] Você pegou a CHAVE!\n");
            temChave = 1; atual->temItem = 0;
        }

        // 4. Vitória
        if (atual->ehSaida) {
            printf("\n*********************************\n");
            printf(" PARABÉNS! VOCÊ ESCAPOU COM VIDA!\n");
            printf("*********************************\n");
            pressionarEnter();
            break;
        }

        // 5. Movimento
        printf("\nSaídas:\n");
        for (int i = 0; i < g->numVertices; i++) {
            if (g->adj[pos][i] == 1) {
                printf(" -> [%d] %s", i, g->salas[i].nome);
                if (g->salas[i].precisaItem) printf(" [TRANCADO]");
                printf("\n");
            }
        }

        printf("\nDestino (-1 Sair): ");
        int dest = lerNumero();
        if (dest == -1) break;

        if (dest >= 0 && dest < g->numVertices && g->adj[pos][dest]) {
            if (g->salas[dest].precisaItem && !temChave) {
                printf("Porta Trancada! Precisa da chave.\n");
            } else {
                if (g->salas[dest].precisaItem) printf("Porta destrancada!\n");
                pos = dest;
            }
        } else {
            printf("Caminho inválido.\n");
        }
    }
}

// --- MENU PRINCIPAL ---

int main() {
    setlocale(LC_ALL, "Portuguese");
    Grafo *meuGrafo = criarGrafo(20);
    int op, u, v;

    // Comando para limpar tela (Windows/Linux)
    system("cls || clear");

    do {
        printf("\n\n");
        printf("====================================================\n");
        printf("          LABIRINTO RPG: FUGA DA PRISÃO             \n");
        printf("====================================================\n");

        printf("\n   [ EDITOR DE MAPA ]\n");
        printf("   1. Adicionar Sala (Manual)\n");
        printf("   2. Criar Passagem (Aresta)\n");
        printf("   3. Remover Sala\n");
        printf("   4. Remover Passagem\n");

        printf("\n   [ ALGORITMOS ]\n");
        printf("   5. Visualizar Matriz\n");
        printf("   6. Explorar (BFS)\n");
        printf("   7. Verificar Grau\n");
        printf("   8. Verificar Conectividade\n");
        printf("   9. Rota de Fuga (Menor Caminho)\n");

        printf("\n   [ ARQUIVOS E JOGO ]\n");
        printf("   10. Carregar Cenário 'Survival' (Padrão)\n");
        printf("   11. Salvar Progresso\n");
        printf("   12. Carregar Progresso\n");

        printf("\n   -------------------------------------------------\n");
        printf("   >>> 13. INICIAR MODO SURVIVAL (JOGAR) <<<\n");
        printf("   -------------------------------------------------\n");
        printf("   0. Sair\n");

        printf("\n   Opção: ");
        op = lerNumero();

        printf("\n----------------------------------------------------\n");

        switch(op) {
            case 1: inserirVerticeManual(meuGrafo); break;
            case 2:
                printf("De ID: "); u = lerNumero();
                printf("Para ID: "); v = lerNumero();
                inserirAresta(meuGrafo, u, v); break;
            case 3:
                printf("ID da Sala: "); u = lerNumero();
                removerVertice(meuGrafo, u); break;
            case 4:
                printf("De ID: "); u = lerNumero();
                printf("Para ID: "); v = lerNumero();
                removerAresta(meuGrafo, u, v); break;
            case 5: exibirMatriz(meuGrafo); break;
            case 6:
                printf("Início ID: "); u = lerNumero();
                if(u != -1) percursoBFS(meuGrafo, u); break;
            case 7: verificarGrau(meuGrafo); break;
            case 8: verificarConectividade(meuGrafo); break;
            case 9:
                printf("Início: "); u = lerNumero();
                printf("Fim: "); v = lerNumero();
                if(u!=-1 && v!=-1) buscarMenorCaminho(meuGrafo, u, v); break;

            case 10: carregarCenarioSurvival(meuGrafo); break;
            case 11: salvarGrafoArquivo(meuGrafo); break;
            case 12: carregarGrafoArquivo(&meuGrafo); break;
            case 13: jogarSurvival(meuGrafo); break;

            case 0: printf("Saindo...\n"); break;
            default: if(op != -1) printf("Opção inválida.\n");
        }

        if (op != 0) {
            pressionarEnter();
            system("cls || clear");
        }

    } while (op != 0);

    liberarGrafo(meuGrafo);
    return 0;
}
