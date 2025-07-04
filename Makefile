NAME    = ircserv
C+	    = c++
CFLAG   = -Wall -Werror -Wextra -std=c++98 -g3
RM      = rm -rf
OBJDIR  = obj
SOURCES = srcs/code/main.cpp srcs/code/server/serverMain.cpp srcs/code/server/command.cpp srcs/code/user/user.cpp

OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(C+) $(CFLAG) -c $< -o $@

$(NAME): $(OBJECTS)
	@$(C+) $(OBJECTS) -o $(NAME)
	@echo "\
	\033[0;32m ___   _____          ___   ____   ___\n\
	) __( )__ __(        )_ _( /  _ \\ / _( \n\
	| _)    | |          _| |_ )  ' / ))_  \n\
	)_(     )_(  _____  )_____(|_()_\\ \\__( \n\
	            )_____(                   "
	@echo "\nExecute the program with : ./ircserv [port] [password]"

clean : 
	@$(RM) -r $(OBJDIR)  > /dev/null 2>&1

fclean : clean
	@$(RM) -r $(OBJDIR)  > /dev/null 2>&1
	@$(RM) $(NAME)


all : $(NAME)

re: fclean all

.PHONY: clean all re fclean