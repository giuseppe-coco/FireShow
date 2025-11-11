# --- Variabili di Configurazione ---

# Compilatore C++
CXX = g++

# Flag per il compilatore
# -std=c++17: Specifica lo standard del C++ (buona pratica)
# -g         : Informazioni di debug
# -Wall      : Tutti gli avvisi
CXXFLAGS = -std=c++17 -g -Wall

# Directory di output per binari e file oggetto
BIN_DIR = bin
OBJ_DIR = obj

# Eseguibile finale
TARGET = $(BIN_DIR)/FireworksSimulator.exe

# --- File Sorgente e Oggetti ---

# Elenco di tutte le directory contenenti file sorgente .cpp
SRC_DIRS = src src/shell libs/glad/src vendors/imgui vendors/imgui/backends

# Trova tutti i file .cpp in quelle directory
# La funzione wildcard cerca i file, la funzione notdir rimuove il percorso
# Questo ci dà una lista tipo: main.cpp Shader.cpp glad.c ...
SRCS = $(wildcard $(patsubst %,%/*.cpp,$(SRC_DIRS)))
SRCS += $(wildcard $(patsubst %,%/*.c,$(SRC_DIRS))) # Aggiungiamo anche i file .c per GLAD

# Genera l'elenco dei file oggetto (.o) corrispondenti ai file sorgente.
# Ogni file .o verrà salvato nella directory OBJ_DIR.
# Esempio: src/main.cpp -> obj/main.o
OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SRCS)))
OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(OBJS)) # Sostituisce anche .c

# --- Percorsi e Librerie ---

# Percorsi per i file di intestazione (.h)
INCLUDES = -Isrc -Ilibs -Ilibs/glad/include -Ilibs/glfw/include -Ivenders/imgui -Ivenders/imgui/backends

# Percorso per le librerie (.a, .lib)
LIBS_PATH = -Llibs/glfw/lib

# Librerie da collegare
LIBS = -lglfw3 -lgdi32 -lopengl32

# --- Regole di Compilazione ---

# La regola di default, "all", ora dipende dall'eseguibile finale.
# 'make' cercherà di creare il TARGET.
all: $(TARGET)

# Regola per creare l'eseguibile finale (il Linking).
# Questa regola dipende da TUTTI i file oggetto.
# Verrà eseguita solo se uno o più file in $(OBJS) sono più recenti del TARGET.
$(TARGET): $(OBJS)
	@echo "Linking..."
	@mkdir -p $(BIN_DIR) # Assicurati che la directory esista
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS_PATH) $(LIBS)
	@echo "Build completato: $(TARGET)"

# Regola pattern per compilare i file .cpp in file .o.
# Spiega a 'make' come trasformare un file .cpp dalla directory 'src'
# in un file .o nella directory 'obj'.
$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(OBJ_DIR) # Assicurati che la directory esista
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Regole simili per le altre directory dei sorgenti
$(OBJ_DIR)/%.o: libs/glad/src/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: vendors/imgui/%.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: vendors/imgui/backends/%.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Regola "clean" per rimuovere tutti i file generati
clean:
	@echo "Cleaning build files..."
	# Usiamo 'rm -f' che è più portabile (funziona anche su Git Bash, WSL, etc.)
	# Il -f ignora gli errori se i file non esistono.
	rm -f $(TARGET)
	rm -f $(OBJ_DIR)/*.o

# La regola .PHONY dice a 'make' che 'all' e 'clean' non sono file reali.
# Questo evita problemi se per caso crei un file chiamato "clean".
.PHONY: all clean