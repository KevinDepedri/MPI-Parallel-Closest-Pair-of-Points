# file to create the big plot for the report
cores = [1, 2, 4, 8, 16, 32, 64]
times_pack = [4500.0, 4500.0, 3267.99, 2559.39, 1364.78, 1216.56, 933.45]
# filled with dummy data for now
times_scater = [4343.12, 4343.12, 3354.87, 2559.39, 1364.78, 1216.56, 933.45]
times_exc_p = [4555.23, 4555.23, 3267.99, 2559.39, 1364.78, 1216.56, 933.45]
times_exc_s = [4300.12, 4300.12, 3334.87, 2767.39, 1364.78, 1216.56, 933.45]

import matplotlib.pyplot as plt
import numpy as np

fig, ax = plt.subplots()
ax.plot(cores, times_pack, label='Pack', color='darkblue')
ax.plot(cores, times_scater, label='Scatter' , color='green')
# ex pac same color as pack but different line style
ax.plot(cores, times_exc_p, label='Exclusive (?) Pack', linestyle='--' , color='darkblue')
# ex scatter same color as scatter but different line style
ax.plot(cores, times_exc_s, label='Exclusive (?) Scatter', linestyle='--' , color='green')
ax.set(xlabel='Number of cores', ylabel='Time (s)',
         title='Time for different operations')
ax.grid()
ax.legend()
# 2048 x 1152 plot size
fig.set_size_inches(18.5, 10.5)

plt.show()