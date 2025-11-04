#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tarefas.h"

void mostrar_menu_principal(void) {
    printf("\n=== SISTEMA AVANÇADO DE GERENCIAMENTO DE TAREFAS ===\n");
    printf("1.  Gerenciar Tarefas\n");
    printf("2.  Buscar e Filtrar\n");
    printf("3.  Relatórios e Estatísticas\n");
    printf("4.  Importar/Exportar Dados\n");
    printf("0.  Sair\n");
    printf("Escolha uma opção: ");
}

void mostrar_menu_tarefas(void) {
    printf("\n=== GERENCIAMENTO DE TAREFAS ===\n");
    printf("1.  Adicionar Tarefa\n");
    printf("2.  Listar Todas as Tarefas\n");
    printf("3.  Editar Tarefa\n");
    printf("4.  Remover Tarefa\n");
    printf("5.  Alterar Status da Tarefa\n");
    printf("6.  Adicionar Tag\n");
    printf("7.  Remover Tag\n");
    printf("8.  Listar Tarefas por Status\n");
    printf("9.  Listar Tarefas por Prioridade\n");
    printf("10. Listar Tarefas por Categoria\n");
    printf("0.  Voltar\n");
    printf("Escolha uma opção: ");
}

void mostrar_menu_busca(void) {
    printf("\n=== BUSCA E FILTROS ===\n");
    printf("1.  Buscar por Descrição\n");
    printf("2.  Buscar por Tag\n");
    printf("3.  Listar Tarefas Vencidas\n");
    printf("4.  Listar Tarefas para Hoje\n");
    printf("5.  Filtrar por Data de Vencimento\n");
    printf("6.  Listar Ordenadas por Prioridade\n");
    printf("0.  Voltar\n");
    printf("Escolha uma opção: ");
}

void mostrar_menu_relatorios(void) {
    printf("\n=== RELATÓRIOS E ESTATÍSTICAS ===\n");
    printf("1.  Estatísticas do Sistema\n");
    printf("2.  Distribuição de Prioridades\n");
    printf("3.  Tempo Médio de Conclusão\n");
    printf("4.  Tarefas Mais Demoradas\n");
    printf("5.  Gerar Relatório Completo em Arquivo\n");
    printf("0.  Voltar\n");
    printf("Escolha uma opção: ");
}

void mostrar_menu_arquivos(void) {
    printf("\n=== IMPORTAR/EXPORTAR DADOS ===\n");
    printf("1.  Salvar Tarefas (binário)\n");
    printf("2.  Carregar Tarefas (binário)\n");
    printf("3.  Exportar para Texto\n");
    printf("4.  Importar de Texto\n");
    printf("5.  Backup Automático\n");
    printf("0.  Voltar\n");
    printf("Escolha uma opção: ");
}

void menu_alterar_status(GerenciadorTarefas *gerenciador) {
    int id, status;
    
    printf("ID da tarefa: ");
    scanf("%d", &id);
    printf("Novo status (0-Pendente, 1-Em Andamento, 2-Concluída): ");
    scanf("%d", &status);
    limpar_buffer();
    
    if (status >= 0 && status <= 2) {
        alterar_status_tarefa(gerenciador, id, (Status)status);
    } else {
        printf("Status inválido!\n");
    }
}

void menu_filtrar_por_data(GerenciadorTarefas *gerenciador) {
    Data inicio, fim;
    
    printf("Data inicial (dia mes ano): ");
    scanf("%d %d %d", &inicio.dia, &inicio.mes, &inicio.ano);
    printf("Data final (dia mes ano): ");
    scanf("%d %d %d", &fim.dia, &fim.mes, &fim.ano);
    limpar_buffer();
    
    if (eh_data_valida(inicio) && eh_data_valida(fim)) {
        filtrar_tarefas_por_data_vencimento(gerenciador, inicio, fim);
    } else {
        printf("Datas inválidas!\n");
    }
}

void backup_automatico(GerenciadorTarefas *gerenciador) {
    char filename[100];
    Data hoje = obter_data_atual();
    sprintf(filename, "backup_%02d%02d%04d.dat", hoje.dia, hoje.mes, hoje.ano);
    
    salvar_tarefas_arquivo(gerenciador, filename);
    printf("Backup automático criado: %s\n", filename);
}

