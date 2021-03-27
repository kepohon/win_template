# makefile    (������)
#	2021.2.25
#
# $@  �^�[�Q�b�g�̃t�@�C������\���܂�
# $<  �ŏ��̕K�{���ڂ̃t�@�C������\���܂�
# $?  �^�[�Q�b�g������ōX�V���ꂽ�K�{�̍��ڂ��A�X�y�[�X�ŋ�؂������X�g�ŕ\���܂�
# $^  ���ׂĂ̕K�{���ڂ��X�y�[�X�ŋ�؂������X�g�ŕ\���܂�
#
# vpath pattern directory-list
#   example =>  vpath %.c src
#

.SUFFIXES : .o .c .rc
NAME=winmain
EXE=${NAME}.exe
SRC=winmain.cpp
RC=menu.rc
#OBJ=${SRC:.cpp=.o}
OBJS=$(SRC:.cpp=.o)
OBJS+=$(RC:.rc=.o)

CC=g++
MENU=windres
#windres <input file> <output file>

CFLAGS=-Wall -O3
#CFLAGS=-Wall -O3 -finput-charset=UTF-8 -fexec-charset=CP932
# CFLAGS=-finput-charset=UTF-8 -fexec-charset=CP932 -I include  # -I �́A�C���N���[�h�t�@�C���̃t�H���_���w��

LFLAGS=-mwindows
# case windows app
#	LFLAGS=-mwindows

all: ${NAME}.exe

${NAME}.exe: ${OBJS}
	${CC} -o $@ $^ ${LFLAGS}

.cpp.o:
	${CC} -o $@ -c $< ${CFLAGS}

.rc.o: $<
	$(MENU) $< $@
#	$(MENU) $(RC) $*.o

#menu.o: menu.rc menu.h
#	windres menu.rc menu.o

#${NAME}.o: 
#Stream.o: Stream.h ArrayStream.h
menu.o: menu.rc menu.h
winmain.o: menu.h
#menu.o: menu.h

clean:
	del *.o
	del *.exe


#c++ -o main.exe main.cpp -finput-charset=UTF-8 -fexec-charset=CP932
