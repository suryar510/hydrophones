MC_FLAGS = -Iinclude

all: mc

mc: bin/mc_sim bin/mc_std

bin/mc_sim: src/main.cpp src/input_sim.cpp src/process_phase_shift.cpp src/output_stdout.cpp
	g++ ${MC_FLAGS} $^ -o $@

bin/mc_std: src/main.cpp src/input_stdin.cpp src/process_phase_shift.cpp src/output_stdout.cpp
	g++ ${MC_FLAGS} $^ -o $@

