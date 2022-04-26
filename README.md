# LTL-Fuzzer 

LTL-Fuzzer is a greybox fuzzer to find violations of arbitrary Linear-time Temporal Logic(LTL) properties. It is built on top of the [AFL](https://lcamtuf.coredump.cx/afl/) fuzzer and involves additional program instrumentation to check if a particular execution trace is accepted by the Büchi automaton representing the negation of the given LTL property. Our framework takes as input a sequential program written in C/C++, and an LTL property. It finds violations, or counterexample traces, of the LTL property in stateful software systems. 

LTL-Fuzzer substantially extends directed greybox fuzzing to witness arbitrarily complex event orderings. We note that existing directed greybox fuzzing approaches are limited to witnessing reaching a location or witnessing simple event orderings such as crashes or use-after-free. This is the main contribution of our work: algorithms and an implementation
of our ideas in a tool that is able to validate any LTL property, thereby covering a much more expressive class of properties than crashes or use-after-free. Our work adapts directed greybox fuzzing (which directs the search towards specific program locations) to find violations of temporal logic formulae.

# Publication

The paper PDF can be found at [https://arxiv.org/abs/2109.02312](https://arxiv.org/abs/2109.02312).

```
	@InProceedings{ltlfuzzer,
	 title={Linear-time Temporal Logic guided Greybox Fuzzing},
	 author={Meng, Ruijie and Dong, Zhen and Li, Jialin and Beschastnikh, Ivan and Roychoudhury, Abhik},
	 booktitle={proceedings of the 37th IEEE International Conference on Software Engineering (ICSE 2022)},
	 year={2022},
	 publisher={ACM}
	}
```


# Building

## Requirements

* Ubuntu 18.04
* 8G RAM

## Installing dependencies

* Install basic dependencies:
```
   sudo apt install -y build-essential make cmake ninja-build git binutils-gold binutils-dev curl wget
```

* Install Boost 1.71:
```
   sudo apt install libboost-all-dev libboost-dev
```

* Install LLVM 11.0.0 with Gold-plugin(can refer to this [building script](./scripts/build-llvm-11.sh). After that, please copy the following libraries into the specified location:
```
   sudo cp /usr/lib/llvm-11/lib/libLTO.so /usr/lib/bfd-plugins/
   sudo cp /usr/lib/llvm-11/lib/LLVMgold.so /usr/lib/bfd-plugins/ 
```
* Install spot 2.9.7. You could also follow thses [instructions](https://spot.lrde.epita.fr/install.html).
  
```
   wget -q -O - https://www.lrde.epita.fr/repo/debian.gpg | sudo apt-key add -
   sudo echo 'deb http://www.lrde.epita.fr/repo/debian/ stable/' >> /etc/apt/sources.list
   sudo apt update
   sudo apt install -y spot libspot-dev libgtest-dev
```
* Install Python3 and related modules:

```
   sudo install python3 python3-dev python3-pip
   sudo pip3 install --upgrade pip
   sudo pip3 install networkx pydot pydotplus
```

## Installing LTL-Fuzzer

* Clone LTL-Fuzzer and them compile as follows: 
```
  cd LTL-Fuzzer 
  mkdir build
  cd build
  cmake ../
  make
  cd ../AFLGo
  make 
  cd llvm-mode
  make
  cd ../distance_calculator
  cmake -G Ninja ./
  cmake --build ./
```
# Example Usage

## RERS Example

### Testing Framework Introduction

* The subject under test in RERS19 (http://rers-challenge.org/2020/) is located in:
```
  LTL-Fuzzer/experiment/Problem1/src
```
* All events specified in the subject is stored in the following file:
```
  LTL-Fuzzer/experiment/Problem1/all_event_dir/all_events.txt
```
* Program locations corresponding to events are stored in the following file:
```
  LTL-Fuzzer/experiment/Problem1/target/targets.txt
```
* Instrumented subjects are stored in the following directory:
```
  LTL-Fuzzer/experiment/Problem1/build_dir
```
* The LTL property under check is stored in the following directory:
```
  LTL-Fuzzer/experiment/Problem1/ltl_dir
```
* The fuzzing initial seeds and outputs are stored, respectively:
```
  LTL-Fuzzer/experiment/Problem1/input_folder
  LTL-Fuzzer/experiment/Problem1/output_folder
```

### Preparing for Instrumentation

* Specifying the home directory, e.g., at home dir:
```
  export LTLFuzzer=~/LTL-Fuzzer/
```
* Specifying the subject directory and name under test:
```
  export SUBJECT=$LTLFuzzer/experiment/Problem1/
  export EXECName=Problem1
```
* Specifying a LTL property to be checked:
```
  export LTL="!(! (true U oU) | (! oU U ((oZ & ! oU) & X (! oU U oP))))"
```

### Starting Instrumentation

* Instrumenting the example subject uploaded in the repo, experiment/Problem1
```
  cd $LTLFuzzer/scripts/
  ./instrument-problem1.sh $SUBJECT/target/targets.txt $SUBJECT/src $SUBJECT/build_dir 
```

### Launching Fuzzing

* Launching a fuzzing campaign. We use the default fuzzing options here, but you could change fuzzing options as the instructions shown in the [Config-Options.md](./Config-Options.md).
```
  cd LTL-Fuzzer/build/src
  ./ltlfuzz 0
```

## Protocol Example

### Testing Framework Introduction

* The testing framework is the same as testing RERS subjects besides one additional fuzzing dictionary file:
```
  LTL-Fuzzer/experiment/testTelnet/telnet.dict
```

### Preparing for Instrumentation

* Specifying the home directory, e.g., at home dir:
```
  export LTLFuzzer=~/LTL-Fuzzer/
```
* Specifying the subject directory and name under test:
```
  export SUBJECT=$LTLFuzzer/experiment/testTelnet/
  export EXECName=telnet-server.minimal-net
```
* Specifying a LTL property to be checked:
```
  export LTL="!(G((WILL_DISABLED)->(X(G((DO)|(DONT))))))"
```

### Starting Instrumentation

* Instrumenting the example subject uploaded in the repo, experiment/testTelnet
```
  cd $LTLFuzzer/scripts/
  ./instrument-telnet.sh $SUBJECT/targets/targets.txt $SUBJECT/contiki $SUBJECT/build_dir 
```

### Launching Fuzzing

* Launching a fuzzing campaign. We use the default fuzzing options here, but you could change fuzzing options as the instructions shown in the [Config-Options.md](./Config-Options.md).
```
  cd LTL-Fuzzer/build/src
  ./ltlfuzz 1
```

# Contributions

## Contributors

  * Ruijie Meng
  * Zhen Dong
  * Jialin Li
  * Ivan Beschastnikh
  * Abhik Roychoudhury

## Other Contributors

We use [AFLGo](https://github.com/aflgo/aflgo) as one component to direct fuzzing towards a particular program location. Thanks to AFLGo's developers. We also welcome other contributors to improve and extend LTL-Fuzzer.


# License

This project is licensed under the Apache License 2.0 - see the [LICENSE](./LICENSE) file for details. 
