Nome: Stella santana de jesus 
Curso: Analise e desenvolvimento de sistemas 
# 🏥 Sistema de Triagem e Gerenciamento de Senhas

Sistema desenvolvido em **linguagem C** para informatizar a triagem, classificação de prioridades e organização das filas de atendimento em uma clínica de especialidades médicas.

## 🎯 Objetivo do Sistema

Automatizar o processo de emissão e chamada de senhas na recepção da clínica, garantindo que pacientes com critérios de prioridade ou urgência sejam identificados automaticamente e atendidos na ordem correta. O projeto aplica o conceito de filas (**FIFO** — *First In, First Out*) separadas por categoria de prioridade.

> **Nota:** As categorias e faixas etárias utilizadas neste sistema são regras didáticas definidas para fins acadêmicos e não substituem protocolos clínicos de classificação de risco ou a legislação vigente.

---

## ⚙️ Funcionalidades

* **Módulo 1 — Registro de Senhas:** Cadastro do paciente na triagem (gênero, idade, gestante, lactante, necessidade especial e condição de obesidade) com validação de dados de entrada.
* **Módulo 2 — Classificação Automática:** Identificação automática da categoria e geração de senha sequencial independente por fila:
  * **Urgente (`U001`, `U002`...):** Pacientes com idade superior a 80 anos.
  * **Preferencial (`P001`, `P002`...):** Gestantes, lactantes, pessoas com necessidades especiais, pessoas com obesidade, mulheres com 60 anos ou mais e homens com 65 anos ou mais.
  * **Normal (`N001`, `N002`...):** Pacientes sem condições de prioridade cadastradas.
* **Módulo 3 — Registro e Controle de Atendimento:**
  * Consulta da próxima senha a ser chamada respeitando a precedência (`Urgente > Preferencial > Normal`);
  * Registro da chamada do paciente e exibição da senha em atendimento;
  * Finalização do atendimento e atualização automática da fila (sem retorno de senhas já atendidas).
* **Módulo 4 — Painel de Chamada:** Exibição textual no terminal com a senha em atendimento atual, categoria, próximas senhas na fila e quantidade de pacientes aguardando por categoria.

---

## 🧩 Arquitetura e Funções Implementadas

O código foi estruturado de forma modular, delegando cada responsabilidade para funções específicas fora da `main()`:

| Função | Responsabilidade |
| :--- | :--- |
| `exibirMenu()` | Exibe o menu interativo no terminal e valida a opção escolhida pelo atendente. |
| `cadastrarPaciente()` | Coleta e valida os dados de triagem informados no atendimento. |
| `classificarPrioridade()` | Aplica as regras de idade e condições especiais para definir a categoria (`Urgente`, `Preferencial` ou `Normal`). |
| `gerarSenha()` | Incrementa o contador da categoria e monta o código sequencial da senha (ex.: `U001`, `P001`, `N001`). |
| `enfileirarPaciente()` | Adiciona o paciente ao final da fila correspondente (estrutura FIFO). |
| `consultarProximaSenha()` | Verifica qual é a próxima senha na ordem de prioridade sem removê-la da fila. |
| `chamarProximoPaciente()` | Retira o próximo paciente da fila de espera e o coloca no status *Em Atendimento*. |
| `finalizarAtendimento()` | Conclui o atendimento atual e libera o painel para a próxima chamada. |
| `exibirPainel()` | Renderiza o painel de chamada com a senha atual, próximas senhas e o resumo de espera por categoria. |

---

## 🚀 Como Compilar e Executar

### Pré-requisitos
* Ter um compilador C instalado na máquina 
