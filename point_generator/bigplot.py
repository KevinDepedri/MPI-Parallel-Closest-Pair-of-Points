def efficiency(tserial: int, tparallel: int, core: int) -> float:
    return tserial / (tparallel * core)

def speedup(tserial: int, tparallel: int) -> float:
    return tserial / tparallel

cpu_counts = [1, 2, 4, 8, 16, 24, 32, 48, 64, 80]

packed_times_BF500K = [6802, 7074, 3938, 1671, 952, 723, 510, 414, 313, 242]
packed_speedups_BF500K = [speedup(packed_times_BF500K[0], t) for t in packed_times_BF500K]
packed_efficiencies_BF500K = [efficiency(packed_times_BF500K[0], t, c) for t, c in zip(packed_times_BF500K, cpu_counts)]

packed_excl_times_BF500K = [6802, 6285, 3502, 2705, 1003, 734, 778, 405, 311, 236]
packed_excl_speedups_BF500K = [speedup(packed_excl_times_BF500K[0], t) for t in packed_excl_times_BF500K]
packed_excl_efficiencies_BF500K = [efficiency(packed_excl_times_BF500K[0], t, c) for t, c in zip(packed_excl_times_BF500K, cpu_counts)]

scatter_times_BF500K = [6802, 5880, 3035, 2998, 1974, 616, 467, 331, 277, 242]
scatter_speedups_BF500K = [speedup(scatter_times_BF500K[0], t) for t in scatter_times_BF500K]
scatter_efficiencies_BF500K = [efficiency(scatter_times_BF500K[0], t, c) for t, c in zip(scatter_times_BF500K, cpu_counts)]

scatter_excl_times_BF500K = [6802, 6230, 3499, 1662, 867, 678, 454, 309, 231, 221]
scatter_excl_speedups_BF500K = [speedup(scatter_excl_times_BF500K[0], t) for t in scatter_excl_times_BF500K]
scatter_excl_efficiencies_BF500K = [efficiency(scatter_excl_times_BF500K[0], t, c) for t, c in zip(scatter_excl_times_BF500K, cpu_counts)]

# 25771 35780 20698 8994 4040 3156 2028 1599 1235 1022 
packed_times_BF1M = [25771, 35780, 20698, 8994, 4040, 3156, 2028, 1599, 1235, 1022]
packed_speedups_BF1M = [speedup(packed_times_BF1M[0], t) for t in packed_times_BF1M]
packed_efficiencies_BF1M = [efficiency(packed_times_BF1M[0], t, c) for t, c in zip(packed_times_BF1M, cpu_counts)]

# 25771 25669 14022 7491 4543 2812 2119 1518 1171 964 
packed_excl_times_BF1M = [25771, 25669, 14022, 7491, 4543, 2812, 2119, 1518, 1171, 964]
packed_excl_speedups_BF1M = [speedup(packed_excl_times_BF1M[0], t) for t in packed_excl_times_BF1M]
packed_excl_efficiencies_BF1M = [efficiency(packed_excl_times_BF1M[0], t, c) for t, c in zip(packed_excl_times_BF1M, cpu_counts)]

# 25771 25658 12789 8077 4425 2926 2151 1528 1117 935 
scatter_times_BF1M = [25771, 25658, 12789, 8077, 4425, 2926, 2151, 1528, 1117, 935]
scatter_speedups_BF1M = [speedup(scatter_times_BF1M[0], t) for t in scatter_times_BF1M]
scatter_efficiencies_BF1M = [efficiency(scatter_times_BF1M[0], t, c) for t, c in zip(scatter_times_BF1M, cpu_counts)]

# 25771 25264 12185 6622 3755 2497 2115 1454 1004 846 
scatter_excl_times_BF1M = [25771, 25264, 12185, 6622, 3755, 2497, 2115, 1454, 1004, 846]
scatter_excl_speedups_BF1M = [speedup(scatter_excl_times_BF1M[0], t) for t in scatter_excl_times_BF1M]
scatter_excl_efficiencies_BF1M = [efficiency(scatter_excl_times_BF1M[0], t, c) for t, c in zip(scatter_excl_times_BF1M, cpu_counts)]

rec50M_packed_times = [2923, 3260, 1270, 812, 553, 371, 356, 330, 311, 328]
rec50M_packed_speedups = [speedup(rec50M_packed_times[0], t) for t in rec50M_packed_times]
rec50M_packed_efficiencies = [efficiency(rec50M_packed_times[0], t, c) for t, c in zip(rec50M_packed_times, cpu_counts)]

