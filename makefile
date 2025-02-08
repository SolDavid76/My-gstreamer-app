SRCS_DIR	= srcs/

SRC			=	main.cpp\
				GstreamerApp.cpp\

SRCS		=	$(addprefix $(SRCS_DIR), $(SRC))

INCS		=	-I ./includes/

OBJS_DIR	=	obj/

OBJ			=	${SRC:.cpp=.o}

OBJS		=	$(addprefix $(OBJS_DIR), $(OBJ))

NAME		=	a.out

CC			=	g++

CCFLAGS		=	-g3 -Wall -Wextra -std=c++11 `pkg-config --cflags gstreamer-1.0`

LDFLAGS		=	`pkg-config --libs gstreamer-1.0`

all: 	$(OBJS_DIR) ${NAME}

$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp
	${CC} ${CCFLAGS} -c $< -o $@ ${INCS}

$(NAME): $(OBJS)
	${CC} ${OBJS} -o ${NAME} ${LDFLAGS}

$(OBJS_DIR):
	mkdir $(OBJS_DIR)

clean:
	rm -rf ${OBJS_DIR}

fclean: clean
	rm -f ${NAME}

re:		fclean all

.PHONY:	all clean fclean re