CC:=clang
CARGS:=
BINNAME=main
SRC:= $(wildcard ./*.c)

OBJ:=$(SRC:.c=.o)
# $^ = dependancies
# $< = first depandancy
# $@ = target

$(BINNAME):$(OBJ)
	$(CC) $(CARGS) $^ -o $@

%.o:%.c Makefile
	$(CC) $(CARGS) -c $< -o $@
