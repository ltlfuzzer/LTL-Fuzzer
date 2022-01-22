We now introduce how to change fuzzing parameters.

# For RERS subjects

We could change the following paramaters in the file LTL-Fuzzer.c according to our requirement. Kindly re-compile LTL-Fuzzer after changing parameters.

* The total time of fuzzing via:
```
    this->total_time_budget = TIME_specify
```

* The time to fuzz one target location via:
```
    this->time_budget_one_target = TIME_specify
```

* The time to exploitation of one target location via:
```
    this->time_to_exploitation = TIME_specify
```

* Other AFLGo algorithms and AFL parameters via:
```
    this->aflgo_paras = PARAS_specify
```

# For Protocol subjects

If we would like to fuzzing other protocol implementations, we could change network-related parameters in the file LTL-Fuzzer.c besides paramaters above.

* The network link and port via: 
```
    this->network_link = "-N LINK/PORT";
```

* The protocol name to test via:
```
    this->protocol_name = "-P PROTOCOL";
```

Note that, if execution of some protocol implementations requires other options, please update them into the execution command. 