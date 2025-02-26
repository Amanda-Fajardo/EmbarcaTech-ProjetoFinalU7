#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h" 
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "Jukebox.pio.h"
#include "nota.h"

#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7

#define BT_A 5
#define BT_B 6 
#define BUZZER_PIN 10

// Número atual exibido na matriz de LEDs
volatile int num_atual = 0; 
volatile bool atualizar_display = false; // Flag para atualizar display
volatile int melodia_atual = 0;         // Índice da melodia atual
volatile bool tocando_musica = false;   // Flag para controlar se uma música está tocando

// Variáveis para debouncing
static absolute_time_t last_time_bt_a = 0;
static absolute_time_t last_time_bt_b = 0;
#define DEBOUNCE_DELAY_MS 200 // Tempo de debouncing em milissegundos

// Números de 0 a 9 representados na matriz 5x5
bool numeros[10][NUM_PIXELS] = {
    { 
        0, 1, 1, 1, 0, // 0
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0
    },
    { 
        0, 1, 1, 1, 0, //1
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 1, 0, 0,
        0, 0, 1, 0, 0
    },
    { 
        0, 1, 1, 1, 0, //2
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0
    },
    { 
        0, 1, 1, 1, 0, //3
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0
    },
    { 
        0, 1, 0, 0, 0, //4
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0
    },
    { 
        0, 1, 1, 1, 0, //5
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0
    },
    {
        0, 1, 1, 1, 0, //6
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 0, 0,
        0, 1, 1, 1, 0
    },
    { 
        0, 1, 0, 0, 0, //7
        0, 0, 0, 1, 0,
        0, 1, 0, 0, 0,
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0
    },
    { 
        0, 1, 1, 1, 0, //8
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0
    },
    {
        0, 1, 1, 1, 0, //9
        0, 0, 0, 1, 0,
        0, 1, 1, 1, 0,
        0, 1, 0, 1, 0,
        0, 1, 1, 1, 0
    }
};

// Melodias
Nota melodias[10][16] = {
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}, //Melodia 0, não toca nada
    
    {{DO, 300}, {DO, 300}, {RE, 300}, {DO, 300}, {FA, 300}, {MI, 300}, {DO, 300}, {DO, 300},
    {DO, 300}, {DO, 300}, {RE, 300}, {DO, 300}, {SOL, 300}, {FA, 300}, {DO, 300}, {DO, 300}}, //Melodia 1

    {{DO, 300}, {DO, 300}, {SOL, 300}, {SOL, 300}, {LA, 300}, {LA, 300}, {SOL, 300}, {FA, 300},
    {FA, 300}, {FA, 300}, {MI, 300}, {MI, 300}, {RE, 300}, {RE, 300}, {DO, 300}, {DO, 300}}, //Melodia 2

    {{DO, 300}, {RE, 300}, {MI, 300}, {DO, 300}, {MI, 300}, {FA, 300}, {SOL, 300}, {SOL, 300},
    {SOL, 300}, {LA, 300}, {SI, 300}, {DO, 300}, {DO, 300}, {RE, 300}, {MI, 300}, {DO, 300}}, //Melodia 3

    {{DO, 300}, {MI, 300}, {SOL, 300}, {MI, 300}, {DO, 300}, {MI, 300}, {SOL, 300}, {MI, 300},
    {DO, 300}, {MI, 300}, {SOL, 300}, {MI, 300}, {DO, 300}, {MI, 300}, {SOL, 300}, {MI, 300}}, //Melodia 4

    {{DO, 300}, {RE, 300}, {MI, 300}, {FA, 300}, {SOL, 300}, {LA, 300}, {SI, 300}, {DO, 300},
    {DO, 300}, {SI, 300}, {LA, 300}, {SOL, 300}, {FA, 300}, {MI, 300}, {RE, 300}, {DO, 300}}, //Melodia 5

    {{DO, 300}, {DO, 300}, {RE, 300}, {MI, 300}, {MI, 300}, {RE, 300}, {DO, 300}, {RE, 300},
    {MI, 300}, {MI, 300}, {RE, 300}, {DO, 300}, {RE, 300}, {MI, 300}, {RE, 300}, {DO, 300}}, //Melodia 6

    {{DO, 300}, {MI, 300}, {SOL, 300}, {LA, 300}, {SOL, 300}, {MI, 300}, {DO, 300}, {MI, 300},
    {SOL, 300}, {LA, 300}, {SOL, 300}, {MI, 300}, {DO, 300}, {MI, 300}, {SOL, 300}, {LA, 300}}, //Melodia 7

    {{DO, 300}, {DO, 300}, {SOL, 300}, {SOL, 300}, {LA, 300}, {LA, 300}, {SOL, 300}, {FA, 300}, //Melodia 8
    {FA, 300}, {FA, 300}, {MI, 300}, {MI, 300}, {RE, 300}, {RE, 300}, {DO, 300}, {DO, 300}},

    {{DO, 300}, {MI, 300}, {SOL, 300}, {DO, 300}, {SOL, 300}, {MI, 300}, {DO, 300}, {MI, 300},
    {SOL, 300}, {DO, 300}, {SOL, 300}, {MI, 300}, {DO, 300}, {MI, 300}, {SOL, 300}, {DO, 300}} //Melodia 9
};