rec50M_packed_excl_times = [2164, 2436, 1095, 787, 499, 400, 358, 333, 339, 289]
rec50M_packed_excl_speedups = [speedup(rec50M_packed_excl_times[0], t) for t in rec50M_packed_excl_times]
rec50M_packed_excl_efficiencies = [efficiency(rec50M_packed_excl_times[0], t, c) for t, c in zip(rec50M_packed_excl_times, cpu_counts)]

rec50M_scatter_times = [2736, 3018, 1270, 777, 628, 423, 348, 328, 308, 295]
rec50M_scatter_speedups = [speedup(rec50M_scatter_times[0], t) for t in rec50M_scatter_times]
rec50M_scatter_efficiencies = [efficiency(rec50M_scatter_times[0], t, c) for t, c in zip(rec50M_scatter_times, cpu_counts)]

rec50M_scatter_excl_times = [2336, 2282, 1253, 736, 1004, 416, 353, 374, 323, 316]
rec50M_scatter_excl_speedups = [speedup(rec50M_scatter_excl_times[0], t) for t in rec50M_scatter_excl_times]
rec50M_scatter_excl_efficiencies = [efficiency(rec50M_scatter_excl_times[0], t, c) for t, c in zip(rec50M_scatter_excl_times, cpu_counts)]

rec100M_packed_times = [2605, 3376, 1467, 760, 438, 422, 340, 348, 335, 301]
rec100M_packed_speedups = [speedup(rec100M_packed_times[0], t) for t in rec100M_packed_times]
rec100M_packed_efficiencies = [efficiency(rec100M_packed_times[0], t, c) for t, c in zip(rec100M_packed_times, cpu_counts)]

rec100M_packed_excl_times = [2483, 2231, 1995, 669, 525, 433, 357, 365, 341, 318]
rec100M_packed_excl_speedups = [speedup(rec100M_packed_excl_times[0], t) for t in rec100M_packed_excl_times]
rec100M_packed_excl_efficiencies = [efficiency(rec100M_packed_excl_times[0], t, c) for t, c in zip(rec100M_packed_excl_times, cpu_counts)]

rec100M_scatter_times = [2868, 2883, 1350, 800, 530, 488, 400, 291, 267, 296]
rec100M_scatter_speedups = [speedup(rec100M_scatter_times[0], t) for t in rec100M_scatter_times]
rec100M_scatter_efficiencies = [efficiency(rec100M_scatter_times[0], t, c) for t, c in zip(rec100M_scatter_times, cpu_counts)]

rec100M_scatter_excl_times = [2492, 2257, 1202, 735, 501, 419, 315, 305, 430, 304]
rec100M_scatter_excl_speedups = [speedup(rec100M_scatter_excl_times[0], t) for t in rec100M_scatter_excl_times]
rec100M_scatter_excl_efficiencies = [efficiency(rec100M_scatter_excl_times[0], t, c) for t, c in zip(rec100M_scatter_excl_times, cpu_counts)]

rec200M_packed_times = [4262, 3963, 3180, 2010, 1366, 842, 962, 764, 724, 806]
rec200M_packed_speedups = [speedup(rec200M_packed_times[0], t) for t in rec200M_packed_times]
rec200M_packed_efficiencies = [efficiency(rec200M_packed_times[0], t, c) for t, c in zip(rec200M_packed_times, cpu_counts)]

rec200M_packed_excl_times = [4059, 3454, 2594, 1679, 1262, 1015, 970, 889, 739, 655]
rec200M_packed_excl_speedups = [speedup(rec200M_packed_excl_times[0], t) for t in rec200M_packed_excl_times]
rec200M_packed_excl_efficiencies = [efficiency(rec200M_packed_excl_times[0], t, c) for t, c in zip(rec200M_packed_excl_times, cpu_counts)]

rec200M_scatter_times = [4115, 3808, 2777, 2201, 1253, 1067, 956, 801, 658, 693]
rec200M_scatter_speedups = [speedup(rec200M_scatter_times[0], t) for t in rec200M_scatter_times]
rec200M_scatter_efficiencies = [efficiency(rec200M_scatter_times[0], t, c) for t, c in zip(rec200M_scatter_times, cpu_counts)]

