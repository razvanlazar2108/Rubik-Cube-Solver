#include "config.h"
#include "uart.h"
#include "motoare.h"

int main(void) {
    motoare_init_pini();
    uart_init();

    char buffer_miscari[64];

    while (1) {
        // Citim linia de mișcări direct (ex: "U R2 F' D") imediat ce se apasă Enter
        uart_read_line(buffer_miscari, 64);
        
        // Dacă bufferul nu este gol, execută secvențial mișcările
        if (buffer_miscari[0] != '\0') {
            executa_lista_miscari(buffer_miscari);
        }
    }
    return 0;
}