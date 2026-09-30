## Normas de Arquitectura y Modularidad

- **Cero código monolítico:** Está terminantemente prohibido acumular lógica de negocio, componentes, estilos o peticiones a API en un único fichero gigante.
- **Principio de responsabilidad única:** Cada fichero debe encargarse de una sola cosa (un componente por archivo, un servicio por archivo, un archivo de tipos separado).
- **Creación modular por defecto:** Ante cualquier nueva funcionalidad, **genera siempre ficheros independientes** en lugar de inflar los ya existentes. Por ejemplo:
  - Si creas un componente nuevo, sepáralo en su propia ruta/carpeta con sus tipos y lógica desacoplada.
- **Límite blando de líneas:** Ningún fichero de código debería superar las 150-200 líneas. Si crece más, divídelo en submódulos.