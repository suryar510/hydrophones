MC_FLAGS = -Iinclude

all: uc mc

uc: bin/uc_phase_shift bin/uc_raw bin/uc_sim

mc: bin/mc_sim bin/mc_std bin/mc_byte

bin/uc_phase_shift: src/main.cpp src/input_adc.cpp src/process_phase_shift.cpp src/output_usb.cpp
	avr-g++ $^ -o $@

bin/uc_raw: src/main.cpp src/input_adc.cpp src/process_passthrough.cpp src/output_usb.cpp
	avr-g++ $^ -o $@

bin/uc_sim: src/main.cpp src/input_sim.cpp src/process_phase_shift.cpp src/output_usb.cpp
	avr-g++ $^ -o $@

bin/mc_sim: src/main.cpp src/input_sim.cpp src/process_phase_shift.cpp src/output_stdout.cpp
	g++ ${MC_FLAGS} $^ -o $@

bin/mc_std: src/main.cpp src/input_stdin.cpp src/process_phase_shift.cpp src/output_stdout.cpp
	g++ ${MC_FLAGS} $^ -o $@

bin/mc_byte: src/main.cpp src/input_byte.cpp src/process_phase_shift.cpp src/output_stdout.cpp
	g++ ${MC_FLAGS} $^ -o $@

