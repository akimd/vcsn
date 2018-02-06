v compile -f --debug CPPFLAGS+='-D_LIBCPP_HAS_NO_ASAN' CXXFLAGS+='-fno-omit-frame-pointer' LDFLAGS+='-lbenchmark -lprofiler'  tests/benchmarks/evaluate.cc
export CPUPROFILE=evaluate.prof
v run ./tests/benchmarks/evaluate
pprof --symbolize=demangle=full ./tests/benchmarks/evaluate $CPUPROFILE
pprof --gv --focus=evaluate ./tests/benchmarks/evaluate $CPUPROFILE
