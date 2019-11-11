
NAME_FILE = $(NAME).cpp
OUTPUT_FILE = highgui_mod_wclass
_DEPS = SERVER.hpp CLIENT.hpp interface.hpp
_OBJ_MAIN = highgui_mod_wclass

_OBJ = $(_OBJ_MAIN) $(_OBJ_OTHERS)


OUTPUT_DIR = /home/erik/Documentos/GitHub/PSI3422/Fase1/out
TDIR = test
IDIR = /home/erik/Documentos/GitHub/PSI3422/include
ODIR= /home/erik/Documentos/GitHub/PSI3422/Fase1/obj
LDIR = lib
SDIR = /home/erik/Documentos/GitHub/PSI3422/Fase1

CXX = compila $(NAME_FILE)
CXXFLAGS = -e="-I$(IDIR) -o $(OUTPUT_DIR)/$(OUTPUT_FILE)" -c
LIBS=# -lm

make: 
	$(CXX) $(CXXFLAGS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(OUTPUT_DIR)/*.exe

.PHONY: run
run:
	$(OUTPUT_DIR)/$(OUTPUT_FILE)