rec200M_scatter_excl_times = [4119, 3732, 2871, 2069, 1934, 874, 1097, 777, 647, 726]
rec200M_scatter_excl_speedups = [speedup(rec200M_scatter_excl_times[0], t) for t in rec200M_scatter_excl_times]
rec200M_scatter_excl_efficiencies = [efficiency(rec200M_scatter_excl_times[0], t, c) for t, c in zip(rec200M_scatter_excl_times, cpu_counts)]

rec250M_packed_times = [8723, 7904, 3503, 2256, 1343, 1173, 1115, 1081, 936, 956]
rec250M_packed_speedups = [speedup(rec250M_packed_times[0], t) for t in rec250M_packed_times]
rec250M_packed_efficiencies = [efficiency(rec250M_packed_times[0], t, c) for t, c in zip(rec250M_packed_times, cpu_counts)]

rec250M_packed_excl_times = [5557, 5746, 3768, 2258, 1640, 1434, 1149, 1020, 932, 871]
rec250M_packed_excl_speedups = [speedup(rec250M_packed_excl_times[0], t) for t in rec250M_packed_excl_times]
rec250M_packed_excl_efficiencies = [efficiency(rec250M_packed_excl_times[0], t, c) for t, c in zip(rec250M_packed_excl_times, cpu_counts)]

rec250M_scatter_times = [6406, 8234, 3802, 2272, 1639, 1367, 1099, 936, 823, 1169]
rec250M_scatter_speedups = [speedup(rec250M_scatter_times[0], t) for t in rec250M_scatter_times]
rec250M_scatter_efficiencies = [efficiency(rec250M_scatter_times[0], t, c) for t, c in zip(rec250M_scatter_times, cpu_counts)]

rec250M_scatter_excl_times = [6100, 5747, 3016, 2273, 1441, 1590, 1443, 874, 817, 836]
rec250M_scatter_excl_speedups = [speedup(rec250M_scatter_excl_times[0], t) for t in rec250M_scatter_excl_times]
rec250M_scatter_excl_efficiencies = [efficiency(rec250M_scatter_excl_times[0], t, c) for t, c in zip(rec250M_scatter_excl_times, cpu_counts)]

import matplotlib.pyplot as plt

# create a figure with 4 subplots, 2 rows and 2 columns; one row with packed and packed excl and one with scatter and scatter excl
fig, axs = plt.subplots(2, 2, constrained_layout = True)
# fig.tight_layout(pad=1.0)

# plot cpu counts as x axis, speedups as y axis taking values from rec packed
axs[0, 0].plot(cpu_counts, rec50M_packed_speedups, label='50M')
axs[0, 0].plot(cpu_counts, rec100M_packed_speedups, label='100M')
axs[0, 0].plot(cpu_counts, rec200M_packed_speedups, label='200M')
axs[0, 0].plot(cpu_counts, rec250M_packed_speedups, label='250M')
axs[0, 0].set_xlabel('Number of cores')
axs[0, 0].set_ylabel('Speedup')

# plot cpu counts as x axis, speedups as y axis taking values from rec packed excl
axs[0, 1].plot(cpu_counts, rec50M_packed_excl_speedups, label='50M')
axs[0, 1].plot(cpu_counts, rec100M_packed_excl_speedups, label='100M')
axs[0, 1].plot(cpu_counts, rec200M_packed_excl_speedups, label='200M')
axs[0, 1].plot(cpu_counts, rec250M_packed_excl_speedups, label='250M')
axs[0, 1].set_xlabel('Number of cores')
axs[0, 1].set_ylabel('Speedup')

# plot cpu counts as x axis, speedups as y axis taking values from rec scatter
axs[1, 0].plot(cpu_counts, rec50M_scatter_speedups, label='50M')
axs[1, 0].plot(cpu_counts, rec100M_scatter_speedups, label='100M')
axs[1, 0].plot(cpu_counts, rec200M_scatter_speedups, label='200M')
axs[1, 0].plot(cpu_counts, rec250M_scatter_speedups, label='250M')
axs[1, 0].set_xlabel('Number of cores')
axs[1, 0].set_ylabel('Speedup')

