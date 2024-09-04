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

| Rank | Name           | Elo  | +/- | Games | Score | Draw  |
| ---- | -------------- | ---- | --- | ----- | ----- | ----- |
| 1    | radiance_3.3   |  370 |  19 |  2878 | 89.4% |  5.6% |
| 2    | radiance_3.2   |  349 |  18 |  2881 | 88.2% |  5.6% |
| 3    | radiance_3.1.1 |  242 |  15 |  2881 | 80.1% |  6.3% |
| 4    | radiance_2.4   |   79 |  12 |  2882 | 61.2% |  9.6% |
| 5    | radiance_3.0.1 |   71 |  12 |  2881 | 60.1% |  7.7% |
| 6    | radiance_2.3   |   54 |  12 |  2880 | 57.8% |  9.7% |
| 7    | radiance_2.2   |  -83 |  13 |  2880 | 38.2% |  7.1% |
| 8    | radiance_2.1   | -115 |  13 |  2882 | 34.1% |  6.7% |
| 9    | radiance_2.0   | -141 |  13 |  2884 | 30.8% |  5.4% |
| 10   | radiance_1.5   | -390 |  22 |  2884 |  9.6% |  0.1% |
| 11   | radiance_1.4   | -846 |  77 |  2887 |  0.8% |  0.0% |

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
