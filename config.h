#ifndef CONFIG_H_
#define CONFIG_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>

// Driver Motoare - Driver CNC Enable (Ambele legate pe PB0)
#define EN_PIN      PB0

// Pinii de DIRECȚIE (DIR)
#define DIR_M1      PD2
#define DIR_M2      PD4
#define DIR_M3      PD6
#define DIR_M4      PD7
#define DIR_M5      PC0
#define DIR_M6      PC3

// Pinii de PAȘI (STEP)
#define STEP_M1     PB2
#define STEP_M2     PB3
#define STEP_M3     PB4
#define STEP_M4     PB5
#define STEP_M5     PC1
#define STEP_M6     PC2

#endif /* CONFIG_H_ */