# plot cpu counts as x axis, speedups as y axis taking values from rec scatter excl
axs[1, 1].plot(cpu_counts, rec50M_scatter_excl_speedups, label='50M')
axs[1, 1].plot(cpu_counts, rec100M_scatter_excl_speedups, label='100M')
axs[1, 1].plot(cpu_counts, rec200M_scatter_excl_speedups, label='200M')
axs[1, 1].plot(cpu_counts, rec250M_scatter_excl_speedups, label='250M')
axs[1, 1].set_xlabel('Number of cores')
axs[1, 1].set_ylabel('Speedup')

# set titles for each subplot
axs[0, 0].set_title('Packed')
axs[0, 1].set_title('Packed exclusive')
axs[1, 0].set_title('Scattered')
axs[1, 1].set_title('Scattered exclusive')

# set y axis limit to 11
axs[0, 0].set_ylim(0, 11)
axs[0, 1].set_ylim(0, 11)
axs[1, 0].set_ylim(0, 11)
axs[1, 1].set_ylim(0, 11)

# set legend for each subplot
axs[0, 0].legend()
axs[0, 1].legend(loc='upper left')
axs[1, 0].legend()
axs[1, 1].legend()

# set figure title
fig.suptitle('Speedup of 50M, 100M, 200M, 250M points with Divide et Impera algorithm')

# show the plot
plt.show()

# now the same but with efficiencies
fig, axs = plt.subplots(2, 2, constrained_layout = True)
# plot cpu counts as x axis, speedups as y axis taking values from rec packed
axs[0, 0].plot(cpu_counts, rec50M_packed_efficiencies, label='50M')
axs[0, 0].plot(cpu_counts, rec100M_packed_efficiencies, label='100M')
axs[0, 0].plot(cpu_counts, rec200M_packed_efficiencies, label='200M')
axs[0, 0].plot(cpu_counts, rec250M_packed_efficiencies, label='250M')
axs[0, 0].set_xlabel('Number of cores')
axs[0, 0].set_ylabel('Efficiency')

# plot cpu counts as x axis, speedups as y axis taking values from rec packed excl
axs[0, 1].plot(cpu_counts, rec50M_packed_excl_efficiencies, label='50M')
axs[0, 1].plot(cpu_counts, rec100M_packed_excl_efficiencies, label='100M')
axs[0, 1].plot(cpu_counts, rec200M_packed_excl_efficiencies, label='200M')
axs[0, 1].plot(cpu_counts, rec250M_packed_excl_efficiencies, label='250M')
axs[0, 1].set_xlabel('Number of cores')
axs[0, 1].set_ylabel('Efficiency')

# plot cpu counts as x axis, speedups as y axis taking values from rec scatter
axs[1, 0].plot(cpu_counts, rec50M_scatter_efficiencies, label='50M')
axs[1, 0].plot(cpu_counts, rec100M_scatter_efficiencies, label='100M')
axs[1, 0].plot(cpu_counts, rec200M_scatter_efficiencies, label='200M')
axs[1, 0].plot(cpu_counts, rec250M_scatter_efficiencies, label='250M')
axs[1, 0].set_xlabel('Number of cores')
axs[1, 0].set_ylabel('Efficiency')

# plot cpu counts as x axis, speedups as y axis taking values from rec scatter excl
axs[1, 1].plot(cpu_counts, rec50M_scatter_excl_efficiencies, label='50M')
axs[1, 1].plot(cpu_counts, rec100M_scatter_excl_efficiencies, label='100M')
axs[1, 1].plot(cpu_counts, rec200M_scatter_excl_efficiencies, label='200M')
axs[1, 1].plot(cpu_counts, rec250M_scatter_excl_efficiencies, label='250M')
axs[1, 1].set_xlabel('Number of cores')
axs[1, 1].set_ylabel('Efficiency')

# set titles for each subplot
axs[0, 0].set_title('Packed')
axs[0, 1].set_title('Packed exclusive')
axs[1, 0].set_title('Scattered')
axs[1, 1].set_title('Scattered exclusive')

# set y axis limit to 11
axs[0, 0].set_ylim(0, 1.1)
axs[0, 1].set_ylim(0, 1.1)
axs[1, 0].set_ylim(0, 1.1)
axs[1, 1].set_ylim(0, 1.1)

# set legend for each subplot
axs[0, 0].legend()
axs[0, 1].legend()
axs[1, 0].legend()
axs[1, 1].legend()

# set figure title
fig.suptitle('Efficiency of 50M, 100M, 200M, 250M points with Divide et Impera algorithm')

# show the plot
plt.show()

