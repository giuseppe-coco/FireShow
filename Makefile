# Makefile

CXX = g++

# Flag per il compilatore:
# -g   : Aggiunge informazioni di debug (utile per i crash)
# -Wall: Abilita tutti gli avvisi del compilatore (buona pratica)
CXXFLAGS = -g -Wall

# Elenco dei sorgenti comuni (glad deve essere sempre compilato)
COMMON_SRCS = libs/glad/src/glad.c src/image_loader_adapter.cpp src\ParticleSystem.cpp src\Shader.cpp src\FireworksShell.cpp

# Percorsi dove cercare i file di intestazione (.h)
# Il flag -I dice al compilatore "cerca anche qui"
INCLUDES = -Ilibs -Ilibs/glad/include -Ilibs/glfw/include

# Percorso dove cercare i file delle librerie (.a, .lib)
# Il flag -L dice al linker "cerca anche qui"
LIBS_PATH = -Llibs/glfw/lib

# Elenco delle librerie da collegare al nostro programma
# Il flag -l dice al linker "collega questa libreria"
# NOTA: -lglfw3 cerca un file chiamato libglfw3.a
LIBS = -lglfw3 -lgdi32 -lopengl32

# Directory dove salvare i binari
BIN_DIR = bin


# --- Regole di Compilazione ---

# La regola "all" è la regola di default.
# Viene eseguita quando si digita semplicemente "make" nel terminale.
# Dipende dal nostro TARGET, quindi make cercherà di creare il TARGET.
all:
	@echo "Usa 'make nomefile' (senza estensione) per compilare un sorgente specifico."

# Questa regola spiega a 'make' come creare un eseguibile a partire da un sorgente
# Esempio: "make source1" compila src/source1.cpp in bin/source1.exe
%: src/%.cpp
	$(CXX) $(CXXFLAGS) $(COMMON_SRCS) $< -o $(BIN_DIR)/$@.exe $(INCLUDES) $(LIBS_PATH) $(LIBS)

# La regola "clean" serve a pulire i file generati.
# Utile per fare una ricompilazione completa.
clean:
	# Su Windows, il comando per cancellare è 'del'.
	# /f forza la cancellazione senza chiedere conferma.
	del /f $(BIN_DIR)\*.exe