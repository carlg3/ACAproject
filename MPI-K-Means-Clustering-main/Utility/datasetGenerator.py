# it creates a MxN matrix of random values -- carlg
import numpy as np

def write_matrix(M, N):
    # fpath = f".\\DataSet{M}x{N}.txt"
    fpath = f"DataSet{M}x{N}.txt"

    matrix = np.random.randint(0, 10, (M, N))
    np.savetxt(fpath, matrix, fmt = "%d", delimiter = ",")

# write_matrix(600, 10)
# write_matrix(1500, 10)
# write_matrix(5000, 10)
# write_matrix(10000, 10)
# write_matrix(25000, 10)

write_matrix(1000,2)

# snippet for plotting a 2D dataset 
# ---------------------------------
# data = [eval(line.rstrip("\n")) for line in open("dataset_100x2.txt", "r")]   
# x, y = zip(*data)
# plt.scatter(x, y)
# plt.show()
