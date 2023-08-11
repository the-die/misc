common_flags = -std=c++20 -g

pattern1 = (^Using built-in specs\.$$)
pattern2 = (^Configured with:)
pattern3 = (^Target:)
pattern4 = (^Thread model:)
pattern5 = (^InstalledDir:)
pattern6 = (^LLVM Profile Note)
pattern7 = (^ \(in-process\))
pattern8 = ( version )

clang_output = $(shell touch dummy.cc; clang++ -c -\#\#\# $(common_flags) dummy.cc |& grep -vE \
	"$(pattern1)|$(pattern2)|$(pattern3)|$(pattern4)|$(pattern5)|$(pattern6)|$(pattern7)|$(pattern8)" | \
	sed 's/"-emit-obj"//g; s/"-o"//g; s/"dummy\.o"//g; s/"dummy\.cc"//g; s/"-main-file-name"//g; s/"//g')

all:
	@# clang -cc1 is the frontend, clang is the driver. The driver invokes the frontend with options
	@# appropriate for your system.
	$(clang_output) -emit-module -o A.pcm -fmodules module.modulemap -fmodule-name=A -DENABLE_A

	$(clang_output) -emit-module -o B.pcm -fmodules module.modulemap -fmodule-name=B \
		-fmodule-file=A=A.pcm

	clang++ $(common_flags) main.cc -fmodules -fprebuilt-module-path=prebuilt -o cm -DENABLE_A

clean:
	rm -fr *.pcm cm dummy.cc

.PHONY: all prebuilt clean
