clang:
	# -x<language>, --language <arg>, --language=<arg>
	# Treat subsequent input files as having type <language>
	clang++ -xc++-header precompiled/include.h -o include.h.pch -std=c++17

	# -include-pch <file>
	# Include precompiled header file
	clang++ -include-pch include.h.pch main.cc -o ph -std=c++17

gcc:
	g++ -xc++-header precompiled/include.h -o include.h.gch -std=c++17
	g++ -include include.h main.cc -o ph -std=c++17

clean:
	rm -fr include.h.pch include.h.gch ph

.PHONY: clang gcc clean
