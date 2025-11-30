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
typedef struct Sala {
    int id;
    Adjacente *adjacentes;
    struct Nodo *prox;

    // Mecânicas de Jogo
    char nome[MAX_BUFFER];
    char descricao[MAX_BUFFER];
    int temItem;          // 0 = Não, 1 = Sim (ex: Chave)
    int precisaItem;      // 0 = Não, 1 = Sim (Porta Trancada)
    int ativo;            // Controle lógico (1 = Existe, 0 = Removido)
} Sala;

// Estrutura do grafo
typedef struct {
    Sala *inicio;
    int numNodos;
} Grafo;

// --- IMPLEMENTAÇÃO ---


// --- MENU PRINCIPAL ---
int main() {
    setlocale(LC_ALL, "Portuguese"); // Ativa acentos no console

    Grafo *meuGrafo
    int op2, op1, u, v, op0;

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
                printf("1. Adicionar Sala (Vértice)\n");
                printf("2. Criar Passagem (Aresta)\n");
                printf("3. Remover Sala (Reorganiza IDs)\n");
                printf("4. Bloquear Passagem(Remover Aresta)\n");
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
                        inserirVertice(meuGrafo); break;// cria grafo com estrutura fixa de 20 nodos
                    case 2:
                        printf("De ID: ");scanf("%d", &u);
                        printf("Para ID: ");scanf("%d", &v);
                        inserirAresta(meuGrafo, u, v);break;
                    case 3:
                        printf("ID da Sala: ");
                        scanf("%d", &u);
                        removerVertice(meuGrafo, u);break;
                    case 4:
                        printf("De ID: ");scanf("%d", &u);
                        printf("Para ID: ");scanf("%d", &v);
                        removerAresta(meuGrafo, u, v);break;
                    case 5:
                        exibirMatriz(meuGrafo);break;
                    case 6:
                        printf("Início ID: ");scanf("%d", &u);
                        percursoBFS(meuGrafo, u);break;
                    case 7:
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
                        testesAutomatizados(); break;
                    case 0:
                        liberarGrafo(meuGrafo); break;
                    default:
                        printf("Opção inválida.\n");
                }

            } while (op0 == 2);

        }
        }
        do {
            printf("1. Adicionar Sala (Vértice)\n");
            printf("2. Criar Passagem (Aresta)\n");
            printf("3. Remover Sala (Reorganiza IDs)\n");
            printf("4. Bloquear Passagem(Remover Aresta)\n");
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
                    inserirVertice(meuGrafo); break;// cria grafo com estrutura fixa de 20 nodos
                case 2:
                    printf("De ID: ");scanf("%d", &u);
                    printf("Para ID: ");scanf("%d", &v);
                    inserirAresta(meuGrafo, u, v);break;
                case 3:
                    printf("ID da Sala: ");
                    scanf("%d", &u);
                    removerVertice(meuGrafo, u);break;
                case 4:
                    printf("De ID: ");scanf("%d", &u);
                    printf("Para ID: ");scanf("%d", &v);
                    removerAresta(meuGrafo, u, v);break;
                case 5:
                    exibirMatriz(meuGrafo);break;
                case 6:
                    printf("Início ID: ");scanf("%d", &u);
                    percursoBFS(meuGrafo, u);break;
                case 7:
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
                    testesAutomatizados(); break;
                case 0:
                    liberarGrafo(meuGrafo); break;
                default:
                    printf("Opção inválida.\n");
            }

        } while (op0 == 2);

    } while (op1 != 0);
    printf("Saindo");
    liberarGrafo(meuGrafo);
    return 0;
}
