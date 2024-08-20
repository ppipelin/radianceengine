# Radiance Engine
[![Build Status](https://github.com/ppipelin/radiance/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/ppipelin/radiance/actions/workflows/cmake-multi-platform.yml)
[![Latest Release](https://img.shields.io/github/v/release/ppipelin/radiance?display_name=release)](https://github.com/ppipelin/radiance/releases)
![License](https://img.shields.io/github/license/ppipelin/radiance)

[![Lichess classical rating](https://lichess-shield.vercel.app/api?username=radianceengine&format=classical)](https://lichess.org/@/radianceengine/perf/classical)
[![Lichess rapid rating](https://lichess-shield.vercel.app/api?username=radianceengine&format=rapid)](https://lichess.org/@/radianceengine/perf/rapid)
[![Lichess blitz rating](https://lichess-shield.vercel.app/api?username=radianceengine&format=blitz)](https://lichess.org/@/radianceengine/perf/blitz)
[![Lichess bullet rating](https://lichess-shield.vercel.app/api?username=radianceengine&format=bullet)](https://lichess.org/@/radianceengine/perf/bullet)

C++ chess engine

## Search

- Negamax Framework
- Alpha-Beta Pruning
- Aspiration window
- Late move reduction
- Three-fold repetition
- Transposition tables

## Evaluation

- Piece-square tables
- _AlphaZero_ average piece values
- Endgame heuristics

## Versions tournament

| Rank | Name           | Elo   | +/- | Games | Score | Draw  |
| ---- | -------------- | ----- | --- | ----- | ----- | ----- |
| 1    | radiance_3.2   |   487 |  22 |  3600 | 94.3% |  3.9% |
| 2    | radiance_3.1.1 |   317 |  15 |  3600 | 86.1% |  6.3% |
| 3    | radiance_3.0.1 |   114 |  11 |  3600 | 65.8% |  8.1% |
| 4    | radiance_2.4   |   114 |  11 |  3600 | 65.8% | 10.0% |
| 5    | radiance_2.3   |    93 |  11 |  3600 | 63.1% |  8.9% |
| 6    | radiance_2.2   |   -60 |  11 |  3600 | 41.5% |  6.7% |
| 7    | radiance_2.1   |   -91 |  11 |  3600 | 37.2% |  6.4% |
| 8    | radiance_2.0   |  -122 |  12 |  3600 | 33.2% |  5.0% |
| 9    | radiance_1.5   |  -337 |  17 |  3600 | 12.6% |  0.8% |
| 10   | radiance_1.4   |  -940 |  93 |  3600 |  0.4% |  0.0% |

## Getting started

### Compilation

```
export CXX=g++-12
export CC=gcc-12
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
./radiance
```

### UCI options

| Name         | Type  | Default value |  Valid values                                       | Description                                          |
| ------------ | ----- | ------------- | --------------------------------------------------- | ---------------------------------------------------- |
| `Hash`       | spin  |      256      |                     [1, 131072]                     | Memory allocated to the transposition table (in MB). |
| `Threads`    | spin  |       1       |                       [1, 1]                        | Number of threads used to search.                    |
| `Evaluation` | combo |  "PSQTuned"   | ["PSQTuned", "PSQ", "Shannon", "ShannonHeuristics"] | Type of evaluation function.                         |
| `Search`     | combo |  "abNegamax"  |          ["abNegamax", "Minimax", "Random"]         | Type of search function.                             |

_I'm radiant!_
