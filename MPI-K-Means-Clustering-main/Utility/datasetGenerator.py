# it creates a MxN matrix of random values -- carlg
import numpy as np

def write_matrix(M, N):
    fpath = f".\\dataset_{M}x{N}.txt"

    matrix = np.random.randint(0, 10, (M, N))
    np.savetxt(fpath, matrix, fmt = "%d", delimiter = ",")

write_matrix(1000, 10)

# snippet for plotting a 2D dataset 
# ---------------------------------
# data = [eval(line.rstrip("\n")) for line in open("dataset_100x2.txt", "r")]   
# x, y = zip(*data)
# plt.scatter(x, y)
# plt.show()
