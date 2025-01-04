#### MAIN SETTINGS ####

CPP			:= c++ -std=c++98
FLAGS		:= -Wall -Wextra -Werror -O3
INCLUDES	:= -I includes/

NAME		:= webserv

SRC_DIR		:= srcs/
SRC_FILES	+= Location.cpp
SRC_FILES	+= ParseConfig.cpp
SRC_FILES	+= Server.cpp
SRC_FILES	+= ServerControler.cpp
SRC_FILES	+= utils.cpp

OBJ_DIR		:= obj/
OBJ_FILES	:= $(patsubst %.cpp, $(OBJ_DIR)%.o, $(SRC_FILES))

DEP_DIR		:= dep/
DEPENDS		:= $(patsubst %.o, $(DEP_DIR)%.d, $(OBJ_FILES))
-include $(DEPENDS)

RM			:= /bin/rm -f
MKDIR		:= /bin/mkdir -p

#### TARGET COMPILATION ####

.DEFAULT_GOAL	:= all

all :	 ${NAME}

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp | $(OBJ_DIR) $(DEP_DIR)
	@echo "Compiling $<..."
	@$(CPP) $(FLAGS) -MMD -MF $(patsubst %.o, %.d, $@) $(INCLUDES) -c $< -o $@

$(OBJ_DIR):
	@$(MKDIR) $(OBJ_DIR)

$(DEP_DIR):
	@$(MKDIR) $(DEP_DIR)

$(NAME): $(OBJ_FILES)
	@echo "Linking $(NAME) ..."
	@$(CPP) $(FLAGS) -o $(NAME) $(OBJ_FILES)
	@echo "$(BLUE)[$(NAME) -" \
	"info]: $(GREEN)$(BOLD)Build finished!$(RESET)"

#### ADDITIONAL RULES ####

clean :
	@echo "Cleaning object files..."
	@$(RM) $(OBJ_FILES)
	@$(RM) -r $(OBJ_DIR)
	@$(RM)	$(DEPENDS)
	@$(RM) -r $(DEP_DIR)

fclean : clean
	@echo "Removing $(NAME)..."
	@$(RM) $(NAME)

re : fclean all

run : all
	@./$(NAME)

valgrind : all
	@valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(NAME)

.PHONY : all clean fclean re run valgrind