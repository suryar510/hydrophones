UC_CMD = make -f teensy.mk INCLUDE_FLAGS="-Iinclude" TEENSY=36
MC_CMD = g++ -Iinclude -g

all: uc mc

uc: bin/uc_phase_shift bin/uc_raw bin/uc_sim

mc: bin/mc_sim bin/mc_std bin/mc_byte

bin/uc_phase_shift: src/main.cpp src/input_adc.cpp src/process_phase_shift.cpp src/output_usb.cpp
	${UC_CMD} CPP_FILES="$^" TARGET=$@

bin/uc_raw: src/main.cpp src/input_adc.cpp src/process_byte.cpp src/output_usb.cpp
	${UC_CMD}  CPP_FILES="$^" TARGET=$@

bin/uc_sim: src/main.cpp src/input_sim.cpp src/process_phase_shift.cpp src/output_usb.cpp
	${UC_CMD}  CPP_FILES="$^" TARGET=$@

bin/mc_sim: src/main.cpp src/input_sim.cpp src/process_phase_shift.cpp src/output_stdout.cpp
	${MC_CMD} $^ -o $@

bin/mc_std: src/main.cpp src/input_stdin.cpp src/process_phase_shift.cpp src/output_stdout.cpp
	${MC_CMD} $^ -o $@

bin/mc_byte: src/main.cpp src/input_byte.cpp src/process_phase_shift.cpp src/output_stdout.cpp
	${MC_CMD} $^ -o $@

.PHONY: clean

clean:
	make -f teensy.mk clean
	rm -f bin/*

