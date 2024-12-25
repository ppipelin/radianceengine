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

Time control: 120+1

| Rank | Name           | Elo  | +/- | Games | Score | Draw  |
| ---- | -------------- | ---- | --- | ----- | ----- | ----- |
| 1    | radiance_3.4   |  346 |  30 |   993 | 88.0% |  7.5% |
| 2    | radiance_3.3   |  326 |  30 |   992 | 86.7% |  6.4% |
| 3    | radiance_3.2   |  322 |  29 |   993 | 86.5% |  7.2% |
| 4    | radiance_3.1.1 |  201 |  24 |   991 | 76.1% |  7.1% |
| 5    | radiance_3.0.1 |   46 |  21 |   991 | 56.6% |  9.3% |
| 6    | radiance_2.4   |   11 |  21 |   990 | 51.6% |  9.3% |
| 7    | radiance_2.3   |    9 |  21 |   990 | 51.3% |  9.7% |
| 8    | radiance_2.1   | -162 |  23 |   991 | 28.3% |  7.5% |
| 9    | radiance_1.5   | -178 |  24 |   993 | 26.4% |  5.8% |
| 10   | radiance_2.2   | -194 |  24 |   993 | 24.6% |  4.9% |
| 11   | radiance_2.0   | -215 |  25 |   991 | 22.5% |  6.5% |
| 12   | radiance_1.4   | -744 | 102 |   992 |  1.4% |  0.1% |

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

| Name         | Type  | Default value |  Valid values                                                      | Description                                          |
| ------------ | ----- | ------------- | ------------------------------------------------------------------ | ---------------------------------------------------- |
| `Hash`       | spin  |      256      |                     [1, 131072]                                    | Memory allocated to the transposition table (in MB). |
| `Threads`    | spin  |       1       |                       [1, 1]                                       | Number of threads used to search.                    |
| `Evaluation` | combo |  "PSQTunedBB" | [ "PSQTunedBB", "PSQTuned", "PSQ", "Shannon", "ShannonHeuristics"] | Type of evaluation function.                         |
| `Search`     | combo |  "abNegamax"  |          ["abNegamax", "Minimax", "Random"]                        | Type of search function.                             |

_I'm radiant!_