int main(void) {
    GerenciadorTarefas *gerenciador = criar_gerenciador();
    int opcao_principal, opcao_submenu, id;
    char termo[100], tag[MAX_TAG_LENGTH], categoria[MAX_CATEGORIA], filename[100];
    char *nova_linha;
    
    /* Carregar tarefas automaticamente ao iniciar */
    carregar_tarefas_arquivo(gerenciador, ARQUIVO_TAREFAS);
    
    printf("=== BEM-VINDO AO SISTEMA AVANÇADO DE GERENCIAMENTO DE TAREFAS ===\n");
    printf("Tarefas carregadas: %d\n", gerenciador->total_tarefas);
    
    do {
        mostrar_menu_principal();
        scanf("%d", &opcao_principal);
        limpar_buffer();
        
        switch (opcao_principal) {
            case 1: /* Gerenciar Tarefas */
                do {
                    mostrar_menu_tarefas();
                    scanf("%d", &opcao_submenu);
                    limpar_buffer();
                    
                    switch (opcao_submenu) {
                        case 1: adicionar_tarefa(gerenciador); break;
                        case 2: listar_todas_tarefas(gerenciador); break;
                        case 3: 
                            printf("ID da tarefa a editar: ");
                            scanf("%d", &id);
                            limpar_buffer();
                            editar_tarefa(gerenciador, id);
                            break;
                        case 4:
                            printf("ID da tarefa a remover: ");
                            scanf("%d", &id);
                            limpar_buffer();
                            remover_tarefa(gerenciador, id);
                            break;
                        case 5: menu_alterar_status(gerenciador); break;
                        case 6:
                            printf("ID da tarefa: ");
                            scanf("%d", &id);
                            limpar_buffer();
                            printf("Tag a adicionar: ");
                            fgets(tag, MAX_TAG_LENGTH, stdin);
                            nova_linha = strchr(tag, '\n');
                            if (nova_linha != NULL) *nova_linha = '\0';
                            adicionar_tag_tarefa(gerenciador, id, tag);
                            break;
                        case 7:
                            printf("ID da tarefa: ");
                            scanf("%d", &id);
                            limpar_buffer();
                            printf("Tag a remover: ");
                            fgets(tag, MAX_TAG_LENGTH, stdin);
                            nova_linha = strchr(tag, '\n');
                            if (nova_linha != NULL) *nova_linha = '\0';
                            remover_tag_tarefa(gerenciador, id, tag);
                            break;
                        case 8:
                            printf("Status (0-Pendente, 1-Em Andamento, 2-Concluída): ");
                            scanf("%d", &id);
                            limpar_buffer();
                            if (id >= 0 && id <= 2) {
                                listar_tarefas_por_status(gerenciador, (Status)id);
                            } else {
                                printf("Status inválido!\n");
                            }
                            break;
                        case 9:
                            printf("Prioridade (1-Baixa, 2-Média, 3-Alta, 4-Urgente): ");
                            scanf("%d", &id);
                            limpar_buffer();
                            if (id >= 1 && id <= 4) {
                                listar_tarefas_por_prioridade(gerenciador, (Prioridade)id);
                            } else {
                                printf("Prioridade inválida!\n");
                            }
                            break;
                        case 10:
                            printf("Categoria: ");
                            fgets(categoria, MAX_CATEGORIA, stdin);
                            nova_linha = strchr(categoria, '\n');
                            if (nova_linha != NULL) *nova_linha = '\0';
                            listar_tarefas_por_categoria(gerenciador, categoria);
                            break;
                        case 0: break;
                        default: printf("Opção inválida!\n");
                    }
                    
                    if (opcao_submenu != 0) pausar_tela();
                } while (opcao_submenu != 0);
                break;
                
            case 2: /* Buscar e Filtrar */
                do {
                    mostrar_menu_busca();
                    scanf("%d", &opcao_submenu);
                    limpar_buffer();
                    
                    switch (opcao_submenu) {
                        case 1:
                            printf("Termo de busca: ");
                            fgets(termo, sizeof(termo), stdin);
                            nova_linha = strchr(termo, '\n');
                            if (nova_linha != NULL) *nova_linha = '\0';
                            buscar_tarefas_por_descricao(gerenciador, termo);
                            break;
                        case 2:
                            printf("Tag para buscar: ");
                            fgets(tag, MAX_TAG_LENGTH, stdin);
                            nova_linha = strchr(tag, '\n');
                            if (nova_linha != NULL) *nova_linha = '\0';
                            buscar_tarefas_por_tag(gerenciador, tag);
                            break;
                        case 3: listar_tarefas_vencidas(gerenciador); break;
                        case 4: listar_tarefas_para_hoje(gerenciador); break;
                        case 5: menu_filtrar_por_data(gerenciador); break;
                        case 6: listar_tarefas_ordenadas_por_prioridade(gerenciador); break;
                        case 0: break;
                        default: printf("Opção inválida!\n");
                    }
                    
                    if (opcao_submenu != 0) pausar_tela();
                } while (opcao_submenu != 0);
                break;
                
            case 3: /* Relatórios e Estatísticas */
                do {
                    mostrar_menu_relatorios();
                    scanf("%d", &opcao_submenu);
                    limpar_buffer();
                    
                    switch (opcao_submenu) {
                        case 1: mostrar_estatisticas(gerenciador); break;
                        case 2: mostrar_distribuicao_prioridades(gerenciador); break;
                        case 3: calcular_tempo_medio_conclusao(gerenciador); break;
                        case 4: mostrar_tarefas_mais_demoradas(gerenciador); break;
                        case 5: 
                            printf("Nome do arquivo para relatório: ");
                            fgets(filename, sizeof(filename), stdin);
                            nova_linha = strchr(filename, '\n');
                            if (nova_linha != NULL) *nova_linha = '\0';
                            if (strlen(filename) == 0) {
                                strcpy(filename, "relatorio_completo.txt");
                            }
                            gerar_relatorio_completo_arquivo(gerenciador, filename);
                            break;
                        case 0: break;
                        default: printf("Opção inválida!\n");
                    }
                    
                    if (opcao_submenu != 0) pausar_tela();
                } while (opcao_submenu != 0);
                break;
                
            case 4: /* Importar/Exportar Dados */
                do {
                    mostrar_menu_arquivos();
                    scanf("%d", &opcao_submenu);
                    limpar_buffer();
                    
                    switch (opcao_submenu) {
                        case 1:
                            printf("Nome do arquivo para salvar: ");
                            fgets(filename, sizeof(filename), stdin);
                            nova_linha = strchr(filename, '\n');
                            if (nova_linha != NULL) *nova_linha = '\0';
                            if (strlen(filename) == 0) {
                                strcpy(filename, ARQUIVO_TAREFAS);
                            }
                            salvar_tarefas_arquivo(gerenciador, filename);
                            break;
                        case 2:
                            printf("Nome do arquivo para carregar: ");
                            fgets(filename, sizeof(filename), stdin);
                            nova_linha = strchr(filename, '\n');
                            if (nova_linha != NULL) *nova_linha = '\0';
                            if (strlen(filename) == 0) {
                                strcpy(filename, ARQUIVO_TAREFAS);
                            }
                            carregar_tarefas_arquivo(gerenciador, filename);
                            break;
                        case 3:
                            printf("Nome do arquivo para exportar: ");
                            fgets(filename, sizeof(filename), stdin);
                            nova_linha = strchr(filename, '\n');
                            if (nova_linha != NULL) *nova_linha = '\0';
                            if (strlen(filename) == 0) {
                                strcpy(filename, "tarefas_exportadas.txt");
                            }
                            exportar_tarefas_txt(gerenciador, filename);
                            break;
                        case 4:
                            printf("Nome do arquivo para importar: ");
                            fgets(filename, sizeof(filename), stdin);
                            nova_linha = strchr(filename, '\n');
                            if (nova_linha != NULL) *nova_linha = '\0';
                            if (strlen(filename) == 0) {
                                strcpy(filename, "tarefas_importar.txt");
                            }
                            importar_tarefas_txt(gerenciador, filename);
                            break;
                        case 5: backup_automatico(gerenciador); break;
                        case 0: break;
                        default: printf("Opção inválida!\n");
                    }
                    
                    if (opcao_submenu != 0) pausar_tela();
                } while (opcao_submenu != 0);
                break;
                
            case 0:
                /* Salvar automaticamente ao sair */
                salvar_tarefas_arquivo(gerenciador, ARQUIVO_TAREFAS);
                printf("Dados salvos automaticamente. Saindo do sistema...\n");
                break;
                
            default:
                printf("Opção inválida!\n");
        }
        
    } while (opcao_principal != 0);
    
    liberar_gerenciador(gerenciador);
    return 0;
}