import numpy as np

# now the same, 4 plots with cores as x axis and times as y axis; each subplot has 4 lines
fig, axs = plt.subplots(2, 2, constrained_layout = True)
# plot cpu counts as x axis, speedups as y axis taking values from rec packed
axs[0, 0].plot(cpu_counts, np.log(rec50M_packed_times), label='50M')
axs[0, 0].plot(cpu_counts, np.log(rec100M_packed_times), label='100M')
axs[0, 0].plot(cpu_counts, np.log(rec200M_packed_times), label='200M')
axs[0, 0].plot(cpu_counts, np.log(rec250M_packed_times), label='250M')
axs[0, 0].set_xlabel('Number of cores')
axs[0, 0].set_ylabel('Log(Time(ms))')

# plot cpu counts as x axis, speedups as y axis taking values from rec packed excl
axs[0, 1].plot(cpu_counts, np.log(rec50M_packed_excl_times), label='50M')
axs[0, 1].plot(cpu_counts, np.log(rec100M_packed_excl_times), label='100M')
axs[0, 1].plot(cpu_counts, np.log(rec200M_packed_excl_times), label='200M')
axs[0, 1].plot(cpu_counts, np.log(rec250M_packed_excl_times), label='250M')
axs[0, 1].set_xlabel('Number of cores')
axs[0, 1].set_ylabel('Log(Time(ms))')

# plot cpu counts as x axis, speedups as y axis taking values from rec scatter
axs[1, 0].plot(cpu_counts, np.log(rec50M_scatter_times), label='50M')
axs[1, 0].plot(cpu_counts, np.log(rec100M_scatter_times), label='100M')
axs[1, 0].plot(cpu_counts, np.log(rec200M_scatter_times), label='200M')
axs[1, 0].plot(cpu_counts, np.log(rec250M_scatter_times), label='250M')
axs[1, 0].set_xlabel('Number of cores')
axs[1, 0].set_ylabel('Log(Time(ms))')

# plot cpu counts as x axis, speedups as y axis taking values from rec scatter excl
axs[1, 1].plot(cpu_counts, np.log(rec50M_scatter_excl_times), label='50M')
axs[1, 1].plot(cpu_counts, np.log(rec100M_scatter_excl_times), label='100M')
axs[1, 1].plot(cpu_counts, np.log(rec200M_scatter_excl_times), label='200M')
axs[1, 1].plot(cpu_counts, np.log(rec250M_scatter_excl_times), label='250M')
axs[1, 1].set_xlabel('Number of cores')
axs[1, 1].set_ylabel('Log(Time(ms))')

# set titles for each subplot
axs[0, 0].set_title('Packed')
axs[0, 1].set_title('Packed exclusive')
axs[1, 0].set_title('Scattered')
axs[1, 1].set_title('Scattered exclusive')

# set legend for each subplot
axs[0, 0].legend()
axs[0, 1].legend()
axs[1, 0].legend()
axs[1, 1].legend()

# set figure title
fig.suptitle('Time of 50M, 100M, 200M, 250M points with Divide et Impera algorithm')

# show the plot
plt.show()


# the same but with np.log(y)
fig, axs = plt.subplots(2, 2, constrained_layout = True)
# plot cpu counts as x axis, speedups as y axis taking values from rec packed
axs[0, 0].plot(cpu_counts, np.log(rec50M_packed_times), label='packed')
axs[0, 0].plot(cpu_counts, np.log(rec50M_packed_excl_times), label='packed excl')
axs[0, 0].plot(cpu_counts, np.log(rec50M_scatter_times), label='scatter')
axs[0, 0].plot(cpu_counts, np.log(rec50M_scatter_excl_times), label='scatter excl')
axs[0, 0].set_xlabel('Number of cores')
axs[0, 0].set_ylabel('Log(Time(ms))')
axs[0, 0].set_title('50M points')

# plot cpu counts as x axis, speedups as y axis taking values from rec packed excl
axs[0, 1].plot(cpu_counts, np.log(rec100M_packed_times), label='packed')
axs[0, 1].plot(cpu_counts, np.log(rec100M_packed_excl_times), label='packed excl')
axs[0, 1].plot(cpu_counts, np.log(rec100M_scatter_times), label='scatter')
axs[0, 1].plot(cpu_counts, np.log(rec100M_scatter_excl_times), label='scatter excl')
axs[0, 1].set_xlabel('Number of cores')
axs[0, 1].set_ylabel('Log(Time(ms))')
axs[0, 1].set_title('100M points')

