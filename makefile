OUTPUT_FILE = server8
OUTPUT_TEST_FILE = 
_DEPS = 
_OBJ_MAIN = server8.o
_OBJ_OTHERS = 
_OBJ_TEST_ONLY = 

_GTEST_MAIN = gtest_main.a
_OBJ_TEST = $(_OBJ_TEST_ONLY) $(_OBJ_OTHERS) $(_GTEST_MAIN)

_OBJ = $(_OBJ_MAIN) $(_OBJ_OTHERS)

CXX=g++
CXXFLAGS=-I$(IDIR) -Wall  -g
LIBS=# -lm

OUTPUT_DIR = build
TDIR = test
IDIR = include
ODIR= obj
LDIR = lib
SDIR = src

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
OBJ_TEST = $(patsubst %,$(ODIR)/%,$(_OBJ_TEST))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)
$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

make: $(OBJ)
	$(CXX) -o $(OUTPUT_DIR)/$(OUTPUT_FILE) $^ $(CXXFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(OUTPUT_DIR)/*.exe

.PHONY: run
run:
	./$(OUTPUT_DIR)/$(OUTPUT_FILE)

# Google Test

GTEST_DIR = ../googletest-release-1.8.1\googletest
CPPFLAGS += -isystem $(GTEST_DIR)/include
CXXFLAGS += -g -Wall -Wextra -pthread
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

$(ODIR)/gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc -o $@

$(ODIR)/gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc -o $@

$(ODIR)/gtest.a : $(ODIR)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

$(ODIR)/gtest_main.a : $(ODIR)/gtest-all.o $(ODIR)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

$(ODIR)/%.o : $(TDIR)/%.cpp $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: test
test: $(OBJ_TEST)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $(OUTPUT_DIR)/$(OUTPUT_TEST_FILE)

.PHONY: run-test
run-test:
	./$(OUTPUT_DIR)/$(OUTPUT_TEST_FILE)
