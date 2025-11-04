#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "tarefas.h"

GerenciadorTarefas* criar_gerenciador(void) {
    GerenciadorTarefas *gerenciador = (GerenciadorTarefas*)malloc(sizeof(GerenciadorTarefas));
    if (gerenciador == NULL) {
        printf("Erro ao alocar memória para o gerenciador.\n");
        exit(1);
    }
    gerenciador->inicio = NULL;
    gerenciador->fim = NULL;
    gerenciador->contador_id = 1;
    gerenciador->total_tarefas = 0;
    gerenciador->tarefas_concluidas = 0;
    return gerenciador;
}

void liberar_gerenciador(GerenciadorTarefas *gerenciador) {
    Tarefa *atual = gerenciador->inicio;
    Tarefa *prox;
    
    while (atual != NULL) {
        prox = atual->prox;
        free(atual);
        atual = prox;
    }
    free(gerenciador);
}

Data obter_data_atual(void) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    Data data;
    data.dia = tm_info->tm_mday;
    data.mes = tm_info->tm_mon + 1;
    data.ano = tm_info->tm_year + 1900;
    return data;
}

int comparar_datas(Data d1, Data d2) {
    if (d1.ano != d2.ano) return d1.ano - d2.ano;
    if (d1.mes != d2.mes) return d1.mes - d2.mes;
    return d1.dia - d2.dia;
}

int calcular_dias_entre_datas(Data d1, Data d2) {
    struct tm tm1 = {0}, tm2 = {0};
    time_t time1, time2;
    
    tm1.tm_mday = d1.dia;
    tm1.tm_mon = d1.mes - 1;
    tm1.tm_year = d1.ano - 1900;
    
    tm2.tm_mday = d2.dia;
    tm2.tm_mon = d2.mes - 1;
    tm2.tm_year = d2.ano - 1900;
    
    time1 = mktime(&tm1);
    time2 = mktime(&tm2);
    
    if (time1 == -1 || time2 == -1) return 0;
    
    return (int)difftime(time2, time1) / (60 * 60 * 24);
}

int eh_data_valida(Data data) {
    int dias_no_mes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    if (data.ano < 1900 || data.ano > 2100) return 0;
    if (data.mes < 1 || data.mes > 12) return 0;
    
    /* Verifica ano bissexto */
    if (data.mes == 2) {
        int bissexto = (data.ano % 4 == 0 && data.ano % 100 != 0) || (data.ano % 400 == 0);
        if (data.dia < 1 || data.dia > (28 + bissexto)) return 0;
    } else {
        if (data.dia < 1 || data.dia > dias_no_mes[data.mes - 1]) return 0;
    }
    
    return 1;
}

void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* Limpa o buffer */
    }
}

void pausar_tela(void) {
    printf("\nPressione Enter para continuar...");
    limpar_buffer();
}

void adicionar_tarefa(GerenciadorTarefas *gerenciador) {
    Tarefa *nova_tarefa = (Tarefa*)malloc(sizeof(Tarefa));
    int prioridade, tempo_estimado, i;
    char *nova_linha;
    char resposta;
    
    if (nova_tarefa == NULL) {
        printf("Erro ao alocar memória para nova tarefa.\n");
        return;
    }
    
    nova_tarefa->id = gerenciador->contador_id++;
    nova_tarefa->status = PENDENTE;
    nova_tarefa->data_criacao = obter_data_atual();
    nova_tarefa->num_tags = 0;
    nova_tarefa->prox = NULL;
    
    printf("\n=== ADICIONAR NOVA TAREFA ===\n");
    
    printf("Descrição: ");
    fgets(nova_tarefa->descricao, MAX_DESCRICAO, stdin);
    nova_linha = strchr(nova_tarefa->descricao, '\n');
    if (nova_linha != NULL) *nova_linha = '\0';
    
    printf("Categoria: ");
    fgets(nova_tarefa->categoria, MAX_CATEGORIA, stdin);
    nova_linha = strchr(nova_tarefa->categoria, '\n');
    if (nova_linha != NULL) *nova_linha = '\0';
    
    printf("Prioridade (1-Baixa, 2-Média, 3-Alta, 4-Urgente): ");
    scanf("%d", &prioridade);
    nova_tarefa->prioridade = (Prioridade)prioridade;
    
    printf("Tempo estimado (minutos): ");
    scanf("%d", &tempo_estimado);
    nova_tarefa->tempo_estimado_minutos = tempo_estimado;
    
    do {
        printf("Data de vencimento (dia mes ano): ");
        scanf("%d %d %d", &nova_tarefa->data_vencimento.dia, 
                           &nova_tarefa->data_vencimento.mes, 
                           &nova_tarefa->data_vencimento.ano);
        if (!eh_data_valida(nova_tarefa->data_vencimento)) {
            printf("Data inválida! Tente novamente.\n");
        }
    } while (!eh_data_valida(nova_tarefa->data_vencimento));
    
    limpar_buffer();
    
    /* Adicionar tags */
    printf("Deseja adicionar tags? (s/n): ");
    scanf("%c", &resposta);
    limpar_buffer();
    
    if (resposta == 's' || resposta == 'S') {
        for (i = 0; i < MAX_TAGS; i++) {
            char tag[MAX_TAG_LENGTH];
            printf("Tag %d (ou Enter para parar): ", i + 1);
            fgets(tag, MAX_TAG_LENGTH, stdin);
            nova_linha = strchr(tag, '\n');
            if (nova_linha != NULL) *nova_linha = '\0';
            
            if (strlen(tag) == 0) break;
            
            strcpy(nova_tarefa->tags[nova_tarefa->num_tags], tag);
            nova_tarefa->num_tags++;
            
            if (nova_tarefa->num_tags >= MAX_TAGS) break;
        }
    }
    
    /* Inserir na lista */
    if (gerenciador->inicio == NULL) {
        gerenciador->inicio = nova_tarefa;
        gerenciador->fim = nova_tarefa;
    } else {
        gerenciador->fim->prox = nova_tarefa;
        gerenciador->fim = nova_tarefa;
    }
    
    gerenciador->total_tarefas++;
    if (nova_tarefa->status == CONCLUIDA) {
        gerenciador->tarefas_concluidas++;
    }
    
    printf("Tarefa adicionada com sucesso! ID: %d\n", nova_tarefa->id);
}

