# FarmTech Solutions — Projeto 2: Coleta de Dados e Irrigação Inteligente

**FIAP | Agricultura Digital | ESP32 + Wokwi | Fase 2**

## 1. Contexto e objetivo

A startup fictícia FarmTech Solutions desenvolveu um protótipo didático de irrigação automatizada. Um ESP32 lê dados simulados de umidade do solo, pH e indicadores binários NPK, decide quando acionar uma bomba representada por um relé azul e apresenta informações em um LCD 16×2. O objetivo é reduzir irrigações desnecessárias no cenário experimental e demonstrar sensoriamento, lógica de controle e IoT.

**Cultura de referência do cenário:** hortaliças folhosas, como a alface, **apenas para contextualização**. Os limites de 45/60% de umidade, pH 5,5–7,5 e requisito de dois indicadores NPK ativos são **parâmetros didáticos de software**, não recomendações agronômicas comprovadas para a alface. Um projeto real exigiria caracterização do solo, cultura, estádio fenológico e calibração dos sensores.

## 2. Componentes

| Componente | Função no protótipo | Ligação ao ESP32 |
|---|---|---|
| ESP32 DevKit v1 | Controlador central | — |
| 3 botões verdes N, P, K | Representação binária de nutrientes | GPIO 25, 26, 27 + GND |
| DHT22 | Umidade do ar **usada como substituto didático** da umidade do solo | DATA GPIO 15; VCC 3V3; GND |
| Fotoresistor LDR | Leitura analógica representando **pH simulado**, ajustado manualmente | AO GPIO 34; VCC 3V3; GND |
| Relé azul | Simulação do comando da bomba | IN GPIO 23; VCC 5V; GND |
| LCD 16×2 I²C | Visualização de umidade, pH, NPK, temperatura e irrigação | SDA GPIO 21; SCL GPIO 22; VCC 5V; GND |

> **Limitações:** nem o LDR mede pH real, nem o DHT22 mede umidade do solo, nem os botões aferem concentrações químicas reais. Não há sensor de chuva, vazão ou bomba física conectada; o relé representa a bomba na simulação.

## 3. Circuito e imagem obrigatória

Abrir `diagram.json` no Wokwi para visualizar a montagem. **Antes de enviar o trabalho, tirar uma captura REAL da versão final com os três botões verdes e o LCD, salvando em `imagens/circuito.png`.** Inserir a imagem abaixo quando estiver disponível:

<!-- Depois de adicionar imagens/circuito.png, remova os sinais de comentario desta linha:
![Circuito final no Wokwi](imagens/circuito.png)
-->

**Link do projeto Wokwi:** `ADICIONAR_LINK_PUBLICO_DO_WOKWI`

## 4. Leitura e representação dos nutrientes NPK

Cada botão verde é ligado ao GPIO configurado com `INPUT_PULLUP`. **Um clique completo (pressionar e soltar) alterna o estado registrado** entre 0 (inativo) e 1 (ativo); não é necessário manter pressionado. O software aplica *debounce* de 45 ms para evitar múltiplas alternâncias por um único clique. Inicialmente, N=P=K=0.

- `N=1`, `P=1`, `K=0` significa dois indicadores ativos, atendendo ao critério de autorização.
- A regra `N+P+K >= 2` é uma **convenção binária experimental**; não descreve teores ou doses reais de nutrientes.
- **Sempre que o estado NPK for alterado, o operador deve ajustar MANUALMENTE o LDR** para simular o efeito sobre o pH, conforme o enunciado. O sistema **não calcula automaticamente** o pH a partir dos botões.

## 5. Leitura do pH e umidade

O ESP32 lê o LDR com ADC de 12 bits (valores de 0 a 4095) e converte para a escala didática de 0 a 14:

```cpp
ph = 14.0f * leituraLDR / 4095.0f;
```

Essa é uma **escala artificial**, não uma curva de calibração de pH. Ajuste a luminosidade em lux e observe o pH exibido no LCD: o relacionamento lux → ADC depende da simulação. O DHT22 fornece umidade relativa do ar em %, tomada aqui como indicador substituto da umidade do solo.

## 6. Lógica de decisão

A cada 2 segundos:

