#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <limits>

struct Processo {
    int id;
    int tempoChegada;
    int tempoExecucao;
    int prioridade;

    int tempoRestante;
    int tempoConclusao = 0;
    int tempoTurnaround = 0;
    int tempoEspera = 0;
};

void imprimirResultados(const std::vector<Processo>& processos);
std::vector<Processo> obterEntradaUsuario();
void executarFCFS(std::vector<Processo> processos);
void executarSJF(std::vector<Processo> processos, bool preemptivo);
void executarPrioridade(std::vector<Processo> processos, bool preemptivo);
void executarRoundRobin(std::vector<Processo> processos);

int main() {
    int escolha;
    do {
        std::cout << "\n============================================\n";
        std::cout << "    SIMULADOR DE ALGORITMOS DE ESCALONAMENTO\n";
        std::cout << "============================================\n";
        std::cout << "1. FCFS (First-Come, First-Served)\n";
        std::cout << "2. SJF (Shortest Job First) - Nao Preemptivo\n";
        std::cout << "3. SJF (Shortest Job First) - Preemptivo\n";
        std::cout << "4. Prioridade - Nao Preemptivo\n";
        std::cout << "5. Prioridade - Preemptivo\n";
        std::cout << "6. Round Robin\n";
        std::cout << "0. Sair\n";
        std::cout << "============================================\n";
        std::cout << "Escolha uma opcao: ";
        std::cin >> escolha;

        if (std::cin.fail() || escolha < 0 || escolha > 6) {
             std::cout << "Entrada invalida. Por favor, insira um numero entre 0 e 6.\n";
             std::cin.clear();
             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
             escolha = -1;
             continue;
        }


        if (escolha >= 1 && escolha <= 6) {
            std::vector<Processo> processos = obterEntradaUsuario();
            switch (escolha) {
                case 1:
                    executarFCFS(processos);
                    break;
                case 2:
                    executarSJF(processos, false); 
                    break;
                case 3:
                    executarSJF(processos, true); 
                    break;
                case 4:
                    executarPrioridade(processos, false); 
                    break;
                case 5:
                    executarPrioridade(processos, true); 
                    break;
                case 6:
                    executarRoundRobin(processos);
                    break;
            }
        }

    } while (escolha != 0);

    std::cout << "Encerrando o simulador.\n";
    return 0;
}

std::vector<Processo> obterEntradaUsuario() {
    int n;
    std::cout << "Digite o numero de processos: ";
    std::cin >> n;

    std::vector<Processo> processos(n);
    for (int i = 0; i < n; ++i) {
        processos[i].id = i + 1;
        std::cout << "\n--- Processo P" << processos[i].id << " ---\n";
        std::cout << "Tempo de Chegada: ";
        std::cin >> processos[i].tempoChegada;
        std::cout << "Tempo de Execucao (Burst Time): ";
        std::cin >> processos[i].tempoExecucao;
        processos[i].tempoRestante = processos[i].tempoExecucao;
    }
    return processos;
}

void imprimirResultados(const std::vector<Processo>& processos) {
    float mediaTurnaround = 0.0f;
    float mediaEspera = 0.0f;

    std::cout << "\n--- RESULTADOS FINAIS ---\n";
    std::cout << std::left << std::setw(10) << "Processo"
              << std::setw(15) << "Turnaround"
              << std::setw(15) << "Tempo Espera" << std::endl;
    std::cout << "----------------------------------------\n";


    for (const auto& p : processos) {
        std::cout << std::left << std::setw(10) << ("P" + std::to_string(p.id))
                  << std::setw(15) << p.tempoTurnaround
                  << std::setw(15) << p.tempoEspera << std::endl;
        mediaTurnaround += p.tempoTurnaround;
        mediaEspera += p.tempoEspera;
    }

    std::cout << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Tempo Medio de Turnaround: " << mediaTurnaround / processos.size() << std::endl;
    std::cout << "Tempo Medio de Espera: " << mediaEspera / processos.size() << std::endl;
}

void executarFCFS(std::vector<Processo> processos) {
    std::cout << "\n--- Executando FCFS ---\n";
    int n = processos.size();
    int tempoAtual = 0;
    int processosConcluidos = 0;
    

    std::sort(processos.begin(), processos.end(), [](const Processo& a, const Processo& b) {
        return a.tempoChegada < b.tempoChegada;
    });

    std::vector<Processo> processosFinalizados;

    for(int i = 0; i < n; ++i) {
        Processo& p = processos[i];
        
        if (tempoAtual < p.tempoChegada) {
             std::cout << "Tempo[" << tempoAtual << "-" << p.tempoChegada << "]: CPU Ociosa\n";
             tempoAtual = p.tempoChegada;
        }

        std::cout << "Tempo[" << tempoAtual << "]: Processo P" << p.id << " iniciou a execucao.\n";
        
        p.tempoEspera = tempoAtual - p.tempoChegada;
        tempoAtual += p.tempoExecucao; 
        p.tempoConclusao = tempoAtual;
        p.tempoTurnaround = p.tempoConclusao - p.tempoChegada;

        std::cout << "Tempo[" << tempoAtual << "]: Processo P" << p.id << " concluiu.\n";
        
        processosFinalizados.push_back(p);
    }
    
    imprimirResultados(processosFinalizados);
}

