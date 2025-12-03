/*
 * PROJETO FINAL: Labirinto RPG
* 8. Rede de Locais (Salas, Ambientes, Labirinto)
Tipo: Grafo não orientado
Representação sugerida: Matriz de adjacência
Vértices: Salas ou locais
Arestas: Passagens diretas entre as salas
Implementação:
    • Inserir e remover salas e passagens.
    • Fazer BFS para encontrar todos os locais acessíveis a partir de uma sala inicial.
    • Verificar se há salas inacessíveis no mapa.
2. Verificar Conectividade do Grafo
Objetivo: Implementar uma função que detecta se o grafo é conexo (totalmente conectado).
* Se, através de um percurso, e a partir de um ponto inicial informado pelo usuário, todos os vértices forem
visitados, o grafo é conexo; caso contrário, há componentes desconexos.
3. Busca de Caminho entre Dois Vértices
Objetivo: Encontrar e mostrar o(s) caminho(s) possíveis entre dois vértices informados pelo usuário.
* Pode ser feito com BFS (para menor número de arestas) ou DFS (qualquer caminho), dependendo da aplicação.
Sempre relacionem ao tema escolhido para que a informação seja interessante ao usuário.
* O programa exibe o caminho percorrido, como, por exemplo: Caminho de A até E: A -> C -> D -> E
* 4. Verificação de Grau de Vértice
* Objetivo: Calcular e exibir o grau de cada vértice.
* No grafo orientado, isso envolve contar arestas de entrada e de saída.
* No grafo não orientado, apenas o número total de conexões.
5. Salvar e Carregar o Grafo de Arquivo
Objetivo: Permitir gravar e recuperar o grafo de um arquivo texto.
* Ao sair, o programa salva vértices e arestas em um arquivo.
* Na inicialização, lê o arquivo e reconstrói o grafo.
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
#define MAX_BUFFER 150 // tamanho máximo das strings
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

// Função auxiliar para limpar o buffer
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
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
void adNodoAut(Grafo *g, int id, char *Nome, char *Descricao, int Chave, int Tranca) {
    //alocar todos os campos para o novo
    Nodo *novo = (Nodo*)malloc(sizeof(Nodo));
    novo->id = id;
    strcpy(novo->nome, Nome);
    strcpy(novo->descricao, Descricao);
    novo->temItem = Chave;
    novo->precisaItem = Tranca;
    novo->adjacentes = NULL;
    novo->prox = g->inicio;
    g->inicio = novo;
    g->numNodos++;
    printf("Nodo %d adicionado com sucesso!\n", id);
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
        printf("Insira o nome da Sala:\n");scanf("%s", &nomeSala);
        limparBuffer();
        printf("Insira a descrição da Sala:\n");scanf("%s", &descriçãoSala);
        limparBuffer();
        printf("Sala possui chave?\n(1 - Sim; 0 - Não)\n"); scanf("%d", &chave);
        limparBuffer();
        printf("Sala trancada?\n(1 - Sim; 0 - Não)\n"); scanf("%d", &tranca);
        limparBuffer();
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
    int existe =  0 , quantidade = 1;
    if (g->inicio == NULL) {
        printf("Grafo vazio!\n");
        return;
    }
    Nodo *nodo = g->inicio;
    Nodo *nodoInicial;
    while (nodo->prox != NULL) {
            if (nodo->id == ini) {
                nodoInicial = nodo;
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
    int fila[quantidade];
    int visitados[quantidade];
    int topo = 0, i, j;

    for (i = 0; i < quantidade; i++) {
        visitados[i] = 0;
    }

    fila[topo] = ini;
    topo++;

    printf("\n=== DFS ===\n");

    while (topo > 0) {
        topo--;
        int atual = fila[topo];

        if (visitados[atual]) continue;

        visitados[atual] = 1;
        printf("Visitando nodo %d\n", atual);

        Nodo *nodoAtual = buscarNodo(g, atual);
        Adjacente *adj = nodoAtual->adjacentes;

        // Lista auxiliar para ordenar adjacentes
        int listaAux[quantidade];
        int aux = 0;

        while (adj != NULL) {
            if (!visitados[adj->destino]) {
                listaAux[aux] = adj->destino;
                aux++;
            }
            adj = adj->prox;
        }

        // Ordena a lista auxiliar
        for (i = 0; i < aux - 1; i++) {
            for (j = 0; j < aux - i - 1; j++) {
                if (listaAux[j] > listaAux[j + 1]) {
                    int temp = listaAux[j];
                    listaAux[j] = listaAux[j + 1];
                    listaAux[j + 1] = temp;
                }
            }
        }

        // Adiciona na pilha em ordem reversa (para processar na ordem crescente)
        for (i = aux - 1; i >= 0; i--) {
            fila[topo] = listaAux[i];
            topo++;
        }
    }

    printf("=============\n\n");
}
void carregarMapaPadrao(Grafo *g) {
    // Definindo as salas do jogo (Tema Prisão)
    liberarGrafo(g);

    adNodoAut(g, 0, "Cela 402", "Sua cela escura. Uma cama velha e paredes frias.", 0, 0);
    adNodoAut(g, 1, "Corredor", "Corredor mal iluminado. Cheiro de mofo no ar.", 0, 0);
    adNodoAut(g, 2, "Enfermaria", "Macas vazias e remedios espalhados pelo chao.", 0, 0);
    adNodoAut(g, 3, "Pátio Central", "Area aberta cercada por muros altos. Silencio total.", 0, 0);
    adNodoAut(g, 4, "Escritório", "Mesa bagunçada. Voce encontra uma CHAVE no gaveta!", 1, 0);
    adNodoAut(g, 5, "Refeitório", "Mesas vazias. Bandejas sujas ainda sobre elas.", 0, 0);
    adNodoAut(g, 6, "Portão de Segurança", "Porta de aco reforçado. TRANCADA!", 0, 1);
    adNodoAut(g, 7, "SAÍDA", "Liberdade! O ar fresco da rua te aguarda!", 0, 0);


    // Criando o labirinto (Conexões)
    adicionarAresta(g, 0, 1);
    adicionarAresta(g, 1, 2);
    adicionarAresta(g, 1, 3);
    adicionarAresta(g, 3, 5);
    adicionarAresta(g, 3, 4);
    adicionarAresta(g, 3, 6);
    adicionarAresta(g, 6, 7);

    printf("\nMAPA PADRÃO 'FUGA DA PRISÃO' CARREGADO!\n");
}
void verificarGrau(Grafo *g, int u) {
    int adjacentes = 0;
    if (g->inicio == NULL) {
        printf("Grafo vazio!\n");
        return;
    }

    Nodo *nodoDestino = buscarNodo(g, u);
    if (nodoDestino == NULL) {
        printf("não há conexões, Nodo não existe!\n");
        return;
    }


    Adjacente *adj = nodoDestino->adjacentes;
    while (adj != NULL) {
        adj = adj->prox;
        adjacentes++;
    }
    printf("Nodos adjacentes a %d = %d nodo(s)", u, adjacentes);

}
// BFS modificado para armazenar o caminho (vetor pai)
void buscarMenorCaminho(Grafo *g, int inicio, int fim) {
    if (g->inicio == NULL) {
        printf("Grafo vazio!\n");
        return;
    }

    // Verifica se os nodos existem
    Nodo *nodoInicio = buscarNodo(g, inicio);
    Nodo *nodoFim = buscarNodo(g, fim);
    if (nodoInicio == NULL || nodoFim == NULL) {
        printf("Nodo de início ou fim não existe!\n");
        return;
    }

    // Descobre o maior ID para dimensionar arrays
    int maxId = 0;
    Nodo *temp = g->inicio;
    while (temp != NULL) {
        if (temp->id > maxId) maxId = temp->id;
        temp = temp->prox;
    }
    maxId++; // +1 para usar IDs como índices

    // Aloca estruturas auxiliares
    int *pai = (int*) malloc(maxId * sizeof(int));
    bool *visitado = (bool*) calloc(maxId, sizeof(bool));
    int *fila = (int*) malloc(g->numNodos * sizeof(int));
    int iniF = 0, fimFila = 0;

    // Inicializa vetor pai
    for(int i = 0; i < maxId; i++) pai[i] = -1;

    // Inicia BFS
    visitado[inicio] = true;
    fila[fimFila++] = inicio;
    bool achou = false;

    while (iniF < fimFila) {
        int u = fila[iniF++];

        if (u == fim) {
            achou = true;
            break;
        }

        // Percorre adjacentes do nodo atual
        Nodo *nodoAtual = buscarNodo(g, u);
        Adjacente *adj = nodoAtual->adjacentes;

        while (adj != NULL) {
            int v = adj->destino;
            if (!visitado[v]) {
                visitado[v] = true;
                pai[v] = u;
                fila[fimFila++] = v;
            }
            adj = adj->prox;
        }
    }

    if (achou) {
        printf("\nRota de Fuga Sugerida: ");
        int caminho[100], tam = 0, atual = fim;

        // Reconstrói o caminho de trás para frente usando o vetor pai
        while (atual != -1) {
            caminho[tam++] = atual;
            atual = pai[atual];
        }

        // Imprime o caminho com os nomes das salas
        for (int i = tam - 1; i >= 0; i--) {
            Nodo *n = buscarNodo(g, caminho[i]);
            printf("%s", n->nome);
            if (i > 0) printf(" -> ");
        }
        printf("\nPassos totais: %d\n", tam - 1);
    } else {
        printf("\nNão há caminho possível entre estas salas.\n");
    }

    free(pai);
    free(visitado);
    free(fila);
}
void salvarGrafoArquivo(Grafo *g) {
    if (g->inicio == NULL) {
        printf("Grafo vazio!\n");
        return;
    }

    FILE *f = fopen(ARQUIVO_DADOS, "w");
    if(!f) { printf("Erro ao abrir arquivo.\n"); return; }

    fprintf(f, "%d\n", g->numNodos);

    // Salva atributos dos nodos
    Nodo *nodo = g->inicio;
    while (nodo != NULL) {
        fprintf(f, "%d;%s;%s;%d;%d\n", nodo->id, nodo->nome, nodo->descricao,
                nodo->temItem, nodo->precisaItem);
        nodo = nodo->prox;
    }

    // Salva arestas (evita duplicação percorrendo apenas IDs maiores)
    nodo = g->inicio;
    while (nodo != NULL) {
        Adjacente *adj = nodo->adjacentes;
        while (adj != NULL) {
            if (nodo->id < adj->destino) { // Evita aresta duplicada
                fprintf(f, "%d %d\n", nodo->id, adj->destino);
            }
            adj = adj->prox;
        }
        nodo = nodo->prox;
    }

    fclose(f);
    printf("Dados salvos em '%s'.\n", ARQUIVO_DADOS);
}
void carregarGrafoArquivo(Grafo *g) {
    FILE *f = fopen(ARQUIVO_DADOS, "r");
    if(!f) { printf("Arquivo não encontrado. Salve um mapa primeiro.\n"); return; }

    liberarGrafo(g);

    int qtd;
    if (fscanf(f, "%d\n", &qtd) != 1) {
        fclose(f);
        printf("Arquivo corrompido.\n");
        return;
    }

    // Carrega nodos
    for(int i = 0; i < qtd; i++) {
        int id, temItem, precisaItem;
        char nome[MAX_BUFFER], desc[MAX_BUFFER];
        fscanf(f, "%d;%[^;];%[^;];%d;%d\n", &id, nome, desc, &temItem, &precisaItem);
        adNodoAut(g, id, nome, desc, temItem, precisaItem);
    }

    // Carrega arestas
    int u, v;
    while(fscanf(f, "%d %d\n", &u, &v) != EOF) {
        adicionarAresta(g, u, v);
    }

    fclose(f);
    printf("Mapa carregado com sucesso.\n");
}
void testesAutomatizados() {
    printf("\n=========================================\n");
    printf("   INICIANDO TESTES AUTOMATIZADOS (COMPLETO)\n");
    printf("=========================================\n");

    Grafo t;
    inicializarGrafo(&t);

    // TESTE 1: Inserção
    printf("[TESTE 1] Inserção de Nodos... ");
    adNodoAut(&t, 0, "Sala A", "Teste A", 0, 0);
    adNodoAut(&t, 1, "Sala B", "Teste B", 0, 0);
    adNodoAut(&t, 2, "Sala C", "Teste C", 0, 0);

    if (t.numNodos == 3) printf("PASSOU\n");
    else printf("FALHOU (Qtd: %d)\n", t.numNodos);

    // TESTE 2: Arestas e Grau
    printf("[TESTE 2] Conexão e Grau... ");
    adicionarAresta(&t, 0, 1);
    adicionarAresta(&t, 1, 2);

    Nodo *nodoB = buscarNodo(&t, 1);
    int grauB = 0;
    Adjacente *adj = nodoB->adjacentes;
    while (adj != NULL) {
        grauB++;
        adj = adj->prox;
    }

    if (grauB == 2) printf("PASSOU\n");
    else printf("FALHOU (Grau B: %d)\n", grauB);

    // TESTE 3: Caminho Lógico
    printf("[TESTE 3] Caminho Lógico (A->C via B)... ");
    Nodo *nodoA = buscarNodo(&t, 0);
    bool temCaminhoAB = false;
    adj = nodoA->adjacentes;
    while (adj != NULL) {
        if (adj->destino == 1) temCaminhoAB = true;
        adj = adj->prox;
    }

    bool temCaminhoBC = false;
    adj = nodoB->adjacentes;
    while (adj != NULL) {
        if (adj->destino == 2) temCaminhoBC = true;
        adj = adj->prox;
    }

    if (temCaminhoAB && temCaminhoBC) printf("PASSOU\n");
    else printf("FALHOU\n");

    // TESTE 4: Remoção
    printf("[TESTE 4] Remoção de Nodo... ");
    removerNodo(&t, 1);
    if (t.numNodos == 2) printf("PASSOU\n");
    else printf("FALHOU (Qtd após remoção: %d)\n", t.numNodos);

    liberarGrafo(&t);
    printf("=========================================\n");
}
void jogarSurvival(Grafo *g) {
    if (g->inicio == NULL) {
        printf("Carregue o cenário primeiro!\n");
        return;
    }

    int posAtual = 0;
    int temChave = 0;
    char resposta[MAX_BUFFER];

    // Verifica se o nodo inicial existe
    Nodo *nodoInicial = buscarNodo(g, posAtual);
    if (nodoInicial == NULL) {
        printf("Erro: Sala inicial não encontrada!\n");
        return;
    }

    printf("\n=== INÍCIO DO JOGO ===\n");

    while (1) {
        Nodo *atual = buscarNodo(g, posAtual);
        if (atual == NULL) {
            printf("Erro: Sala não encontrada!\n");
            break;
        }

        printf("\n---------------------------------\n");
        printf("LOCAL: [%d] %s\n", posAtual, atual->nome);
        printf("DESC: %s\n", atual->descricao);

        // 1. Item (Chave)
        if (atual->temItem) {
            printf("[ITEM] Você pegou a CHAVE!\n");
            temChave = 1;
            atual->temItem = 0; // Remove o item da sala
        }

        if (strcmp(atual->nome, "SAÍDA") == 0) {
            printf("\n*********************************\n");
            printf(" PARABÉNS! VOCÊ ESCAPOU COM VIDA!\n");
            printf("*********************************\n");
            printf("\nPressione ENTER para continuar...");
            limparBuffer();
            getchar();
            break;
        }

        // 3. Movimento - Listar saídas disponíveis
        printf("\nSaídas disponíveis:\n");
        Adjacente *adj = atual->adjacentes;
        bool temSaida = false;

        while (adj != NULL) {
            Nodo *destino = buscarNodo(g, adj->destino);
            if (destino != NULL) {
                printf(" -> [%d] %s", adj->destino, destino->nome);
                if (destino->precisaItem) printf(" [TRANCADO]");
                printf("\n");
                temSaida = true;
            }
            adj = adj->prox;
        }

        if (!temSaida) {
            printf("Não há saídas disponíveis. Você está preso!\n");
            printf("\n*** GAME OVER ***\n");
            printf("\nPressione ENTER para continuar...");
            limparBuffer();
            getchar();
            break;
        }

        // 4. Receber escolha do jogador
        printf("\nPara onde deseja ir? (ID da sala, -1 para Sair): ");
        int dest;
        scanf("%d", &dest);
        limparBuffer();

        if (dest == -1) {
            printf("Você desistiu da fuga.\n");
            break;
        }

        // 5. Validar movimento
        bool caminhoValido = false;
        adj = atual->adjacentes;

        while (adj != NULL) {
            if (adj->destino == dest) {
                caminhoValido = true;
                break;
            }
            adj = adj->prox;
        }

        if (!caminhoValido) {
            printf("Caminho inválido! Não há conexão com essa sala.\n");
            continue;
        }

        // 6. Verificar porta trancada
        Nodo *salaDestino = buscarNodo(g, dest);
        if (salaDestino == NULL) {
            printf("Erro: Sala de destino não encontrada!\n");
            continue;
        }

        if (salaDestino->precisaItem && !temChave) {
            printf(">>> PORTA TRANCADA! Você precisa da chave para entrar.\n");
        } else {
            if (salaDestino->precisaItem) {
                printf(">>> Você usou a chave para destrancar a porta!\n");
            }
            posAtual = dest;
        }
    }
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
                limparBuffer();
                printf("1. Fuga da Prisão\n");
                printf("2. Carregar mapa externo\n");
                printf("0. Voltar\n");
                scanf("%d", &op2);
                switch (op2) {
                    case 1:
                        carregarMapaPadrao(&mapa);
                        jogarSurvival(&mapa);
                        break;
                    case 2:
                        carregarGrafoArquivo(&mapa);
                        jogarSurvival(&mapa);
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
                limparBuffer();
                printf("1. Adicionar Sala (Cria Nodo)\n");
                printf("2. Criar Passagem (Cria Aresta)\n");
                printf("3. Remover Sala (Remove Nodo)\n");
                printf("4. Remover Passagem(Remover Aresta)\n");
                printf("5. Visualizar Mapa (DFS)\n");

                printf("6. Explorar (DFS)\n");
                printf("7. Testar Mapa Padrão (Inserção de Nodos e Arestas)\n");
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
                        limparBuffer();
                        adicionarNodo(&mapa, ID); break;
                    case 2:
                        printf("ID de origem: ");scanf("%d", &u);
                        limparBuffer();
                        printf("ID de destino: ");scanf("%d", &v);
                        limparBuffer();
                        adicionarAresta(&mapa, u, v);break;
                    case 3:
                        printf("ID da Sala: ");scanf("%d", &u);
                        limparBuffer();
                        removerNodo(&mapa, u);break;
                    case 4:
                        printf("ID de origem: ");scanf("%d", &u);
                        limparBuffer();
                        printf("ID de destino: ");scanf("%d", &v);
                        limparBuffer();
                        removerAresta(&mapa, u, v);break;
                    case 5:
                        imprimirGrafo(&mapa);break;
                    case 6:
                        printf("Começar de ID: ");scanf("%d", &u);
                        limparBuffer();
                        percorrerGrafo(&mapa, u);break;
                    case 7:
                        carregarMapaPadrao(&mapa);break;
                    case 8:
                        printf("Verificar grau de nodo ID: ");scanf("%d", &u);
                        limparBuffer();
                        verificarGrau(&mapa, u);break;
                    case 9:
                        percorrerGrafo(&mapa, 0); break;
                    case 10:
                        printf("Início: "); scanf("%d", &u);
                        limparBuffer();
                        printf("Fim: "); scanf("%d", &v);
                        limparBuffer();
                        buscarMenorCaminho(&mapa, u, v); break;
                    case 11:
                        salvarGrafoArquivo(&mapa); break;
                    case 12:
                        carregarGrafoArquivo(&mapa); break;
                    case 13:
                        testesAutomatizados(); break;
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
