# 🥛 Pasteurizadora de Leche – Simulador de Proceso Industrial

Este proyecto consiste en un simulador del proceso de pasteurización de leche en una planta industrial, desarrollado en **C++** como proyecto académico.

El sistema modela el comportamiento de distintos componentes de una planta de procesamiento, permitiendo controlar variables operativas y monitorear el estado del sistema mediante sensores simulados.

El programa permite manipular el funcionamiento del proceso a través de un archivo `.txt`, desde donde se pueden activar o desactivar válvulas y modificar condiciones operativas como temperatura y presión, simulando diferentes escenarios dentro de la planta.

---

## 🚀 Funcionalidades

- Simulación del proceso de pasteurización de leche en una planta industrial.
- Control de variables del sistema mediante un archivo de configuración `.txt`.
- Manejo de válvulas de entrada y salida del sistema.
- Simulación de variaciones de temperatura y presión dentro del tanque.
- Implementación de sensores virtuales que detectan:
  - Temperatura elevada
  - Presión elevada
  - Contaminación del tanque al abrir válvulas incompatibles simultáneamente.
- Simulación del funcionamiento de una bomba, mostrando su estado operativo y los PSI generados durante el proceso.

---

## 🛠️ Tecnologías Utilizadas

- **C++**
- Simulación de procesos
- Manejo de archivos (`.txt`) para control de variables
- Lógica de sensores y control de estados
