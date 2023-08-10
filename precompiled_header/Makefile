all:
	# -x<language>, --language <arg>, --language=<arg>
	# Treat subsequent input files as having type <language>
	clang++ -xc++-header include.h -o include.h.pch -std=c++17

	# -include-pch <file>
	# Include precompiled header file
	clang++ -include-pch include.h.pch main.cc -o ph -std=c++17

clean:
	rm -fr include.h.pch ph

.PHONY: all clean