Tarefa* buscar_tarefa_por_id(GerenciadorTarefas *gerenciador, int id) {
    Tarefa *atual = gerenciador->inicio;
    while (atual != NULL) {
        if (atual->id == id) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

void editar_tarefa(GerenciadorTarefas *gerenciador, int id) {
    Tarefa *tarefa = buscar_tarefa_por_id(gerenciador, id);
    char nova_descricao[MAX_DESCRICAO];
    char nova_categoria[MAX_CATEGORIA];
    char input[10];
    int nova_prioridade;
    char *nova_linha;
    
    if (tarefa == NULL) {
        printf("Tarefa com ID %d não encontrada.\n", id);
        return;
    }
    
    printf("\n=== EDITAR TAREFA ID: %d ===\n", id);
    printf("Nova descrição (atual: %s): ", tarefa->descricao);
    fgets(nova_descricao, MAX_DESCRICAO, stdin);
    nova_linha = strchr(nova_descricao, '\n');
    if (nova_linha != NULL) *nova_linha = '\0';
    if (strlen(nova_descricao) > 0) {
        strcpy(tarefa->descricao, nova_descricao);
    }
    
    printf("Nova categoria (atual: %s): ", tarefa->categoria);
    fgets(nova_categoria, MAX_CATEGORIA, stdin);
    nova_linha = strchr(nova_categoria, '\n');
    if (nova_linha != NULL) *nova_linha = '\0';
    if (strlen(nova_categoria) > 0) {
        strcpy(tarefa->categoria, nova_categoria);
    }
    
    printf("Nova prioridade (atual: %d): ", tarefa->prioridade);
    fgets(input, 10, stdin);
    if (strlen(input) > 1) {
        nova_prioridade = atoi(input);
        tarefa->prioridade = (Prioridade)nova_prioridade;
    }
    
    printf("Tarefa editada com sucesso!\n");
}

void remover_tarefa(GerenciadorTarefas *gerenciador, int id) {
    Tarefa *atual = gerenciador->inicio;
    Tarefa *anterior = NULL;
    
    while (atual != NULL && atual->id != id) {
        anterior = atual;
        atual = atual->prox;
    }
    
    if (atual == NULL) {
        printf("Tarefa com ID %d não encontrada.\n", id);
        return;
    }
    
    if (anterior == NULL) {
        gerenciador->inicio = atual->prox;
    } else {
        anterior->prox = atual->prox;
    }
    
    if (atual == gerenciador->fim) {
        gerenciador->fim = anterior;
    }
    
    printf("Tarefa '%s' removida com sucesso!\n", atual->descricao);
    free(atual);
    gerenciador->total_tarefas--;
}

void buscar_tarefas_por_descricao(GerenciadorTarefas *gerenciador, const char *termo) {
    Tarefa *atual = gerenciador->inicio;
    int encontrou = 0;
    char descricao_lower[MAX_DESCRICAO];
    char termo_lower[100];
    int i;
    
    /* Converter termo para minúsculo */
    strcpy(termo_lower, termo);
    for (i = 0; termo_lower[i]; i++) {
        termo_lower[i] = tolower(termo_lower[i]);
    }
    
    printf("\n=== RESULTADOS DA BUSCA: '%s' ===\n", termo);
    
    while (atual != NULL) {
        /* Converter descrição para minúsculo */
        strcpy(descricao_lower, atual->descricao);
        for (i = 0; descricao_lower[i]; i++) {
            descricao_lower[i] = tolower(descricao_lower[i]);
        }
        
        if (strstr(descricao_lower, termo_lower) != NULL) {
            const char *status_str[] = {"Pendente", "Em Andamento", "Concluída"};
            printf("ID: %d | %s | Status: %s | Prioridade: %d | Categoria: %s\n",
                   atual->id, atual->descricao, status_str[atual->status], 
                   atual->prioridade, atual->categoria);
            encontrou = 1;
        }
        atual = atual->prox;
    }
    
    if (!encontrou) {
        printf("Nenhuma tarefa encontrada com o termo '%s'.\n", termo);
    }
}

void buscar_tarefas_por_tag(GerenciadorTarefas *gerenciador, const char *tag) {
    Tarefa *atual = gerenciador->inicio;
    int encontrou = 0;
    int i;
    
    printf("\n=== TAREFAS COM TAG: '%s' ===\n", tag);
    
    while (atual != NULL) {
        for (i = 0; i < atual->num_tags; i++) {
            if (strcmp(atual->tags[i], tag) == 0) {
                const char *status_str[] = {"Pendente", "Em Andamento", "Concluída"};
                printf("ID: %d | %s | Status: %s | Prioridade: %d | Categoria: %s\n",
                       atual->id, atual->descricao, status_str[atual->status], 
                       atual->prioridade, atual->categoria);
                encontrou = 1;
                break;
            }
        }
        atual = atual->prox;
    }
    
    if (!encontrou) {
        printf("Nenhuma tarefa encontrada com a tag '%s'.\n", tag);
    }
}

void alterar_status_tarefa(GerenciadorTarefas *gerenciador, int id, Status novo_status) {
    Tarefa *tarefa = buscar_tarefa_por_id(gerenciador, id);
    const char *status_str[] = {"Pendente", "Em Andamento", "Concluída"};
    
    if (tarefa == NULL) {
        printf("Tarefa com ID %d não encontrada.\n", id);
        return;
    }
    
    if (tarefa->status == novo_status) {
        printf("Tarefa já está com status '%s'.\n", status_str[novo_status]);
        return;
    }
    
    tarefa->status = novo_status;
    
    if (novo_status == CONCLUIDA) {
        tarefa->data_conclusao = obter_data_atual();
        gerenciador->tarefas_concluidas++;
    } else if (tarefa->status == CONCLUIDA && novo_status != CONCLUIDA) {
        gerenciador->tarefas_concluidas--;
    }
    
    printf("Status da tarefa '%s' alterado para '%s'!\n", 
           tarefa->descricao, status_str[novo_status]);
}

void adicionar_tag_tarefa(GerenciadorTarefas *gerenciador, int id, const char *tag) {
    Tarefa *tarefa = buscar_tarefa_por_id(gerenciador, id);
    int i;
    
    if (tarefa == NULL) {
        printf("Tarefa com ID %d não encontrada.\n", id);
        return;
    }
    
    if (tarefa->num_tags >= MAX_TAGS) {
        printf("Número máximo de tags atingido para esta tarefa.\n");
        return;
    }
    
    /* Verificar se tag já existe */
    for (i = 0; i < tarefa->num_tags; i++) {
        if (strcmp(tarefa->tags[i], tag) == 0) {
            printf("Tag '%s' já existe nesta tarefa.\n", tag);
            return;
        }
    }
    
    strcpy(tarefa->tags[tarefa->num_tags], tag);
    tarefa->num_tags++;
    printf("Tag '%s' adicionada à tarefa '%s'!\n", tag, tarefa->descricao);
}

void remover_tag_tarefa(GerenciadorTarefas *gerenciador, int id, const char *tag) {
    Tarefa *tarefa = buscar_tarefa_por_id(gerenciador, id);
    int i, j;
    
    if (tarefa == NULL) {
        printf("Tarefa com ID %d não encontrada.\n", id);
        return;
    }
    
    for (i = 0; i < tarefa->num_tags; i++) {
        if (strcmp(tarefa->tags[i], tag) == 0) {
            /* Remover tag deslocando as outras */
            for (j = i; j < tarefa->num_tags - 1; j++) {
                strcpy(tarefa->tags[j], tarefa->tags[j + 1]);
            }
            tarefa->num_tags--;
            printf("Tag '%s' removida da tarefa '%s'!\n", tag, tarefa->descricao);
            return;
        }
    }
    
    printf("Tag '%s' não encontrada na tarefa.\n", tag);
}

void listar_todas_tarefas(GerenciadorTarefas *gerenciador) {
    Tarefa *atual = gerenciador->inicio;
    const char *status_str[] = {"Pendente", "Em Andamento", "Concluída"};
    const char *prioridade_str[] = {"", "Baixa", "Média", "Alta", "Urgente"};
    int i;
    
    printf("\n=== TODAS AS TAREFAS ===\n");
    
    if (atual == NULL) {
        printf("Nenhuma tarefa cadastrada.\n");
        return;
    }
    
    while (atual != NULL) {
        printf("ID: %d\n", atual->id);
        printf("  Descrição: %s\n", atual->descricao);
        printf("  Categoria: %s\n", atual->categoria);
        printf("  Status: %s\n", status_str[atual->status]);
        printf("  Prioridade: %s\n", prioridade_str[atual->prioridade]);
        printf("  Tempo estimado: %d minutos\n", atual->tempo_estimado_minutos);
        printf("  Data criação: %02d/%02d/%04d\n", 
               atual->data_criacao.dia, atual->data_criacao.mes, atual->data_criacao.ano);
        printf("  Data vencimento: %02d/%02d/%04d\n", 
               atual->data_vencimento.dia, atual->data_vencimento.mes, atual->data_vencimento.ano);
        
        if (atual->num_tags > 0) {
            printf("  Tags: ");
            for (i = 0; i < atual->num_tags; i++) {
                printf("#%s ", atual->tags[i]);
            }
            printf("\n");
        }
        
        if (atual->status == CONCLUIDA) {
            printf("  Data conclusão: %02d/%02d/%04d\n", 
                   atual->data_conclusao.dia, atual->data_conclusao.mes, atual->data_conclusao.ano);
        }
        
        printf("  ---\n");
        atual = atual->prox;
    }
}

void listar_tarefas_por_status(GerenciadorTarefas *gerenciador, Status status) {
    Tarefa *atual = gerenciador->inicio;
    int encontrou = 0;
    const char *status_str[] = {"Pendente", "Em Andamento", "Concluída"};
    
    printf("\n=== TAREFAS COM STATUS: %s ===\n", status_str[status]);
    
    while (atual != NULL) {
        if (atual->status == status) {
            printf("ID: %d | %s | Prioridade: %d | Categoria: %s | Vencimento: %02d/%02d/%04d\n",
                   atual->id, atual->descricao, atual->prioridade, atual->categoria,
                   atual->data_vencimento.dia, atual->data_vencimento.mes, atual->data_vencimento.ano);
            encontrou = 1;
        }
        atual = atual->prox;
    }
    
    if (!encontrou) {
        printf("Nenhuma tarefa com status '%s'.\n", status_str[status]);
    }
}

void listar_tarefas_por_prioridade(GerenciadorTarefas *gerenciador, Prioridade p) {
    Tarefa *atual = gerenciador->inicio;
    int encontrou = 0;
    const char *nomes_prioridade[] = {"", "Baixa", "Média", "Alta", "Urgente"};
    
    printf("\n=== TAREFAS COM PRIORIDADE: %s ===\n", nomes_prioridade[p]);
    while (atual != NULL) {
        if (atual->prioridade == p && atual->status != CONCLUIDA) {
            printf("ID: %d | %s | Categoria: %s | Vencimento: %02d/%02d/%04d\n",
                   atual->id, atual->descricao, atual->categoria,
                   atual->data_vencimento.dia, atual->data_vencimento.mes, atual->data_vencimento.ano);
            encontrou = 1;
        }
        atual = atual->prox;
    }
    
    if (!encontrou) {
        printf("Nenhuma tarefa com esta prioridade.\n");
    }
}

void listar_tarefas_por_categoria(GerenciadorTarefas *gerenciador, const char *categoria) {
    Tarefa *atual = gerenciador->inicio;
    int encontrou = 0;
    
    printf("\n=== TAREFAS DA CATEGORIA: %s ===\n", categoria);
    while (atual != NULL) {
        if (strcmp(atual->categoria, categoria) == 0 && atual->status != CONCLUIDA) {
            printf("ID: %d | %s | Prioridade: %d | Vencimento: %02d/%02d/%04d\n",
                   atual->id, atual->descricao, atual->prioridade,
                   atual->data_vencimento.dia, atual->data_vencimento.mes, atual->data_vencimento.ano);
            encontrou = 1;
        }
        atual = atual->prox;
    }
    
    if (!encontrou) {
        printf("Nenhuma tarefa nesta categoria.\n");
    }
}

void listar_tarefas_vencidas(GerenciadorTarefas *gerenciador) {
    Tarefa *atual = gerenciador->inicio;
    Data hoje = obter_data_atual();
    int encontrou = 0;
    
    printf("\n=== TAREFAS VENCIDAS ===\n");
    
    while (atual != NULL) {
        if (atual->status != CONCLUIDA && comparar_datas(atual->data_vencimento, hoje) < 0) {
            int dias_atraso = calcular_dias_entre_datas(atual->data_vencimento, hoje);
            printf("ID: %d | %s | Vencimento: %02d/%02d/%04d | Atraso: %d dias\n",
                   atual->id, atual->descricao,
                   atual->data_vencimento.dia, atual->data_vencimento.mes, atual->data_vencimento.ano,
                   dias_atraso);
            encontrou = 1;
        }
        atual = atual->prox;
    }
    
    if (!encontrou) {
        printf("Nenhuma tarefa vencida.\n");
    }
}

void listar_tarefas_para_hoje(GerenciadorTarefas *gerenciador) {
    Tarefa *atual = gerenciador->inicio;
    Data hoje = obter_data_atual();
    int encontrou = 0;
    
    printf("\n=== TAREFAS PARA HOJE ===\n");
    
    while (atual != NULL) {
        if (atual->status != CONCLUIDA && comparar_datas(atual->data_vencimento, hoje) == 0) {
            printf("ID: %d | %s | Prioridade: %d | Categoria: %s | Tempo estimado: %d min\n",
                   atual->id, atual->descricao, atual->prioridade, atual->categoria,
                   atual->tempo_estimado_minutos);
            encontrou = 1;
        }
        atual = atual->prox;
    }
    
    if (!encontrou) {
        printf("Nenhuma tarefa para hoje.\n");
    }
}

void mostrar_estatisticas(GerenciadorTarefas *gerenciador) {
    Tarefa *atual = gerenciador->inicio;
    int contador_prioridade[5] = {0};
    int contador_status[3] = {0};
    int total_tempo_estimado = 0;
    int tarefas_com_tempo = 0;
    
    printf("\n=== ESTATÍSTICAS DO SISTEMA ===\n");
    printf("Total de tarefas: %d\n", gerenciador->total_tarefas);
    printf("Tarefas concluídas: %d\n", gerenciador->tarefas_concluidas);
    printf("Tarefas pendentes: %d\n", gerenciador->total_tarefas - gerenciador->tarefas_concluidas);
    
    while (atual != NULL) {
        contador_prioridade[atual->prioridade]++;
        contador_status[atual->status]++;
        
        if (atual->tempo_estimado_minutos > 0) {
            total_tempo_estimado += atual->tempo_estimado_minutos;
            tarefas_com_tempo++;
        }
        atual = atual->prox;
    }
    
    printf("\nDistribuição por prioridade:\n");
    printf("  Baixa: %d (%.1f%%)\n", contador_prioridade[BAIXA], 
           (float)contador_prioridade[BAIXA] / gerenciador->total_tarefas * 100);
    printf("  Média: %d (%.1f%%)\n", contador_prioridade[MEDIA], 
           (float)contador_prioridade[MEDIA] / gerenciador->total_tarefas * 100);
    printf("  Alta: %d (%.1f%%)\n", contador_prioridade[ALTA], 
           (float)contador_prioridade[ALTA] / gerenciador->total_tarefas * 100);
    printf("  Urgente: %d (%.1f%%)\n", contador_prioridade[URGENTE], 
           (float)contador_prioridade[URGENTE] / gerenciador->total_tarefas * 100);
    
    printf("\nDistribuição por status:\n");
    printf("  Pendente: %d\n", contador_status[PENDENTE]);
    printf("  Em Andamento: %d\n", contador_status[EM_ANDAMENTO]);
    printf("  Concluída: %d\n", contador_status[CONCLUIDA]);
    
    if (tarefas_com_tempo > 0) {
        printf("\nTempo total estimado: %d minutos (%.1f horas)\n", 
               total_tempo_estimado, total_tempo_estimado / 60.0);
        printf("Tempo médio por tarefa: %.1f minutos\n", 
               (float)total_tempo_estimado / tarefas_com_tempo);
    }
}

void salvar_tarefas_arquivo(GerenciadorTarefas *gerenciador, const char *filename) {
    FILE *arquivo = fopen(filename, "wb");
    Tarefa *atual = gerenciador->inicio;
    
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo %s\n", filename);
        return;
    }
    
    /* Salvar contador_id */
    fwrite(&gerenciador->contador_id, sizeof(int), 1, arquivo);
    
    /* Salvar tarefas */
    while (atual != NULL) {
        fwrite(atual, sizeof(Tarefa), 1, arquivo);
        atual = atual->prox;
    }
    
    fclose(arquivo);
    printf("Tarefas salvas em %s com sucesso!\n", filename);
}

void carregar_tarefas_arquivo(GerenciadorTarefas *gerenciador, const char *filename) {
    FILE *arquivo = fopen(filename, "rb");
    Tarefa *atual;
    Tarefa *prox;
    
    if (arquivo == NULL) {
        printf("Arquivo %s não encontrado. Iniciando com lista vazia.\n", filename);
        return;
    }
    
    /* Limpar lista atual */
    atual = gerenciador->inicio;
    while (atual != NULL) {
        prox = atual->prox;
        free(atual);
        atual = prox;
    }
    gerenciador->inicio = NULL;
    gerenciador->fim = NULL;
    gerenciador->total_tarefas = 0;
    gerenciador->tarefas_concluidas = 0;
    
    /* Carregar contador_id */
    fread(&gerenciador->contador_id, sizeof(int), 1, arquivo);
    
    /* Carregar tarefas */
    while (!feof(arquivo)) {
        Tarefa *nova_tarefa = (Tarefa*)malloc(sizeof(Tarefa));
        if (fread(nova_tarefa, sizeof(Tarefa), 1, arquivo) != 1) {
            free(nova_tarefa);
            break;
        }
        
        nova_tarefa->prox = NULL;
        
        if (gerenciador->inicio == NULL) {
            gerenciador->inicio = nova_tarefa;
            gerenciador->fim = nova_tarefa;
        } else {
            gerenciador->fim->prox = nova_tarefa;
            gerenciador->fim = nova_tarefa;
        }
        
        gerenciador->total_tarefas++;
        if (nova_tarefa->status == CONCLUIDA) {
            gerenciador->tarefas_concluidas++;
        }
    }
    
    fclose(arquivo);
    printf("Tarefas carregadas de %s com sucesso!\n", filename);
}

void exportar_tarefas_txt(GerenciadorTarefas *gerenciador, const char *filename) {
    FILE *arquivo = fopen(filename, "w");
    Tarefa *atual = gerenciador->inicio;
    const char *status_str[] = {"PENDENTE", "EM_ANDAMENTO", "CONCLUIDA"};
    const char *prioridade_str[] = {"", "BAIXA", "MEDIA", "ALTA", "URGENTE"};
    int i;
    
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo %s\n", filename);
        return;
    }
    
    fprintf(arquivo, "=== EXPORTAÇÃO DE TAREFAS ===\n");
    fprintf(arquivo, "Data da exportação: %02d/%02d/%04d\n\n",
            obter_data_atual().dia, obter_data_atual().mes, obter_data_atual().ano);
    
    while (atual != NULL) {
        fprintf(arquivo, "ID: %d\n", atual->id);
        fprintf(arquivo, "Descrição: %s\n", atual->descricao);
        fprintf(arquivo, "Categoria: %s\n", atual->categoria);
        fprintf(arquivo, "Status: %s\n", status_str[atual->status]);
        fprintf(arquivo, "Prioridade: %s\n", prioridade_str[atual->prioridade]);
        fprintf(arquivo, "Tempo estimado: %d minutos\n", atual->tempo_estimado_minutos);
        fprintf(arquivo, "Data criação: %02d/%02d/%04d\n", 
                atual->data_criacao.dia, atual->data_criacao.mes, atual->data_criacao.ano);
        fprintf(arquivo, "Data vencimento: %02d/%02d/%04d\n", 
                atual->data_vencimento.dia, atual->data_vencimento.mes, atual->data_vencimento.ano);
        
        if (atual->num_tags > 0) {
            fprintf(arquivo, "Tags: ");
            for (i = 0; i < atual->num_tags; i++) {
                fprintf(arquivo, "#%s ", atual->tags[i]);
            }
            fprintf(arquivo, "\n");
        }
        
        if (atual->status == CONCLUIDA) {
            fprintf(arquivo, "Data conclusão: %02d/%02d/%04d\n", 
                    atual->data_conclusao.dia, atual->data_conclusao.mes, atual->data_conclusao.ano);
        }
        
        fprintf(arquivo, "---\n");
        atual = atual->prox;
    }
    
    fclose(arquivo);
    printf("Tarefas exportadas para %s com sucesso!\n", filename);
}

