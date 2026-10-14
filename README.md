# FarmTech Solutions — Projeto 2: Coleta de Dados e Irrigação Inteligente

**FIAP | Agricultura Digital | Fase 2 | ESP32 + Wokwi + C++**

Protótipo acadêmico de irrigação inteligente com leitura simulada de umidade, pH e indicadores NPK. O ESP32 comanda um módulo relé e apresenta as informações em um display LCD 16×2.

**[Assistir à demonstração no YouTube](https://www.youtube.com/watch?v=7XPyGxeccl8)** · **[Abrir o simulador Wokwi](https://wokwi.com/)**

> **Escopo:** demonstração didática de lógica de automação; não se trata de um sistema agronômico calibrado. Os resultados devem ser interpretados como simulação.

## 1. Contexto e objetivo

No cenário da startup fictícia FarmTech Solutions, o projeto demonstra como informações obtidas de sensores podem orientar o acionamento de uma irrigação automática. O ESP32 recebe leituras de umidade e pH simulados, consulta três indicadores binários NPK e decide se deve energizar um relé que representa a bomba de água.

A cultura escolhida como **referência contextual** são hortaliças folhosas, a exemplo da alface. As faixas usadas neste protótipo foram definidas para fins pedagógicos; **não representam recomendação de manejo validada para essa cultura**.

## 2. Componentes e conexões

| Componente | Função na simulação | Conexões |
|---|---|---|
| ESP32 DevKit V1 | Executa o controle automático | Controlador central |
| Botão verde N | Indicador binário de nitrogênio | GPIO 25 e GND |
| Botão verde P | Indicador binário de fósforo | GPIO 26 e GND |
| Botão verde K | Indicador binário de potássio | GPIO 27 e GND |
| DHT22 | Mede umidade do ar, usada como **substituta didática** da umidade do solo, e temperatura | DATA GPIO 15; 3V3; GND |
| LDR / fotoresistor | Gera leitura analógica usada como **pH simulado** | AO GPIO 34; 3V3; GND |
| Módulo relé azul | Representa o comando de uma bomba de irrigação | IN GPIO 23; 5V; GND |
| LCD 16×2 I²C | Exibe valores e decisões do controlador | SDA GPIO 21; SCL GPIO 22; 5V; GND |

> **Limitações dos sensores:** o DHT22 não mede umidade do solo, o LDR não mede pH e os botões não quantificam nutrientes. Não há bomba hidráulica, sensor de vazão ou pluviômetro conectados. O acionamento do relé é o indicador da irrigação.

## 3. Circuito e evidências do Wokwi

As imagens a seguir são capturas da simulação e fazem parte da pasta [`imagens/`](imagens/).

### 3.1 Circuito completo

![Montagem do ESP32 com os três botões verdes, DHT22, LDR, LCD e relé no Wokwi](imagens/01-circuito-completo.png)

### 3.2 Umidade e pH no LCD

![Leitura de umidade e pH simulados no LCD](imagens/02-monitoramento-ph-umidade.png)

### 3.3 Indicadores NPK

![Estados dos indicadores NPK e sua classificação no LCD](imagens/03-npk-adequado.png)

### 3.4 Irrigação desligada

![Estado de bomba desligada apresentado no LCD](imagens/04-irrigacao-desligada.png)

### 3.5 pH inadequado

![Indicação de pH fora da faixa no LCD](imagens/05-ph-inadequado.png)

### 3.6 Temperatura

![Temperatura apresentada no LCD](imagens/06-temperatura.png)

### 3.7 Evidência complementar do desligamento

![Captura complementar de bomba desligada](imagens/07-irrigacao-desligada-adicional.png)

**Projeto Wokwi:** o link compartilhável específico do projeto ainda precisa ser acrescentado aqui. Para reproduzir a montagem, utilize `sketch.ino`, `diagram.json` e `libraries.txt` deste repositório.

**Evidência ainda não identificada no conjunto de capturas:** imagem do LCD exibindo `BOMBA LIGADA` na configuração final com três botões. O vídeo está referenciado na seção 11, mas as imagens deste repositório não devem ser interpretadas como comprovação de todos os cenários de teste.

## 4. Funcionamento dos botões NPK

Os botões N, P e K são conectados aos GPIOs 25, 26 e 27 com `INPUT_PULLUP`. Ao pressionar e soltar um botão, seu indicador alterna entre `0` (inativo) e `1` (ativo). Isso permite realizar os testes com um clique por nutriente, sem manter vários botões pressionados.

O programa aplica *debounce* de **45 ms** para reduzir falsas alterações de estado por oscilação de contato. No início da simulação, os três indicadores estão desativados (`N=0`, `P=0`, `K=0`).

Para autorizar a irrigação, o código exige **pelo menos dois indicadores ativos**:

```cpp
nutrientesOK = (int)npk[0] + (int)npk[1] + (int)npk[2] >= 2;
```

Essa regra é uma condição lógica da simulação, não uma avaliação química do solo. **Depois de alterar um indicador NPK, o operador também deve ajustar manualmente o LDR**, simulando uma alteração de pH. O programa não relaciona automaticamente nutrientes e pH.

## 5. Leitura de umidade e pH

O DHT22 fornece umidade relativa do ar, utilizada na simulação como substituto da umidade do solo. A leitura é exibida em porcentagem. Também é apresentada a temperatura fornecida pelo sensor.

O ESP32 recebe o sinal analógico do LDR pelo GPIO 34, usando uma resolução de **12 bits (0 a 4095)**. O valor é convertido artificialmente para uma escala de pH de **0 a 14**:

```cpp
ph = 14.0f * leituraLDR / 4095.0f;
```

Essa transformação **não é uma calibração química**. Para alterar o pH durante a execução, abra o controle **Illumination (lux)** do LDR e mova o ajuste até obter no LCD a faixa desejada. O valor exato de lux pode variar conforme a resposta simulada do componente.

## 6. Regras de controle da irrigação

| Critério | Parâmetro didático |
|---|---|
| Umidade para ligar | Inferior a **45%** |
| Umidade para desligar | Igual ou superior a **60%** |
| pH permitido | **5,5 a 7,5**, incluindo limites |
| NPK suficiente | **2 ou 3** indicadores ativos |
| Atualização das leituras | Aproximadamente **2 segundos** |
| Alternância das telas do LCD | Aproximadamente **3 segundos** |

**Sequência de decisão implementada no código:**

1. Se o DHT22 retornar uma leitura inválida, o comando de irrigação é desligado por segurança.
2. Se o pH estiver fora da faixa ou o NPK for insuficiente, a irrigação é desligada.
3. Se as condições forem adequadas e a bomba estiver desligada, a irrigação é acionada quando a umidade for inferior a 45%.
4. Se a irrigação estiver ligada, ela é mantida na faixa intermediária e desligada quando a umidade atingir 60%.

Essa diferença entre os limites de ligar e desligar caracteriza **histerese**, que evita alternâncias rápidas próximas de um único limite.

No circuito simulado, `HIGH` no GPIO 23 é o comando de acionamento do módulo relé, e `LOW` o comando de desligamento. O LCD indica o estado lógico da irrigação.

## 7. Informações apresentadas no LCD

O LCD 16×2 alterna automaticamente entre cinco telas:

1. **Umidade (%) e pH simulado**;
2. **N, P, K (0/1)** e condição NPK;
3. **Irrigação:** `BOMBA LIGADA` ou `BOMBA DESLIGADA`;
4. **Temperatura** do DHT22;
5. **pH** e classificação `ADEQUADO` ou `INADEQUADO`.

O display foi incorporado para tornar os dados acessíveis diretamente na simulação, inclusive quando o Monitor Serial não estiver visível no navegador. O código também possui mensagens de diagnóstico via `Serial`.

## 8. Como executar o projeto

1. Abra [Wokwi](https://wokwi.com/) e crie um projeto **ESP32 Arduino**.
2. Substitua integralmente o conteúdo de `sketch.ino` e `diagram.json` pelos arquivos deste repositório.
3. Instale no **Library Manager** as bibliotecas indicadas em [`libraries.txt`](libraries.txt): `DHT sensor library for ESPx` e `LiquidCrystal I2C`.
4. Inicie a simulação e aguarde a inicialização do LCD.
5. Clique no **DHT22** e altere o controle **Humidity** para simular a umidade.
6. Clique no **LDR** e altere **Illumination (lux)**, acompanhando o valor do pH no LCD.
7. Clique individualmente nos botões verdes N, P e K para alternar seus estados.
8. Sempre que alterar NPK, reajuste manualmente o LDR para reproduzir a mudança de pH proposta no enunciado.
9. Observe a tela de irrigação e o comando do relé nos testes.

> Se o LCD continuar mostrando a umidade inicial após uma mudança no DHT22, **não considere o teste de desligamento validado**. Confira primeiro se a leitura exibida foi atualizada; um comando do relé só pode ser interpretado junto com o valor efetivamente lido pelo programa.

## 9. Cenários de teste e situação das evidências

| ID | Manipulação no Wokwi | Resultado esperado | Registro disponível |
|---|---|---|---|
| T01 | DHT22 30–40%; pH 5,5–7,5; N e P ativos | Bomba liga | Funcionamento informado durante testes anteriores; falta captura identificada da versão final |
| T02 | Após ligar, elevar a umidade para 70% | Bomba desliga | Funcionamento confirmado em teste anterior com interruptores; houve divergência de leitura na revisão posterior com botões |
| T03 | Umidade baixa; N e P ativos; pH acima de 7,5 | Bomba desliga | Captura de pH inadequado e bomba desligada disponível, sem comprovar isoladamente toda a sequência |
| T04 | Umidade baixa; pH adequado; somente N ativo | Bomba desliga | Sem evidência individual identificada |
| T05 | Clicar uma vez em N e outra em P | LCD indica N=1 e P=1 | Captura de NPK adequado disponível |
| T06 | Clicar novamente em N | Indicador N volta a 0 | Sem evidência individual identificada |
| T07 | Simular falha na leitura do DHT22 | Desliga relé | Proteção presente no código; teste prático não documentado |

**Observação sobre a revisão final:** durante os testes, foi relatado que o controle do DHT22 havia sido alterado para aproximadamente 70%, mas o LCD permanecia em 40%. Isso impede afirmar que o teste de desligamento foi inteiramente comprovado nessa revisão. É necessária nova verificação da atualização da leitura para encerrá-lo tecnicamente.

Os parâmetros e critérios acima são **valores experimentais**, não dados agronômicos validados.

## 10. Organização do repositório

```text
FIAPCap1UmMapaDoTesouro/
├── README.md
├── sketch.ino
├── diagram.json
├── libraries.txt
├── ROTEIRO_VIDEO.md
├── CHECKLIST_ENTREGA.md
└── imagens/
    ├── 01-circuito-completo.png
    ├── 02-monitoramento-ph-umidade.png
    ├── 03-npk-adequado.png
    ├── 04-irrigacao-desligada.png
    ├── 05-ph-inadequado.png
    ├── 06-temperatura.png
    └── 07-irrigacao-desligada-adicional.png
```

**Repositório:** [GitHub — FIAPCap1UmMapaDoTesouro](https://github.com/joaohamacek/FIAPCap1UmMapaDoTesouro)

## 11. Demonstração em vídeo

O funcionamento do protótipo foi apresentado em um vídeo de demonstração publicado no YouTube para atender ao entregável da atividade.

### [▶ Assistir ao vídeo — FarmTech Solutions, FIAP Fase 2](https://www.youtube.com/watch?v=7XPyGxeccl8)

**Link direto:** https://www.youtube.com/watch?v=7XPyGxeccl8

A atividade solicita vídeo de **até 5 minutos** e visibilidade **Não listado**. Essas configurações devem ser conferidas no YouTube Studio antes do envio final.

## 12. Integrantes e atividades opcionais

**Integrantes:** inserir nomes completos e RMs do grupo antes da submissão, se exigidos pelo portal da disciplina.

**Programa Ir Além (opcional):** não implementado nesta entrega. O projeto não utiliza previsão meteorológica por API nem análise estatística em R.

## 13. Referências e considerações finais

- [Wokwi — documentação do simulador](https://docs.wokwi.com/)
- [Arduino-ESP32 — documentação oficial](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
- **FIAP — Atividade Projeto 2: Iniciando a coleta de dados**, enunciado da disciplina.

O projeto demonstra o uso integrado de sensores simulados, leitura analógica, estados binários, tomada de decisão e acionamento de um relé em ambiente virtual. A aplicação real demandaria sensores apropriados, calibração, critérios de cultivo e dispositivos de proteção elétrica e hidráulica.
