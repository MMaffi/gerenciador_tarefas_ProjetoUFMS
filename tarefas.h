#ifndef TAREFAS_H
#define TAREFAS_H

#define MAX_DESCRICAO 100
#define MAX_CATEGORIA 50
#define MAX_TAGS 5
#define MAX_TAG_LENGTH 20
#define ARQUIVO_TAREFAS "tarefas.dat"
#define ARQUIVO_RELATORIO "relatorio.txt"

typedef enum {
    BAIXA = 1,
    MEDIA = 2,
    ALTA = 3,
    URGENTE = 4
} Prioridade;

typedef enum {
    PENDENTE = 0,
    EM_ANDAMENTO = 1,
    CONCLUIDA = 2
} Status;

typedef struct Data {
    int dia;
    int mes;
    int ano;
} Data;

typedef struct Tarefa {
    int id;
    char descricao[MAX_DESCRICAO];
    char categoria[MAX_CATEGORIA];
    char tags[MAX_TAGS][MAX_TAG_LENGTH];
    int num_tags;
    Prioridade prioridade;
    Status status;
    Data data_criacao;
    Data data_vencimento;
    Data data_conclusao;
    int tempo_estimado_minutos; /* Tempo estimado em minutos */
    struct Tarefa *prox;
} Tarefa;

typedef struct {
    Tarefa *inicio;
    Tarefa *fim;
    int contador_id;
    int total_tarefas;
    int tarefas_concluidas;
} GerenciadorTarefas;

/* Funções de inicialização */
GerenciadorTarefas* criar_gerenciador(void);
void liberar_gerenciador(GerenciadorTarefas *gerenciador);

/* Funções de manipulação de tarefas */
void adicionar_tarefa(GerenciadorTarefas *gerenciador);
void concluir_tarefa(GerenciadorTarefas *gerenciador, int id);
void remover_tarefa(GerenciadorTarefas *gerenciador, int id);
void editar_tarefa(GerenciadorTarefas *gerenciador, int id);
void alterar_status_tarefa(GerenciadorTarefas *gerenciador, int id, Status novo_status);
void adicionar_tag_tarefa(GerenciadorTarefas *gerenciador, int id, const char *tag);
void remover_tag_tarefa(GerenciadorTarefas *gerenciador, int id, const char *tag);

/* Funções de busca e filtros */
Tarefa* buscar_tarefa_por_id(GerenciadorTarefas *gerenciador, int id);
void buscar_tarefas_por_descricao(GerenciadorTarefas *gerenciador, const char *termo);
void buscar_tarefas_por_tag(GerenciadorTarefas *gerenciador, const char *tag);
void filtrar_tarefas_por_data_vencimento(GerenciadorTarefas *gerenciador, Data data_inicio, Data data_fim);
void listar_tarefas_vencidas(GerenciadorTarefas *gerenciador);
void listar_tarefas_para_hoje(GerenciadorTarefas *gerenciador);

/* Funções de listagem e relatórios */
void listar_todas_tarefas(GerenciadorTarefas *gerenciador);
void listar_tarefas_por_status(GerenciadorTarefas *gerenciador, Status status);
void listar_tarefas_por_prioridade(GerenciadorTarefas *gerenciador, Prioridade p);
void listar_tarefas_por_categoria(GerenciadorTarefas *gerenciador, const char *categoria);
void listar_tarefas_ordenadas_por_prioridade(GerenciadorTarefas *gerenciador);
void listar_tarefas_ordenadas_por_data(GerenciadorTarefas *gerenciador);

/* Funções de estatísticas */
void mostrar_estatisticas(GerenciadorTarefas *gerenciador);
void calcular_tempo_medio_conclusao(GerenciadorTarefas *gerenciador);
void mostrar_distribuicao_prioridades(GerenciadorTarefas *gerenciador);
void mostrar_tarefas_mais_demoradas(GerenciadorTarefas *gerenciador);

/* Funções de arquivo */
void salvar_tarefas_arquivo(GerenciadorTarefas *gerenciador, const char *filename);
void carregar_tarefas_arquivo(GerenciadorTarefas *gerenciador, const char *filename);
void exportar_tarefas_txt(GerenciadorTarefas *gerenciador, const char *filename);
void importar_tarefas_txt(GerenciadorTarefas *gerenciador, const char *filename);
void gerar_relatorio_completo_arquivo(GerenciadorTarefas *gerenciador, const char *filename);
void gerar_relatorio_estatisticas_arquivo(GerenciadorTarefas *gerenciador, const char *filename);

/* Funções de utilidade */
Data obter_data_atual(void);
int comparar_datas(Data d1, Data d2);
int calcular_dias_entre_datas(Data d1, Data d2);
int eh_data_valida(Data data);
void limpar_buffer(void);
void pausar_tela(void);

/* Funções de ordenação */
void ordenar_tarefas_por_prioridade(GerenciadorTarefas *gerenciador);
void ordenar_tarefas_por_data_vencimento(GerenciadorTarefas *gerenciador);
void ordenar_tarefas_por_data_criacao(GerenciadorTarefas *gerenciador);

#endif