void importar_tarefas_txt(GerenciadorTarefas *gerenciador, const char *filename) {
    FILE *arquivo = fopen(filename, "r");
    char linha[200];
    int tarefas_importadas = 0;
    
    if (arquivo == NULL) {
        printf("Arquivo %s não encontrado.\n", filename);
        return;
    }
    
    printf("\n=== IMPORTANDO TAREFAS DE %s ===\n", filename);
    
    /* Pular cabeçalho */
    fgets(linha, sizeof(linha), arquivo);
    fgets(linha, sizeof(linha), arquivo);
    fgets(linha, sizeof(linha), arquivo);
    
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (strstr(linha, "ID:") != NULL) {
            Tarefa *nova_tarefa = (Tarefa*)malloc(sizeof(Tarefa));
            if (nova_tarefa == NULL) {
                printf("Erro de alocação de memória.\n");
                break;
            }
            
            /* Inicializar nova tarefa */
            nova_tarefa->id = gerenciador->contador_id++;
            nova_tarefa->status = PENDENTE;
            nova_tarefa->data_criacao = obter_data_atual();
            nova_tarefa->num_tags = 0;
            nova_tarefa->prox = NULL;
            
            /* Ler ID */
            sscanf(linha, "ID: %d", &nova_tarefa->id);
            nova_tarefa->id = gerenciador->contador_id - 1;
            
            /* Ler descrição */
            fgets(linha, sizeof(linha), arquivo);
            if (strstr(linha, "Descrição:") != NULL) {
                strncpy(nova_tarefa->descricao, linha + 12, MAX_DESCRICAO - 1);
                nova_tarefa->descricao[strcspn(nova_tarefa->descricao, "\n")] = '\0';
            }
            
            /* Ler categoria */
            fgets(linha, sizeof(linha), arquivo);
            if (strstr(linha, "Categoria:") != NULL) {
                strncpy(nova_tarefa->categoria, linha + 11, MAX_CATEGORIA - 1);
                nova_tarefa->categoria[strcspn(nova_tarefa->categoria, "\n")] = '\0';
            }
            
            /* Ler status */
            fgets(linha, sizeof(linha), arquivo);
            if (strstr(linha, "Status:") != NULL) {
                char status_str[20];
                sscanf(linha, "Status: %19s", status_str);
                if (strcmp(status_str, "CONCLUIDA") == 0) {
                    nova_tarefa->status = CONCLUIDA;
                } else if (strcmp(status_str, "EM_ANDAMENTO") == 0) {
                    nova_tarefa->status = EM_ANDAMENTO;
                } else {
                    nova_tarefa->status = PENDENTE;
                }
            }
            
            /* Ler prioridade */
            fgets(linha, sizeof(linha), arquivo);
            if (strstr(linha, "Prioridade:") != NULL) {
                char prioridade_str[20];
                sscanf(linha, "Prioridade: %19s", prioridade_str);
                if (strcmp(prioridade_str, "URGENTE") == 0) {
                    nova_tarefa->prioridade = URGENTE;
                } else if (strcmp(prioridade_str, "ALTA") == 0) {
                    nova_tarefa->prioridade = ALTA;
                } else if (strcmp(prioridade_str, "MEDIA") == 0) {
                    nova_tarefa->prioridade = MEDIA;
                } else {
                    nova_tarefa->prioridade = BAIXA;
                }
            }
            
            /* Ler tempo estimado */
            fgets(linha, sizeof(linha), arquivo);
            if (strstr(linha, "Tempo estimado:") != NULL) {
                sscanf(linha, "Tempo estimado: %d minutos", &nova_tarefa->tempo_estimado_minutos);
            }
            
            /* Ler data vencimento */
            fgets(linha, sizeof(linha), arquivo);
            fgets(linha, sizeof(linha), arquivo);
            if (strstr(linha, "Data vencimento:") != NULL) {
                sscanf(linha, "Data vencimento: %d/%d/%d", 
                       &nova_tarefa->data_vencimento.dia,
                       &nova_tarefa->data_vencimento.mes,
                       &nova_tarefa->data_vencimento.ano);
            }
            
            /* Adicionar à lista */
            if (gerenciador->inicio == NULL) {
                gerenciador->inicio = nova_tarefa;
                gerenciador->fim = nova_tarefa;
            } else {
                gerenciador->fim->prox = nova_tarefa;
                gerenciador->fim = nova_tarefa;
            }
            
            gerenciador->total_tarefas++;
            if (nova_tarefa->status == CONCLUIDA) {
                gerenciador->tarefas_concluidas++;
            }
            
            tarefas_importadas++;
            printf("Tarefa '%s' importada (ID: %d)\n", nova_tarefa->descricao, nova_tarefa->id);
            
            /* Pular linha separadora */
            fgets(linha, sizeof(linha), arquivo);
        }
    }
    
    fclose(arquivo);
    printf("\nTotal de tarefas importadas: %d\n", tarefas_importadas);
}

