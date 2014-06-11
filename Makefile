PROGRAM = acre
SOURCES = codeQualityChecker config \
					returnElseTraversal structClassKeywordTraversal staticConstructorTraversal \
					loopConditionTraversal earlyExitCandidateTraversal acrePragmaTraversal

_OBJS = $(SOURCES:=.o)
OBJS = $(patsubst %, $(OBJ_DIR)/%, $(_OBJS)) # From stackoverflow: http://stackoverflow.com/questions/1814270/gcc-g-option-to-place-all-object-files-into-separate-directory

CC	= gcc-4.4
CXX	= g++-4.4
CXXFLAGS = -pthread -g -std=c++0x

# Source and object directories
SRC_DIR=src
OBJ_DIR=build

# ROSE installation directory
ROSE_DIR = /home/users/norris/soft/pat
DEFINES	= -DBOOST_REGEX_MATCH_EXTRA
INCLUDES = -I$(ROSE_DIR)/include
JVMLIB_DIR = /usr/lib/jvm/java-7-openjdk-amd64/jre/lib/amd64/server

LIBS = -Wl,-rpath,$(ROSE_DIR)/lib -L$(ROSE_DIR)/lib -lrose \
     -lboost_date_time -lboost_thread -lboost_filesystem -lboost_program_options \
     -lboost_regex  -lboost_system  -lboost_wave -lpthread -lz  \
     -Wl,-rpath,$(JVMLIB_DIR) -L$(JVMLIB_DIR) -ljvm -lconfig++

# Default make rule to use
all: $(OBJ_DIR) $(PROGRAM) 

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.C
	$(CXX) -c $(CXXFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(OBJ_DIR):
	@mkdir $@

$(PROGRAM): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LIBS)

clean:
	$(RM) $(OBJ_DIR)/*.o $(PROGRAM) *.ti
	$(RM) -d $(OBJ_DIR)

