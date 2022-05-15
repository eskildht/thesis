# Overview

This repository contains a standard B<sup>+</sup>-tree implementation. Moreover, it contains a B<sup>+</sup>-tree implementation parallelizing the already established access methods of the standard B<sup>+</sup>-tree.

The parallel B<sup>+</sup>-tree implementation is sustained in-memory, makes use of a thread pool design pattern, and supports batch processing of operations grouped by type, as well as so-called single key operations. Furthermore, Bloom filters can optionally be enabled and essential variables, such as order, the number of threads, and the number of base B<sup>+</sup>-trees, can be configured.

In order to evaluate performance of the trees in various configurations the repository contains a command line program.

## Build

```
cd build
make
```

## Run: options and variables

```
USAGE:
	./optimized [FLAGS] [OPTIONS] --test <test>

FLAGS:
	--batch                     Enable batching during test and general build, if --tree option has value parallel
	--bloom-disable             Disable bloom filter usage if --tree option has value parallel
	--help                      Print this help information
	--show                      Print the tree after build if --tree-size value <= 1000

OPTIONS:
	--build-distr-high <num>    Highest possible key value during tree build [default: 1000000]
	--build-distr-low <num>     Lowest possible key value during tree build [default: 1]
	--op <num>                  Number of operations to perform for the --test value specified [default: 1000000]
	--op-distr-high <num>       Highest possible key value during test operation [default: 1000000]
	--op-distr-low <num>        Lowest possible key value during test operation [default: 1]
	--order <num>               Order of the Bplustree(s) [default: 5]
	--test <test>               The test to carry out [default: ] [possible values: delete, insert, search, update]
	--threads <num>             Number of threads to use in the thread pool if --tree has value parallel [default: std::thread::hardware_concurrency()]
	--tree <type>               The tree data structure to create [default: parallel] [possible values: basic, parallel]
	--tree-size <num>           The number of inserts to do during tree build (overridden by --op if --test has value insert) [default: 1000000]
	--trees <num>               Number of Bplustrees to use if --tree option has value parallel [default: std::thread::hardware_concurrency()]
```
