# Manual de Usuario — TPE Arquitectura de Computadoras

## Tabla de Contenidos

1. [Compilación y ejecución](#compilación-y-ejecución)
2. [Shell](#shell)
3. [Comandos del sistema](#comandos-del-sistema)
4. [Manejo de excepciones](#manejo-de-excepciones)
5. [Kernel Cracker](#kernel-cracker)
6. [Pacman](#pacman)

---

## Compilación y ejecución

### Compilar

```bash
./compile.sh
```

Este script levanta el contenedor, compila el Toolchain y luego el proyecto completo (bootloader, kernel, userland e imagen), y para el contenedor al finalizar.

### Ejecutar en QEMU

```bash
./run.sh
```

Inicia el sistema operativo en QEMU con soporte de audio.

---

## Shell

Al iniciar, el sistema presenta un prompt interactivo:

```
@shell>
```

El intérprete de comandos acepta una línea de texto a la vez. Los comandos se escriben seguidos de sus argumentos separados por espacios. Si el comando no existe, el sistema lo indica con un mensaje de error.

Para salir del shell se puede escribir `exit`.

---

## Comandos del sistema

### `help`

Muestra la lista de todos los comandos disponibles con una breve descripción.

```
@shell> help
```

---

### `time`

Muestra la hora actual del sistema

```
@shell> time
```

---

### `echo`

Imprime en pantalla el texto ingresado a continuación del comando.

```
@shell> echo Hola mundo
```

Salida:
```
Hola mundo
```

---

### `clear`

Limpia la pantalla.

```
@shell> clear
```

---

### `size <n>`

Cambia el tamaño del texto en la shell. El parámetro `n` es un número entero positivo.

```
@shell> size 16
@shell> size 10
```

Si el valor ingresado no es válido, el sistema lo indica con un mensaje de error.

---

### `regs`

Muestra el valor actual de todos los registros del procesador (RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, R8–R15, RIP, RFLAGS, CS, DS, SS, ES, FS, GS).

```
@shell> regs
```

---

## Manejo de excepciones

El kernel maneja dos tipos de excepciones por hardware. Ante cualquiera de ellas, se muestra en pantalla el tipo de error, el valor del Instruction Pointer y todos los registros del procesador en el momento del error. Luego el sistema se recupera y regresa al prompt de la shell.

### `divzero`

Provoca intencionalmente una división por cero para verificar el manejador de excepciones correspondiente.

```
@shell> divzero
```

---

### `invalidop`

Provoca una excepción de opcode inválido.

```
@shell> invalidop
```

Ejemplo de salida:
```
Provocando Invalid Opcode...
[EXCEPCION: Invalid Opcode]
RIP: 0x...
RAX: 0x... RBX: 0x... ...
```

---

## Kernel Cracker

El módulo Kernel Cracker busca por fuerza bruta un string de hasta 5 letras minúsculas cuyo hash coincida con un valor cargado en el kernel. Los comandos relacionados son `sethash`, `hash` y `bruteforce`.

### Función de hash

```c
hash(str) = 5381
para cada carácter c: hash = hash * 33 + c
```

El espacio de búsqueda cubre todas las combinaciones de `a` a `zzzzz`

---

### `hash <texto>`

Calcula e imprime el valor del hash para el texto dado.

```
@shell> hash hola
hash for hola: 2087101688
```

Útil para obtener el hash de una cadena conocida antes de cargarlo con `sethash`.

---

### `sethash <valor>`

Carga en el kernel el valor de hash objetivo que `bruteforce` deberá encontrar. 

```
@shell> sethash 2087101688
```

---

### `bruteforce`

Inicia la búsqueda por fuerza bruta del string que genera el hash actualmente cargado en el kernel. Prueba todas las combinaciones de letras minúsculas de `a` a `zzzzz`.

Al finalizar imprime el string encontrado y los ciclos de CPU insumidos.

```
@shell> bruteforce
hola
1234567890 cpu cycles
```

Si no existe ninguna combinación de hasta 5 letras que genere ese hash, imprime `couldnt find solution`.

---

## Pacman

### Iniciar el juego

```
@shell> pacman
```

Se presenta un menú de selección de modo de juego.

---

### Menú principal

| Tecla | Acción |
|-------|--------|
| `1`   | 1 jugador: Pacman (humano) vs fantasmas (IA) |
| `2`   | 2 jugadores: Pacman (humano) vs Blinky (humano) |
| `Q`   | Salir a la shell |

---

### Controles

El jugador se mueve con las teclas WASD. Si hubiese 2 jugadores, el segundo se mueve con las teclas IJKL.

---

### Fantasmas (IA)

En el modo 1 jugador, los 4 fantasmas tienen comportamientos distintos:

| Fantasma | Color   | Comportamiento |
|----------|---------|----------------|
| Blinky   | Rojo    | Persigue directamente a Pacman |
| Pinky    | Rosa    | Apunta 4 tiles adelante de Pacman |
| Inky     | Celeste | Flanqueo combinado con la posición de Blinky |
| Clyde    | Naranja | Persigue a Pacman, pero huye cuando está a menos de 8 tiles |

Los fantasmas son liberados en intervalos escalonados al inicio de cada vida.

---

### Puntuación

| Evento              | Puntos |
|---------------------|--------|
| Pellet pequeño      | 10     |
| Power pellet        | 50     |
| Comer un fantasma   | 200    |

El puntaje acumulado se muestra en el HUD a la izquierda del mapa.

---

### Power pellets y modo asustado

Al comer un power pellet:

- Todos los fantasmas se ponen azules y huyen aleatoriamente durante 4 segundos.
- Pacman puede comerlos durante este período para sumar puntos.
- Cuando quedan pocos ticks, los fantasmas flashean en blanco/azul como advertencia.
- Al ser comido, un fantasma vuelve a la casa  y reaparece normalmente.

---

### Vidas y Game Over

El juego comienza con 3 vidas. Al chocar con un fantasma normal se pierde una vida, los personajes se reinician y el laberinto mantiene el estado actual. Al perder todas las vidas se muestra la pantalla de **Game Over** con el puntaje final.

---

### Victoria

El juego termina con victoria al comer todos los pellets del mapa. Se muestra la pantalla de **You Win** con el puntaje final.

---

### Sonido

Suena la melodía clásica de Pacman al iniciar cada partida.