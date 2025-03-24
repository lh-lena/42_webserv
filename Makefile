#### MAIN SETTINGS ####

CPP			:= c++
STD_98		:= -std=c++98
FLAGS		:= -Wall -Wextra -Werror -O3 -g
INCLUDES	:= -I includes/

NAME		:= webserv

SRC_DIR		:= srcs/
SRC_FILES	+= main.cpp
SRC_FILES	+= Location.cpp
SRC_FILES	+= Response.cpp
SRC_FILES	+= Request.cpp
SRC_FILES	+= ParseConfig.cpp
SRC_FILES	+= Server.cpp
SRC_FILES	+= ServerControler.cpp
SRC_FILES	+= Connection.cpp
SRC_FILES	+= RequestHandler.cpp
SRC_FILES	+= CGI.cpp
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

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp Makefile | $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CPP) $(STD_98) $(FLAGS) -MMD -MF $(patsubst %.o, %.d, $@) $(INCLUDES) -c $< -o $@

$(OBJ_DIR):
	@$(MKDIR) $(OBJ_DIR)

$(OBJ_DIR)/unit_tests:
	@$(MKDIR) $(OBJ_DIR)/unit_tests

$(NAME): $(OBJ_FILES)
	@echo "Linking $(NAME) ..."
	@$(CPP) $(STD_98) $(FLAGS) -o $(NAME) $(OBJ_FILES)
	@echo "$(BLUE)[$(NAME) -" \
	"info]: $(GREEN)$(BOLD)Build finished!$(RESET)"

#### ADDITIONAL RULES ####

clean :
	@echo "Cleaning object files..."
	@$(RM) $(OBJ_FILES)
	@$(RM) -r $(OBJ_DIR)

fclean : clean
	@echo "Removing $(NAME)..."
	@$(RM) $(NAME)

re : fclean all

run : all
	@./$(NAME)

valgrind : all
	@valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(NAME) conf/serv.conf

#### TEST SETTINGS ####

TEST_DIR		= unit_tests/
TEST_NAME		:= test_runner
GTEST_LIBS		:= -L/opt/homebrew/Cellar/googletest/1.15.2/lib -lgtest -lgtest_main -pthread
STD_20			:= -std=c++20 -fPIE
TEST_INCLUDES	:= -I/opt/homebrew/Cellar/googletest/1.15.2/include

# TEST_FILES		+= test_main.cpp
# TEST_FILES		+= UtilsTest.cpp
# TEST_FILES		+= CGIHandlerTest.cpp
# TEST_FILES		+= LocationTest.cpp

TEST_OBJ		:= $(patsubst %.cpp, $(OBJ_DIR)%.o, $(TEST_FILES))

test: $(TEST_NAME)
	@echo "Running unit tests..."
	@./$(TEST_NAME)

$(OBJ_DIR)%.o: $(TEST_DIR)%.cpp | $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CPP) $(STD_20) $(TEST_INCLUDES) -c $< -o $@

$(TEST_NAME): $(OBJ_FILES) $(TEST_OBJ)
	@echo "Linking $(TEST_NAME) ..."
	@$(CPP) $(STD_20) -o $(TEST_NAME) $(OBJ_FILES) $(TEST_OBJ) $(GTEST_LIBS)
	@echo "$(BLUE)[$(TEST_NAME) -" \
	"info]: $(GREEN)$(BOLD)Build finished!$(RESET)"


clean_test: clean
	@echo "Cleaning test object files..."
	@$(RM) $(TEST_OBJ)
	@$(RM) $(TEST_NAME)

re_test: clean_test test


.PHONY : all clean fclean re run valgrind test clean_test re_test