void filtrar_tarefas_por_data_vencimento(GerenciadorTarefas *gerenciador, Data data_inicio, Data data_fim) {
    Tarefa *atual = gerenciador->inicio;
    int encontrou = 0;
    const char *status_str[] = {"Pendente", "Em Andamento", "Concluída"};
    
    printf("\n=== TAREFAS COM VENCIMENTO ENTRE %02d/%02d/%04d E %02d/%02d/%04d ===\n",
           data_inicio.dia, data_inicio.mes, data_inicio.ano,
           data_fim.dia, data_fim.mes, data_fim.ano);
    
    while (atual != NULL) {
        if (comparar_datas(atual->data_vencimento, data_inicio) >= 0 &&
            comparar_datas(atual->data_vencimento, data_fim) <= 0) {
            printf("ID: %d | %s | Status: %s | Prioridade: %d | Vencimento: %02d/%02d/%04d\n",
                   atual->id, atual->descricao, status_str[atual->status],
                   atual->prioridade, atual->data_vencimento.dia,
                   atual->data_vencimento.mes, atual->data_vencimento.ano);
            encontrou = 1;
        }
        atual = atual->prox;
    }
    
    if (!encontrou) {
        printf("Nenhuma tarefa encontrada no período especificado.\n");
    }
}

void gerar_relatorio_completo_arquivo(GerenciadorTarefas *gerenciador, const char *filename) {
    FILE *arquivo = fopen(filename, "w");
    Tarefa *atual = gerenciador->inicio;
    const char *status_str[] = {"Pendente", "Em Andamento", "Concluída"};
    const char *prioridade_str[] = {"", "Baixa", "Média", "Alta", "Urgente"};
    int contador_prioridade[5] = {0};
    int contador_status[3] = {0};
    int total_tempo = 0;
    int i;
    int encontrou;
    
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo %s\n", filename);
        return;
    }
    
    fprintf(arquivo, "=== RELATÓRIO COMPLETO DE TAREFAS ===\n");
    fprintf(arquivo, "Data do relatório: %02d/%02d/%04d\n\n",
            obter_data_atual().dia, obter_data_atual().mes, obter_data_atual().ano);
    
    /* Estatísticas */
    while (atual != NULL) {
        contador_prioridade[atual->prioridade]++;
        contador_status[atual->status]++;
        total_tempo += atual->tempo_estimado_minutos;
        atual = atual->prox;
    }
    
    fprintf(arquivo, "ESTATÍSTICAS GERAIS:\n");
    fprintf(arquivo, "Total de tarefas: %d\n", gerenciador->total_tarefas);
    fprintf(arquivo, "Tarefas concluídas: %d\n", gerenciador->tarefas_concluidas);
    fprintf(arquivo, "Tarefas pendentes: %d\n", gerenciador->total_tarefas - gerenciador->tarefas_concluidas);
    fprintf(arquivo, "Tempo total estimado: %d minutos (%.1f horas)\n\n", total_tempo, total_tempo / 60.0);
    
    fprintf(arquivo, "DISTRIBUIÇÃO POR PRIORIDADE:\n");
    for (i = BAIXA; i <= URGENTE; i++) {
        fprintf(arquivo, "  %s: %d tarefas\n", prioridade_str[i], contador_prioridade[i]);
    }
    fprintf(arquivo, "\n");
    
    fprintf(arquivo, "DISTRIBUIÇÃO POR STATUS:\n");
    for (i = PENDENTE; i <= CONCLUIDA; i++) {
        fprintf(arquivo, "  %s: %d tarefas\n", status_str[i], contador_status[i]);
    }
    fprintf(arquivo, "\n");
    
    /* Tarefas pendentes por prioridade */
    fprintf(arquivo, "TAREFAS PENDENTES (ordenadas por prioridade):\n");
    for (i = URGENTE; i >= BAIXA; i--) {
        atual = gerenciador->inicio;
        encontrou = 0;
        
        while (atual != NULL) {
            if (atual->prioridade == i && atual->status != CONCLUIDA) {
                if (!encontrou) {
                    fprintf(arquivo, "\n%s:\n", prioridade_str[i]);
                    encontrou = 1;
                }
                fprintf(arquivo, "  [%d] %s | Categoria: %s | Venc: %02d/%02d/%04d | %d min\n",
                       atual->id, atual->descricao, atual->categoria,
                       atual->data_vencimento.dia, atual->data_vencimento.mes, atual->data_vencimento.ano,
                       atual->tempo_estimado_minutos);
            }
            atual = atual->prox;
        }
        
        if (!encontrou) {
            fprintf(arquivo, "\n%s: Nenhuma tarefa\n", prioridade_str[i]);
        }
    }
    
    fclose(arquivo);
    printf("Relatório gerado em %s com sucesso!\n", filename);
}