void executarSJF(std::vector<Processo> processos, bool preemptivo) {
    if (preemptivo) {
        std::cout << "\n--- Executando SJF Preemptivo ---\n";
    } else {
        std::cout << "\n--- Executando SJF Nao Preemptivo ---\n";
    }

    int n = processos.size();
    int tempoAtual = 0;
    int processosConcluidos = 0;
    std::vector<Processo> filaProntos;
    std::vector<Processo> processosFinalizados;
    
    while(processosConcluidos < n) {
        for (auto& p : processos) {
            if (p.tempoChegada <= tempoAtual && p.tempoRestante > 0) {
                 bool na_fila = false;
                 for(const auto& pronto : filaProntos) if(pronto.id == p.id) na_fila = true;
                 for(const auto& finalizado : processosFinalizados) if(finalizado.id == p.id) na_fila = true;
                 if(!na_fila) {
                    filaProntos.push_back(p);
                    std::cout << "Tempo[" << tempoAtual << "]: Processo P" << p.id << " entrou na fila de prontos.\n";
                 }
            }
        }
        
        if (filaProntos.empty()) {
            std::cout << "Tempo[" << tempoAtual << "]: CPU Ociosa\n";
            tempoAtual++;
            continue;
        }

        std::sort(filaProntos.begin(), filaProntos.end(), [](const Processo& a, const Processo& b) {
            return a.tempoRestante < b.tempoRestante;
        });

        Processo& pAtual = filaProntos[0];

        if (preemptivo) {
            std::cout << "Tempo[" << tempoAtual << "]: Processo P" << pAtual.id << " esta executando.\n";
            pAtual.tempoRestante--;
            tempoAtual++;
            
            if (pAtual.tempoRestante == 0) {
                std::cout << "Tempo[" << tempoAtual << "]: Processo P" << pAtual.id << " concluiu.\n";
                for(auto& pOrig : processos) {
                    if(pOrig.id == pAtual.id) {
                        pOrig.tempoConclusao = tempoAtual;
                        pOrig.tempoTurnaround = pOrig.tempoConclusao - pOrig.tempoChegada;
                        pOrig.tempoEspera = pOrig.tempoTurnaround - pOrig.tempoExecucao;
                        processosFinalizados.push_back(pOrig);
                        break;
                    }
                }
                filaProntos.erase(filaProntos.begin());
                processosConcluidos++;
            }
        } else {
             std::cout << "Tempo[" << tempoAtual << "]: Processo P" << pAtual.id << " iniciou a execucao.\n";
             int tempoExecucao = pAtual.tempoExecucao;
             tempoAtual += tempoExecucao;
             pAtual.tempoRestante = 0;
             std::cout << "Tempo[" << tempoAtual << "]: Processo P" << pAtual.id << " concluiu.\n";

             for(auto& pOrig : processos) {
                 if(pOrig.id == pAtual.id) {
                     pOrig.tempoConclusao = tempoAtual;
                     pOrig.tempoTurnaround = pOrig.tempoConclusao - pOrig.tempoChegada;
                     pOrig.tempoEspera = pOrig.tempoTurnaround - pOrig.tempoExecucao;
                     processosFinalizados.push_back(pOrig);
                     break;
                 }
             }
             filaProntos.erase(filaProntos.begin());
             processosConcluidos++;
        }
    }
    imprimirResultados(processosFinalizados);
}


