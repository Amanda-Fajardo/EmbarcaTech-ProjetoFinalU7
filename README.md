# EmbarcaTech-ProjetoFinalU7

Este projeto utiliza a placa **BitDogLab** com um **Raspberry Pi Pico** para controlar uma matriz de LEDs WS2812, dois botões e um buzzer. O sistema permite exibir uma sequência de números de 0 a 9 de forma interativa, além de tocar melodias correspondentes. A interação ocorre por meio dos dois botões presentes na placa, que utilizam **interrupções (IRQ)** e **debounce via software** para garantir uma experiência responsiva e precisa.

# Componentes utilizados

• Matriz 5x5 de LEDs (endereçáveis) WS2812, conectada à GPIO 7;

• LED RGB, com os pinos conectados às GPIOs (11, 12 e 13);

• Botão A conectado à GPIO 5;

• Botão B conectado à GPIO 6;

• Buzzer conectado à GPIO 10.

# Funcionalidades

1. **Buzzer**

   Reproduz melodias pré-definidas correspondentes a cada número (exceto para o número 0, que não toca música).

   As melodias são armazenadas em um array de notas, onde cada nota possui uma frequência e duração específicas.

2. **Matriz de LEDs**

   Exibe os números de 0 a 9 em um formato fixo na matriz de LEDs.

   Cada número é representado por um padrão de LEDs acesos e apagados.

3. **Controle com Botões**

   Botão A: Decrementa o número exibido e faz com que o buzzer toque a melodia correspondente ao número anterior.

   Botão B: Incrementa o número exibido e faz com que o buzzer toque a próxima melodia.

   O tratamento dos botões é feito via interrupções (IRQ) com debouncing para evitar leituras incorretas.

5. **Debouncing**

   Implementado via software para garantir que apenas uma leitura válida seja considerada por pressionamento.

# Como compilar e executar

  Clone o repositório do projeto através da URL do repositório (ou copie o código para o seu ambiente de desenvolvimento).

  Configure o ambiente para compilar projetos para o Raspberry Pi Pico.

Compile o código usando o comando:

```cmake
mkdir build
cd build
cmake ..
make
```
Conecte o Raspberry Pi Pico ao computador no modo de bootloader (segure o botão BOOTSEL ao conectar o USB).

Copie o arquivo `.uf2` gerado para o Pico.

# Demonstração

https://drive.google.com/file/d/1EU87w3-aVmTakBhbCzM3iWi-nQQ-N5jy/view?usp=drive_link