void ordenar_tarefas_por_prioridade(GerenciadorTarefas *gerenciador) {
    /* Bubble sort */
    int trocado;
    Tarefa **ptr;
    Tarefa *p1;
    Tarefa *p2;
    Tarefa *atual;
    
    if (gerenciador->inicio == NULL) return;
    
    do {
        trocado = 0;
        ptr = &(gerenciador->inicio);
        
        while (*ptr != NULL && (*ptr)->prox != NULL) {
            p1 = *ptr;
            p2 = p1->prox;
            
            if (p1->prioridade < p2->prioridade) {
                p1->prox = p2->prox;
                p2->prox = p1;
                *ptr = p2;
                trocado = 1;
            }
            ptr = &((*ptr)->prox);
        }
    } while (trocado);
    
    /* Atualizar ponteiro fim */
    atual = gerenciador->inicio;
    while (atual != NULL && atual->prox != NULL) {
        atual = atual->prox;
    }
    gerenciador->fim = atual;
}

void listar_tarefas_ordenadas_por_prioridade(GerenciadorTarefas *gerenciador) {
    ordenar_tarefas_por_prioridade(gerenciador);
    listar_todas_tarefas(gerenciador);
}

void calcular_tempo_medio_conclusao(GerenciadorTarefas *gerenciador) {
    Tarefa *atual = gerenciador->inicio;
    int total_tarefas_concluidas = 0;
    int total_dias = 0;
    
    while (atual != NULL) {
        if (atual->status == CONCLUIDA) {
            int dias = calcular_dias_entre_datas(atual->data_criacao, atual->data_conclusao);
            total_dias += dias;
            total_tarefas_concluidas++;
        }
        atual = atual->prox;
    }
    
    printf("\n=== TEMPO MÉDIO DE CONCLUSÃO ===\n");
    if (total_tarefas_concluidas > 0) {
        printf("Tarefas concluídas analisadas: %d\n", total_tarefas_concluidas);
        printf("Tempo médio para conclusão: %.1f dias\n", (float)total_dias / total_tarefas_concluidas);
        printf("Total de dias acumulados: %d dias\n", total_dias);
    } else {
        printf("Nenhuma tarefa concluída para análise.\n");
    }
}

