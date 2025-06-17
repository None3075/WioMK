# Sistema IoT para Evaluar el Desempeño del Proceso de Enseñanza y Aprendizaje en el Aula

## Descripción
Este proyecto utiliza la base del projecto [WioMK](https://github.com/mkbaraka/WioMK) para crear un sistema IoT que permite evaluar el desempeño del proceso de enseñanza y aprendizaje en el aula. El sistema está diseñado para ser utilizado con la Wio Terminal y se conecta a un servidor web para enviar datos de las sesiones de clase.

## Instalación del Dispositivo
1. Configure los datos de la red WiFi en el archivo `ClassAssistant.cpp`
2. Añade la ip del Servidor Web en el archivo `ClassAssistant.cpp`
3. Añade el User token en el archivo `tracker.cpp`
4. Cargar el código a la Wio Terminal utilizando Arduino IDE.

### SavaDataInterface
Este script a sido cambiado y por tanto ya no enseña interfaz gráfica, sino que se conecta directamente al servidor web para enviar datos. Los datos, estadisticas, estados de la clase y graficas se pueden consultar en el servidor web. El hecho de que no se muestre la interfaz gráfica es debido a limitaciones de la Wio Terminal.

### Controles
- **Botón A (Izquierda)**: Desactivado.
- **Botón B (Centro)**: Inicia y termina la sesión de clase.
- **Botón C (Derecha)**: Al estar en sesión, presionarlo iniciará o terminará el descanso de la clase.

## Contribución
1. Haz un fork del proyecto.
2. Crea una rama para tu feature:
    ```sh
    git checkout -b feature/nueva-feature
    ```
3. Realiza los cambios y haz commits:
    ```sh
    git commit -m "Descripción de los cambios"
    ```
4. Empuja tu rama:
    ```sh
    git push origin feature/nueva-feature
    ```
5. Abre un Pull Request.

## Licencia
Este proyecto está bajo la Licencia MIT. Para más detalles, consulta el archivo [LICENSE](LICENSE).

Proyecto dirigido por [Oihane Gomez Carmona](https://scholar.google.es/citations?hl=es&user=ptqq8JAAAAAJ).

[Repositorio en GitHub](https://github.com/mkbaraka/WioMK)
