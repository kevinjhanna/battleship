CFLAGS= -Wall -g -pedantic

TARGET1=batallaNavalFront
TARGET2=batallaNavalBack
TARGET3=initializeGame
TARGET4=freeGame
TARGET5=load
TARGET6=loadgame
TARGET7=loadthemes
TARGET8=saveGame
TARGET9=random
TARGET10=wherebomb
TARGET11=main
TARGET15=batallaNaval


$(TARGET15): $(TARGET1).c $(TARGET2).c $(TARGET3).c $(TARGET4).c $(TARGET5).c $(TARGET6).c $(TARGET7).c $(TARGET8).c $(TARGET9).c $(TARGET10).c $(TARGET11).c
	gcc $(CFLAGS) -o $(TARGET15) $(TARGET1).c $(TARGET2).c $(TARGET3).c $(TARGET4).c $(TARGET5).c $(TARGET6).c $(TARGET7).c $(TARGET8).c $(TARGET9).c $(TARGET10).c $(TARGET11).c

         
