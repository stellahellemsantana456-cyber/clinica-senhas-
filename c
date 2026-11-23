#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PACIENTES 500
#define TAM_SENHA 8

typedef enum {
    CAT_URGENTE = 0,
    CAT_PREFERENCIAL = 1,
    CAT_NORMAL = 2
} Categoria;

typedef struct {
    int id;
    char genero; // 'F' ou 'M'
    int idade;
    bool gestante;
    bool lactante;
    bool necessidade_especial;
    bool obesidade;
    Categoria categoria;
    char senha[TAM_SENHA];
} Paciente;

typedef struct {
    Paciente itens[MAX_PACIENTES];
    int inicio;
    int fim;
    int tamanho;
} FilaCategoria;

typedef struct {
    FilaCategoria filas[3]; // 0: Urgente, 1: Preferencial, 2: Normal
    int cont_sequencial[3]; // Contador independente por categoria
    int proximo_id;
    bool em_atendimento;
    Paciente paciente_atual;
} SistemaTriagem;

/* Prototipos das funcoes modularizadas */
void limpar_buffer(void);
int ler_inteiro_intervalo(const char *prompt, int min, int max);
char ler_genero(const char *prompt);
bool ler_sim_nao(const char *prompt);

void inicializar_sistema(SistemaTriagem *sys);
void inicializar_fila(FilaCategoria *fila);
bool fila_vazia(const FilaCategoria *fila);
bool fila_cheia(const FilaCategoria *fila);
bool enfileirar(FilaCategoria *fila, Paciente p);
bool desenfileirar(FilaCategoria *fila, Paciente *removido);
bool espiar_fila(const FilaCategoria *fila, Paciente *primeiro);

Categoria classificar_paciente(const Paciente *p);
void gerar_senha(SistemaTriagem *sys, Categoria cat, char destino[TAM_SENHA]);
void cadastrar_paciente(SistemaTriagem *sys);

bool consultar_proxima_senha(const SistemaTriagem *sys, Paciente *proxima);
void registrar_chamada(SistemaTriagem *sys);
void identificar_em_atendimento(const SistemaTriagem *sys);
void finalizar_atendimento(SistemaTriagem *sys);

const char *nome_categoria(Categoria cat);
void exibir_painel_chamada(const SistemaTriagem *sys);
void exibir_menu(void);

int main(void) {
    SistemaTriagem sistema;
    inicializar_sistema(&sistema);

    int opcao;
    do {
        exibir_menu();
        opcao = ler_inteiro_intervalo("Escolha uma opcao: ", 0, 7);

        switch (opcao) {
            case 1:
                cadastrar_paciente(&sistema);
                break;
            case 2: {
                Paciente prox;
                if (consultar_proxima_senha(&sistema, &prox)) {
                    printf("\n[PROXIMA SENHA] %s | Categoria: %s | ID: %d | Idade: %d\n",
                           prox.senha, nome_categoria(prox.categoria), prox.id, prox.idade);
                } else {
                    printf("\nNao ha pacientes aguardando na fila.\n");
                }
                break;
            }
            case 3:
                registrar_chamada(&sistema);
                break;
            case 4:
                identificar_em_atendimento(&sistema);
                break;
            case 5:
                finalizar_atendimento(&sistema);
                break;
            case 6:
                exibir_painel_chamada(&sistema);
                break;
            case 7:
                printf("\nContadores emitidos -> Urgente: %d | Preferencial: %d | Normal: %d\n",
                       sistema.cont_sequencial[CAT_URGENTE],
                       sistema.cont_sequencial[CAT_PREFERENCIAL],
                       sistema.cont_sequencial[CAT_NORMAL]);
                break;
            case 0:
                printf("\nEncerrando o sistema de triagem...\n");
                break;
            default:
                printf("\nOpcao invalida.\n");
                break;
        }
    } while (opcao != 0);

    return 0;
}

/* --- Funcoes auxiliares de entrada e validacao --- */

void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descarta caracteres residuais */
    }
}

int ler_inteiro_intervalo(const char *prompt, int min, int max) {
    int valor;
    int lidos;
    while (true) {
        printf("%s", prompt);
        lidos = scanf("%d", &valor);
        if (lidos == 1 && valor >= min && valor <= max) {
            limpar_buffer();
            return valor;
        }
        limpar_buffer();
        printf("Entrada invalida. Informe um numero inteiro entre %d e %d.\n", min, max);
    }
}

char ler_genero(const char *prompt) {
    char buffer[16];
    while (true) {
        printf("%s", prompt);
        if (scanf("%15s", buffer) == 1) {
            limpar_buffer();
            char g = (char)toupper((unsigned char)buffer[0]);
            if (strlen(buffer) == 1 && (g == 'F' || g == 'M')) {
                return g;
            }
        } else {
            limpar_buffer();
        }
        printf("Genero invalido. Digite 'F' para feminino ou 'M' para masculino.\n");
    }
}