// Acende o LED na cor especificada
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// Converte valores RGB para um número de 32 bits
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

// Debounce para os botões      
bool debounce(uint gpio) {
    static absolute_time_t last_time = 0;                   //armazena o último momento da leitura
    absolute_time_t now = get_absolute_time();              //obtém o tempo atual
    if (absolute_time_diff_us(last_time, now) < 30000) {    //calcula a diferença de tempo entre a última leitura e o tempo atual
        return false;                                       //se a diferença for menor que 30.000 µs (30 ms), a função retorna false
    }                                                       //a leitura atual

    last_time = now; //atualiza o tempo para a última leitura válida
    return !gpio_get(gpio);
}

void buzzer_config(){
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0);
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(BUZZER_PIN, 0);
}

// Função para tocar uma nota
void tocar_nota(int freq, int duracao) {
    if (freq == 0) {
        pwm_set_gpio_level(BUZZER_PIN, 0); // Silencia o buzzer
    } else {
        uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
        uint wrap = clock_get_hz(clk_sys) / freq;
        pwm_set_wrap(slice_num, wrap);

        // Ajuste dinâmico do duty cycle com base na frequência
        uint duty_cycle;
        if (freq < 300) {
            duty_cycle = wrap / 3; // Duty cycle de 50% para frequências baixas
        } else if (freq < 450) {
            duty_cycle = wrap / 4; // Duty cycle de ~33% para frequências médias
        } else {
            duty_cycle = wrap / 5; // Duty cycle de 25% para frequências altas
        }
        pwm_set_chan_level(slice_num, pwm_gpio_to_channel(BUZZER_PIN), duty_cycle);
    }
    sleep_ms(duracao);
    pwm_set_gpio_level(BUZZER_PIN, 0); // Desliga o buzzer
    sleep_ms(20); // Pausa entre as notas
}

// Função para tocar uma melodia completa
void tocar_melodia(int indice) {
    if (indice == 0) return;

    tocando_musica = true;
    for (int i = 0; i < 16; i++) {
        tocar_nota(melodias[indice][i].freq, melodias[indice][i].duracao);
    }
    tocando_musica = false;
}

// Interrupção dos botões
void gpio_irq(uint gpio, uint32_t events) {
    absolute_time_t now = get_absolute_time();

    if (gpio == BT_A) {
        if (absolute_time_diff_us(last_time_bt_a, now) > DEBOUNCE_DELAY_MS * 1000) {
            if (!gpio_get(BT_A)) {
                num_atual = (num_atual + 9) % 10; // Decrementa o número
                melodia_atual = num_atual;        // Atualiza a melodia atual de acordo com o número atual
                atualizar_display = true;
            }
            last_time_bt_a = now;
        }
    } else if (gpio == BT_B) {
        if (absolute_time_diff_us(last_time_bt_b, now) > DEBOUNCE_DELAY_MS * 1000) {
            if (!gpio_get(BT_B)) {
                num_atual = (num_atual + 1) % 10; // Incrementa o número
                melodia_atual = num_atual;        // Atualiza a melodia atual de acordo com o número atual
                atualizar_display = true;
            }
            last_time_bt_b = now;
        }
    }
}

// Exibe o número na matriz de LEDs
void exibir_numero(int numero) {
    if (numero < 0 || numero > 9) return; // Evita números inválidos

    for (int i = 0; i < NUM_PIXELS; i++) {     
        if (numeros[numero][i]) {             
            put_pixel(urgb_u32(70, 0, 70)); // Acende em roxo
        } else {
            put_pixel(urgb_u32(0, 0, 0)); // Apaga o LED
        }
    }
    sleep_ms(10); 
}

int main() {
    stdio_init_all();

    // Configuração dos botões
    gpio_init(BT_A);
    gpio_init(BT_B);
    gpio_set_dir(BT_A, GPIO_IN);
    gpio_set_dir(BT_B, GPIO_IN);
    gpio_pull_up(BT_A);
    gpio_pull_up(BT_B);

    buzzer_config();
    
    // Interrupção dos botões
    gpio_set_irq_enabled_with_callback(BT_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq);
    gpio_set_irq_enabled_with_callback(BT_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq);
    
    // Inicializa o PIO para WS2812
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &Jukebox_program);

    Jukebox_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    // Exibe o primeiro número na inicialização
    exibir_numero(num_atual);

    while (1) {
        if (atualizar_display && !tocando_musica) {
            exibir_numero(num_atual); // Atualiza o número na matriz de LEDs
            if (num_atual != 0) { 
                tocar_melodia(melodia_atual);
            }
            atualizar_display = false;
        }
    }

    return 0;
}