# plot cpu counts as x axis, speedups as y axis taking values from rec scatter
axs[1, 0].plot(cpu_counts, np.log(rec200M_packed_times), label='packed')
axs[1, 0].plot(cpu_counts, np.log(rec200M_packed_excl_times), label='packed excl')
axs[1, 0].plot(cpu_counts, np.log(rec200M_scatter_times), label='scatter')
axs[1, 0].plot(cpu_counts, np.log(rec200M_scatter_excl_times), label='scatter excl')
axs[1, 0].set_xlabel('Number of cores')
axs[1, 0].set_ylabel('Log(Time(ms))')
axs[1, 0].set_title('200M points')

# plot cpu counts as x axis, speedups as y axis taking values from rec scatter excl
axs[1, 1].plot(cpu_counts, np.log(rec250M_packed_times), label='packed')
axs[1, 1].plot(cpu_counts, np.log(rec250M_packed_excl_times), label='packed excl')
axs[1, 1].plot(cpu_counts, np.log(rec250M_scatter_times), label='scatter')
axs[1, 1].plot(cpu_counts, np.log(rec250M_scatter_excl_times), label='scatter excl')
axs[1, 1].set_xlabel('Number of cores')
axs[1, 1].set_ylabel('Log(Time(ms))')
axs[1, 1].set_title('250M points')

# set legend for each subplot
axs[0, 0].legend()
axs[0, 1].legend()
axs[1, 0].legend()
axs[1, 1].legend()

# set figure title
fig.suptitle('Log(Time) of 50M, 100M, 200M, 250M points with Divide et Impera algorithm')

# show the plot
plt.show()


# now the same but with bruteforce
fig, axs = plt.subplots(1, 2, constrained_layout = True)
# plot cpu counts as x axis, speedups as y axis taking values from rec packed
axs[0].plot(cpu_counts, np.log(packed_times_BF500K), label='packed')
axs[0].plot(cpu_counts, np.log(packed_excl_times_BF500K), label='packed excl')
axs[0].plot(cpu_counts, np.log(scatter_times_BF500K), label='scatter')
axs[0].plot(cpu_counts, np.log(scatter_excl_times_BF500K), label='scatter excl')
axs[0].set_xlabel('Number of cores')
axs[0].set_ylabel('Log(Time(ms))')
axs[0].set_title('500K points')

# plot cpu counts as x axis, speedups as y axis taking values from rec packed excl
axs[1].plot(cpu_counts, np.log(packed_times_BF1M), label='packed')
axs[1].plot(cpu_counts, np.log(packed_excl_times_BF1M), label='packed excl')
axs[1].plot(cpu_counts, np.log(scatter_times_BF1M), label='scatter')
axs[1].plot(cpu_counts, np.log(scatter_excl_times_BF1M), label='scatter excl')
axs[1].set_xlabel('Number of cores')
axs[1].set_ylabel('Log(Time(ms))')
axs[1].set_title('1M points')

# set legend for each subplot
axs[0].legend()
axs[1].legend()

# set figure title
fig.suptitle('Log(Time) of 500K, 1M points with Bruteforce algorithm')

# show the plot
plt.show()

# now the speedup but with bruteforce
fig, axs = plt.subplots(2, 2, constrained_layout = True)
axs[0, 0].plot(cpu_counts, packed_speedups_BF500K, label='500K')
axs[0, 0].plot(cpu_counts, packed_speedups_BF1M, label='1M')
axs[0,0].set_title('Packed')
axs[0,0].set_xlabel('Number of cores')
axs[0,0].set_ylabel('Speedup')

axs[0, 1].plot(cpu_counts, scatter_speedups_BF500K, label='500K')
axs[0, 1].plot(cpu_counts, scatter_speedups_BF1M, label='1M')
axs[0,1].set_title('Scatter')
axs[0,1].set_xlabel('Number of cores')
axs[0,1].set_ylabel('Speedup')

axs[1, 0].plot(cpu_counts, packed_excl_speedups_BF500K, label='500K')
axs[1, 0].plot(cpu_counts, packed_excl_speedups_BF1M, label='1M')
axs[1,0].set_title('Packed exclusive')
axs[1,0].set_xlabel('Number of cores')
axs[1,0].set_ylabel('Speedup')