bool ler_sim_nao(const char *prompt) {
    char buffer[16];
    while (true) {
        printf("%s", prompt);
        if (scanf("%15s", buffer) == 1) {
            limpar_buffer();
            char resp = (char)toupper((unsigned char)buffer[0]);
            if (strlen(buffer) == 1 && (resp == 'S' || resp == 'N')) {
                return (resp == 'S');
            }
        } else {
            limpar_buffer();
        }
        printf("Resposta invalida. Digite 'S' para sim ou 'N' para nao.\n");
    }
}

/* --- Gerenciamento de filas --- */

void inicializar_fila(FilaCategoria *fila) {
    fila->inicio = 0;
    fila->fim = 0;
    fila->tamanho = 0;
}

void inicializar_sistema(SistemaTriagem *sys) {
    for (int i = 0; i < 3; i++) {
        inicializar_fila(&sys->filas[i]);
        sys->cont_sequencial[i] = 0;
    }
    sys->proximo_id = 1;
    sys->em_atendimento = false;
    memset(&sys->paciente_atual, 0, sizeof(Paciente));
}

bool fila_vazia(const FilaCategoria *fila) {
    return fila->tamanho == 0;
}

bool fila_cheia(const FilaCategoria *fila) {
    return fila->tamanho >= MAX_PACIENTES;
}

bool enfileirar(FilaCategoria *fila, Paciente p) {
    if (fila_cheia(fila)) {
        return false;
    }
    fila->itens[fila->fim] = p;
    fila->fim = (fila->fim + 1) % MAX_PACIENTES;
    fila->tamanho++;
    return true;
}

bool desenfileirar(FilaCategoria *fila, Paciente *removido) {
    if (fila_vazia(fila)) {
        return false;
    }
    if (removido != NULL) {
        *removido = fila->itens[fila->inicio];
    }
    fila->inicio = (fila->inicio + 1) % MAX_PACIENTES;
    fila->tamanho--;
    return true;
}

bool espiar_fila(const FilaCategoria *fila, Paciente *primeiro) {
    if (fila_vazia(fila)) {
        return false;
    }
    if (primeiro != NULL) {
        *primeiro = fila->itens[fila->inicio];
    }
    return true;
}

/* --- Classificacao, geracao de senha e cadastro --- */

Categoria classificar_paciente(const Paciente *p) {
    if (p->idade > 80) {
        return CAT_URGENTE;
    }

    bool prioridade_idade = (p->genero == 'F' && p->idade >= 60) ||
                              (p->genero == 'M' && p->idade >= 65);

    if (p->gestante || p->lactante || p->necessidade_especial || p->obesidade || prioridade_idade) {
        return CAT_PREFERENCIAL;
    }

    return CAT_NORMAL;
}

void gerar_senha(SistemaTriagem *sys, Categoria cat, char destino[TAM_SENHA]) {
    char prefixo = 'N';
    if (cat == CAT_URGENTE) {
        prefixo = 'U';
    } else if (cat == CAT_PREFERENCIAL) {
        prefixo = 'P';
    }

    sys->cont_sequencial[cat]++;
    snprintf(destino, TAM_SENHA, "%c%03d", prefixo, sys->cont_sequencial[cat]);
}

void cadastrar_paciente(SistemaTriagem *sys) {
    Paciente p;
    memset(&p, 0, sizeof(Paciente));

    printf("\n=== CADASTRO DE PACIENTE ===\n");
    p.genero = ler_genero("Genero (F - Feminino / M - Masculino): ");
    p.idade = ler_inteiro_intervalo("Idade (0 a 130): ", 0, 130);

    if (p.genero == 'F') {
        p.gestante = ler_sim_nao("Gestante? (S/N): ");
    } else {
        p.gestante = false;
        printf("Gestante: Nao aplicavel para genero masculino.\n");
    }

    p.lactante = ler_sim_nao("Lactante? (S/N): ");
    p.necessidade_especial = ler_sim_nao("Possui necessidade especial? (S/N): ");
    p.obesidade = ler_sim_nao("Possui condicao de obesidade? (S/N): ");

    p.categoria = classificar_paciente(&p);

    if (fila_cheia(&sys->filas[p.categoria])) {
        printf("\nErro: fila da categoria %s atingiu a capacidade maxima (%d).\n",
               nome_categoria(p.categoria), MAX_PACIENTES);
        return;
    }

    p.id = sys->proximo_id++;
    gerar_senha(sys, p.categoria, p.senha);

    if (enfileirar(&sys->filas[p.categoria], p)) {
        printf("\nPaciente cadastrado com sucesso!\n");
        printf("ID: %d | Senha emitida: %s | Categoria: %s\n",
               p.id, p.senha, nome_categoria(p.categoria));
    }
}

/* --- Registro de atendimento --- */

bool consultar_proxima_senha(const SistemaTriagem *sys, Paciente *proxima) {
    /* Ordem estrita de precedencia: Urgente -> Preferencial -> Normal */
    for (int cat = CAT_URGENTE; cat <= CAT_NORMAL; cat++) {
        if (espiar_fila(&sys->filas[cat], proxima)) {
            return true;
        }
    }
    return false;
}

