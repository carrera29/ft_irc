NAME	=	irc

SRC_DIR	= ./src
OBJ_DIR	= ./obj
INC_DIR	= ./include

SRCS	=	$(SRC_DIR)/conexion_prueba.cpp
# 			$(SRC_DIR)/server.cpp \
# 			$(SRC_DIR)/client.cpp \
# 			$(SRC_DIR)/channel.cpp \

OBJS	=	$(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CC		=	c++
CFLAGS	=	-std=c++98 -Wall -Wextra -I $(INC_DIR)


all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean:	clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