axs[1, 1].plot(cpu_counts, scatter_excl_speedups_BF500K, label='500K')
axs[1, 1].plot(cpu_counts, scatter_excl_speedups_BF1M, label='1M ')
axs[1,1].set_title('Scatter exclusive')
axs[1,1].set_xlabel('Number of cores')
axs[1,1].set_ylabel('Speedup')

# set legend for each subplot
axs[0, 0].legend()
axs[0, 1].legend()
axs[1, 0].legend()
axs[1, 1].legend()

# set figure title
fig.suptitle('Speedup of 500K, 1M points with Bruteforce algorithm')

# show the plot
plt.show()

# now the same but with efficiency
fig, axs = plt.subplots(2, 2, constrained_layout = True)
axs[0, 0].plot(cpu_counts, packed_efficiencies_BF500K, label='500K')
axs[0, 0].plot(cpu_counts, packed_efficiencies_BF1M, label='1M')
axs[0, 0].set_title('Packed')
axs[0, 0].set_xlabel('Number of cores')
axs[0, 0].set_ylabel('Efficiency')

axs[0, 1].plot(cpu_counts, scatter_efficiencies_BF500K, label='500K')
axs[0, 1].plot(cpu_counts, scatter_efficiencies_BF1M, label='1M')
axs[0, 1].set_title('Scatter')
axs[0, 1].set_xlabel('Number of cores')
axs[0, 1].set_ylabel('Efficiency')

axs[1, 0].plot(cpu_counts, packed_excl_efficiencies_BF500K, label='500K')
axs[1, 0].plot(cpu_counts, packed_excl_efficiencies_BF1M, label='1M')
axs[1, 0].set_title('Packed exclusive')
axs[1, 0].set_xlabel('Number of cores')
axs[1, 0].set_ylabel('Efficiency')

axs[1, 1].plot(cpu_counts, scatter_excl_efficiencies_BF500K, label='500K')
axs[1, 1].plot(cpu_counts, scatter_excl_efficiencies_BF1M, label='1M')
axs[1, 1].set_title('Scatter exclusive')
axs[1, 1].set_xlabel('Number of cores')
axs[1, 1].set_ylabel('Efficiency')

# set legend for each subplot
axs[0, 0].legend()
axs[0, 1].legend()
axs[1, 0].legend()
axs[1, 1].legend()

# set figure title
fig.suptitle('Efficiency of 500K, 1M points with Bruteforce algorithm')

# show the plot
plt.show()

# bruteforce times by configuration
fig, axs = plt.subplots(2, 2, constrained_layout = True)
axs[0, 0].plot(cpu_counts, np.log(packed_times_BF500K), label='500K')
axs[0, 0].plot(cpu_counts, np.log(packed_times_BF1M), label='1M')
axs[0, 0].set_title('Packed')
axs[0, 0].set_xlabel('Number of cores')
axs[0, 0].set_ylabel('Log(Time(ms))')

axs[0, 1].plot(cpu_counts, np.log(scatter_times_BF500K), label='500K')
axs[0, 1].plot(cpu_counts, np.log(scatter_times_BF1M), label='1M')
axs[0, 1].set_title('Scatter')
axs[0, 1].set_xlabel('Number of cores')
axs[0, 1].set_ylabel('Log(Time(ms))')

axs[1, 0].plot(cpu_counts, np.log(packed_excl_times_BF500K), label='500K')
axs[1, 0].plot(cpu_counts, np.log(packed_excl_times_BF1M), label='1M')
axs[1, 0].set_title('Packed exclusive')
axs[1, 0].set_xlabel('Number of cores')
axs[1, 0].set_ylabel('Log(Time(ms))')

axs[1, 1].plot(cpu_counts, np.log(scatter_excl_times_BF500K), label='500K')
axs[1, 1].plot(cpu_counts, np.log(scatter_excl_times_BF1M), label='1M')
axs[1, 1].set_title('Scatter exclusive')
axs[1, 1].set_xlabel('Number of cores')
axs[1, 1].set_ylabel('Log(Time(ms))')

# set legend for each subplot
axs[0, 0].legend()
axs[0, 1].legend()
axs[1, 0].legend()
axs[1, 1].legend()

# set figure title
fig.suptitle('Log(Time) of 500K, 1M points with Bruteforce algorithm')

# show the plot
plt.show()