void registrar_chamada(SistemaTriagem *sys) {
    if (sys->em_atendimento) {
        printf("\nAviso: ja existe um paciente em atendimento (Senha %s - %s).\n",
               sys->paciente_atual.senha, nome_categoria(sys->paciente_atual.categoria));
        printf("Finalize o atendimento atual antes de chamar a proxima senha.\n");
        return;
    }

    Paciente chamado;
    bool encontrou = false;

    for (int cat = CAT_URGENTE; cat <= CAT_NORMAL; cat++) {
        if (desenfileirar(&sys->filas[cat], &chamado)) {
            encontrou = true;
            break;
        }
    }

    if (!encontrou) {
        printf("\nNao ha senhas aguardando chamada no momento.\n");
        return;
    }

    sys->em_atendimento = true;
    sys->paciente_atual = chamado;

    printf("\n>>> CHAMADA REALIZADA <<<\n");
    printf("Senha: %s | Categoria: %s | ID: %d | Idade: %d\n",
           chamado.senha, nome_categoria(chamado.categoria), chamado.id, chamado.idade);
}

void identificar_em_atendimento(const SistemaTriagem *sys) {
    if (!sys->em_atendimento) {
        printf("\nNenhum paciente esta em atendimento no momento.\n");
        return;
    }

    printf("\n=== PACIENTE EM ATENDIMENTO ===\n");
    printf("Senha: %s\n", sys->paciente_atual.senha);
    printf("Categoria: %s\n", nome_categoria(sys->paciente_atual.categoria));
    printf("ID: %d | Genero: %c | Idade: %d\n",
           sys->paciente_atual.id, sys->paciente_atual.genero, sys->paciente_atual.idade);
}

void finalizar_atendimento(SistemaTriagem *sys) {
    if (!sys->em_atendimento) {
        printf("\nOperacao invalida: nao ha atendimento ativo para finalizar.\n");
        return;
    }

    printf("\nAtendimento finalizado para a senha %s (%s).\n",
           sys->paciente_atual.senha, nome_categoria(sys->paciente_atual.categoria));
    printf("A senha foi encerrada e nao retornara a fila.\n");

    sys->em_atendimento = false;
    memset(&sys->paciente_atual, 0, sizeof(Paciente));
}

/* --- Painel de chamada e interface --- */

const char *nome_categoria(Categoria cat) {
    switch (cat) {
        case CAT_URGENTE:
            return "Urgente";
        case CAT_PREFERENCIAL:
            return "Preferencial";
        case CAT_NORMAL:
        default:
            return "Normal";
    }
}

void exibir_painel_chamada(const SistemaTriagem *sys) {
    printf("\n============================================================\n");
    printf("                  PAINEL DE CHAMADA - TRIAGEM               \n");
    printf("============================================================\n");

    if (sys->em_atendimento) {
        printf("Senha atualmente chamada : %s\n", sys->paciente_atual.senha);
        printf("Categoria do atendimento : %s\n", nome_categoria(sys->paciente_atual.categoria));
    } else {
        printf("Senha atualmente chamada : Nenhuma\n");
        printf("Categoria do atendimento : -\n");
    }

    printf("------------------------------------------------------------\n");
    printf("Quantidade em espera     : Urgente: %d | Preferencial: %d | Normal: %d\n",
           sys->filas[CAT_URGENTE].tamanho,
           sys->filas[CAT_PREFERENCIAL].tamanho,
           sys->filas[CAT_NORMAL].tamanho);
    printf("------------------------------------------------------------\n");
    printf("Proximas senhas aguardando (ordem de chamada):\n");

    int exibidas = 0;
    for (int cat = CAT_URGENTE; cat <= CAT_NORMAL; cat++) {
        const FilaCategoria *f = &sys->filas[cat];
        for (int i = 0; i < f->tamanho; i++) {
            int idx = (f->inicio + i) % MAX_PACIENTES;
            const Paciente *p = &f->itens[idx];
            printf("  %d. %s (%s) - Idade: %d\n",
                   ++exibidas, p->senha, nome_categoria(p->categoria), p->idade);
        }
    }

    if (exibidas == 0) {
        printf("  Nenhuma senha na fila.\n");
    }

    printf("============================================================\n");
}

void exibir_menu(void) {
    printf("\n--- SISTEMA DE TRIAGEM E FILAS ---\n");
    printf("1. Cadastrar paciente e emitir senha\n");
    printf("2. Consultar proxima senha a ser chamada\n");
    printf("3. Registrar chamada do paciente\n");
    printf("4. Identificar senha em atendimento\n");
    printf("5. Finalizar atendimento\n");
    printf("6. Exibir painel de chamada\n");
    printf("7. Consultar total de senhas emitidas por categoria\n");
    printf("0. Sair\n");
}