void mostrar_distribuicao_prioridades(GerenciadorTarefas *gerenciador) {
    Tarefa *atual = gerenciador->inicio;
    int contador[5] = {0};
    const char *nomes_prioridade[] = {"", "Baixa", "Média", "Alta", "Urgente"};
    int i;
    
    while (atual != NULL) {
        contador[atual->prioridade]++;
        atual = atual->prox;
    }
    
    printf("\n=== DISTRIBUIÇÃO DE PRIORIDADES ===\n");
    for (i = BAIXA; i <= URGENTE; i++) {
        float percentual = (float)contador[i] / gerenciador->total_tarefas * 100;
        printf("%s: %d tarefas (%.1f%%)\n", nomes_prioridade[i], contador[i], percentual);
    }
}

void mostrar_tarefas_mais_demoradas(GerenciadorTarefas *gerenciador) {
    Tarefa *atual = gerenciador->inicio;
    Tarefa *top_tarefas[5] = {NULL};
    int i, j;
    
    printf("\n=== TAREFAS COM MAIOR TEMPO ESTIMADO ===\n");
    
    /* Encontrar as 5 tarefas com maior tempo estimado */
    for (i = 0; i < 5; i++) {
        Tarefa *maior = NULL;
        atual = gerenciador->inicio;
        
        while (atual != NULL) {
            int ja_na_lista = 0;
            for (j = 0; j < i; j++) {
                if (top_tarefas[j] == atual) {
                    ja_na_lista = 1;
                    break;
                }
            }
            
            if (!ja_na_lista && (maior == NULL || atual->tempo_estimado_minutos > maior->tempo_estimado_minutos)) {
                maior = atual;
            }
            atual = atual->prox;
        }
        
        top_tarefas[i] = maior;
    }
    
    /* Exibir resultados */
    for (i = 0; i < 5 && top_tarefas[i] != NULL; i++) {
        if (top_tarefas[i]->tempo_estimado_minutos > 0) {
            printf("%d. %s - %d minutos\n", i + 1, top_tarefas[i]->descricao, 
                   top_tarefas[i]->tempo_estimado_minutos);
        }
    }
    
    if (top_tarefas[0] == NULL || top_tarefas[0]->tempo_estimado_minutos == 0) {
        printf("Nenhuma tarefa com tempo estimado registrado.\n");
    }
}