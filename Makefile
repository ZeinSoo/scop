# **************************************************************************** #
#                                    CONFIG                                    #
# **************************************************************************** #

NAME        = scop

CXX         = c++
CFLAGS      = -Wall -Wextra -g3 -MMD -MP -std=c++20 ##-DGLM_ENABLE_EXPERIMENTAL

INCLUDES    = -Iincs -Ilibs/include ##-Iglm

SRCDIR      = srcs/
OBJDIR      = .obj/

SRCS        = main.cpp
OBJS        = $(addprefix $(OBJDIR), $(SRCS:.cpp=.o))
DEPS        = $(OBJS:.o=.d)

LIBDIR      = libs
DEPSDIR     = deps
CMAKE_DIR   = $(DEPSDIR)/cmake

LIBS        = $(LIBDIR)/libglfw3.a \
              $(LIBDIR)/libGLEW.a \
              -lGL -lX11 -lXrandr -lXi -ldl -lpthread

CMAKE_BIN   = $(CMAKE_DIR)/bin/cmake

# **************************************************************************** #
#                                     RULES                                    #
# **************************************************************************** #

all: libs $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) $(LIBS) -o $(NAME)

# **************************************************************************** #
#                             COMPILATION C++                                  #
# **************************************************************************** #

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

-include $(DEPS)

# **************************************************************************** #
#                                   LIBRARIES                                  #
# **************************************************************************** #

libs: $(LIBDIR)/libglfw3.a $(LIBDIR)/libGLEW.a

# ---------- CMake binaire ---------- #

$(CMAKE_BIN):
	mkdir -p $(DEPSDIR)
	@echo "Downloading CMake..."
	wget -q https://github.com/Kitware/CMake/releases/download/v3.29.6/cmake-3.29.6-linux-x86_64.tar.gz -O $(DEPSDIR)/cmake.tar.gz
	tar -xzf $(DEPSDIR)/cmake.tar.gz -C $(DEPSDIR)
	mv $(DEPSDIR)/cmake-3.29.6-linux-x86_64 $(CMAKE_DIR)
	rm $(DEPSDIR)/cmake.tar.gz
	@echo "CMake downloaded to $(CMAKE_BIN)"

# ---------- GLFW ---------- #

$(DEPSDIR)/glfw:
	mkdir -p $(DEPSDIR)
	git clone https://github.com/glfw/glfw.git $(DEPSDIR)/glfw

$(LIBDIR)/libglfw3.a: $(DEPSDIR)/glfw $(CMAKE_BIN)
	mkdir -p $(LIBDIR)
	$(CMAKE_BIN) -S $(DEPSDIR)/glfw -B $(DEPSDIR)/glfw/build \
		-DGLFW_BUILD_WAYLAND=OFF \
		-DGLFW_BUILD_X11=ON
	$(CMAKE_BIN) --build $(DEPSDIR)/glfw/build
	cp $(DEPSDIR)/glfw/build/src/libglfw3.a $(LIBDIR)
	mkdir -p $(LIBDIR)/include
	cp -r $(DEPSDIR)/glfw/include/GLFW $(LIBDIR)/include/

# ---------- GLEW ---------- #

$(DEPSDIR)/glew:
	mkdir -p $(DEPSDIR)
	git clone https://github.com/nigels-com/glew.git $(DEPSDIR)/glew

$(LIBDIR)/libGLEW.a: $(DEPSDIR)/glew
	cd $(DEPSDIR)/glew && make extensions
	cd $(DEPSDIR)/glew && make
	cp $(DEPSDIR)/glew/lib/libGLEW.a $(LIBDIR)
	mkdir -p $(LIBDIR)/include
	cp -r $(DEPSDIR)/glew/include/GL $(LIBDIR)/include/

# **************************************************************************** #
#                                    CLEAN                                     #
# **************************************************************************** #

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)
	rm -rf $(DEPSDIR)
	rm -rf $(LIBDIR)

re: fclean all

.PHONY: all clean fclean re libs

