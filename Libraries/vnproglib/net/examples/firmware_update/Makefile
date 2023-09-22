
all: firmware_update

firmware_update: library Program.cs
	@mcs -out:firmware_update -r:VectorNav.Core Program.cs

library:
	cd ../.. && make
	@cp ../../bin/VectorNav.Core.dll .

clean:
	@rm -rf firmware_update
	@rm -rf VectorNav.Core.dll
	@cd ../.. && make clean

