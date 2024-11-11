CC = gcc
CFLAGS = -mavx512f -O3 -Wall -c
TARGET = chacha20_v
OBJS = chacha20_v.o double_whole_round.o state_init.o state_to_vectors.o permute_v_state.o vectors_to_state.o encrypt.o run_test.o calculate_throughput.o

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -mavx512f -O3 -Wall -o $(TARGET)

chacha20_v.o: chacha20_v.c
	$(CC) $(CFLAGS) chacha20_v.c 

double_whole_round.o: double_whole_round.c
	$(CC) $(CFLAGS) double_whole_round.c 

state_init.o: state_init.c
	$(CC) $(CFLAGS) state_init.c

state_to_vectors.o: state_to_vectors.c
	$(CC) $(CFLAGS) state_to_vectors.c

permute_v_state.o: permute_v_state.c
	$(CC) $(CFLAGS) permute_v_state.c

vectors_to_state.o: vectors_to_state.c
	$(CC) $(CFLAGS) vectors_to_state.c

encrypt.o: encrypt.c
	$(CC) $(CFLAGS) encrypt.c

run_test.o: run_test.c
	$(CC) $(CFLAGS) run_test.c

calculate_throughput.o: calculate_throughput.c
	$(CC) $(CFLAGS) calculate_throughput.c

clean: 
	cmd /C del /Q $(TARGET) $(OBJS) chacha20_v.exe

.PHONY: clean