CFLAGS := -g -Wall -Werror

DEPFLAGS = -MMD -MF $(@:.o=.d)
CC_WRITE_DEP = $(CC) $(CFLAGS) -c $< -o "$@" $(DEPFLAGS)

CLEAN := -rm -rf *.o *.d *.out *.gch
