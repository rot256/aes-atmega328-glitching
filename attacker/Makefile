glitch.json: glitch.v
	yosys -p "synth_ice40 -json $@" $?

%.asc: %.json go-board.pcf
	nextpnr-ice40 --package vq100 --hx1k --json $< --pcf go-board.pcf --asc $@

%.bin: %.asc
	icepack $? $@

flash: glitch.bin
	sudo iceprog $<

clean:
	rm -f *.json *.bin

.PHONY: flash clean



