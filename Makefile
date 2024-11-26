### COMPILATION ###
CPP = c++
CPPFLAGS = -std=c++98 -g3 -Wall -Wextra -Werror -g3

### PROJECT NAME ###
PROJECT = ircserv
PROJECT_DIR = ./

### SOURCE FILE ###
SRC_DIR = ./src/
SRC_FILE = main.cpp \
			Server.cpp \
			Client.cpp \
			Message.cpp \
			log.cpp \
			utils.cpp

## OBJECT FILE ###
OBJ_DIR = .obj
OBJ_SRC = $(addprefix $(OBJ_DIR)/, $(SRC_FILE:.cpp=.o))

.PHONY= all clean fclean

### RULES ###
all : $(PROJECT)

# COMPILE PROJECT
$(PROJECT) : $(OBJ_SRC) $(OBJ_MANDATORY)
	$(CPP) $(CPPFLAGS) $(OBJ_SRC) -o $(PROJECT)

# COMPILE OBJECT FILE
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CPP) $(CPPFLAGS) -c $< -o $@

# DELETE OBJECT FILE AND EXECUTABLE
fclean : clean
	rm -f $(PROJECT)

# DELETE OBJECT FILE
clean :
	rm -f $(OBJ_DIR)/*.o
	@rm -df $(OBJ_DIR)/

# DELETE EXECUTABLE OBJECT FILE THEN MAKE ALL
re : fclean all