void executarPrioridade(std::vector<Processo> processos, bool preemptivo) {
     if (preemptivo) {
        std::cout << "\n--- Executando Prioridade Preemptivo ---\n";
    } else {
        std::cout << "\n--- Executando Prioridade Nao Preemptivo ---\n";
    }

    for(auto& p : processos) {
        std::cout << "Digite a prioridade para o Processo P" << p.id << ": ";
        std::cin >> p.prioridade;
    }

    int n = processos.size();
    int tempoAtual = 0;
    int processosConcluidos = 0;
    std::vector<Processo> filaProntos;
    std::vector<Processo> processosFinalizados;
    
    while(processosConcluidos < n) {
        for (auto& p : processos) {
            if (p.tempoChegada <= tempoAtual && p.tempoRestante > 0) {
                 bool na_fila = false;
                 for(const auto& pronto : filaProntos) if(pronto.id == p.id) na_fila = true;
                 for(const auto& finalizado : processosFinalizados) if(finalizado.id == p.id) na_fila = true;
                 if(!na_fila) {
                    filaProntos.push_back(p);
                    std::cout << "Tempo[" << tempoAtual << "]: Processo P" << p.id << " entrou na fila de prontos.\n";
                 }
            }
        }
        
        if (filaProntos.empty()) {
            std::cout << "Tempo[" << tempoAtual << "]: CPU Ociosa\n";
            tempoAtual++;
            continue;
        }

        std::sort(filaProntos.begin(), filaProntos.end(), [](const Processo& a, const Processo& b) {
            return a.prioridade < b.prioridade;
        });

        Processo& pAtual = filaProntos[0];

        if (preemptivo) {
             std::cout << "Tempo[" << tempoAtual << "]: Processo P" << pAtual.id << " esta executando (Prioridade: " << pAtual.prioridade << ").\n";
             pAtual.tempoRestante--;
             tempoAtual++;
             
             if (pAtual.tempoRestante == 0) {
                 std::cout << "Tempo[" << tempoAtual << "]: Processo P" << pAtual.id << " concluiu.\n";
                 for(auto& pOrig : processos) {
                    if(pOrig.id == pAtual.id) {
                        pOrig.tempoConclusao = tempoAtual;
                        pOrig.tempoTurnaround = pOrig.tempoConclusao - pOrig.tempoChegada;
                        pOrig.tempoEspera = pOrig.tempoTurnaround - pOrig.tempoExecucao;
                        processosFinalizados.push_back(pOrig);
                        break;
                    }
                 }
                 filaProntos.erase(filaProntos.begin());
                 processosConcluidos++;
             }
        } else { 
             std::cout << "Tempo[" << tempoAtual << "]: Processo P" << pAtual.id << " iniciou a execucao (Prioridade: " << pAtual.prioridade << ").\n";
             int tempoExecucao = pAtual.tempoExecucao;
             tempoAtual += tempoExecucao;
             pAtual.tempoRestante = 0;
             std::cout << "Tempo[" << tempoAtual << "]: Processo P" << pAtual.id << " concluiu.\n";

             for(auto& pOrig : processos) {
                 if(pOrig.id == pAtual.id) {
                     pOrig.tempoConclusao = tempoAtual;
                     pOrig.tempoTurnaround = pOrig.tempoConclusao - pOrig.tempoChegada;
                     pOrig.tempoEspera = pOrig.tempoTurnaround - pOrig.tempoExecucao;
                     processosFinalizados.push_back(pOrig);
                     break;
                 }
             }
             filaProntos.erase(filaProntos.begin());
             processosConcluidos++;
        }
    }
    imprimirResultados(processosFinalizados);
}


void executarRoundRobin(std::vector<Processo> processos) {
    int quantum;
    std::cout << "\n--- Executando Round Robin ---\n";
    std::cout << "Digite o valor do Quantum de tempo: ";
    std::cin >> quantum;

    int n = processos.size();
    int tempoAtual = 0;
    int processosConcluidos = 0;
    std::vector<int> filaProntos;
    std::vector<bool> naFila(n + 1, false);
    
    std::sort(processos.begin(), processos.end(), [](const Processo& a, const Processo& b) {
        return a.tempoChegada < b.tempoChegada;
    });

    int proxProcessoIdx = 0;

    while (processosConcluidos < n) {
        while (proxProcessoIdx < n && processos[proxProcessoIdx].tempoChegada <= tempoAtual) {
            if (!naFila[processos[proxProcessoIdx].id]) {
                filaProntos.push_back(proxProcessoIdx);
                naFila[processos[proxProcessoIdx].id] = true;
                std::cout << "Tempo[" << tempoAtual << "]: Processo P" << processos[proxProcessoIdx].id << " entrou na fila de prontos.\n";
            }
            proxProcessoIdx++;
        }

        if (filaProntos.empty()) {
            std::cout << "Tempo[" << tempoAtual << "]: CPU Ociosa\n";
            tempoAtual++;
            continue;
        }

        int pIdx = filaProntos.front();
        filaProntos.erase(filaProntos.begin());
        Processo& pAtual = processos[pIdx];

        int tempoExecutar = std::min(quantum, pAtual.tempoRestante);
        std::cout << "Tempo[" << tempoAtual << "]: Processo P" << pAtual.id << " iniciou a execucao por " << tempoExecutar << "ms.\n";
        
        tempoAtual += tempoExecutar;
        pAtual.tempoRestante -= tempoExecutar;

        while (proxProcessoIdx < n && processos[proxProcessoIdx].tempoChegada <= tempoAtual) {
            if (!naFila[processos[proxProcessoIdx].id]) {
                filaProntos.push_back(proxProcessoIdx);
                naFila[processos[proxProcessoIdx].id] = true;
                std::cout << "Tempo[" << tempoAtual << "]: Processo P" << processos[proxProcessoIdx].id << " entrou na fila de prontos.\n";
            }
            proxProcessoIdx++;
        }


        if (pAtual.tempoRestante == 0) {
            std::cout << "Tempo[" << tempoAtual << "]: Processo P" << pAtual.id << " concluiu.\n";
            pAtual.tempoConclusao = tempoAtual;
            pAtual.tempoTurnaround = pAtual.tempoConclusao - pAtual.tempoChegada;
            pAtual.tempoEspera = pAtual.tempoTurnaround - pAtual.tempoExecucao;
            processosConcluidos++;
            naFila[pAtual.id] = false; 
        } else {
             std::cout << "Tempo[" << tempoAtual << "]: Processo P" << pAtual.id << " foi preemptado (restam " << pAtual.tempoRestante << "ms) e voltou para o fim da fila.\n";
            filaProntos.push_back(pIdx); 
        }
    }
    imprimirResultados(processos);
}