# Informatikprojekt: Performancevergleich eines Partikelsystem umgesetzt mit OpenGL und Vulkan
## Informationen
Autor: Niklas Birk\
Betreuer: Prof. Dr. Daniel Scherzer\
Fach: Informatikprojekt

## Paper
Die schriftliche Ausarbeitung des Themas ist in [Informatikprojekt (PDF)](Informatikprojekt_release_01042020.pdf) zu finden. 
In dieser Arbeit wird eine Einführung in Partikelsysteme und Grafik-APIs beschrieben. Anschließend wird die Implementierung des Projektes beschrieben. Die Implementierung wird benötigt, um einen Performancevergleich durchzuführen. Das ist der wissenschaftlichere Teil dieser Arbeit und ist als letzten Teil zu finden.

## Projektdateien
Das Projekt enthält im wesentlichen drei Varianten:
- CPU
- OpenGL
- Vulkan

Der Name bezieht sich hierbei auf Art der Simulation der Partikel.
Die CPU-Variante simuliert die Partikel auf der CPU, 
während die anderen beiden Varianten sich die Compute-Funktionalitäten der
jeweiligen Grafik-API zu nutze machen.\
Bei der CPU- und OpenGL-Variante wird OpenGL zum rendern benutzt und 
bei der Vulkan-Variante entsprechend Vulkan.

### CPU-Variante
Zur CPU-Variante gehören folgende Dateien:
- [cpuMain.c](cpuMain.c): Enthält die main-Funktion
- ( [initOpenGL.c](initOpenGL.c): Enthält wesentliches zum initialiseren von OpenGL )
- ( [initOpenGL.h](initOpenGL.h): Header-Datei zu [initOpenGL.c](initOpenGL.c) )

### OpenGL-Variante
Zur OpenGL-Variante gehören folgende Dateien:
- [openglMain.c](openglMain.c): Enthält die main-Funktion
- [initOpenGL.c](initOpenGL.c): Enthält wesentliches zum initialiseren von OpenGL
- [initOpenGL.h](initOpenGL.h): Header-Datei zu [initOpenGL.c](initOpenGL.c)
- [ComputeShader.glsl](shaders/opengl/ComputeShader.glsl): Der Compute-Shader für OpenGL
- [VertexShader.glsl](shaders/opengl/VertexShader.glsl): Der Vertex-Shader für OpenGL
- [FragmentShader.glsl](shaders/opengl/FragmentShader.glsl): Der Fragment-Shader für OpenGL

### Vulkan-Variante
Zur OpenGL-Variante gehören folgende Dateien:
- [vulkanMain.c](vulkanMain.c): Enthält die main-Funktion
- [initVulkan.c](initVulkan.c): Enthält wesentliches zum initialiseren von Vulkan
- [initVulkan.h](initVulkan.h): Header-Datei zu [initVulkan.c](initVulkan.c)
- [ComputeShader.comp](shaders/vulkan/ComputeShader.comp): Der Compute-Shader für Vulkan in GLSL
- [VertexShader.frag](shaders/vulkan/VertexShader.vert): Der Vertex-Shader für Vulkan in GLSL
- [FragmentShader.vert](shaders/vulkan/FragmentShader.frag): Der Fragment-Shader für Vulkan in GLSL
- [runCompiler.bat](shaders/vulkan/runCompiler.bat): Eine Batch-Datei zum Übersetzen der Shader in SPIR-V.\
__!Bitte den Pfad zur glslangValidator.exe auf Ihrem System anpassen!__\
Cmake sollte sich automatisch um das kopieren der Dateien und Ausführen der Batch-Datei im Zielverzeichnis kümmern.

### Partikelsystem und Sonstige
Für das Partikelsystem sind folgende Dateien vorhanden:
- [particlesystem.c](particlesystem.c): Enthält wesentliche Funktionen zum Erstellen eines Partikelsystems
- [particlesystem.h](particlesystem.h): Header-Datei zu [particlesystem.c](particlesystem.c)
- [utils.c](utils.c): Enthält eine kleine Hilfsfunktion zum Lesen von Dateien. 
- [utils.h](utils.h): Header-Datei zu [utils.c](utils.c) und enthält #define für wichtige Konstanten

## Ausführung
Cmake erstellt drei Ziele:
- Informatikprojekt: Die CPU-Variante
- Informatikprojekt_OpenGL: Die OpenGL-Variante
- Informatikprojekt_Vulkan: Die Vulkan-Variante

Die Compute-Shader sehen aktuell eine Workgroupgröße von x=1024, y=1 und z=1 vor.\
Sollte dies Ihr System nicht zulassen, dann kann das entsprechend in den Shadern geändert werden.
Bitte aber auch die Dispatch-Anweisungen anpassen. 

Die maximale Anzahl der Partikel ist durch die Dispatch-Anweisungen bzw. die maximale Anzahl an Gruppen beschränkt. 
Bei meinem System waren es etwas mehr 60 Millionen Partikel. 
Bei 60 Millionen Partikeln gibt es knapp 60 Tausend Arbeitsgruppen, bei meinem System ist die maximale Anzahl 65536.
Sollte also die Ausführung nichts anzeigen, dann kann es sein, dass Sie die Lokale X Größe anpassen müssen.
(Anmerkung: Für Partikelanzahl < 1024 sind es zu wenige Gruppen (0), die der Dispatch-Anweisung mit gegeben werden).



