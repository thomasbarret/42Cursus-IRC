# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tbarret <tbarret@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/14 12:36:22 by tbarret           #+#    #+#              #
#    Updated: 2024/09/27 11:19:46 by tbarret          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRC = Main.cpp \
	  src/Server/Socket.cpp \
	  src/Server/Server.cpp \
	  src/Client/Client.cpp \
	  src/Server/Command.cpp \
	  src/Server/Channel.cpp \
	  src/Server/Mode.cpp \
	  src/Utility/String.cpp \
	  src/Utility/Signal.cpp \

BOT_SRC = bot/Main.cpp \
		  bot/src/Client/Bot.cpp \

BOT_OBJ = ${BOT_SRC:.cpp=.o}

BOT_NAME = ircbot

OBJ = ${SRC:.cpp=.o}
NAME = ircserv
HEADER  = ./interface/IRC.hpp
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -f

all: ${NAME}

%.o: %.cpp
	$(CC) ${CFLAGS} -c $< -o $@

${NAME}: ${OBJ}
	${CC} ${CFLAGS} ${OBJ} -I ${HEADER} -o ${NAME}

clean:
	${RM} ${OBJ}
	${RM} ${BOT_OBJ}

fclean: clean
	${RM} ${NAME}
	${RM} ${BOT_NAME}

bonus: ${BOT_OBJ}
	${CC} ${CFLAGS} ${BOT_OBJ} -I ${HEADER} -o ${BOT_NAME}

re: fclean all

.PHONY : all clean fclean re