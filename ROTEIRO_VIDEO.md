# Roteiro do vídeo — FarmTech Solutions (até 5 minutos)

**0:00–0:35 — Apresentação**: identificar a equipe e o propósito do protótipo de irrigação inteligente da FIAP.

**0:35–1:15 — Componentes**: mostrar no Wokwi ESP32, três botões N/P/K, LDR, DHT22, relé azul e LCD. Explicar que são substitutos didáticos, e que o relé representa a bomba.

**1:15–2:00 — Leituras**: mostrar LCD indicando umidade, pH, temperatura e NPK; explicar limites do software: 45%, 60% e pH 5,5–7,5.

**2:00–3:00 — NPK e pH**: clicar N e P (um clique cada), confirmar no LCD N=1/P=1, e **alterar manualmente o LDR** para simular a mudança de pH. Mostrar valor dentro da faixa.

**3:00–3:45 — Ligar e desligar**: com umidade abaixo de 45%, NPK suficiente e pH adequado, mostrar `BOMBA LIGADA` e LED do relé; elevar umidade acima de 60% e mostrar `BOMBA DESLIGADA`.

**3:45–4:30 — Bloqueios**: colocar pH fora da faixa e/ou deixar apenas um NPK ativo; mostrar desativação da irrigação. Ajustar LDR manualmente ao mudar NPK.

**4:30–5:00 — Encerramento**: comentar lógica de histerese, limitações da simulação e localização do código/documentação no GitHub.

**Publicação:** YouTube como **Não listado**; inserir URL no README antes de entregar. Não afirmar que um teste deu certo sem filmá-lo e conferi-lo.