1. Ler a umidade do DHT22 e o ADC do LDR. Os botões são monitorados continuamente.
2. Verificar `N + P + K >= 2` e `5,5 <= pH <= 7,5`.
3. Se alguma condição não for atendida, **desligar** a irrigação.
4. Se as condições forem atendidas, **ligar** quando `umidade < 45%`.
5. Depois de ligada, **manter ligada** entre 45% e 60%, desligando ao atingir `umidade >= 60%` (*histerese*).
6. Falha na leitura do DHT22: **desligar o relé por segurança**.

O comando digital `HIGH` no GPIO 23 aciona o relé usado no protótipo; `LOW` desliga. A realimentação visual vem do LCD e do LED de acionamento do relé no Wokwi.

## 7. Telas do LCD

O LCD muda automaticamente a cada 3 segundos entre cinco telas:

1. Umidade (%) e pH simulado.
2. Estados binários N, P, K e condição NPK.
3. Estado da irrigação: `BOMBA LIGADA` ou `BOMBA DESLIGADA`.
4. Temperatura (DHT22).
5. pH e avaliação `ADEQUADO`/`INADEQUADO`.

O LCD foi incluído como alternativa ao Monitor Serial, que apresentou dificuldades de visualização no navegador durante o desenvolvimento. As mensagens Serial continuam no código.

## 8. Execução no Wokwi

1. Acessar [Wokwi](https://wokwi.com/) e criar um projeto **ESP32 Arduino**.
2. Substituir **inteiramente** os arquivos `sketch.ino` e `diagram.json` pelos deste repositório.
3. No **Library Manager**, instalar `DHT sensor library for ESPx` e `LiquidCrystal I2C` (ver `libraries.txt`).
4. Salvar, iniciar a simulação e esperar aparecer a leitura no LCD.
5. Ajustar `Humidity` do DHT22; ajustar `Illumination (lux)` do LDR observando o pH exibido.
6. Clicar uma vez em cada botão N, P ou K para alternar seu indicador; confirmar no LCD.
7. Reajustar manualmente o LDR ao mudar NPK. Verificar o estado do relé e do LCD.

## 9. Plano de testes e evidências

| ID | Condições de entrada | Resultado esperado | Estado da verificação |
|---|---|---|---|
| T01 | Umidade 40%, N=1/P=1/K=0, pH adequado | Bomba liga | Validado na versão anterior com interruptores; retestar botões |
| T02 | Após T01, elevar umidade a 70% | Bomba desliga | Validado na versão anterior com interruptores; retestar botões |
| T03 | Umidade baixa, 2 NPK ativos, pH fora da faixa | Bomba desliga | Informado como funcionando na versão anterior; retestar botões |
| T04 | Umidade baixa, somente 1 NPK ativo, pH adequado | Bomba desliga | Pendente de confirmação |
| T05 | Clicar uma vez no N e uma vez no P, sem segurar | LCD registra N=1, P=1 | Pendente — versão final com botões |
| T06 | Após T05, clicar novamente no N | LCD registra N=0 | Pendente — versão final com botões |
| T07 | Simular falha de leitura DHT22 | Relé desliga | Pendente |

**Evidências a incluir:** `imagens/circuito.png`, `imagens/bomba-ligada.png`, `imagens/bomba-desligada.png`. As imagens precisam ser capturas reais do projeto final; não usar ilustrações como se fossem testes realizados.

## 10. Estrutura do repositório

```text
FarmTech_FIAP_GitHub/
├── README.md
├── sketch.ino
├── diagram.json
├── libraries.txt
├── ROTEIRO_VIDEO.md
├── CHECKLIST_ENTREGA.md
└── imagens/
    └── (adicionar capturas reais da versão final)
```

## 11. Vídeo de demonstração

O funcionamento completo do sistema de irrigação inteligente FarmTech Solutions pode ser acompanhado no vídeo abaixo.

**Vídeo demonstrativo — FIAP Fase 2**

[Assistir à demonstração no YouTube](https://www.youtube.com/watch?v=7XPyGxeccl8)

O vídeo apresenta o monitoramento de umidade, pH e NPK, além do acionamento e desligamento automático da irrigação por meio do ESP32.

## 12. Opcional “Ir Além”

Não implementado nesta entrega: previsão do tempo por API externa e análise em R. Os requisitos opcionais não devem ser apresentados como executados.

## 13. Referências

- [Wokwi — documentação](https://docs.wokwi.com/)
- [Arduino-ESP32 — documentação](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
- Enunciado **FIAP — Atividade Projeto 2: Iniciando a coleta de dados**, fornecido na disciplina.



  
